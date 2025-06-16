// DLG_SettingMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_SettingMain.h"
#include "afxdialogex.h"
#include "DLG_TagMapping.h"



// CDLG_SettingMain 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_SettingMain, CDialog)

CDLG_SettingMain::CDLG_SettingMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_SettingMain::IDD, pParent)
{
	m_pDlgProgramSetting = NULL;
	m_pDlgDataBaseSetting = NULL;
	m_pDlgGatherTypeSetting = NULL;
}

CDLG_SettingMain::~CDLG_SettingMain()
{
}

void CDLG_SettingMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SETING_GATHER, m_ButttonGatherSet);
	DDX_Control(pDX, IDC_BUTTON_SETING_DB, m_ButttonDBSet);
	DDX_Control(pDX, IDC_BUTTON_SETING_GATHERTYPE_SET, m_ButttonGatherTypeSet);
}


BEGIN_MESSAGE_MAP(CDLG_SettingMain, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SETING_GATHER, &CDLG_SettingMain::OnBnClickedButtonSetingGather)
	ON_BN_CLICKED(IDC_BUTTON_SETING_DB, &CDLG_SettingMain::OnBnClickedButtonSetingDb)
	ON_BN_CLICKED(IDOK, &CDLG_SettingMain::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SETING_GATHERTYPE_SET, &CDLG_SettingMain::OnBnClickedButtonSetingGathertypeSet)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDLG_SettingMain 메시지 처리기입니다.


BOOL CDLG_SettingMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ButttonGatherSet.LoadBitmaps(IDB_BMP_BUTTON_GATHER_N,IDB_BMP_BUTTON_GATHER_D,IDB_BMP_BUTTON_GATHER_D,IDB_BMP_BUTTON_GATHER_D);
	m_ButttonGatherSet.SizeToContent();

	m_ButttonDBSet.LoadBitmaps(IDB_BMP_BUTTON_DB_N,IDB_BMP_BUTTON_DB_D,IDB_BMP_BUTTON_DB_D,NULL);
	m_ButttonDBSet.SizeToContent();

	m_ButttonGatherTypeSet.LoadBitmaps(IDB_BMP_BUTTON_TYPE_N,IDB_BMP_BUTTON_TYPE_D,IDB_BMP_BUTTON_TYPE_D,NULL);
	m_ButttonGatherTypeSet.SizeToContent();

	CRect rtClient,rtWin;
	GetClientRect(&rtWin);

	m_pDlgProgramSetting = new CDLG_ProgramSetting;
	m_pDlgProgramSetting->Create(IDD_DLG_PROGRAMSETTING, this);
	m_pDlgProgramSetting->GetClientRect(&rtClient);
	m_pDlgProgramSetting->MoveWindow(rtWin.left + 155,rtWin.top + 5, rtClient.Width(), rtClient.Height());
	m_pDlgProgramSetting->ShowWindow(SW_SHOW);


	m_pDlgDataBaseSetting = new CDLG_DataBaseSetting;
	m_pDlgDataBaseSetting->Create(IDD_DLG_DBSETTING_SAVE, this);
	m_pDlgDataBaseSetting->GetClientRect(&rtClient);
	m_pDlgDataBaseSetting->MoveWindow(rtWin.left + 155,rtWin.top + 5, rtClient.Width(), rtClient.Height());
	m_pDlgDataBaseSetting->ShowWindow(SW_HIDE);

	m_pDlgGatherTypeSetting = new CDLG_GatherInfoSetting;
	m_pDlgGatherTypeSetting->Create(IDD_DLG_GATHERINFO_SETTING, this);
	m_pDlgGatherTypeSetting->GetClientRect(&rtClient);
	m_pDlgGatherTypeSetting->MoveWindow(rtWin.left + 155,rtWin.top + 5, rtClient.Width(), rtClient.Height());
	m_pDlgGatherTypeSetting->ShowWindow(SW_HIDE);
	//*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDLG_SettingMain::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CDLG_SettingMain::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_pDlgProgramSetting)
	{
		m_pDlgProgramSetting->DestroyWindow();
		delete m_pDlgProgramSetting;
		m_pDlgProgramSetting = NULL;
	}
	if (m_pDlgDataBaseSetting)
	{
		m_pDlgDataBaseSetting->DestroyWindow();
		delete m_pDlgDataBaseSetting;
		m_pDlgDataBaseSetting = NULL;
	}

	if (m_pDlgGatherTypeSetting)
	{
		m_pDlgGatherTypeSetting->DestroyWindow();
		delete m_pDlgGatherTypeSetting;
		m_pDlgGatherTypeSetting = NULL;
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDLG_SettingMain::OnBnClickedButtonSetingGather()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgProgramSetting->ShowWindow(SW_SHOW);
	m_pDlgDataBaseSetting->ShowWindow(SW_HIDE);
	m_pDlgGatherTypeSetting->ShowWindow(SW_HIDE);
}


void CDLG_SettingMain::OnBnClickedButtonSetingDb()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgProgramSetting->ShowWindow(SW_HIDE);
	m_pDlgDataBaseSetting->ShowWindow(SW_SHOW);
	m_pDlgGatherTypeSetting->ShowWindow(SW_HIDE);
}

void CDLG_SettingMain::OnBnClickedButtonSetingGathertypeSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgProgramSetting->ShowWindow(SW_HIDE);
	m_pDlgDataBaseSetting->ShowWindow(SW_HIDE);
	m_pDlgGatherTypeSetting->ShowWindow(SW_SHOW);
}

void CDLG_SettingMain::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgProgramSetting->SetSettingSave();
	m_pDlgDataBaseSetting->SetSettingSave();
	m_pDlgGatherTypeSetting->SetSettingSave();
	CDialog::OnOK();
}

void CDLG_SettingMain::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnClose();
}
