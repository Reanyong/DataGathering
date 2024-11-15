#pragma once
// CDlgTagDic 대화 상자
#include <set>
#include "Control_List/ListBoxControl/XListCtrl.h"
#include <thread>

class CDlgTagDic : public CDialog
{
	DECLARE_DYNAMIC(CDlgTagDic)

public:
	CDlgTagDic(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgTagDic();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAGDIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CEdit m_EditSearch;
	CButton m_BtnSearch;
	CButton m_BtnSave;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedMoveRight();						// Right Button
	afx_msg void OnBnClickedMoveLeft();							// Left Button
	afx_msg void OnBnClickedSave();								// Insert Button
	afx_msg void OnBnClickedSearchTagGroup();					// Search Button
	afx_msg void OnEnChangeEditSearchTagGroup();				// Edit Ctrl 변경
	afx_msg void OnLvnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnSearchDbtaggroup();				// Search DB TagGroup 기능
	//afx_msg void OnLvnBeginlabeleditListInsert(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();

private:
	CAdo_Control* m_DBConnect;		// DB Conneciont ADO Control
	COdbc* codbc;
	HWND m_WindHwnd;				//

	int m_nDBType;					// DB_Type 지정 (MSSQL, PostgreSQL)
	CString m_strLogTitle;			// DB_Connection Log Title 지정

	CEdit m_EditCtrl;
	int m_nEditItem;				//
	int m_nEditSubItem;				//

	CWnd* m_pParent;				// 부모 DLG

	void LoadTagDic();				// TAGDIC 호출 함수
	void LoadEVTag();				// EView Tag List 호출 함수

	CXListCtrl m_ListCtrlInsert;	// DB TAG_DIC List 우측 리스트 컨트롤
	CListCtrl m_ListCtrlEMS;		// EView Tag List 좌측 리스트 컨트롤

	CStringArray comboItems;		// 콤보 박스 저장

	bool m_bIsModified;				// 추가된 항목이 있는지 여부를 나타내는 플래그

	std::set<CString> m_NewTags;
};
