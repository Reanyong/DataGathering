#pragma once


// CDLG_DataBaseNameSet ��ȭ �����Դϴ�.

class CDLG_DataBaseNameSet : public CDialog
{
	DECLARE_DYNAMIC(CDLG_DataBaseNameSet)

public:
	CDLG_DataBaseNameSet(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_DataBaseNameSet();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_DATABASE_NAME_SET };

protected:
	void ShowSettingData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
};
