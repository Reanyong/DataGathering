﻿
#pragma once
/////////////////////////////////////////////////////////////////////////////
// COutputList 창

#include "Control_List/ListBoxControl/XListCtrl.h"

class COutputList : public CListBox
{
// 생성입니다.
public:
	COutputList();

// 구현입니다.
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
// 생성입니다.
public:
	COutputWnd();

// 특성입니다.
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

// 구현입니다.
public:
	virtual ~COutputWnd();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

