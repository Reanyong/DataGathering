#if !defined(AFX_WNDMENUBAR_H__8AF5679A_D73D_4099_82F1_9F0D47B7771D__INCLUDED_)
#define AFX_WNDMENUBAR_H__8AF5679A_D73D_4099_82F1_9F0D47B7771D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndMenubar.h : header file
//

//#define MENUBAR_MSG


/////////////////////////////////////////////////////////////////////////////
// CWndMenubar window
class CWndMenubar : public CWnd
{
public:
	CWndMenubar();           // protected constructor used by dynamic creation
	virtual ~CWndMenubar();
	DECLARE_DYNCREATE(CWndMenubar)

// Attributes
public:
	CXTTreeCtrl m_TreeCtrl;
	CImageList m_ImageList;

	CXTPToolBar m_wndToolBar;
	UINT m_nToolbarId;

	CString m_szRoot;

	CBitmap m_bmpInfo;
	BITMAP m_bm;

	HTREEITEM m_hRoot;
	HTREEITEM m_hBranch1;
	HTREEITEM m_hBranch2;
	HTREEITEM m_hBranch3;
	HTREEITEM m_hBranch4;
	HTREEITEM m_hBranch5;
	HTREEITEM m_hBranch6;

// Operations
public:
	void SetRootString(const char *s) {
		m_szRoot = s;
	}
	void SetToolbarId(UINT ui) {
		m_nToolbarId = ui;
	}

	void RemoveTreeItrem(HTREEITEM hItem);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndMenubar)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//afx_msg void OnFilterColor(UINT nID);
	//afx_msg void OnUpdateFilterColor(CCmdUI* pCmd);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndMenubar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg BOOL OnNotifyDblclk(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNotifyRclk(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdtDesc(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMENUBAR_H__8AF5679A_D73D_4099_82F1_9F0D47B7771D__INCLUDED_)
