// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DataGatheringDoc.h"

#include "MainFrm.h"
/*#include "DLG_DataBaseSetting.h"
#include "DLG_ProgramSetting.h"
#include "DLG_GatherInfoSetting.h"
#include "DLG_WeatherSeting.h"
#include "DLG_ISmartSetting.h"*/
#include "DLG_SettingMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_USER_OPEN_FRAME, OnOpenChildFrame)
	ON_REGISTERED_MESSAGE(wm_EVViewStop, OnEasyViewStop)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND(ID_BT_LOGFOLDER_OPEN, &CMainFrame::OnBtLogfolderOpen)
	ON_COMMAND(ID_BT_LOGVIEW, &CMainFrame::OnBtLogview)
	ON_COMMAND(ID_BT_LOG_ROOT, &CMainFrame::OnBtLogRoot)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	//theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	theApp.m_nAppLook = ID_VIEW_APPLOOK_OFF_2007_AQUA;
	m_bEndCheck = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = FALSE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons =  TRUE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = FALSE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = FALSE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	mdiTabParams.m_bTabCloseButton = FALSE; //닫기 버튼 비활성화
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON_USER);
	//m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	//하단 상태표시줄 사용시 주석 해제
	/*
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}*/

	//하단 상태 표시줄
	/*CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);*/

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	//여기 12/30
	//CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	//여기 12/30
	//EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 탐색 창이 왼쪽에 만들어지므로 일시적으로 왼쪽에 도킹을 비활성화합니다.
	//여기 12/30
	//EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);


	// 캡션 표시줄을 만듭니다.
	/*if (!CreateCaptionBar())
	{
		TRACE0("캡션 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}*/

	// Outlook 표시줄이 만들어지고 왼쪽에 도킹할 수 있도록 허용해야 합니다.
	//여기 12/30
	//EnableDocking(CBRS_ALIGN_LEFT);
	//EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	DWORD dwStyle_Output = m_wndOutput.GetControlBarStyle();
	dwStyle_Output &=~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT);
	m_wndOutput.SetControlBarStyle(dwStyle_Output);

	m_wndTree.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndTree);

	DWORD dwStyle_Tree = m_wndTree.GetControlBarStyle();
	dwStyle_Tree &=~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT);
	m_wndTree.SetControlBarStyle(dwStyle_Tree);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	//여기 12/30
	//EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 창 제목 표시줄에서 문서 이름 및 응용 프로그램 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	//여기 12/30
	//ModifyStyle(0, FWS_PREFIXTITLE);
	//ModifyStyle(WS_CAPTION, NULL); // 타이틀바 없애기

	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED); //--> 윈도우가 생성되고 완료되고 난 뒤에

	SetTimer(2, 100, NULL);

	//SetTimer(5, 10000, NULL);

	return 0;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 클래스 뷰를 만듭니다.
/*	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 파일 뷰를 만듭니다.
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("파일 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}*/
	//ID_VIEW_TREE_WND        "메뉴 항목"
	// 트리 리스트 창을 만듭니다.
	CString strTreeWnd;
	bNameValid = strTreeWnd.LoadString(ID_MY_VIEW_TREE_WND);
	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE;
	DWORD dwPaneTreeStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBRS_LEFT | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;
	ASSERT(bNameValid);
	if (!m_wndTree.Create(strTreeWnd, this, CRect(0, 0, 210, 120), FALSE, ID_MY_VIEW_TREE_WND, dwPaneTreeStyle, AFX_CBRS_REGULAR_TABS /*| WS_THICKFRAME*/))
	//if (!m_wndTree.Create(strTreeWnd, this, CRect(0, 0, 210, 120), FALSE, ID_MY_VIEW_TREE_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI |WS_DLGFRAME  ,dwNoCloseBarStyle/*| WS_THICKFRAME*/))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	DWORD dwPaneOutputStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBRS_BOTTOM | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(ID_MY_VIEW_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 500, 270), FALSE, ID_MY_VIEW_OUTPUT_WND, dwPaneOutputStyle,AFX_CBRS_REGULAR_TABS /*| WS_THICKFRAME*/))
	//if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 500, 270), TRUE, ID_MY_VIEW_OUTPUT_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI  |WS_DLGFRAME /*| WS_THICKFRAME*/))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	//m_wndOutput.IsResizable() = FALSE;
	// 속성 창을 만듭니다.
	/*CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}*/

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	/*HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);
	*/
	//HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

