#pragma once


// CDLG_IsSmartAccessSetting ��ȭ �����Դϴ�.

class CDLG_IsSmartAccessSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_IsSmartAccessSetting)

public:
	CDLG_IsSmartAccessSetting(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_IsSmartAccessSetting();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_ISMART_ACCESS_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
