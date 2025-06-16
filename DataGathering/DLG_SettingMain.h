#pragma once

#include "DLG_ProgramSetting.h"
#include "DLG_DataBaseSetting.h"
#include "DLG_GatherInfoSetting.h"

// CDLG_SettingMain 대화 상자입니다.

class CDLG_SettingMain : public CDialog
{
	DECLARE_DYNAMIC(CDLG_SettingMain)

public:
	CDLG_SettingMain(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_SettingMain();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SETTING_MAIN };
protected:
	CBitmapButton m_ButttonGatherSet;
	CBitmapButton m_ButttonDBSet;
	CBitmapButton m_ButttonGatherTypeSet;

	 CDLG_ProgramSetting *m_pDlgProgramSetting;
	 CDLG_DataBaseSetting *m_pDlgDataBaseSetting;
	 CDLG_GatherInfoSetting *m_pDlgGatherTypeSetting;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonSetingGather();
	afx_msg void OnBnClickedButtonSetingDb();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSetingGathertypeSet();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};
