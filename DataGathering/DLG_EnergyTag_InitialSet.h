#pragma once


// CDLG_EnergyTag_InitialSet 대화 상자입니다.

class CDLG_EnergyTag_InitialSet : public CDialog
{
	DECLARE_DYNAMIC(CDLG_EnergyTag_InitialSet)

public:
	CDLG_EnergyTag_InitialSet(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_EnergyTag_InitialSet();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ENERGYTAG_INITIAL_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonPath();
};
