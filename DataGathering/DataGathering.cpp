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

// DataGathering.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "DataGathering.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "DataGatheringDoc.h"
#include "DataGatheringView.h"
#include "FormView_Main.h"
#include "FormView_Weather.h"
#include "FormView_AlarmTag.h"
#include "FormView_AlarmFMS.h"
#include "FormView_IConnect.h"
#include "FormView_TAGGather.h"
#include "FormView_TagMapping.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDataGatheringApp

BEGIN_MESSAGE_MAP(CDataGatheringApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CDataGatheringApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	//ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	//ON_COMMAND(ID_USER_NEW_VIEW_OPEN, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CDataGatheringApp 생성

CDataGatheringApp::CDataGatheringApp()
{
	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DataGathering.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CDataGatheringApp 개체입니다.

CDataGatheringApp theApp;


// CDataGatheringApp 초기화

BOOL CDataGatheringApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	//실행중인 프로그램 검사
	BOOL bEventCheck = EventActionCheck(); //현재 실행중인 체크
	if(bEventCheck == TRUE)
		return FALSE;

	//테스트 할때만
	/*HANDLE hEvent;
	hEvent = CreateEvent(NULL,FALSE,TRUE,"EVDataGathering-D");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//AfxMessageBox("이미 실행중 입니다");
		return FALSE;
	}*/

	CWinAppEx::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	//////////////////////////////////////////////////////////////////////////
	//2017-03-24 false : 작업표시줄에 단일로 표시, true : 출력 멀티로 표시
	EnableTaskbarInteraction(FALSE); //작업 표시줄

	CoInitialize(NULL);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	//LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	LoadStdProfileSettings();  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_Main));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_TAGGather));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	/* test
	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_Weather));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_IConnect));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_TagMapping));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_AlarmFMS));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormView_AlarmTag));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	*/

	//2015-10 추가
	//도킹 정보 수정 사항 레지스트리에 정보 초기화 한다.
	CWinAppEx::CleanState();

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  MDI 응용 프로그램에서는 m_pMainWnd를 설정한 후 바로 이러한 호출이 발생해야 합니다.

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//2016-10 추가

	//Project 정보 Read
	BOOL bEVOpenCheck = GetEVInfoRead(cmdInfo.m_strFileName);
	if(bEVOpenCheck == FALSE)
		return FALSE;


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	//AfxMessageBox(cmdInfo.m_strFileName);
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();


	CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
	pMain->SetMenu(NULL);

	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "DataGathering - Start", USER_COLOR_BLACK, "프로그램 시작 하였습니다.");


	CWnd *pWnd = AfxGetMainWnd();
	pWnd->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_MAIN_VIEW); //

	return TRUE;
}

int CDataGatheringApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.

	AfxOleTerm(FALSE);

	DeleteCriticalSection(&g_cs); //동기화 종료

	return CWinAppEx::ExitInstance();
}

// CDataGatheringApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CDataGatheringApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDataGatheringApp 사용자 지정 로드/저장 메서드

void CDataGatheringApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CDataGatheringApp::LoadCustomState()
{
}

void CDataGatheringApp::SaveCustomState()
{
}

BOOL CDataGatheringApp::EventActionCheck()
{
	HANDLE hEvent;
	CString strCaptionName = "";

	hEvent = CreateEvent(NULL,FALSE,TRUE,PRO_TITLE_NAME);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//AfxMessageBox("이미 실행중 입니다");
		CWnd*  pWnd = NULL;
		for(int nCount = 0; nCount < 2 ; nCount++)
		{
			if(nCount == 0)
				strCaptionName.Format("%s - 메인 정보",PRO_TITLE_NAME);
			else if(nCount == 1)
				strCaptionName.Format("%s - ",PRO_TITLE_NAME);

			pWnd = CWnd::FindWindow(NULL, strCaptionName);
			if(pWnd != NULL)
			{
				if(pWnd->IsIconic())
					pWnd->ShowWindow(SW_RESTORE);
				else
					pWnd->SetForegroundWindow();

				return TRUE;
			}
		}
		return TRUE;
	}

	return FALSE;
}

