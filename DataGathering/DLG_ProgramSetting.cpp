// DLG_ProgramSetting.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_ProgramSetting.h"
#include "afxdialogex.h"
#include "DLG_WeatherSeting.h"
#include "DLG_EnergyTag_InitialSet.h"
#include "DLG_ISmarAccesstSetting.h"


// CDLG_ProgramSetting ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDLG_ProgramSetting, CDialogEx)

CDLG_ProgramSetting::CDLG_ProgramSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDLG_ProgramSetting::IDD, pParent)
{
	m_pThreadCalibration = NULL;
}

CDLG_ProgramSetting::~CDLG_ProgramSetting()
{
	StopThreadCalibration();
}

void CDLG_ProgramSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GATHER_INTERVAL, m_Combo_GatherInterval);
	DDX_Control(pDX, IDC_COMBO_DELETE_INTERVAL, m_Combo_DeleteInterval);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_mccCalander);
}


BEGIN_MESSAGE_MAP(CDLG_ProgramSetting, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_WEATHER_DIRECT, &CDLG_ProgramSetting::OnBnClickedRadioWeatherDirect)
	ON_BN_CLICKED(IDC_RADIO_WEATHER_WEB, &CDLG_ProgramSetting::OnBnClickedRadioWeatherWeb)
	ON_BN_CLICKED(IDC_RADIO_WEATHER_USE_N, &CDLG_ProgramSetting::OnBnClickedRadioWeatherUseN)
	ON_BN_CLICKED(IDC_RADIO_ISMART_DIRECT, &CDLG_ProgramSetting::OnBnClickedRadioIsmartDirect)
	ON_BN_CLICKED(IDC_RADIO_ISMART_WEB, &CDLG_ProgramSetting::OnBnClickedRadioIsmartWeb)
	ON_BN_CLICKED(IDC_RADIO_ISMART_USE_N, &CDLG_ProgramSetting::OnBnClickedRadioIsmartUseN)
	ON_BN_CLICKED(IDC_RADIO_ENGMNG_DIRECT, &CDLG_ProgramSetting::OnBnClickedRadioEngmngDirect)
	ON_BN_CLICKED(IDC_RADIO_ENGMNG_WEB, &CDLG_ProgramSetting::OnBnClickedRadioEngmngWeb)
	ON_BN_CLICKED(IDC_RADIO_ENGMNG_USE_N, &CDLG_ProgramSetting::OnBnClickedRadioEngmngUseN)
	ON_BN_CLICKED(IDC_BUTTON_WEATHER_SET, &CDLG_ProgramSetting::OnBnClickedButtonWeatherSet)
	ON_BN_CLICKED(IDC_BUTTON_ISMART_SET, &CDLG_ProgramSetting::OnBnClickedButtonIsmartSet)
	ON_BN_CLICKED(IDC_BUTTON_ENGMNG_SET, &CDLG_ProgramSetting::OnBnClickedButtonEngmngSet)
	ON_BN_CLICKED(IDC_RADIO_AUTORUN, &CDLG_ProgramSetting::OnBnClickedRadioAutorun)
	ON_BN_CLICKED(IDC_RADIO_USERURN, &CDLG_ProgramSetting::OnBnClickedRadioUserurn)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATION, &CDLG_ProgramSetting::OnBnClickedButtonCalibration)
	ON_MESSAGE(WM_ADDLIST, OnAddList)
	ON_MESSAGE(WM_ENABLECONTROL, OnEnableControl)
	ON_BN_CLICKED(IDC_RADIO_BEMS, &CDLG_ProgramSetting::OnBnClickedRadioBems)
	ON_BN_CLICKED(IDC_RADIO_EMS, &CDLG_ProgramSetting::OnBnClickedRadioEms)
END_MESSAGE_MAP()


// CDLG_ProgramSetting �޽��� ó�����Դϴ�.

LRESULT CDLG_ProgramSetting::OnAddList(WPARAM wParm, LPARAM lParm)
{
	try
	{
		char* buf = {0,};
		buf = (char*)lParm;

		if(strlen(buf) <=0)
			return 0;

		if( wParm == 1 )
			GetDlgItem(IDC_STATIC_PROGRESS2)->SetWindowText(buf);
		else
			GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(buf);

		//CString sMsg;
		//sMsg.Format("[%04d-%02d-%02d %02d:%02d:%02d.%03d] %s",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond, st.wMilliseconds, buf );
	}
	catch(...)
	//catch(std::exception &e)
	{
		//e.what();
	}

	return 0;
}

