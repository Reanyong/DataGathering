#pragma once


// CDLG_ISmarAccesstSetting 대화 상자입니다.

class CDLG_ISmarAccesstSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_ISmarAccesstSetting)

public:
	CDLG_ISmarAccesstSetting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_ISmarAccesstSetting();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ISMART_ACCESS_SETTING };
protected:
	ST_FILE_ISMARTACCESS m_stISmartAccess;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
