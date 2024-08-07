// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")��
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե�
// Microsoft Foundation Classes Reference �� ���� ���� ������ ����
// �߰������� �����Ǵ� �����Դϴ�.
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������
// http://msdn.microsoft.com/officeui�� �����Ͻʽÿ�.
//
// Copyright (C) Microsoft Corporation
// ��� �Ǹ� ����.

// MainFrm.cpp : CMainFrame Ŭ������ ����
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

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
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

	// ������ ���� ���� ���־� ������ �� ��Ÿ���� �����մϴ�.
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D; // ����� �� �ִ� �ٸ� ��Ÿ��...
	mdiTabParams.m_bActiveTabCloseButton = FALSE;      // FALSE�� �����Ͽ� �� ���� �����ʿ� �ݱ� ���߸� ��ġ�մϴ�.
	mdiTabParams.m_bTabIcons =  TRUE;    // TRUE�� �����Ͽ� MDI ���� ���� �������� Ȱ��ȭ�մϴ�.
	mdiTabParams.m_bAutoColor = FALSE;    // FALSE�� �����Ͽ� MDI ���� �ڵ� �� ������ ��Ȱ��ȭ�մϴ�.
	mdiTabParams.m_bDocumentMenu = FALSE; // �� ������ ������ �����ڸ��� ���� �޴��� Ȱ��ȭ�մϴ�.
	mdiTabParams.m_bTabCloseButton = FALSE; //�ݱ� ��ư ��Ȱ��ȭ
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON_USER);
	//m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	//�ϴ� ����ǥ���� ���� �ּ� ����
	/*
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}*/

	//�ϴ� ���� ǥ����
	/*CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);*/

	// Visual Studio 2005 ��Ÿ�� ��ŷ â ������ Ȱ��ȭ�մϴ�.
	//���� 12/30
	//CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 ��Ÿ�� ��ŷ â �ڵ� ���� ������ Ȱ��ȭ�մϴ�.
	//���� 12/30
	//EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Ž�� â�� ���ʿ� ��������Ƿ� �Ͻ������� ���ʿ� ��ŷ�� ��Ȱ��ȭ�մϴ�.
	//���� 12/30
	//EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);


	// ĸ�� ǥ������ ����ϴ�.
	/*if (!CreateCaptionBar())
	{
		TRACE0("ĸ�� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}*/

	// Outlook ǥ������ ��������� ���ʿ� ��ŷ�� �� �ֵ��� ����ؾ� �մϴ�.
	//���� 12/30
	//EnableDocking(CBRS_ALIGN_LEFT);
	//EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// ��ŷ â�� ����ϴ�.
	if (!CreateDockingWindows())
	{
		TRACE0("��ŷ â�� ������ ���߽��ϴ�.\n");
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

	// ���� â ���� ��ȭ ���ڸ� Ȱ��ȭ�մϴ�.
	//���� 12/30
	//EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// â ���� ǥ���ٿ��� ���� �̸� �� ���� ���α׷� �̸��� ������ ��ȯ�մϴ�.
	// ���� �̸��� ����� �׸��� �Բ� ǥ�õǹǷ� �۾� ǥ������ ��ɼ��� �����˴ϴ�.
	//���� 12/30
	//ModifyStyle(0, FWS_PREFIXTITLE);
	//ModifyStyle(WS_CAPTION, NULL); // Ÿ��Ʋ�� ���ֱ�

	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED); //--> �����찡 �����ǰ� �Ϸ�ǰ� �� �ڿ�

	SetTimer(2, 100, NULL);

	//SetTimer(5, 10000, NULL);

	return 0;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Ŭ���� �並 ����ϴ�.