BOOL CDLG_ProgramSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	GetGatherSetting();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDLG_ProgramSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDLG_ProgramSetting::GetGatherSetting()
{
	CString str;
	for(int i = 0; i< 15 ; i++)
	{
		str.Format("%d��",i+1);
		m_Combo_GatherInterval.AddString(str);
	}

	//m_Combo_DeleteInterval.AddString("2�� ����");
	//m_Combo_DeleteInterval.AddString("5�� ����");
	//m_Combo_DeleteInterval.AddString("1���� ����");
	m_Combo_DeleteInterval.AddString("2���� ����");

	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);

	m_Combo_GatherInterval.SetCurSel(stGatherInfo.nGathe_IntervalPos);
	//m_Combo_DeleteInterval.SetCurSel(stGatherInfo.nDelete_IntervalPos);
	m_Combo_DeleteInterval.SetCurSel(0);

	// �ֱ� ���� ���
	/*time_t now = time(0);
	tm* ltm = localtime(&now);

	AfxMessageBox((char*)ltm->tm_mday);*/

	CButton *btRadioCheck;
	if(stGatherInfo.nAutoRun_Check == 1)
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_AUTORUN);
	else
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_USERURN);

	btRadioCheck->SetCheck(1);

	//<-- 20210305 ksw ��ǰ���� ini ���� ���� �߰�
	if(stGatherInfo.nProduct_Check == 1)
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_EMS);
	else
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_BEMS);

	btRadioCheck->SetCheck(1);
	//--> 20210305 ksw ��ǰ���� ini ���� ���� �߰�

	//���� ���� Ÿ��
	switch(stGatherInfo.nWeather_GatherType)
	{
	case 0:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_DIRECT);
		GetDlgItem(IDC_BUTTON_WEATHER_SET)->EnableWindow(TRUE);
		break;
	case 1:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_WEB);
		GetDlgItem(IDC_BUTTON_WEATHER_SET)->EnableWindow(FALSE);
		break;
	case 2:
	default:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_USE_N);
		GetDlgItem(IDC_BUTTON_WEATHER_SET)->EnableWindow(FALSE);
		break;

	}
	btRadioCheck->SetCheck(1);

	//���� ���� Ÿ��
	switch(stGatherInfo.nISmart_GatherType)
	{
	case 0:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_DIRECT);
		GetDlgItem(IDC_BUTTON_ISMART_SET)->EnableWindow(TRUE);
		break;
	case 1:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_WEB);
		GetDlgItem(IDC_BUTTON_ISMART_SET)->EnableWindow(FALSE);
		break;
	case 2:
	default:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_USE_N);
		GetDlgItem(IDC_BUTTON_ISMART_SET)->EnableWindow(FALSE);
		break;

	}
	btRadioCheck->SetCheck(1);

	//���� ���� Ÿ��
	switch(stGatherInfo.nEngMng_GatherType)
	{
	case 0:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_DIRECT);
		GetDlgItem(IDC_BUTTON_ENGMNG_SET)->EnableWindow(TRUE);
		break;
	case 1:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_WEB);
		GetDlgItem(IDC_BUTTON_ENGMNG_SET)->EnableWindow(FALSE);
		break;
	case 2:
	default:
		btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_USE_N);
		GetDlgItem(IDC_BUTTON_ENGMNG_SET)->EnableWindow(FALSE);
		break;

	}
	btRadioCheck->SetCheck(1);

	//20210308 ksw TagTread Ȱ��ȭ�϶��� ��ư ��밡��
	if(g_nCheckTagThreadRun == 0)
		GetDlgItem(IDC_BUTTON_CALIBRATION)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_CALIBRATION)->EnableWindow(TRUE);

	//GetDlgItem(IDC_EDIT_URL_LIVE)->SetWindowText(stGatherInfo.szUrlLive);
	//GetDlgItem(IDC_EDIT_URL_FORECAST)->SetWindowText(stGatherInfo.szUrlForecast);
}

