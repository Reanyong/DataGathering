// DLG_WeatherSeting.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_WeatherSeting.h"
#include "afxdialogex.h"


// CDLG_WeatherSeting ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDLG_WeatherSeting, CDialog)

CDLG_WeatherSeting::CDLG_WeatherSeting(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_WeatherSeting::IDD, pParent)
{

}

CDLG_WeatherSeting::~CDLG_WeatherSeting()
{
}

void CDLG_WeatherSeting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_WeatherSeting, CDialog)
	ON_BN_CLICKED(IDOK, &CDLG_WeatherSeting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_LIVE_USE_YN, &CDLG_WeatherSeting::OnBnClickedCheckUseYn)
	ON_BN_CLICKED(IDC_CHECK_FORECAST_USE_YN, &CDLG_WeatherSeting::OnBnClickedCheckForecastUseYn)
	ON_BN_CLICKED(IDC_BUTTON_LIVE_LAT_COPY, &CDLG_WeatherSeting::OnBnClickedButtonLiveLatCopy)
	ON_BN_CLICKED(IDC_BUTTON_LIVE_LON_COPY, &CDLG_WeatherSeting::OnBnClickedButtonLiveLonCopy)
END_MESSAGE_MAP()


// CDLG_WeatherSeting �޽��� ó�����Դϴ�.

BOOL CDLG_WeatherSeting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	ShowSettingData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDLG_WeatherSeting::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strBuffer = "";
	ST_WEATHER_INFO stWeatherInfo;
	memset(&stWeatherInfo,0x00,sizeof(stWeatherInfo));
	CButton *btUseYN = (CButton*)GetDlgItem(IDC_CHECK_LIVE_USE_YN);
	if(btUseYN->GetCheck())
		sprintf_s(stWeatherInfo.szLiveUSE_YN,"1");
	else
		sprintf_s(stWeatherInfo.szLiveUSE_YN,"0");
		
	GetDlgItem(IDC_EDIT_SERVICES_KEY)->GetWindowText(strBuffer);
	sprintf_s(stWeatherInfo.szServiceKey,"%s",strBuffer);
	GetDlgItem(IDC_EDIT_LIVE_LAT)->GetWindowText(strBuffer);
	sprintf_s(stWeatherInfo.szLiveLat,"%s",strBuffer);
	GetDlgItem(IDC_EDIT_LIVE_LON)->GetWindowText(strBuffer);
	sprintf_s(stWeatherInfo.szLiveLon,"%s",strBuffer);

	btUseYN = (CButton*)GetDlgItem(IDC_CHECK_FORECAST_USE_YN);
	if(btUseYN->GetCheck())
		sprintf_s(stWeatherInfo.szForecastUSE_YN,"1");
	else
		sprintf_s(stWeatherInfo.szForecastUSE_YN,"0");
	GetDlgItem(IDC_EDIT_FORECAST_LAT)->GetWindowText(strBuffer);
	sprintf_s(stWeatherInfo.szForecastLat,"%s",strBuffer);
	GetDlgItem(IDC_EDIT_FORECAST_LON)->GetWindowText(strBuffer);
	sprintf_s(stWeatherInfo.szForecastLon,"%s",strBuffer);

	_setInfoWeatherWrite(&stWeatherInfo,g_stProjectInfo.szProjectIniPath);
	CDialog::OnOK();
}

void CDLG_WeatherSeting::ShowSettingData()
{
	ST_WEATHER_INFO stWeatherInfo;
	stWeatherInfo = _getInfoWeatherRead(g_stProjectInfo.szProjectIniPath);
	
	GetDlgItem(IDC_EDIT_SERVICES_KEY)->SetWindowText(stWeatherInfo.szServiceKey);
	GetDlgItem(IDC_EDIT_LIVE_LAT)->SetWindowText(stWeatherInfo.szLiveLat);
	GetDlgItem(IDC_EDIT_LIVE_LON)->SetWindowText(stWeatherInfo.szLiveLon);

	CButton *btUseYN = (CButton*)GetDlgItem(IDC_CHECK_LIVE_USE_YN);
	
	int nUseYN = atoi(stWeatherInfo.szLiveUSE_YN);
	if(nUseYN == 0)
	{
		btUseYN->SetCheck(0);
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("�ǽð� ���� �̻��");
	}
	else
	{
		btUseYN->SetCheck(1);
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("�ǽð� ���� ���");
	}

	
	GetDlgItem(IDC_EDIT_FORECAST_LAT)->SetWindowText(stWeatherInfo.szForecastLat);
	GetDlgItem(IDC_EDIT_FORECAST_LON)->SetWindowText(stWeatherInfo.szForecastLon);

	btUseYN = (CButton*)GetDlgItem(IDC_CHECK_FORECAST_USE_YN);
	nUseYN = atoi(stWeatherInfo.szForecastUSE_YN);
	if(nUseYN == 0)
	{
		btUseYN->SetCheck(0);
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("���� ���� �̻��");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(FALSE);
	}
	else
	{
		btUseYN->SetCheck(1);
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("���� ���� ���");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(TRUE);
	}
}


void CDLG_WeatherSeting::OnBnClickedCheckUseYn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btUseYN = (CButton*)GetDlgItem(IDC_CHECK_LIVE_USE_YN);
	if(btUseYN->GetCheck())
	{
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("�ǽð� ���� ���");
	}
	else
	{
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("�ǽð� ���� �̻��");
	}
}



void CDLG_WeatherSeting::OnBnClickedCheckForecastUseYn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *btUseYN = (CButton*)GetDlgItem(IDC_CHECK_FORECAST_USE_YN);
	if(btUseYN->GetCheck())
	{
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("���� ���� ���");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(TRUE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("���� ���� �̻��");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(FALSE);
	}
}


void CDLG_WeatherSeting::OnBnClickedButtonLiveLatCopy()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strBuffer;
	GetDlgItem(IDC_EDIT_LIVE_LAT)->GetWindowText(strBuffer);
	if(strBuffer.IsEmpty() == TRUE)
		AfxMessageBox("�Է��� ������ �����ϴ�.");
	else
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->SetWindowText(strBuffer);

}


void CDLG_WeatherSeting::OnBnClickedButtonLiveLonCopy()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strBuffer;
	GetDlgItem(IDC_EDIT_LIVE_LON)->GetWindowText(strBuffer);
	if(strBuffer.IsEmpty() == TRUE)
		AfxMessageBox("�Է��� ������ �����ϴ�.");
	else
		GetDlgItem(IDC_EDIT_FORECAST_LON)->SetWindowText(strBuffer);
}


BOOL CDLG_WeatherSeting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
