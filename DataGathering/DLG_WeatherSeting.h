#pragma once


// CDLG_WeatherSeting ��ȭ �����Դϴ�.

class CDLG_WeatherSeting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_WeatherSeting)

public:
	CDLG_WeatherSeting(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_WeatherSeting();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_WEATHER_SETTING };

protected:
	void ShowSettingData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