//	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
//	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	//UpdateMDITabbedBarsIcons();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.x = 5;
	cs.y = 5;

	cs.cx = USER_WINDOWS_SIZE_CX;
	cs.cy = USER_WINDOWS_SIZE_CY;

	//cs.style &= ~WS_MAXIMIZEBOX;
	//cs.style &= ~( WS_BORDER);
	//cs.style &= ~WS_MAXIMIZEBOX;

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("캡션 표시줄을 만들지 못했습니다.\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	//pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	//m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	lpMMI->ptMinTrackSize.x = USER_WINDOWS_SIZE_CX;
	lpMMI->ptMinTrackSize.y = USER_WINDOWS_SIZE_CY;
	lpMMI->ptMaxTrackSize.x = USER_WINDOWS_SIZE_CX;
	lpMMI->ptMaxTrackSize.y = USER_WINDOWS_SIZE_CY;

	CMDIFrameWndEx::OnGetMinMaxInfo(lpMMI);
}


//20200227	나정호 수정	현재 사용하지 않는 기능 보이지 않도록 수정
LRESULT CMainFrame::OnOpenChildFrame(WPARAM wParam, LPARAM lParam)
{
	CString strMsgTital, strMsgContent;

	CMDIChildWnd *pChildWnd;
	CString szDocTitle;
	/*CDLG_DataBaseSetting dlgDBSetting;
	CDLG_ProgramSetting dlgProgramSetting;
	CDLG_GatherInfoSetting dlgGatherInfoSetting;
	CDLG_WeatherSeting		dlgWeatherSetting;*/
	CDLG_SettingMain      dlgSettingMain;

	int nActivateView = 0;
	switch (lParam)
	{
	case IDS_MY_TREE_PROGRAM_SETTING_1:
		strMsgTital.LoadString(IDS_MY_TREE_PROGRAM_SETTING_1);
		strMsgTital += " 설정 Open";
		_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, strMsgTital);
		//dlgProgramSetting.DoModal();
		ShowDlgPopUp(&dlgSettingMain);
		return 0;
/*	case IDS_MY_TREE_PROGRAM_SETTING_4:
		strMsgTital.LoadString(IDS_MY_TREE_PROGRAM_SETTING_4);
		strMsgTital += ": [Open]";
		_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, strMsgTital);

		ShowDlgPopUp(&dlgGatherInfoSetting);
		return 0;//IDS_MY_TREE_GATHERING_TYPE_SETTING

	case IDS_MY_TREE_PROGRAM_SETTING_2:
		strMsgTital.LoadString(IDS_MY_TREE_PROGRAM_SETTING_2);
		strMsgTital += ": [Open]";
		_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, strMsgTital);

		ShowDlgPopUp(&dlgDBSetting);
		return 0;
	case IDS_MY_TREE_PROGRAM_SETTING_5:
		strMsgTital.LoadString(IDS_MY_TREE_PROGRAM_SETTING_5);
		strMsgTital += ": [Open]";
		_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, strMsgTital);

		ShowDlgPopUp(&dlgWeatherSetting);
		return 0;*/
	case IDS_MY_TREE_MAIN_VIEW:
		nActivateView = FORM_VIEW_ID_1;
		szDocTitle.LoadString(IDS_MY_TREE_MAIN_VIEW); //메인
		break;
	case IDS_MY_TREE_VIEW_1:
		nActivateView = FORM_VIEW_ID_2;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_1); //데이터 게더링
		break;
				/*
	case IDS_MY_TREE_VIEW_2:
		nActivateView = FORM_VIEW_ID_3;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_2); //날씨
		break;

	case IDS_MY_TREE_VIEW_4:
		nActivateView = FORM_VIEW_ID_4;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_4); //한전데이터 수집
		break;
	case IDS_MY_TREE_VIEW_5:
		nActivateView = FORM_VIEW_ID_5;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_5); //에너지관리공단 데이터 제공
		break;
	case IDS_MY_TREE_VIEW_6:
		nActivateView = FORM_VIEW_ID_6;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_6); //설비알람
		break;
	case IDS_MY_TREE_VIEW_7:
		nActivateView = FORM_VIEW_ID_7;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_7); //TAG알람
		break;
		*/
	}

	g_nActivateDocCheck = nActivateView;
	//해당윈도우 가 활성화 됫는지 여부
	pChildWnd = FindMDIChildWnd(szDocTitle, nActivateView);

	if (pChildWnd)
	{
		pChildWnd->MDIActivate();
#ifdef _DEBUG
		TRACE("'%s' Open 상태\n",szDocTitle);
#endif
		return 0;
		//return;
	}

