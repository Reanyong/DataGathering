// DLG_DataBaseSetting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_DataBaseSetting.h"
#include "afxdialogex.h"
#include "DLG_DataBaseNameSet.h"


#define WM_USER_MSG_DBSETTING	(WM_USER+2000)

// CDLG_DataBaseSetting 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_DataBaseSetting, CDialog)

CDLG_DataBaseSetting::CDLG_DataBaseSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_DataBaseSetting::IDD, pParent)
{

	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	DB_Connect = NULL;
}

CDLG_DataBaseSetting::~CDLG_DataBaseSetting()
{
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	CoUninitialize();
}

void CDLG_DataBaseSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_DataBaseSetting, CDialog)
	ON_BN_CLICKED(IDC_BT_CONNECTION_TEST, &CDLG_DataBaseSetting::OnBnClickedBtConnectionTest)
	ON_MESSAGE(WM_USER_MSG_DBSETTING,OnUserMsgDBSetting)
	ON_BN_CLICKED(IDC_RADIO_ORACLE, &CDLG_DataBaseSetting::OnBnClickedRadioOracle)
	ON_BN_CLICKED(IDC_RADIO_MSSQL, &CDLG_DataBaseSetting::OnBnClickedRadioMssql)
	ON_BN_CLICKED(IDC_BUTTON_DBNAME_SET, &CDLG_DataBaseSetting::OnBnClickedButtonDbnameSet)
	ON_BN_CLICKED(IDC_RADIO_POSTGRE, &CDLG_DataBaseSetting::OnBnClickedRadioPostgre)
END_MESSAGE_MAP()


// CDLG_DataBaseSetting 메시지 처리기입니다.

