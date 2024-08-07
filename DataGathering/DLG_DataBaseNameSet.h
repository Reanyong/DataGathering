#pragma once


// CDLG_DataBaseNameSet 대화 상자입니다.

class CDLG_DataBaseNameSet : public CDialog
{
	DECLARE_DYNAMIC(CDLG_DataBaseNameSet)

public:
	CDLG_DataBaseNameSet(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_DataBaseNameSet();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DATABASE_NAME_SET };

protected:
	void ShowSettingData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
};