// CDataGatheringApp 메시지 처리기
BOOL CDataGatheringApp::GetEVInfoRead(CString strFileName)
{
	InitializeCriticalSection(&g_cs); //동기화 초기화

	char path[256],buf[128];
	CString strMsg = "",strCreatePath;

	// <-- 20191230-Doo EasyEMS.ini도 적용되도록 수정
	//GetCurrentDirectory(sizeof(path),path);
	//strcpy_s(g_stProjectInfo.szProjectPath, path);
	//strcat_s(path, "\\EasyView.INI");
	EV_GetConfigFile(path);
	// -->

	GetPrivateProfileString("EasyView", "Project", "", buf, sizeof(buf), path);
	sprintf_s(g_stProjectInfo.szProjectName, buf);

	sprintf_s(g_stProjectInfo.szDTGatheringPath,"%s\\EVDataGathering",g_stProjectInfo.szProjectPath);
	sprintf_s(g_stProjectInfo.szProjectIniPath,"%s\\EVDataGathering\\%s.ini",g_stProjectInfo.szProjectPath,g_stProjectInfo.szProjectName);

	strCreatePath = _createFolder(g_stProjectInfo.szDTGatheringPath,"Log");
	sprintf_s(g_stProjectInfo.szProjectLogPath,"%s",strCreatePath);
	Sleep(50);
	strCreatePath = _createFolder(g_stProjectInfo.szDTGatheringPath,"Alarm");
	Sleep(50);
	strCreatePath = _createFolder(g_stProjectInfo.szDTGatheringPath,"Alarm\\FMS");
	sprintf_s(g_stProjectInfo.szProjectFMSLogPath,"%s",strCreatePath);
	Sleep(50);

	strCreatePath = _createFolder(g_stProjectInfo.szDTGatheringPath,"WeatherXmlLog");
	sprintf_s(g_stProjectInfo.szWeatherLivePath,"%s",strCreatePath);

	if(strlen(g_stProjectInfo.szProjectName) < 1)
	{
#ifdef _LANG_KOREAN
		MessageBox(NULL, EV_ERR_MSGK_0001002, EV_ERR_CODE_0001002, EV_ERR_TYPE_0001002 | MB_SYSTEMMODAL);
#else
		MessageBox(NULL, EV_ERR_MSGE_0001002, EV_ERR_CODE_0001002, EV_ERR_TYPE_0001002 | MB_SYSTEMMODAL);
#endif

		return FALSE;
	}

	int retVal = EV_OpenMem(g_stProjectInfo.szProjectName);
	if(retVal < 0)
	{
#ifdef _LANG_KOREAN
		MessageBox(NULL, EV_ERR_MSGK_0001002, EV_ERR_CODE_0001002, EV_ERR_TYPE_0001002 | MB_SYSTEMMODAL);
#else
		MessageBox(NULL, EV_ERR_MSGE_0001002, EV_ERR_CODE_0001002, EV_ERR_TYPE_0001002 | MB_SYSTEMMODAL);
#endif
		return FALSE;
	}
	else
	{
#ifdef _DEBUG
		TRACE( "프로젝트 : [%s] 공유 메모리 : [Open] 성공 : [CODE(%d)]", g_stProjectInfo.szProjectName, retVal );
#endif
		memset(buf,0x00,sizeof(buf));
		sprintf_s(buf, "GM_EVVIEW_END_%s", g_stProjectInfo.szProjectName);
		wm_EVViewStop = RegisterWindowMessage(buf);
	}

	sprintf_s(g_stProjectInfo.szDTGatheringLogPath,"%s",g_stProjectInfo.szDTGatheringPath);

	strMsg.Format("Start!");
	_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);

	strMsg.Format("프로젝트 : [%s] 공유 메모리 : [Open] 성공 : [CODE(%d)]",g_stProjectInfo.szProjectName, retVal);
	_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering Start", USER_COLOR_BLACK, strMsg);
	_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);

	_getDataGatheringInfo(g_stProjectInfo.szDTGatheringPath,g_stProjectInfo.szProjectName);

	return TRUE;
}


// CDataGatheringApp 메시지 처리기
BOOL CDataGatheringApp::SetInitialize(CString strFileName)
{
//	char path[256];
	CString strMsg = "";
	CString strCreatePath = "";

	//GetCurrentDirectory(sizeof(path),path);
	//sprintf_s(g_stProjectInfo.szProjectPath,"%s",path);

	/*&strCreatePath = _createFolder(strFileName,"SiteList");
	Sleep(50);
	strCreatePath = _createFolder(strFileName,"Alarm");
	Sleep(50);
	strCreatePath = _createFolder(strFileName,"WeatherXmlLog");
	Sleep(50);*/
	strCreatePath = _createFolder(strFileName,"Alarm\\Tag");
	Sleep(50);
	strCreatePath = _createFolder(strFileName,"Alarm");
	Sleep(50);
	strCreatePath = _createFolder(strFileName,"WeatherXmlLog\\Live");
	Sleep(50);
	sprintf_s(g_stProjectInfo.szWeatherLivePath,"%s",strCreatePath);
	/*strCreatePath = _createFolder(strFileName,"WeatherXmlLog\\Forecast");
	Sleep(50);
	sprintf_s(g_stProjectInfo.szWeatherForecastPath,"%s",strCreatePath);*/

#ifdef _DEBUG
	TRACE( "필요한 폴더 항목 체크 : [%s]", strCreatePath);
#endif
	strMsg.Format("위치 : [%s]",strCreatePath);
	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "Folder Check", USER_COLOR_BLACK, strMsg);

	//_getDataGatheringInfo(g_stProjectInfo.szProjectPath,PRO_TITLE_NAME);
	//sprintf_s(g_stProjectInfo.szProjectIniPath,"%s\\%s.ini",path,PRO_TITLE_NAME);

	//strMsg.Format("Start!");
	//_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);


	return TRUE;
}

void CDataGatheringApp::OnFileNew()
{
	CMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();

	for(int i = 0; i < g_nActivateDocCheck; i++)
	{
		pTemplate = (CMultiDocTemplate *)GetNextDocTemplate(pos);
	}
	if (g_nActivateDocCheck > 0)
		pTemplate->OpenDocumentFile(NULL);
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