BOOL CDLG_DataBaseSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.


	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(0,0,"DB Setting","");

	ShowToolTip();
	ShowSettingData();

	/*CButton *btSubCheck = (CButton*)GetDlgItem(IDC_CHECK_USE_SUB);
	if(btSubCheck->GetCheck() == TRUE)
		ShowControlEnable_Sub(TRUE);
	else
		ShowControlEnable_Sub(FALSE);
		*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDLG_DataBaseSetting::OnBnClickedBtConnectionTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowControlEnable(FALSE);


	CString strServer,strDBName,strUserId,strUserPw;
	UINT unMSGID;
	CButton *btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_MSSQL);
	if(btRadioDBType->GetCheck())
		unMSGID = DB_MSSQL;

	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_ORACLE);
	if(btRadioDBType->GetCheck())
		unMSGID = DB_ORACLE;
	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_MYSQL);
	if(btRadioDBType->GetCheck())
		unMSGID = DB_MYSQL;

	//20200210 jhs : postgre 라디오 버튼 추가
	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_POSTGRE);
	if(btRadioDBType->GetCheck())
		unMSGID = DB_POSTGRE;

	GetDlgItem(IDC_EDIT_DB_SERVER)->GetWindowTextA(strServer);
	GetDlgItem(IDC_EDIT_DB_NAME)->GetWindowTextA(strDBName);
	GetDlgItem(IDC_EDIT_DB_USERID)->GetWindowTextA(strUserId);
	GetDlgItem(IDC_EDIT_DB_USERPW)->GetWindowTextA(strUserPw);

	DB_Connect->DB_ConnectionInfo(strServer,strDBName,strUserId,strUserPw,unMSGID);
	if(DB_Connect->DB_Connection() == TRUE)
		AfxMessageBox("접속 성공");
	else
		AfxMessageBox("접속 실패");

	DB_Connect->DB_Close();
	ShowControlEnable(TRUE);

	if(unMSGID == DB_ORACLE)
		GetDlgItem(IDC_EDIT_DB_SERVER)->EnableWindow(FALSE);
}



LRESULT CDLG_DataBaseSetting::OnUserMsgDBSetting(WPARAM wParam, LPARAM lParam)
{
	CString sMsgBuffer,strMsg;
	sMsgBuffer = (LPCSTR)(long)wParam;

	int nMsgType = 0;
	nMsgType = lParam;

	strMsg.Format("Control Class Message : %s", sMsgBuffer);
	AfxMessageBox(strMsg);

	return 0;
}

void CDLG_DataBaseSetting::ShowControlEnable(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_DB_SERVER)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_DB_NAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_DB_USERID)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_DB_USERPW)->EnableWindow(bEnable);
	GetDlgItem(IDC_BT_CONNECTION_TEST)->EnableWindow(bEnable);
}


void CDLG_DataBaseSetting::ShowSettingData()
{
	ST_DBINFO stDbInfo;

	CButton *btRadioMsSql = (CButton*)GetDlgItem(IDC_RADIO_MSSQL);
	CButton *btRadioOracle = (CButton*)GetDlgItem(IDC_RADIO_ORACLE);
	CButton *btRadioMySql = (CButton*)GetDlgItem(IDC_RADIO_MYSQL);
	CButton *btRadioPostgre = (CButton*)GetDlgItem(IDC_RADIO_POSTGRE); // 20200210 jsh -> postgre 라디오 버튼 설정
	stDbInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	switch(stDbInfo.unDBType)
	{
	case DB_MSSQL:
		btRadioMsSql->SetCheck(1);
		btRadioOracle->SetCheck(0);
		btRadioMySql->SetCheck(0);
		btRadioPostgre->SetCheck(0);

		GetDlgItem(IDC_STATIC_DB_SVERVER)->SetWindowText("DB Server : ");
		GetDlgItem(IDC_STATIC_DB_NAME)->SetWindowText("DB Name : ");
		break;
	case DB_ORACLE:
		btRadioMsSql->SetCheck(0);
		btRadioOracle->SetCheck(1);
		btRadioMySql->SetCheck(0);
		btRadioPostgre->SetCheck(0);

		GetDlgItem(IDC_STATIC_DB_SVERVER)->SetWindowText("DB Server : ");
		GetDlgItem(IDC_STATIC_DB_NAME)->SetWindowText("TnsNames : ");
		GetDlgItem(IDC_EDIT_DB_SERVER)->EnableWindow(FALSE);
		break;
	case DB_POSTGRE:
		btRadioMsSql->SetCheck(0);
		btRadioOracle->SetCheck(0);
		btRadioMySql->SetCheck(0);
		btRadioPostgre->SetCheck(1);

		GetDlgItem(IDC_STATIC_DB_SVERVER)->SetWindowText("DB Server : ");
		GetDlgItem(IDC_STATIC_DB_NAME)->SetWindowText("DB NAME : ");
		break;
	/*case DB_MYSQL:
		btRadioMsSql->SetCheck(0);
		btRadioOracle->SetCheck(0);
		btRadioMySql->SetCheck(1);
		break;*/
	/*default:
		btRadioMsSql->SetCheck(1);
		btRadioOracle->SetCheck(0);
		btRadioPostgre->SetCheck(0);
		break;*/
	}

	GetDlgItem(IDC_EDIT_DB_SERVER)->SetWindowText(stDbInfo.szServer);
	GetDlgItem(IDC_EDIT_DB_NAME)->SetWindowText(stDbInfo.szDB);
	GetDlgItem(IDC_EDIT_DB_USERID)->SetWindowText(stDbInfo.szID);
	GetDlgItem(IDC_EDIT_DB_USERPW)->SetWindowText(stDbInfo.szPW);

	//GetDlgItem(IDC_EDIT_HMI_DB_NAME)->SetWindowText(stDbInfo.szHMIDBName);
	//GetDlgItem(IDC_EDIT_WEATHER_DB_NAME)->SetWindowText(stDbInfo.szWTDBName);
	Invalidate(FALSE);
}

