// FormView_IConnect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_IConnect.h"
#include "ChildFrm.h"


// CFormView_IConnect

IMPLEMENT_DYNCREATE(CFormView_IConnect, CFormView)

CFormView_IConnect::CFormView_IConnect()
	: CFormView(CFormView_IConnect::IDD)
{
	m_bThreadStart = TRUE;
	m_pThreadMain = NULL;
	m_dwRowCnt = 0;
}

CFormView_IConnect::~CFormView_IConnect()
{
}

void CFormView_IConnect::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormView_IConnect, CFormView)
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnUserMessage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CFormView_IConnect::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CFormView_IConnect �����Դϴ�.

#ifdef _DEBUG
void CFormView_IConnect::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_IConnect::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormView_IConnect �޽��� ó�����Դϴ�.

TCHAR*	_lpszSiteConnect_Column[] = {"����Ʈ��","ID","���� ����"};
TCHAR* _lpszCurrent_ST[] = {"��ȣ","����Ʈ��","ó������"};
void CFormView_IConnect::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	//CString strTitle;
	g_nActivateDocCheck = FORM_VIEW_ID_4;
	m_strTitle.LoadString(IDS_MY_TREE_VIEW_4);
	GetDocument()->SetTitle(m_strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();	
	pWnd->SetMenu(NULL);

	_CreateFont(&m_Font);

	ComposeList(m_xListSiteList, USER_CONTROL_LIST, IDC_STATIC_SITE_LIST,3,_lpszSiteConnect_Column);
	m_xListSiteList.SetFont(&m_Font);

	ComposeList(m_xListCheckList, USER_CONTROL_LIST+1, IDC_STATIC_ALARM_CHECK_LIST,3,_lpszCurrent_ST);
	m_xListCheckList.SetFont(&m_Font);

	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	//ST_FILE_ISMARTACCESS stISmartAccess = _getInfoFileReadISmartAccess(g_stProjectInfo.szProjectIniPath);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	if(stGatherInfo.nISmart_GatherType == 0)
	{
		if(stGatherInfo.nAutoRun_Check == 1)
		{
			GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
			SetTimer(1,500, NULL);
			m_bThreadStart = FALSE;
			_addSystemMsg(FORM_VIEW_ID_5, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "���� ���� : [�ڵ� ����]");

			//_addCurrentstateMsg(0,0, m_strTitle, "�ڵ� ����");
		}
		else
		{
			GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			_addSystemMsg(FORM_VIEW_ID_5, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "���� ���� : [���� ����]");
			//_addCurrentstateMsg(0,0, m_strTitle, "���� ����");
		}
	}
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
-����Ʈ ��Ʈ�ѷ� ����
-BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_IConnect::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
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
			lvcolumn.fmt = LVCFMT_LEFT;//LVCFMT_CENTER;
			break;
		default:
			lvcolumn.fmt = LVCFMT_LEFT;
			break;
		}

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		nRight = (rect.right / nColumns);

		if(listCtrl == m_xListCheckList)
		{
			if(nPos == 0)
				nRight = nRight - 250;
			else if (nPos == 1)
				nRight = nRight - 200;
			else if(nPos == 2)
				nRight = 2000;
		}
		else
		{
			if(nPos == 0)
				nRight = nRight - 25;
		}
		lvcolumn.cx = nRight;
		listCtrl.InsertColumn(nPos, &lvcolumn);
	}
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : �̺�Ʈ
- ����� �̺�Ʈ �޽��� 
-LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam)
*/
//////////////////////////////////////////////////////////////////////////
LRESULT CFormView_IConnect::OnUserMessage(WPARAM wParam, LPARAM lParam)
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

	_addSystemMsg(FORM_VIEW_ID_5, USER_COLOR_BLUE, "DB log : [DB Access..]",rgbRet, sMsgBuffer);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����Ʈ ��Ʈ�� ��µ� ����
