#pragma once


// CDLG_DataBaseSetting 대화 상자입니다.

class CDLG_DataBaseSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_DataBaseSetting)

public:
	CDLG_DataBaseSetting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_DataBaseSetting();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DBSETTING_SAVE };
protected:
	CAdo_Control *DB_Connect;
	CToolTipCtrl m_tooltop;

protected:
	void ShowSettingData();
	void ShowControlEnable(BOOL bEnable);
	void SetDBInfoSetting();
	void ShowToolTip();
public:
	void SetSettingSave() {SetDBInfoSetting();}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtConnectionTest();
	afx_msg LRESULT OnUserMsgDBSetting(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioOracle();
	afx_msg void OnBnClickedRadioMssql();
	afx_msg void OnBnClickedButtonDbnameSet();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioPostgre();
};
