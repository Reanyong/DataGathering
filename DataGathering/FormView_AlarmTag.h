#pragma once


#include "Control_List/ListBoxControl/XListCtrl.h"
#include "Thread_TagAlarmMain.h"
// CFormView_AlarmTag 폼 뷰입니다.

class CFormView_AlarmTag : public CFormView
{
	DECLARE_DYNCREATE(CFormView_AlarmTag)

protected:
	CFormView_AlarmTag();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormView_AlarmTag();

public:
	enum { IDD = IDD_FORMVIEW_ALARM_TAG };
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

	CThread_TagAlarmMain *m_pTagAlarmMain;
	ST_GATHERINFO m_stGatherInfo;
	CString m_strTitle;
	BOOL m_bStartRunCheck;
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	BOOL StartThread();
	void StopThread(BOOL bStopType);
public:
	int SetStartDeviceList(const char *szData1,const char *szData2,const char *szData3,const char *szData4);
	int SetDeviceList(int nItemRow,const char *szData1,const char *szData2,const char *szData3,const char *szData4);
	void SetDeviceListRemove();
	int SetAlarmOccursMsg(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnDestroy();
};