/*	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Ŭ���� �� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}

	// ���� �並 ����ϴ�.
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("���� �� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}*/
	//ID_VIEW_TREE_WND        "�޴� �׸�"
	// Ʈ�� ����Ʈ â�� ����ϴ�.
	CString strTreeWnd;
	bNameValid = strTreeWnd.LoadString(ID_MY_VIEW_TREE_WND);
	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE;
	DWORD dwPaneTreeStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBRS_LEFT | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;
	ASSERT(bNameValid);
	if (!m_wndTree.Create(strTreeWnd, this, CRect(0, 0, 210, 120), FALSE, ID_MY_VIEW_TREE_WND, dwPaneTreeStyle, AFX_CBRS_REGULAR_TABS /*| WS_THICKFRAME*/))
	//if (!m_wndTree.Create(strTreeWnd, this, CRect(0, 0, 210, 120), FALSE, ID_MY_VIEW_TREE_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI |WS_DLGFRAME  ,dwNoCloseBarStyle/*| WS_THICKFRAME*/))
	{
		TRACE0("��� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}

	// ��� â�� ����ϴ�.
	DWORD dwPaneOutputStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBRS_BOTTOM | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(ID_MY_VIEW_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 500, 270), FALSE, ID_MY_VIEW_OUTPUT_WND, dwPaneOutputStyle,AFX_CBRS_REGULAR_TABS /*| WS_THICKFRAME*/))
	//if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 500, 270), TRUE, ID_MY_VIEW_OUTPUT_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI  |WS_DLGFRAME /*| WS_THICKFRAME*/))
	{
		TRACE0("��� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}
	//m_wndOutput.IsResizable() = FALSE;
	// �Ӽ� â�� ����ϴ�.
	/*CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("�Ӽ� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

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
		TRACE0("ĸ�� ǥ������ ������ ���߽��ϴ�.\n");
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

// CMainFrame ����

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


// CMainFrame �޽��� ó����

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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	lpMMI->ptMinTrackSize.x = USER_WINDOWS_SIZE_CX;
	lpMMI->ptMinTrackSize.y = USER_WINDOWS_SIZE_CY;
	lpMMI->ptMaxTrackSize.x = USER_WINDOWS_SIZE_CX;
	lpMMI->ptMaxTrackSize.y = USER_WINDOWS_SIZE_CY;

	CMDIFrameWndEx::OnGetMinMaxInfo(lpMMI);
}


//20200227	����ȣ ����	���� ������� �ʴ� ��� ������ �ʵ��� ����
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
		strMsgTital += " ���� Open";
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
		szDocTitle.LoadString(IDS_MY_TREE_MAIN_VIEW); //����
		break;
	case IDS_MY_TREE_VIEW_1:
		nActivateView = FORM_VIEW_ID_2;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_1); //������ �Դ���
		break;
				/*
	case IDS_MY_TREE_VIEW_2:
		nActivateView = FORM_VIEW_ID_3;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_2); //����
		break;

	case IDS_MY_TREE_VIEW_4:
		nActivateView = FORM_VIEW_ID_4;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_4); //���������� ����
		break;
	case IDS_MY_TREE_VIEW_5:
		nActivateView = FORM_VIEW_ID_5;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_5); //�������������� ������ ����
		break;
	case IDS_MY_TREE_VIEW_6:
		nActivateView = FORM_VIEW_ID_6;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_6); //����˶�
		break;
	case IDS_MY_TREE_VIEW_7:
		nActivateView = FORM_VIEW_ID_7;
		szDocTitle.LoadString(IDS_MY_TREE_VIEW_7); //TAG�˶�
		break;
		*/
	}

	g_nActivateDocCheck = nActivateView;
	//�ش������� �� Ȱ��ȭ �̴��� ����
	pChildWnd = FindMDIChildWnd(szDocTitle, nActivateView);

	if (pChildWnd)
	{
		pChildWnd->MDIActivate();
#ifdef _DEBUG
		TRACE("'%s' Open ����\n",szDocTitle);
#endif
		return 0;
		//return;
	}

#ifdef _DEBUG
	TRACE("'%s' ȭ�� ��ȯ\n",szDocTitle);
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

	//WriteLog("���α׷��� ����Ǿ����ϴ�.");
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
		FlashWindow(TRUE); //�۾�ǥ���� �����̰��ϴ� ���
		KillTimer(nIDEvent);
		SetTimer(5,5000,NULL);
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnBtLogfolderOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Log", USER_COLOR_LIME, "Log Folder Open.");
}


void CMainFrame::OnBtLogview()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Log", USER_COLOR_LIME, "Log View View Open.");
}


void CMainFrame::OnBtLogRoot()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}
