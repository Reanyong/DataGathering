#pragma once


// CDLG_ProgramSetting ��ȭ �����Դϴ�.

#include "Thread_Calibration.h"

class CDLG_ProgramSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDLG_ProgramSetting)

public:
	CDLG_ProgramSetting(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_ProgramSetting();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_PROGRAMSETTING };
protected:
	CComboBox m_Combo_GatherInterval;
	CComboBox m_Combo_DeleteInterval;
protected:
	void GetGatherSetting();
	void SetGatherSetting();
public:
	void SetSettingSave() {SetGatherSetting();}

	void StartThreadCalibration(CTime ctSelTime);
	void StopThreadCalibration();

	CThread_Calibration *m_pThreadCalibration;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedRadioWeatherDirect();
	afx_msg void OnBnClickedRadioWeatherWeb();
	afx_msg void OnBnClickedRadioWeatherUseN();
	afx_msg void OnBnClickedRadioIsmartDirect();
	afx_msg void OnBnClickedRadioIsmartWeb();
	afx_msg void OnBnClickedRadioIsmartUseN();
	afx_msg void OnBnClickedRadioEngmngDirect();
	afx_msg void OnBnClickedRadioEngmngWeb();
	afx_msg void OnBnClickedRadioEngmngUseN();
	afx_msg void OnBnClickedButtonWeatherSet();
	afx_msg void OnBnClickedButtonIsmartSet();
	afx_msg void OnBnClickedButtonEngmngSet();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioAutorun();
	afx_msg void OnBnClickedRadioUserurn();
	afx_msg void OnBnClickedButtonCalibration();
	afx_msg LRESULT OnAddList(WPARAM wParm, LPARAM lParm);
	afx_msg LRESULT OnEnableControl(WPARAM wParm, LPARAM lParm); //20210308 ksw ��Ʈ�� Enable ���� �Լ�
	CMonthCalCtrl m_mccCalander;
	afx_msg void OnBnClickedRadioBems();
	afx_msg void OnBnClickedRadioEms();

	// �ʱ�ȭ SET
	afx_msg void OnBnClickedBtnTrcMinute();
	afx_msg void OnBnClickedBtnTrcQuater();
	afx_msg void OnBnClickedBtnTrcHour();
	afx_msg void OnBnClickedBtnTrcMonth();

	CAdo_Control* DB_Connect;
	HWND m_WindHwnd;
	int m_nDBType;

	void OnTruncateTable(UINT nID);
};
