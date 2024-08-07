#pragma once
#include "Control_List/ListBoxControl/XListCtrl.h"

// CDlgEMSChart 대화 상자

class CDlgEMSChart : public CDialog
{
	DECLARE_DYNAMIC(CDlgEMSChart)

public:
	CDlgEMSChart(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEMSChart();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EMSCHART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedMoveRight();				// Right Button
	afx_msg void OnBnClickedMoveLeft();					// Left Button
	afx_msg void OnBnClickedSave();						// Save Button
	afx_msg void OnClose();

private:
	CAdo_Control* m_DBConnect;		// DB Conneciont ADO Control
	COdbc* codbc;					// PostgreSQL Connection

	int m_nDBType;					// DB_Type 지정 (MSSQL, PostgreSQL)
	CString m_strLogTitle;			// DB_Connection Log Title 지정

	CWnd* m_pParent;				// 부모 DLG
	void LoadTagDic();				// TAGDIC 호출 함수

	CButton m_BtnSave;

	CXListCtrl m_ListTAGDIC;		// HM_TAG_DIC List Ctrl
	CXListCtrl m_ListChartSection;	// Chart Section List Ctrl
};
