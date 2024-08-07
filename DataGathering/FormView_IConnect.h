#pragma once

#include "Control_List/ListBoxControl/XListCtrl.h"
#include "Thread_IConnect.h"

// CFormView_IConnect 폼 뷰입니다.

class CFormView_IConnect : public CFormView
{
	DECLARE_DYNCREATE(CFormView_IConnect)

protected:
	CFormView_IConnect();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormView_IConnect();

public:
	enum { IDD = IDD_FORMVIEW_ICONNECT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CFont m_Font;
	CXListCtrl m_xListSiteList;
	CXListCtrl m_xListCheckList;
	BOOL m_bThreadStart;

	CThread_IConnect *m_pThreadMain;
	
	DWORD m_dwRowCnt;	
	CString m_strTitle;
public:
	void ListRemoveItem_Site();
	int ListInsertItem_Site(const char *szData1,const char *szData2,const char *szData3);
	void ListInsertItem_Item(const char *szData1,const char *szData2,BOOL bType);
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	BOOL StartThread();
	void StopThread();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStart();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


