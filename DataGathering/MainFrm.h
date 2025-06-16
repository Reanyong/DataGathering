// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "CalendarBar.h"
#include "Resource.h"

#include "OutputWnd.h"
#include "TreeWnd.h"

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonBar				m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages			m_PanelImages;
	CMFCRibbonStatusBar			m_wndStatusBar;
	COutlookBar					m_wndNavigationBar;
	CCalendarBar				m_wndCalendar;
	CMFCCaptionBar				m_wndCaptionBar;

	CTreeWnd					m_wndTree;
	COutputWnd					m_wndOutput;
	BOOL						m_bEndCheck;
protected:
	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth);
	BOOL CreateCaptionBar();

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

	CMDIChildWnd *FindMDIChildWnd(const char *szTitle, int nActivateDoc);

	//
	void ShowDlgPopUp(CDialog *dlg);
	//메시지 저장
	void MsgWork();

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg LRESULT OnOpenChildFrame(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEasyViewStop(UINT wParm, LONG lParm);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBtLogfolderOpen();
	afx_msg void OnBtLogview();
	afx_msg void OnBtLogRoot();
};