#ifdef _DEBUG
	TRACE("'%s' 화면 전환\n",szDocTitle);
#endif

	PostMessage(WM_COMMAND, ID_FILE_NEW);
	return 0;
}

void CMainFrame::ShowDlgPopUp(CDialog *dlg)
{
	dlg->DoModal();

	dlg = NULL;
}

CMDIChildWnd *CMainFrame::FindMDIChildWnd(const char *szTitle, int nActivateDoc)
{
	CDocTemplate *pDocTemplate;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();

	if (pos)
	{
		for (int i = 0; i < nActivateDoc; i++)
			pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
	}
	else
		return NULL;

	pos = pDocTemplate->GetFirstDocPosition();

	while(pos != NULL)
	{
		CDataGatheringDoc *pDoc = (CDataGatheringDoc *)pDocTemplate->GetNextDoc(pos);
		if (strcmp(szTitle, pDoc->GetTitle()) == 0)
		{
			CView *pView;
			POSITION pos1 = pDoc->GetFirstViewPosition();
			if (pos1)
				pView = pDoc->GetNextView(pos1);
			else
				return NULL;
			return (CMDIChildWnd *)pView->GetParentFrame();
			break;
		}
	}

	return NULL;
}

LRESULT CMainFrame::OnEasyViewStop(UINT wParm, LONG lParm)
{
	m_bEndCheck = TRUE;
	//_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath,"","engine dead....\r\n");
	CString strMsg = "";
	strMsg.Format("End by the engine!");
	_systemLog(strMsg,g_stProjectInfo.szDTGatheringLogPath);

	PostMessage(WM_CLOSE);

	TRACE("engine dead....\n");
	return 0;
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CString strMsg = "";
	if(m_bEndCheck == FALSE)
	{
		_addSystemMsg(0, RGB(255, 0, 255), JT_INTERLOCK_CODE_01, RGB(255, 0, 255), JT_INTERLOCK_MSG_01);
		if(MessageBox(JT_INTERLOCK_MSG_01, JT_INTERLOCK_CODE_01, MB_YESNO)== IDNO)
			return;

		strMsg.Format("End by the user!");
		_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);
	}
	else
	{
		strMsg.Format("End by the engine!");
		_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);
	}

	//WriteLog("프로그램이 종료되었습니다.");
	CMDIFrameWnd::OnClose();
}

void CMainFrame::MsgWork()
{
	CSysMsg msg;

	if (_getSystemMsg(&msg) < 1)
		return;

	switch(msg.m_nIsOutput)
	{
	case FORM_VIEW_ID_SYSTEM:
		m_wndOutput.InsertMessage(FORM_VIEW_ID_SYSTEM, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
			break;
	//case LOG_MESSAGE_1:
	//	m_wndOutput.InsertMessage(LOG_MESSAGE_1, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
	//	break;
	case LOG_MESSAGE_2:
		m_wndOutput.InsertMessage(LOG_MESSAGE_2, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	case LOG_MESSAGE_3:
		m_wndOutput.InsertMessage(LOG_MESSAGE_3, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	case LOG_MESSAGE_4:
		m_wndOutput.InsertMessage(LOG_MESSAGE_4, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	case LOG_MESSAGE_5:
		m_wndOutput.InsertMessage(LOG_MESSAGE_5, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	case LOG_MESSAGE_6:
		m_wndOutput.InsertMessage(LOG_MESSAGE_6, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	case LOG_MESSAGE_7:
		m_wndOutput.InsertMessage(LOG_MESSAGE_7, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	default:
		m_wndOutput.InsertMessage(FORM_VIEW_ID_SYSTEM, msg.m_crTitle, msg.m_sTitle, msg.m_crBody, msg.m_sBody);
		break;
	}
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		return;
	}
	if(nIDEvent ==2 )
	{
		MsgWork();
		return;
	}
	if(nIDEvent == 5)
	{
		FlashWindow(TRUE); //작업표시줄 깜박이게하는 기능
		KillTimer(nIDEvent);
		SetTimer(5,5000,NULL);
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnBtLogfolderOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Log", USER_COLOR_LIME, "Log Folder Open.");
}


void CMainFrame::OnBtLogview()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Log", USER_COLOR_LIME, "Log View View Open.");
}


void CMainFrame::OnBtLogRoot()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}
