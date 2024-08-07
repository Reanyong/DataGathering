
#pragma once
/////////////////////////////////////////////////////////////////////////////
// COutputList â

#include "Control_List/ListBoxControl/XListCtrl.h"

class COutputList : public CListBox
{
// �����Դϴ�.
public:
	COutputList();

// �����Դϴ�.
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// �����Դϴ�.
public:
	COutputWnd();

// Ư���Դϴ�.
protected:
	CFont			m_Font;
	CMFCTabCtrl		m_wndTabs;
	int				m_nOutPutTabCount;
	CXListCtrl		m_OutputList[10];

public:
	int InsertMessage(int nListCtrl, COLORREF crText1, const char *szMsg1, COLORREF crText2, const char *szMsg2);
protected:
	void InitListCtrl_SysTime(CXListCtrl &list);
	void InitListCtrl_Common(CXListCtrl &list);
	void SelectTab(int nTab);
	void SelectTabView(int nTab);

	void AdjustHorzScroll(CListBox& wndListBox);

// �����Դϴ�.
public:
	virtual ~COutputWnd();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