void CDLG_ProgramSetting::SetGatherSetting()
{
	CString strText = "";
	ST_GATHERINFO stGatherInfo;
	memset(&stGatherInfo,0x00,sizeof(stGatherInfo));

	int nGatherInterval = m_Combo_GatherInterval.GetCurSel();
	int nDeleteInterval = m_Combo_DeleteInterval.GetCurSel();

	int nAutoRunCheck = 0;
	int nProductCheck = 0;
	int nDempRunCheck = 0;
	int nDempAutoCheck = 0;
	int nWeatherType = 2;
	int nIsmartType = 2;
	int nEngMngtType = 2;

	//<-- 20210305 ksw ��ǰ���� ini ���� ���� �߰�
	CButton *btRadioProductCheck = (CButton*)GetDlgItem(IDC_RADIO_BEMS);
	if(btRadioProductCheck->GetCheck() == 1)
		nProductCheck = 0;
	btRadioProductCheck = (CButton*)GetDlgItem(IDC_RADIO_EMS);
	if(btRadioProductCheck->GetCheck() == 1)
		nProductCheck = 1;
	//--> 20210305 ksw ��ǰ���� ini ���� ���� �߰�


	//20210902 ksw ���� �ֱ� ini ���� ���� �߰�


	CButton *btRadioAutoRunCheck = (CButton*)GetDlgItem(IDC_RADIO_AUTORUN);
	if(btRadioAutoRunCheck->GetCheck() == 1)
		nAutoRunCheck = 1;
	btRadioAutoRunCheck = (CButton*)GetDlgItem(IDC_RADIO_USERURN);
	if(btRadioAutoRunCheck->GetCheck() == 1)
		nAutoRunCheck = 0;

	stGatherInfo.nProduct_Check = nProductCheck; // 20210305 ksw ��ǰ���� ini ���� ���� �߰�, ����ü ���� �߰�
	stGatherInfo.nGathe_IntervalPos = nGatherInterval;
	stGatherInfo.nDelete_IntervalPos = nDeleteInterval;
	stGatherInfo.nAutoRun_Check = nAutoRunCheck;



	CButton *btRadioWeatherCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_DIRECT);
	if(btRadioWeatherCheck->GetCheck() == 1)
		nWeatherType = 0;
	btRadioWeatherCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_WEB);
	if(btRadioWeatherCheck->GetCheck() == 1)
		nWeatherType = 1;
	btRadioWeatherCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_USE_N);
	if(btRadioWeatherCheck->GetCheck() == 1)
		nWeatherType = 2;

	CButton *btRadioIsmartCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_DIRECT);
	if(btRadioIsmartCheck->GetCheck() == 1)
		nIsmartType = 0;
	btRadioIsmartCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_WEB);
	if(btRadioIsmartCheck->GetCheck() == 1)
		nIsmartType = 1;
	btRadioIsmartCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_USE_N);
	if(btRadioIsmartCheck->GetCheck() == 1)
		nIsmartType = 2;

	CButton *btRadioEngMngCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_DIRECT);
	if(btRadioEngMngCheck->GetCheck() == 1)
		nEngMngtType = 0;
	btRadioEngMngCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_WEB);
	if(btRadioEngMngCheck->GetCheck() == 1)
		nEngMngtType = 1;
	btRadioEngMngCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_USE_N);
	if(btRadioEngMngCheck->GetCheck() == 1)
		nEngMngtType = 2;

	stGatherInfo.nWeather_GatherType = nWeatherType;
	stGatherInfo.nISmart_GatherType = nIsmartType;
	stGatherInfo.nEngMng_GatherType = nEngMngtType;

	_setInfoGatherWrite(&stGatherInfo,g_stProjectInfo.szProjectIniPath);
}


void CDLG_ProgramSetting::OnBnClickedRadioWeatherDirect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_DIRECT);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_WEB);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_USE_N);
	btRadioCheck->SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_WEATHER_SET)->EnableWindow(TRUE);
}


void CDLG_ProgramSetting::OnBnClickedRadioWeatherWeb()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_DIRECT);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_WEB);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_USE_N);
	btRadioCheck->SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_WEATHER_SET)->EnableWindow(FALSE);
}


void CDLG_ProgramSetting::OnBnClickedRadioWeatherUseN()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_DIRECT);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_WEB);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_WEATHER_USE_N);
	btRadioCheck->SetCheck(TRUE);
	GetDlgItem(IDC_BUTTON_WEATHER_SET)->EnableWindow(FALSE);
}

void CDLG_ProgramSetting::OnBnClickedButtonWeatherSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDLG_WeatherSeting dlg;
	dlg.DoModal();
}


void CDLG_ProgramSetting::OnBnClickedRadioIsmartDirect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_DIRECT);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_WEB);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_USE_N);
	btRadioCheck->SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_ISMART_SET)->EnableWindow(TRUE);
}


void CDLG_ProgramSetting::OnBnClickedRadioIsmartWeb()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_DIRECT);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_WEB);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_USE_N);
	btRadioCheck->SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_ISMART_SET)->EnableWindow(FALSE);
}


void CDLG_ProgramSetting::OnBnClickedRadioIsmartUseN()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_DIRECT);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_WEB);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ISMART_USE_N);
	btRadioCheck->SetCheck(TRUE);
	GetDlgItem(IDC_BUTTON_ISMART_SET)->EnableWindow(FALSE);
}

void CDLG_ProgramSetting::OnBnClickedButtonIsmartSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDLG_ISmarAccesstSetting dlg;
	dlg.DoModal();
}

