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

// DataGathering.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	//ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	//ON_COMMAND(ID_USER_NEW_VIEW_OPEN, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CDataGatheringApp ����

CDataGatheringApp::CDataGatheringApp()
{
	m_bHiColorIcons = TRUE;

	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DataGathering.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CDataGatheringApp ��ü�Դϴ�.

CDataGatheringApp theApp;


// CDataGatheringApp �ʱ�ȭ

BOOL CDataGatheringApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	//�������� ���α׷� �˻�
	BOOL bEventCheck = EventActionCheck(); //���� �������� üũ
	if(bEventCheck == TRUE)
		return FALSE;

	//�׽�Ʈ �Ҷ���
	/*HANDLE hEvent;
	hEvent = CreateEvent(NULL,FALSE,TRUE,"EVDataGathering-D");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//AfxMessageBox("�̹� ������ �Դϴ�");
		return FALSE;
	}*/

	CWinAppEx::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	//////////////////////////////////////////////////////////////////////////
	//2017-03-24 false : �۾�ǥ���ٿ� ���Ϸ� ǥ��, true : ��� ��Ƽ�� ǥ��
	EnableTaskbarInteraction(FALSE); //�۾� ǥ����

	CoInitialize(NULL);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	//LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.
	LoadStdProfileSettings();  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_Main));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_TAGGather));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	/* test
	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_Weather));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_IConnect));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_TagMapping));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_AlarmFMS));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_DATAGATHERINGTYPE,
		RUNTIME_CLASS(CDataGatheringDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormView_AlarmTag));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	*/

	//2015-10 �߰�
	//��ŷ ���� ���� ���� ������Ʈ���� ���� �ʱ�ȭ �Ѵ�.
	CWinAppEx::CleanState();

	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	//  MDI ���� ���α׷������� m_pMainWnd�� ������ �� �ٷ� �̷��� ȣ���� �߻��ؾ� �մϴ�.

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//2016-10 �߰�

	//Project ���� Read
	BOOL bEVOpenCheck = GetEVInfoRead(cmdInfo.m_strFileName);
	if(bEVOpenCheck == FALSE)
		return FALSE;


	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	//AfxMessageBox(cmdInfo.m_strFileName);
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();


	CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
	pMain->SetMenu(NULL);

	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "DataGathering - Start", USER_COLOR_BLACK, "���α׷� ���� �Ͽ����ϴ�.");


	CWnd *pWnd = AfxGetMainWnd();
	pWnd->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_MAIN_VIEW); //

	return TRUE;
}

int CDataGatheringApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.

	AfxOleTerm(FALSE);

	DeleteCriticalSection(&g_cs); //����ȭ ����

	return CWinAppEx::ExitInstance();
}

// CDataGatheringApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CDataGatheringApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDataGatheringApp ����� ���� �ε�/���� �޼���

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
		//AfxMessageBox("�̹� ������ �Դϴ�");
		CWnd*  pWnd = NULL;
		for(int nCount = 0; nCount < 2 ; nCount++)
		{
			if(nCount == 0)
				strCaptionName.Format("%s - ���� ����",PRO_TITLE_NAME);
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

// CDataGatheringApp �޽��� ó����
BOOL CDataGatheringApp::GetEVInfoRead(CString strFileName)
{
	InitializeCriticalSection(&g_cs); //����ȭ �ʱ�ȭ

	char path[256],buf[128];
	CString strMsg = "",strCreatePath;

	// <-- 20191230-Doo EasyEMS.ini�� ����ǵ��� ����
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
		TRACE( "������Ʈ : [%s] ���� �޸� : [Open] ���� : [CODE(%d)]", g_stProjectInfo.szProjectName, retVal );
#endif
		memset(buf,0x00,sizeof(buf));
		sprintf_s(buf, "GM_EVVIEW_END_%s", g_stProjectInfo.szProjectName);
		wm_EVViewStop = RegisterWindowMessage(buf);
	}

	sprintf_s(g_stProjectInfo.szDTGatheringLogPath,"%s",g_stProjectInfo.szDTGatheringPath);

	strMsg.Format("Start!");
	_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);

	strMsg.Format("������Ʈ : [%s] ���� �޸� : [Open] ���� : [CODE(%d)]",g_stProjectInfo.szProjectName, retVal);
	_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering Start", USER_COLOR_BLACK, strMsg);
	_systemLog(strMsg,g_stProjectInfo.szProjectLogPath);

	_getDataGatheringInfo(g_stProjectInfo.szDTGatheringPath,g_stProjectInfo.szProjectName);

	return TRUE;
}


// CDataGatheringApp �޽��� ó����
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
	TRACE( "�ʿ��� ���� �׸� üũ : [%s]", strCreatePath);
#endif
	strMsg.Format("��ġ : [%s]",strCreatePath);
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnOK();
}
