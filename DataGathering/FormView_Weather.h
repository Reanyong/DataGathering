#pragma once

//#include "Control_List/GridCtrl/GridCtrl.h"
#include "Control_List/ListBoxControl/XListCtrl.h"
#include "Thread_WeatherGather.h"
// CFormView_Weather 폼 뷰입니다.

class CFormView_Weather : public CFormView
{
	DECLARE_DYNCREATE(CFormView_Weather)

protected:
	CFormView_Weather();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormView_Weather();

public:
	enum { IDD = IDD_FORMVIEW_WEATHER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	//멤버 변수 선언
protected:
	//CGridCtrl m_gridForecastGrib;
	CFont m_Font;
	CXListCtrl m_xListLiveGrid;
	CXListCtrl m_xListForecastGrid;

	CThread_WeatherGather *m_pThreadWeather;

	BOOL m_bThreadStart;
	CString m_strTitle;
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	int OutputWeatherHistory(CXListCtrl &listCtrl,const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8);
	BOOL StartThread();
	void StopThread();
public:
	void ShowRun_State(int nState);
	void OutputWeatherLiveHistory(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8);
	void OutputWeatherForecastHistory(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5,const char *szData6,const char *szData7,const char *szData8);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStart();
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


