#pragma once


// CDLG_EnergyTag_InitialSet ��ȭ �����Դϴ�.

class CDLG_EnergyTag_InitialSet : public CDialog
{
	DECLARE_DYNAMIC(CDLG_EnergyTag_InitialSet)

public:
	CDLG_EnergyTag_InitialSet(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_EnergyTag_InitialSet();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_ENERGYTAG_INITIAL_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonPath();
};
