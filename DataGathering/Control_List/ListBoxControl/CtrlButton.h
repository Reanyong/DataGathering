#if !defined(AFX_CTRLBUTTON_H__C5EAD630_B657_44C8_877B_1ABEB0C02818__INCLUDED_)
#define AFX_CTRLBUTTON_H__C5EAD630_B657_44C8_877B_1ABEB0C02818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtrlButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCtrlButton window

class CCtrlButton : public CButton
{
// Construction
public:
	CCtrlButton(CWnd* pParent, CRect rect, UINT nID, int nRow, int nColumn);

// Attributes
public:
	int m_nRow, m_nCol;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlButton)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLBUTTON_H__C5EAD630_B657_44C8_877B_1ABEB0C02818__INCLUDED_)
