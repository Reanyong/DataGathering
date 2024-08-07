#pragma once

#include "Control_List/ListBoxControl/XListCtrl.h"

#include "Thread_Main.h"
// CFormView_Main 폼 뷰입니다.

class CFormView_Main : public CFormView
{
	DECLARE_DYNCREATE(CFormView_Main)

protected:
	CFormView_Main();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormView_Main();

public:
	enum { IDD = IDD_FORMVIEW_MAIN };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CFont m_Font;
	CXListCtrl m_xListProcessorList;
	CXListCtrl m_xListProcessorMsgList;

	CThread_Main *m_pThreadMain;	
	DWORD m_dwRowCnt;
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);	
	void StartThread();
	void StopThread();
	
public:
	int ListInsertItem_Info(const char *szData1,const char *szData2,const char *szData3);
	int ListInsertItem_Msg(const char *szData1,const char *szData2,const char *szData3);
	void ShowSettingInfo(CString strIniPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


