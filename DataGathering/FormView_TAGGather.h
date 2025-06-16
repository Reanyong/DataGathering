#pragma once

#include "Control_List/ListBoxControl/XListCtrl.h"
#include "Thread_MinGatherMain.h"

// CFormView_TAGGather 폼 뷰입니다.

class CFormView_TAGGather : public CFormView
{
	DECLARE_DYNCREATE(CFormView_TAGGather)

protected:
	CFormView_TAGGather();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormView_TAGGather();

public:
	enum { IDD = IDD_FORMVIEW_TAG_GATHER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:

	BOOL m_bStartRunCheck;
	CString m_strTitle;
	ST_GATHERINFO m_stGatherInfo;
	CThread_MinGatherMain* m_pThread_MinGatherMain;

	CXListCtrl m_xListSiteList;		// Processor List Ctrl
	CXListCtrl m_xListTagDicList;	// TAG_DIC List Ctrl

	CFont m_Font;
	BOOL m_bWait; //20210308 ksw 시작 대기
	BOOL m_b50Check; //20210308 ksw 시작 대기 50초 체크 변수
	int m_nMin;
	int m_nSec;

	CAdo_Control* DB_Connect;
	COdbc* codbc;

	HWND m_WindHwnd;

	int m_nDBType;
	CString m_strLogTitle;

public:
	void ListInitialization();
	void ShowListData(const char* szData1,const char* szData2,const char* szData3,const char* szData4);
	void ShowListMsgUdate(int nRow, const char *szStatus,const char *szMsg);

	void ShowText_State(const char* szData1);
	void ShowText_Count(const char* szData1);
	void ShowText_Processor(const char* szData1);
	void LoadTagDic();
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	void StartThread();
	void StopThread(BOOL bStopType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonGatherStart();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnTagdicInsert();
	afx_msg void OnHeaderBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnEmschart();
};


