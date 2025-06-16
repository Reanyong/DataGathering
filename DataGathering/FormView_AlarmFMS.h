#pragma once

#include "Control_List/ListBoxControl/XListCtrl.h"
#include "Thread_FMSAlarmMain.h"
// CFormView_AlarmFMS 폼 뷰입니다.

class CFormView_AlarmFMS : public CFormView
{
	DECLARE_DYNCREATE(CFormView_AlarmFMS)

protected:
	CFormView_AlarmFMS();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormView_AlarmFMS();

public:
	enum { IDD = IDD_FORMVIEW_ALARM_FMS };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CFont m_Font;
	CXListCtrl m_xListSiteList;
	CXListCtrl m_xListAlarmCheckList;
	BOOL m_bThreadStart;
	BOOL m_bStartRunCheck;
	ST_GATHERINFO m_stGatherInfo;
	CThread_FMSAlarmMain *m_pThreadMain;
	CString m_strTitle;
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	BOOL StartThread();
	void StopThread(BOOL bStopType);
public:
	void ListRemoveItem_Site();
	int ListInsertItem_Site(const char *szData1,const char *szData2,const char *szData3);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStart();
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};


