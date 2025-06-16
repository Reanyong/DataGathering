// FormView_Weather.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_Weather.h"
#include "ChildFrm.h"


// CFormView_Weather

IMPLEMENT_DYNCREATE(CFormView_Weather, CFormView)

CFormView_Weather::CFormView_Weather()
	: CFormView(CFormView_Weather::IDD)
{
	m_bThreadStart = TRUE;
	m_pThreadWeather = NULL;
}

CFormView_Weather::~CFormView_Weather()
{
}

void CFormView_Weather::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormView_Weather, CFormView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CFormView_Weather::OnBnClickedButtonStart)
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnUserMessage)
END_MESSAGE_MAP()


// CFormView_Weather 진단입니다.

#ifdef _DEBUG
void CFormView_Weather::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_Weather::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

TCHAR*	_lpszLiveGrid[] = {"Site","시간","온도","강수량","하늘코드","습도","강수코드","풍속"};
TCHAR*	_lpszForecastGrid[] = {"Site","시간","온도","습도","최고온도","최저온도","날씨","풍향"};


// CFormView_Weather 메시지 처리기입니다.

void CFormView_Weather::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();


	g_nActivateDocCheck = FORM_VIEW_ID_3;
	m_strTitle.LoadString(IDS_MY_TREE_VIEW_2);
	GetDocument()->SetTitle(m_strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();
	pWnd->SetMenu(NULL);

	 _CreateFont(&m_Font);

	ComposeList(m_xListLiveGrid, USER_CONTROL_LIST, IDC_STATIC_LIVE_GRID,8,_lpszLiveGrid);
	m_xListLiveGrid.SetFont(&m_Font);

	ComposeList(m_xListForecastGrid, USER_CONTROL_LIST+1, IDC_STATIC_FOREACSTGRID,8,_lpszForecastGrid);
	m_xListForecastGrid.SetFont(&m_Font);

	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	ST_WEATHER_INFO stWeatherInfo;
	stWeatherInfo = _getInfoWeatherRead(g_stProjectInfo.szProjectIniPath);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	int nGatherUse_YN = 0;
	nGatherUse_YN = atoi(stWeatherInfo.szLiveUSE_YN);
	if(stGatherInfo.nWeather_GatherType == 0 || stGatherInfo.nWeather_GatherType == 1)
	{
		if(nGatherUse_YN == 1)
		{
			_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Weather Gather Use Check", USER_COLOR_RED, "수집 허용 상태입니다.");
			if(stGatherInfo.nAutoRun_Check == 1)
			{
				GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 정지..");
				SetTimer(1,500,0);
				m_bThreadStart = FALSE;
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "수집 상태 : [자동 수집]");
				//_addCurrentstateMsg(0,0, m_strTitle, "자동 수집");
			}
			else
			{
				GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 시작..");
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "수집 상태 : [수동 수집]");
				//_addCurrentstateMsg(0,0, m_strTitle, "수동 수집");
			}
		}
		else
			_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Weather Gather Use Check", USER_COLOR_RED, "수집 미허용 상태입니다.");
	}
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
-리스트 컨트롤러 생성
-BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_Weather::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
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

BOOL CFormView_Weather::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}

void CFormView_Weather::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_pThreadWeather != NULL)
		m_pThreadWeather->EndCheck();
	StopThread();
}

void CFormView_Weather::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer(nIDEvent);
	switch(nIDEvent)
	{
	case 1:
		StartThread();
		break;
	default:
		break;
	}
	CFormView::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
