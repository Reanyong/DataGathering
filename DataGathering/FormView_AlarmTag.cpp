// FormView_AlarmTag.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_AlarmTag.h"
#include "ChildFrm.h"

// CFormView_AlarmTag

IMPLEMENT_DYNCREATE(CFormView_AlarmTag, CFormView)

CFormView_AlarmTag::CFormView_AlarmTag()
	: CFormView(CFormView_AlarmTag::IDD)
{
	m_bThreadStart = TRUE;
	m_pTagAlarmMain = NULL;
	m_bStartRunCheck = TRUE;
}

CFormView_AlarmTag::~CFormView_AlarmTag()
{
	//StopThread();
}

void CFormView_AlarmTag::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormView_AlarmTag, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_START, &CFormView_AlarmTag::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnUserMessage)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFormView_AlarmTag �����Դϴ�.

#ifdef _DEBUG
void CFormView_AlarmTag::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_AlarmTag::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


TCHAR*	_lpszSiteTag_Column[] = {"����Ʈ��","����̽���","�˶� ����","���� ����"};
TCHAR*	_lpszTagAlarm_Check[] = {"�˶�����","�߻��ð�","������ ��","UMSó�����","�߻� �޽���"};


// CFormView_AlarmTag �޽��� ó�����Դϴ�.
void CFormView_AlarmTag::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	g_nActivateDocCheck = FORM_VIEW_ID_7;
	m_strTitle.LoadString(IDS_MY_TREE_VIEW_7);
	GetDocument()->SetTitle(m_strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();	
	pWnd->SetMenu(NULL);

	_CreateFont(&m_Font);

	ComposeList(m_xListSiteList, USER_CONTROL_LIST, IDC_STATIC_SITE_LIST,4,_lpszSiteTag_Column);
	m_xListSiteList.SetFont(&m_Font);

	ComposeList(m_xListAlarmCheckList, USER_CONTROL_LIST+1, IDC_STATIC_ALARM_CHECK_LIST,5,_lpszTagAlarm_Check);
	m_xListAlarmCheckList.SetFont(&m_Font);


	m_stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	//GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	SetTimer(ID_START_THREAD, 100, NULL);


	/*ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	if(stGatherInfo.nDemp_AutoCheck == 1)
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
		SetTimer(1,1000,0);
		m_bThreadStart = FALSE;
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "���� ���� : [�ڵ� ����]");
	}
	else
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "���� ���� : [���� ����]");
	}	*/
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
-����Ʈ ��Ʈ�ѷ� ����
-BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_AlarmTag::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
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

BOOL CFormView_AlarmTag::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}