void CDLG_DataBaseSetting::SetDBInfoSetting()
{
	CString strDataBuff;
	ST_DBINFO stDbInfo;
	memset(&stDbInfo,0x00, sizeof(stDbInfo));

	CButton *btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_MSSQL);
	if(btRadioDBType->GetCheck())
		stDbInfo.unDBType = DB_MSSQL;
	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_ORACLE);
	if(btRadioDBType->GetCheck())
		stDbInfo.unDBType = DB_ORACLE;
	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_MYSQL);
	if(btRadioDBType->GetCheck())
		stDbInfo.unDBType = DB_MYSQL;

	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_POSTGRE);
	if(btRadioDBType->GetCheck())
		stDbInfo.unDBType = DB_POSTGRE;


	GetDlgItem(IDC_EDIT_DB_SERVER)->GetWindowText(strDataBuff);
	strcat_s(stDbInfo.szServer,strDataBuff);
	GetDlgItem(IDC_EDIT_DB_NAME)->GetWindowText(strDataBuff);
	strcat_s(stDbInfo.szDB,strDataBuff);
	GetDlgItem(IDC_EDIT_DB_USERID)->GetWindowText(strDataBuff);
	strcat_s(stDbInfo.szID,strDataBuff);
	GetDlgItem(IDC_EDIT_DB_USERPW)->GetWindowText(strDataBuff);
	strcat_s(stDbInfo.szPW,strDataBuff);

	//GetDlgItem(IDC_EDIT_HMI_DB_NAME)->GetWindowText(strDataBuff);
	//strcat_s(stDbInfo.szHMIDBName,strDataBuff);
	//GetDlgItem(IDC_EDIT_WEATHER_DB_NAME)->GetWindowText(strDataBuff);
	//strcat_s(stDbInfo.szWTDBName,strDataBuff);

	_setInfoDBWrite(&stDbInfo,g_stProjectInfo.szProjectIniPath);
}

BOOL CDLG_DataBaseSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_tooltop.RelayEvent(pMsg);
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDLG_DataBaseSetting::ShowToolTip()
{
	m_tooltop.Create(this);
	m_tooltop.AddTool(GetDlgItem(IDC_RADIO_MSSQL),"MS-SQL 사용");
	m_tooltop.AddTool(GetDlgItem(IDC_RADIO_ORACLE),"Oracle 사용시 Oracle 클라이언트 설치 필요");
	m_tooltop.AddTool(GetDlgItem(IDC_RADIO_MYSQL),"My SQL 사용");

	m_tooltop.AddTool(GetDlgItem(IDC_BT_CONNECTION_TEST),"메인 DB 접속 Test");

	m_tooltop.SetTipBkColor(RGB(255,255,0));
	m_tooltop.SetTipTextColor(RGB(255,0,0));
}

void CDLG_DataBaseSetting::OnBnClickedRadioMssql()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_STATIC_DB_SVERVER)->SetWindowText("DB Server : ");
	GetDlgItem(IDC_STATIC_DB_NAME)->SetWindowText("DB Name : ");
	GetDlgItem(IDC_EDIT_DB_SERVER)->EnableWindow(TRUE);
}

void CDLG_DataBaseSetting::OnBnClickedRadioOracle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_STATIC_DB_SVERVER)->SetWindowText("DB Server : ");
	GetDlgItem(IDC_STATIC_DB_NAME)->SetWindowText("TnsNames : ");

	GetDlgItem(IDC_EDIT_DB_SERVER)->EnableWindow(FALSE);
}

void CDLG_DataBaseSetting::OnBnClickedButtonDbnameSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDLG_DataBaseNameSet dlg;
	dlg.DoModal();
}

//20200210 jsh -> postgre 추가
void CDLG_DataBaseSetting::OnBnClickedRadioPostgre()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_STATIC_DB_SVERVER)->SetWindowText("DB Server : ");
	GetDlgItem(IDC_STATIC_DB_NAME)->SetWindowText("DB Name : ");
	GetDlgItem(IDC_EDIT_DB_SERVER)->EnableWindow(TRUE);
}
