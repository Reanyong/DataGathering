#if !defined(AFX_TREEVIEWPANE_H__9971E8E2_6143_46EA_9715_0FE94E88F3F8__INCLUDED_)
#define AFX_TREEVIEWPANE_H__9971E8E2_6143_46EA_9715_0FE94E88F3F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeViewPane window
#include <Afxcview.h>
class CTreeViewPane : public CWnd
{
// Construction
public:
	CTreeViewPane();

// Attributes
public:

	CVisualStudioTreeCtrl m_wndClassView;
	CImageList m_ilClassView;
	CXTPToolBar m_wndToolBar;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewPane)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeViewPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWPANE_H__9971E8E2_6143_46EA_9715_0FE94E88F3F8__INCLUDED_)
