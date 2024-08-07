// FormView_AlarmFMS.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_AlarmFMS.h"
#include "ChildFrm.h"

// CFormView_AlarmFMS

IMPLEMENT_DYNCREATE(CFormView_AlarmFMS, CFormView)

CFormView_AlarmFMS::CFormView_AlarmFMS()
	: CFormView(CFormView_AlarmFMS::IDD)
{
	m_bThreadStart = TRUE;
	m_pThreadMain = NULL;
	m_bStartRunCheck = TRUE;
}

CFormView_AlarmFMS::~CFormView_AlarmFMS()
{
}

void CFormView_AlarmFMS::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormView_AlarmFMS, CFormView)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CFormView_AlarmFMS::OnBnClickedButtonStart)
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnUserMessage)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFormView_AlarmFMS �����Դϴ�.

#ifdef _DEBUG
void CFormView_AlarmFMS::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_AlarmFMS::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormView_AlarmFMS �޽��� ó�����Դϴ�.
TCHAR*	_lpszSiteFMS_Column[] = {"����Ʈ��","�˶� ����","���� ����"};
TCHAR*	_lpszFMSAlarm_Check[] = {"�˶�����","�߻��ð�","������ ��","UMSó�����","�߻� �޽���"};

void CFormView_AlarmFMS::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	g_nActivateDocCheck = FORM_VIEW_ID_6;
	m_strTitle.LoadString(IDS_MY_TREE_VIEW_6);
	GetDocument()->SetTitle(m_strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();	
	pWnd->SetMenu(NULL);

	_CreateFont(&m_Font);

	ComposeList(m_xListSiteList, USER_CONTROL_LIST, IDC_STATIC_SITE_LIST,3,_lpszSiteFMS_Column);
	m_xListSiteList.SetFont(&m_Font);

	ComposeList(m_xListAlarmCheckList, USER_CONTROL_LIST+1, IDC_STATIC_ALARM_CHECK_LIST,5,_lpszFMSAlarm_Check);
	m_xListAlarmCheckList.SetFont(&m_Font);

	m_stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	//GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	SetTimer(ID_START_THREAD, 100, NULL);
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
-����Ʈ ��Ʈ�ѷ� ����
-BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_AlarmFMS::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
{
	int nRight = 0;
	CRect rcCtrl,rect;
	GetDlgItem(nPosListId)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(nPosListId)->ShowWindow(SW_HIDE);

	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | WS_VSCROLL ;
	if (!listCtrl.Create(dwStyle, rect, this, nListID))
	{
		TRACE(_T("Failed to create output window.\n"));
		return FALSE;
	}

	for(int nPos = 0; nPos< nColumns; nPos++)
	{
		if(szColumn[nPos] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		switch(nPos)
		{
		case 0:
			lvcolumn.fmt = LVCFMT_CENTER;
			break;
		default:
			lvcolumn.fmt = LVCFMT_LEFT;
			break;
		}

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		nRight = (rect.right / nColumns);
		if(nPos == 0)
			nRight = nRight - 25;

		lvcolumn.cx = nRight;

		listCtrl.InsertColumn(nPos, &lvcolumn);
	}

	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}

BOOL CFormView_AlarmFMS::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}


void CFormView_AlarmFMS::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	KillTimer(nIDEvent);

	switch(nIDEvent)
	{
	case ID_START_THREAD:
		{
			int nStartRun = m_stGatherInfo.nAutoRun_Check;
			if(nStartRun == 1)
			{
				GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
				m_bStartRunCheck = FALSE;

				StartThread();
			}
			else
			{
				GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
				m_bStartRunCheck = TRUE;
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			}			
			Invalidate(TRUE);
		}
		break;
	case ID_STOP_THREAD: //��ư���� ����
		StopThread(TRUE);

		GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(TRUE);

		break;
	case ID_AUTO_STOP_THREAD: //�ý��� �ڵ� ����
		StopThread(FALSE);

		GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(TRUE);
		break;
	}

	CFormView::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ��ư �̺�Ʈ 
- ���� ��ư Ŭ���� �̺�Ʈ
-void OnBnClickedButtonStart()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_AlarmFMS::OnBnClickedButtonStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	if(m_bStartRunCheck == TRUE)
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
		StartThread();
		m_bStartRunCheck = FALSE;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
		m_bStartRunCheck = TRUE;
		SetTimer(ID_AUTO_STOP_THREAD, 100, NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ������ ������ ���� ���� ������ ����
-BOOL StartThread()
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_AlarmFMS::StartThread()
{
	if(m_pThreadMain == NULL)
	{
		m_pThreadMain = (CThread_FMSAlarmMain *)AfxBeginThread(RUNTIME_CLASS(CThread_FMSAlarmMain),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
		m_pThreadMain->SetLPVOID(this);
		m_pThreadMain->SetWnd(m_hWnd);
		m_pThreadMain->SetProgramName(m_strTitle);
		m_pThreadMain->ResumeThread();
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ������ ����/���� ����
-void StopThread()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_AlarmFMS::StopThread(BOOL bStopType)
{
	if(bStopType == TRUE)
		m_pThreadMain->ButtonStop();

	if(m_pThreadMain != NULL)
	{
		while (1)
		{
			if(m_pThreadMain->GetStop() != TRUE)
				m_pThreadMain->Stop();
			else
				break;
		}
		m_pThreadMain = NULL;
	}
}

LRESULT CFormView_AlarmFMS::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	CString sMsgBuffer = "";
	sMsgBuffer = (LPCSTR)(long)wParam;

	//lParam = 0 : SUCCESS
	//lParam = 1 : Error
	int nMsgType = 0;
	nMsgType = lParam;
	COLORREF rgbRet;

	if(nMsgType == LOG_COLOR_BLUE)
		rgbRet = RGB(0, 0, 255);
	else if(nMsgType == LOG_COLOR_RED)
		rgbRet =  RGB(255, 0, 0);

	_addSystemMsg(LOG_MESSAGE_6, USER_COLOR_BLUE, "DB log : [DB Access..]",rgbRet, sMsgBuffer);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����Ʈ ��Ʈ�� ������� ����
-void ListRemoveItem_Site()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_AlarmFMS::ListRemoveItem_Site()
{
	int nItem = m_xListSiteList.GetItemCount();

	if(nItem == 0)
		return;
	m_xListSiteList.DeleteItem(nItem-1);
	m_xListSiteList.Invalidate(FALSE);
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����Ʈ ��Ʈ�� ����Ʈ ���� ���
-int ListInsertItem_Site(const char *szData1,const char *szData2,const char *szData3)
*/
//////////////////////////////////////////////////////////////////////////
int CFormView_AlarmFMS::ListInsertItem_Site(const char *szData1,const char *szData2,const char *szData3)
{
	int nItem = m_xListSiteList.GetItemCount();

	m_xListSiteList.InsertItem(nItem, szData1);
	m_xListSiteList.SetItemText(nItem, 1, szData2);
	m_xListSiteList.SetItemText(nItem, 2, szData3);

	m_xListSiteList.Invalidate(FALSE);

	return nItem;
}

void CFormView_AlarmFMS::OnDestroy()
{
	CFormView::OnDestroy();
	StopThread(FALSE);
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