-void ListRemoveItem_Site()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_IConnect::ListRemoveItem_Site()
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
- ����Ʈ ��Ʈ�� ������ ���
-int ListInsertItem_Site(const char *szData1,const char *szData2,const char *szData3)
*/
//////////////////////////////////////////////////////////////////////////
int CFormView_IConnect::ListInsertItem_Site(const char *szData1,const char *szData2,const char *szData3)
{
	int nItem = m_xListSiteList.GetItemCount();

	BOOL bDataCheck = FALSE;
	for(int nI = 0; nI < nItem ; nI++)
	{
		CString strRetData1 = m_xListSiteList.GetComboText(nI,0);
		if(strRetData1 == szData1)
		{
			m_xListSiteList.SetItemText(nItem, 2, szData3);
			bDataCheck = TRUE;	
			break;
		}
	}
	if(bDataCheck == FALSE)
	{
		m_xListSiteList.InsertItem(nItem, szData1);
		m_xListSiteList.SetItemText(nItem, 1, szData2);
		m_xListSiteList.SetItemText(nItem, 2, szData3);
	}
	
	m_xListSiteList.Invalidate(FALSE);

	return nItem;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����Ʈ ��Ʈ�� ������ ���
-void ListInsertItem_Item(const char *szData1,const char *szData2,BOOL bType)
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_IConnect::ListInsertItem_Item(const char *szData1,const char *szData2,BOOL bType)
{
	CString strNum = "";
	int nItem = m_xListCheckList.GetItemCount();
	if (nItem > 1024)
	{
		m_xListCheckList.DeleteItem(nItem-1);
		nItem = m_xListCheckList.GetItemCount();
	}
	m_dwRowCnt++; m_dwRowCnt &= 0xffff;
	
	strNum.Format("%03d",m_dwRowCnt + 1);
		
	nItem = m_xListCheckList.InsertItem(0, strNum);
	m_xListCheckList.SetItemText(nItem, 1, szData1);
	if(bType == TRUE)
		m_xListCheckList.SetItemText(nItem, 2, szData2, RGB(255, 0, 255), RGB(255, 255, 255));
	else 
		m_xListCheckList.SetItemText(nItem, 2, szData2);
	//m_xListCheckList.SetItemText(nItem, 1, szData1);
	//m_xListCheckList.SetItemText(nItem, 1, szMsg2, crText2, RGB(255, 255, 255));
	//nItem = list.InsertItem(nItem, szMsg2, crText, ::GetSysColor(COLOR_WINDOW));

	m_xListCheckList.Invalidate(FALSE);
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ������ ������ ���� ���� ������ ����
-BOOL StartThread()
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_IConnect::StartThread()
{
	if(m_pThreadMain == NULL)
	{
		m_pThreadMain = (CThread_IConnect *)AfxBeginThread(RUNTIME_CLASS(CThread_IConnect),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
		m_pThreadMain->SetLPVOID(this);
		m_pThreadMain->SetWnd(m_hWnd);
		m_pThreadMain->SetProgramName(m_strTitle);
		m_pThreadMain->ResumeThread();
	}

	SetTimer(2,100,NULL);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ������ ����/���� ����
-void StopThread()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_IConnect::StopThread()
{

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


void CFormView_IConnect::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	KillTimer(nIDEvent);
	switch(nIDEvent)
	{
	case 1:
		StartThread();
		break;
	case 2:
		if(m_pThreadMain != NULL)
		{
			BOOL bRunState = m_pThreadMain->GetRunState();
			if(bRunState == TRUE)
			{
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			}
			else
				SetTimer(2,100,NULL);
		}
		break;
	default:
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
void CFormView_IConnect::OnBnClickedButtonStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strProcessorTitle = "";
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	strProcessorTitle.LoadString(IDS_MY_TREE_VIEW_4);

	m_dwRowCnt = 0;

	if(m_bThreadStart == TRUE)
	{
		m_bThreadStart = FALSE;
		StartThread();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"�����ڿ� ���� ���� ����");
		_addSystemMsg(FORM_VIEW_ID_5, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "���� : [�����ڿ� ���� ���� ����]");
	}
	else
	{
		m_bThreadStart = TRUE;
		StopThread();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);		

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"�����ڿ� ���� ���� ����");
		_addSystemMsg(FORM_VIEW_ID_5, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "���� : [�����ڿ� ���� ���� ����]");
	}
}


BOOL CFormView_IConnect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}
