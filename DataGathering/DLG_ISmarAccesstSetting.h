#pragma once


// CDLG_ISmarAccesstSetting ��ȭ �����Դϴ�.

class CDLG_ISmarAccesstSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_ISmarAccesstSetting)

public:
	CDLG_ISmarAccesstSetting(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_ISmarAccesstSetting();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_ISMART_ACCESS_SETTING };
protected:
	ST_FILE_ISMARTACCESS m_stISmartAccess;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