-문구 표시 수집 시작/정지 표시
-void ShowRun_State(int nState)
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_Weather::ShowRun_State(int nState)
{
	CString strData = "";
	if(nState == 1)
		strData = "Run";
	else if(nState == 2)
		strData = "Stop";

	GetDlgItem(IDC_STATIC_RUN_STATE)->SetWindowText(strData);
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
-리스트 컨트롤에 출력할 정보
-int OutputWeatherHistory(CXListCtrl &listCtrl,const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8)
*/
//////////////////////////////////////////////////////////////////////////
int CFormView_Weather::OutputWeatherHistory(CXListCtrl &listCtrl,const char *szData1,
	const char *szData2,const char *szData3,const char *szData4,const char *szData5,
	const char *szData6,const char *szData7,const char *szData8)
{
	int nItem = listCtrl.GetItemCount();
	if (nItem > 512)
	{
		listCtrl.DeleteItem(nItem-1);
		//listCtrl.DeleteAllItems();
		//Invalidate(FALSE);
		nItem = listCtrl.GetItemCount();
	}


	nItem = listCtrl.InsertItem(0, szData1);
	listCtrl.SetItemText(nItem, 1, szData2);
	listCtrl.SetItemText(nItem, 2, szData3);
	listCtrl.SetItemText(nItem, 3, szData4);
	listCtrl.SetItemText(nItem, 4, szData5);
	listCtrl.SetItemText(nItem, 5, szData6);
	listCtrl.SetItemText(nItem, 6, szData7);
	if(listCtrl == m_xListForecastGrid)
		listCtrl.SetItemText(nItem, 7, szData8);

	listCtrl.Invalidate(FALSE);
	return nItem;
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
-리스트 컨트롤에 출력할 정보
-int OutputWeatherLiveHistory(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8)
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_Weather::OutputWeatherLiveHistory(const char *szData1,const char *szData2,const char *szData3,
	const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8)
{
	OutputWeatherHistory(m_xListLiveGrid,szData1,szData2,szData3,szData4,szData5,szData6,szData7,"");
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
-리스트 컨트롤에 출력할 정보
-int OutputWeatherForecastHistory(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8)
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_Weather::OutputWeatherForecastHistory(const char *szData1,const char *szData2,const char *szData3,
	const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8)
{
	OutputWeatherHistory(m_xListForecastGrid,szData1,szData2,szData3,szData4,szData5,szData6,szData7,szData8);
}


//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 날씨 데이터 수집 시작 스레드 생성
-BOOL StartThread()
*/
//////////////////////////////////////////////////////////////////////////
BOOL CFormView_Weather::StartThread()
{
	if(m_pThreadWeather == NULL)
	{
		m_pThreadWeather = (CThread_WeatherGather *)AfxBeginThread(RUNTIME_CLASS(CThread_WeatherGather),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
		m_pThreadWeather->SetLPVOID(this);
		m_pThreadWeather->SetWnd(m_hWnd);
		m_pThreadWeather->SetProgramName(m_strTitle);
		m_pThreadWeather->ResumeThread();
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 버튼 이벤트
- 시작 버튼 클릭시 이벤트
-void OnBnClickedButtonStart()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_Weather::OnBnClickedButtonStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	ST_WEATHER_INFO stWeatherInfo;
	stWeatherInfo = _getInfoWeatherRead(g_stProjectInfo.szProjectIniPath);
	int nGatherUse_YN = 0;
	nGatherUse_YN = atoi(stWeatherInfo.szLiveUSE_YN);

	CString strProcessorTitle = "";
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	strProcessorTitle.LoadString(IDS_MY_TREE_VIEW_1);

	if(nGatherUse_YN == 1)
	{
		if(m_bThreadStart == TRUE)
		{
			m_bThreadStart = FALSE;
			StartThread();
			GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 정지");

			_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"관리자에 의해 수집 시작");
			_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "상태 : [관리자에 의해 수집 시작]");
		}
		else
		{
			m_bThreadStart = TRUE;
			StopThread();
			GetDlgItem(IDC_BUTTON_START)->SetWindowText("수집 시작");
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

			_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"관리자에 의해 수집 정지");
			_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "상태 : [관리자에 의해 수집 정지]");
		}
	}
	else
	{
		AfxMessageBox("수집 미허용 상태입니다.");
		_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Weather Gather Use Check", USER_COLOR_RED, "환경설정 트리 '기상청연동설정' 에서 수집허용 설정이 가능합니다.");
	}
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 스레드 종료/수집 종료
-void StopThread()
*/
//////////////////////////////////////////////////////////////////////////
void CFormView_Weather::StopThread()
{
	if(m_pThreadWeather != NULL)
	{
		while (1)
		{
			if(m_pThreadWeather->GetStop() != TRUE)
				m_pThreadWeather->Stop();
			else
				break;
		}
		m_pThreadWeather = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 이벤트
- 사용자 이벤트 메시지
-LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam)
*/
//////////////////////////////////////////////////////////////////////////
LRESULT CFormView_Weather::OnUserMessage(WPARAM wParam, LPARAM lParam)
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

	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "DB log : [DB Access..]",rgbRet, sMsgBuffer);
	return 0;
}
