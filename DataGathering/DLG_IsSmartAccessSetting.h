#pragma once


// CDLG_IsSmartAccessSetting 대화 상자입니다.

class CDLG_IsSmartAccessSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_IsSmartAccessSetting)

public:
	CDLG_IsSmartAccessSetting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_IsSmartAccessSetting();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ISMART_ACCESS_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