void CDLG_ProgramSetting::OnBnClickedRadioEngmngDirect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_DIRECT);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_WEB);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_USE_N);
	btRadioCheck->SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_ENGMNG_SET)->EnableWindow(TRUE);
}


void CDLG_ProgramSetting::OnBnClickedRadioEngmngWeb()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_DIRECT);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_WEB);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_USE_N);
	btRadioCheck->SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_ENGMNG_SET)->EnableWindow(FALSE);
}


void CDLG_ProgramSetting::OnBnClickedRadioEngmngUseN()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_DIRECT);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_WEB);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_ENGMNG_USE_N);
	btRadioCheck->SetCheck(TRUE);
	GetDlgItem(IDC_BUTTON_ENGMNG_SET)->EnableWindow(FALSE);
}

void CDLG_ProgramSetting::OnBnClickedButtonEngmngSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDLG_EnergyTag_InitialSet dlg;
	dlg.DoModal();
}


void CDLG_ProgramSetting::OnBnClickedRadioAutorun()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_AUTORUN);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_USERURN);
	btRadioCheck->SetCheck(FALSE);
}


void CDLG_ProgramSetting::OnBnClickedRadioUserurn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_AUTORUN);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_USERURN);
	btRadioCheck->SetCheck(TRUE);
}


void CDLG_ProgramSetting::OnBnClickedButtonCalibration()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CTime ctSelTime, ctNow = CTime::GetCurrentTime();

	//�޷¿��� ������ ��¥�� 0��0�� ����
	m_mccCalander.GetCurSel(ctSelTime);
	ctSelTime = CTime(ctSelTime.GetYear(), ctSelTime.GetMonth(), ctSelTime.GetDay(), 0, 0, 0);
	ctNow = CTime(ctNow.GetYear(), ctNow.GetMonth(), ctNow.GetDay(), 0, 0, 0);

	if( ctSelTime > ctNow )
	{
		AfxMessageBox("���� ��¥�� �����ϼ���");
		return;
	}

	StartThreadCalibration(ctSelTime);
	GetDlgItem(IDC_BUTTON_CALIBRATION)->EnableWindow(FALSE);
}

void CDLG_ProgramSetting::StartThreadCalibration(CTime ctSelTime)
{
	StopThreadCalibration();

	if(m_pThreadCalibration == NULL)
	{
		m_pThreadCalibration=(CThread_Calibration*)AfxBeginThread(RUNTIME_CLASS(CThread_Calibration),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
		m_pThreadCalibration->SetOwner(this);
		m_pThreadCalibration->SetStartTime(ctSelTime);
		m_pThreadCalibration->ResumeThread();
		//<-- 20210308 ksw ��ǰ �б�
		CButton *btRadioProductCheck = (CButton*)GetDlgItem(IDC_RADIO_BEMS);
		if(btRadioProductCheck->GetCheck() == 1)
			m_pThreadCalibration->SetProduct(0);
		btRadioProductCheck = (CButton*)GetDlgItem(IDC_RADIO_EMS);
		if(btRadioProductCheck->GetCheck() == 1)
			m_pThreadCalibration->SetProduct(1);
		//--> 20210308 ksw ��ǰ �б�
	}
}

void CDLG_ProgramSetting::StopThreadCalibration()
{
	DWORD dwExitCode;

	if (NULL == m_pThreadCalibration)
		return;

	DWORD dw = GetTickCount();
	m_pThreadCalibration->Stop();
#ifdef _DEBUG
	TRACE("DataGathering Main Thread Stop \n");
#endif

	int nErr = 0;
	while (1)
	{
		if(GetExitCodeThread(m_pThreadCalibration->m_hThread, &dwExitCode))
		{
			if(dwExitCode != STILL_ACTIVE)
				break;
		}
		else break;
		Sleep(100);
		if(nErr++ > TIMEWAIT_FILELOG)
		{
			ASSERT(FALSE);
			return;
		}
	}
	//delete m_pThreadCalibration;
	m_pThreadCalibration = NULL;

}

void CDLG_ProgramSetting::OnBnClickedRadioBems() //20210305 ksw ��ǰ���� ������ư �̺�Ʈ �߰�
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_BEMS);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_EMS);
	btRadioCheck->SetCheck(FALSE);
}


void CDLG_ProgramSetting::OnBnClickedRadioEms() //20210305 ksw ��ǰ���� ������ư �̺�Ʈ �߰�
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_BEMS);
	btRadioCheck->SetCheck(FALSE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_EMS);
	btRadioCheck->SetCheck(TRUE);
}

LRESULT CDLG_ProgramSetting::OnEnableControl(WPARAM wParm, LPARAM lParm) //20210308 ksw ��Ʈ�� Enable ���� �Լ�
{
	GetDlgItem(wParm)->EnableWindow(lParm);
	return 0;
}