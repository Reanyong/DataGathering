// FormView_AlarmTag.cpp : 구현 파일입니다.
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


// CFormView_AlarmTag 진단입니다.

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


TCHAR*	_lpszSiteTag_Column[] = {"사이트명","디바이스명","알람 갯수","현재 상태"};
TCHAR*	_lpszTagAlarm_Check[] = {"알람정보","발생시간","관제점 명","UMS처리결과","발생 메시지"};


// CFormView_AlarmTag 메시지 처리기입니다.
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
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 정지..");
		SetTimer(1,1000,0);
		m_bThreadStart = FALSE;
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "수집 상태 : [자동 수집]");
	}
	else
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 시작..");
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "수집 상태 : [수동 수집]");
	}	*/
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
-리스트 컨트롤러 생성
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}


void CFormView_AlarmTag::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer(nIDEvent);
	switch(nIDEvent)
	{
	case ID_START_THREAD:
		{
			int nStartRun = m_stGatherInfo.nAutoRun_Check;
			if(nStartRun == 1)
			{
				GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 정지..");
				m_bStartRunCheck = FALSE;

				StartThread();
			}
			else
			{
				GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 시작..");
				m_bStartRunCheck = TRUE;
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			}			
			Invalidate(TRUE);
		}
		break;
	case ID_STOP_THREAD: //버튼으로 정지
		StopThread(TRUE);

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		break;
	case ID_AUTO_STOP_THREAD: //시스템 자동 정지
		StopThread(FALSE);

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		break;
	}
	CFormView::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 버튼 이벤트 
- 시작 버튼 클릭시 이벤트
-void OnBnClickedButtonStart()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_AlarmTag::OnBnClickedButtonStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*CString strttext,s11;
	char *s1 = "똠방각하", *s2 = "맹구";
	char s3[] = "foo", s4[] = "FOO";
	s11 = "FOO";
	// "똠방각하"와 "맹구"를 비교
	if (!strcmp(s1, s2)) {
		strttext = "같군요";
	} else {
		strttext = "다르군요";
	}
	// 결과: 다르군요
	// "foo" 와 "FOO" 를 비교
	if (!strcmpi(s3, s11)) {
		strttext = "(대소문자 구분 없을 때에는) 같군요";
	} else {
		strttext = "(대소문자 구분 없을 때에는) 다르군요";
	}
	// (대소문자 구분 없을 때에는) 같군요
	return ;*/
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	if(m_bStartRunCheck == TRUE)
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 정지..");
		StartThread();
		m_bStartRunCheck = FALSE;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 시작..");
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
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 정지");

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"관리자에 의해 수집 시작");
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "상태 : [관리자에 의해 수집 시작]");
	}
	else
	{
		m_bThreadStart = TRUE;
		StopThread();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 시작");
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"관리자에 의해 수집 정지");
		_addSystemMsg(FORM_VIEW_ID_3, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "상태 : [관리자에 의해 수집 정지]");
	}	*/
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 한전망 데이터 수집 시작 쓰레드 생성
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
- 호출 방법 : 호출
- 스레드 종료/수집 종료
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
- 호출 방법 : 호출
- 디바이스 정보 변경된정보 리스트 컨트롤 출력
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
- 호출 방법 : 호출
- 수집시작시 디바이스 정보 리스트 컨트롤 출력
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
- 호출 방법 : 호출
- 알람 발생정보 리스트컨트롤 출력
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
- 호출 방법 : 호출
- 디바이스 정보 리스트 컨트롤 출력정보 삭제
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
- 호출 방법 : 이벤트
- 사용자 이벤트 메시지 
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
