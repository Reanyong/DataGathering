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
	afx_msg void OnComboBoxSelectionChange();           // Change ComboBox
	afx_msg void OnNMDblclkListChartSection(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditKillFocus();

private:
	CAdo_Control* m_DBConnect;		// DB Conneciont ADO Control
	COdbc* codbc;					// PostgreSQL Connection
	HWND m_WindHwnd;

	int m_nDBType;					// DB_Type 지정 (MSSQL, PostgreSQL)
	CString m_strLogTitle;			// DB_Connection Log Title 지정

	CWnd* m_pParent;				// 부모 DLG
	void LoadTagDic();				// TAGDIC 호출 함수
	void LoadEMSChart();

	void LoadComboBoxSections();

	std::vector<std::pair<CString, CString>> CDlgEMSChart::GetDataForSection(const CString& section);
	std::vector<CString> GetIniSections();

	CString GetAppDirectory();       // 애플리케이션 디렉토리 가져오는 함수
	CString GetIniFilePath();        // INI 파일 경로 가져오는 함수

	CButton m_BtnSave;
	CComboBox m_ComboSection;

	CXListCtrl m_ListTAGDIC;		// HM_TAG_DIC List Ctrl
	CXListCtrl m_ListChartSection;	// Chart Section List Ctrl

	CEdit m_EditControl;
	int m_nEditItem;
	int m_nEditSubItem;

	void StartEdit(int nItem, int nSubItem);
	void EndEdit();
};
