#pragma once


// CDLG_WeatherSeting 대화 상자입니다.

class CDLG_WeatherSeting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_WeatherSeting)

public:
	CDLG_WeatherSeting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_WeatherSeting();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_WEATHER_SETTING };

protected:
	void ShowSettingData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnBnClickedCheckUseYn();
	afx_msg void OnBnClickedCheckForecastUseYn();
	afx_msg void OnBnClickedButtonLiveLatCopy();
	afx_msg void OnBnClickedButtonLiveLonCopy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
