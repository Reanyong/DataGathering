// DLG_WeatherSeting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_WeatherSeting.h"
#include "afxdialogex.h"


// CDLG_WeatherSeting 대화 상자입니다.

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


// CDLG_WeatherSeting 메시지 처리기입니다.

BOOL CDLG_WeatherSeting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ShowSettingData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDLG_WeatherSeting::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("실시간 수집 미사용");
	}
	else
	{
		btUseYN->SetCheck(1);
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("실시간 수집 사용");
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
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("예보 수집 미사용");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(FALSE);
	}
	else
	{
		btUseYN->SetCheck(1);
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("예보 수집 사용");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(TRUE);
	}
}


void CDLG_WeatherSeting::OnBnClickedCheckUseYn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *btUseYN = (CButton*)GetDlgItem(IDC_CHECK_LIVE_USE_YN);
	if(btUseYN->GetCheck())
	{
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("실시간 수집 사용");
	}
	else
	{
		GetDlgItem(IDC_EDIT_SERVICES_KEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIVE_LAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LIVE_LON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_LIVE_USE_YN)->SetWindowText("실시간 수집 미사용");
	}
}



void CDLG_WeatherSeting::OnBnClickedCheckForecastUseYn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *btUseYN = (CButton*)GetDlgItem(IDC_CHECK_FORECAST_USE_YN);
	if(btUseYN->GetCheck())
	{
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("예보 수집 사용");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(TRUE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FORECAST_LON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FORECAST_USE_YN)->SetWindowText("예보 수집 미사용");
		GetDlgItem(IDC_BUTTON_LIVE_LAT_COPY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LIVE_LON_COPY)->EnableWindow(FALSE);
	}
}


void CDLG_WeatherSeting::OnBnClickedButtonLiveLatCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strBuffer;
	GetDlgItem(IDC_EDIT_LIVE_LAT)->GetWindowText(strBuffer);
	if(strBuffer.IsEmpty() == TRUE)
		AfxMessageBox("입력한 정보가 없습니다.");
	else
		GetDlgItem(IDC_EDIT_FORECAST_LAT)->SetWindowText(strBuffer);

}


void CDLG_WeatherSeting::OnBnClickedButtonLiveLonCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strBuffer;
	GetDlgItem(IDC_EDIT_LIVE_LON)->GetWindowText(strBuffer);
	if(strBuffer.IsEmpty() == TRUE)
		AfxMessageBox("입력한 정보가 없습니다.");
	else
		GetDlgItem(IDC_EDIT_FORECAST_LON)->SetWindowText(strBuffer);
}


BOOL CDLG_WeatherSeting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