void CFormView_AlarmTag::OnTimer(UINT_PTR nIDEvent)
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

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		break;
	case ID_AUTO_STOP_THREAD: //�ý��� �ڵ� ����
		StopThread(FALSE);

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
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
void CFormView_AlarmTag::OnBnClickedButtonStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	/*CString strttext,s11;
	char *s1 = "�c�氢��", *s2 = "�ͱ�";
	char s3[] = "foo", s4[] = "FOO";
	s11 = "FOO";
	// "�c�氢��"�� "�ͱ�"�� ��
	if (!strcmp(s1, s2)) {
		strttext = "������";
	} else {
		strttext = "�ٸ�����";
	}
	// ���: �ٸ�����
	// "foo" �� "FOO" �� ��
	if (!strcmpi(s3, s11)) {
		strttext = "(��ҹ��� ���� ���� ������) ������";
	} else {
		strttext = "(��ҹ��� ���� ���� ������) �ٸ�����";
	}
	// (��ҹ��� ���� ���� ������) ������
	return ;*/
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
		SetTimer(ID_STOP_THREAD, 100, NULL);
	}


	/*CString strProcessorTitle = "";
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	strProcessorTitle.LoadString(IDS_MY_TREE_VIEW_2);

	if(m_bThreadStart == TRUE)
	{
		m_bThreadStart = FALSE;
		StartThread();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"�����ڿ� ���� ���� ����");
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "���� : [�����ڿ� ���� ���� ����]");
	}
	else
	{
		m_bThreadStart = TRUE;
		StopThread();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"�����ڿ� ���� ���� ����");
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "���� : [�����ڿ� ���� ���� ����]");
	}	*/
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ������ ������ ���� ���� ������ ����
-BOOL StartThread()
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_AlarmTag::StartThread()
{
	if(m_pTagAlarmMain == NULL)
	{
		m_pTagAlarmMain = (CThread_TagAlarmMain *)AfxBeginThread(RUNTIME_CLASS(CThread_TagAlarmMain),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
		m_pTagAlarmMain->SetLPVOID(this);
		m_pTagAlarmMain->SetWnd(m_hWnd);
		m_pTagAlarmMain->SetProgramName(m_strTitle);
		m_pTagAlarmMain->ResumeThread();
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
void CFormView_AlarmTag::StopThread(BOOL bStopType)
{
	if(bStopType == TRUE)
		m_pTagAlarmMain->ButtonStop();

	if(m_pTagAlarmMain != NULL)
	{
		while (1)
		{
			if(m_pTagAlarmMain->GetStop() != TRUE)
				m_pTagAlarmMain->Stop();
			else
				break;
		}
		m_pTagAlarmMain = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����̽� ���� ��������� ����Ʈ ��Ʈ�� ���
-int SetDeviceList(int nItemRow,const char *szData1,const char *szData2,const char *szData3,const char *szData4)
*/
//////////////////////////////////////////////////////////////////////////
int CFormView_AlarmTag::SetDeviceList(int nItemRow,const char *szData1,const char *szData2,const char *szData3,const char *szData4)
{
	int nItem = m_xListSiteList.GetItemCount();
	//nItem = m_xListSiteList.InsertItem(0, szData1);
	m_xListSiteList.SetItemText(nItemRow, 0, szData1);
	m_xListSiteList.SetItemText(nItemRow, 1, szData2);
	m_xListSiteList.SetItemText(nItemRow, 2, szData3);
	m_xListSiteList.SetItemText(nItemRow, 3, szData4);
	
	m_xListSiteList.Invalidate(FALSE);

	return nItem;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- �������۽� ����̽� ���� ����Ʈ ��Ʈ�� ���
-int SetStartDeviceList(const char *szData1,const char *szData2,const char *szData3,const char *szData4)
*/
//////////////////////////////////////////////////////////////////////////
int CFormView_AlarmTag::SetStartDeviceList(const char *szData1,const char *szData2,const char *szData3,const char *szData4)
{
	int nItem = m_xListSiteList.InsertItem(0, szData1);
	m_xListSiteList.SetItemText(nItem, 1, szData2);
	m_xListSiteList.SetItemText(nItem, 2, szData3);
	m_xListSiteList.SetItemText(nItem, 3, szData4);

	m_xListSiteList.Invalidate(FALSE);

	return nItem;
}


//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- �˶� �߻����� ����Ʈ��Ʈ�� ���
-int SetAlarmOccursMsg(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5)
*/
//////////////////////////////////////////////////////////////////////////
int CFormView_AlarmTag::SetAlarmOccursMsg(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5)
{
	int nItem = m_xListAlarmCheckList.GetItemCount();
	if (nItem > 512)
	{
		m_xListAlarmCheckList.DeleteItem(nItem-1);
		nItem = m_xListAlarmCheckList.GetItemCount();
	}

	nItem = m_xListAlarmCheckList.InsertItem(0, szData1);
	m_xListAlarmCheckList.SetItemText(nItem, 1, szData2);
	m_xListAlarmCheckList.SetItemText(nItem, 2, szData3);
	m_xListAlarmCheckList.SetItemText(nItem, 3, szData4);
	m_xListAlarmCheckList.SetItemText(nItem, 4, szData5);
	
	m_xListAlarmCheckList.Invalidate(FALSE);

	return nItem;
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����̽� ���� ����Ʈ ��Ʈ�� ������� ����
-void SetDeviceListRemove()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_AlarmTag::SetDeviceListRemove()
{
	int nItem = m_xListSiteList.GetItemCount();
	
	if(nItem == 0)
		return;
	m_xListSiteList.DeleteItem(nItem-1);
	m_xListSiteList.Invalidate(FALSE);
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : �̺�Ʈ
- ����� �̺�Ʈ �޽��� 
-LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam)
*/
//////////////////////////////////////////////////////////////////////////
LRESULT CFormView_AlarmTag::OnUserMessage(WPARAM wParam, LPARAM lParam)
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

	_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "DB log : [DB Access..]",rgbRet, sMsgBuffer);
	return 0;
}

void CFormView_AlarmTag::OnDestroy()
{
	CFormView::OnDestroy();
	StopThread(FALSE);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
