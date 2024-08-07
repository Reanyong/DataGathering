#pragma once

#include "ViewTree.h"

// CTreeWnd
class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


class CTreeWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CTreeWnd)

public:
	CTreeWnd();
	virtual ~CTreeWnd();
	
	void AdjustLayout();
protected:
	CClassToolBar m_wndToolBar;
	CViewTree m_wndTreeView;
	CImageList m_TreeViewImages;
	UINT m_nCurrSort;

	CFont			m_Font;

protected:
	void SetTreeItem();

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnNotifyDblclk(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};


