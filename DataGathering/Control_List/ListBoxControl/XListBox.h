#if !defined(AFX_XLISTBOX_H__2251F25D_7299_4FAA_A108_EB291D7E6625__INCLUDED_)
#define AFX_XLISTBOX_H__2251F25D_7299_4FAA_A108_EB291D7E6625__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXListBox window

class CXListBox : public CListBox
{
// Construction
public:
	CXListBox();

// Attributes
public:

// Operations
public:
	void AddEntry(LPCTSTR lpszFrom, LPCTSTR lpszItem, COLORREF color = RGB(0, 0, 0));
	void AddEntry_2(LPCTSTR lpszItem, COLORREF color = RGB(0, 0, 0));
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void SetStringCount(int n)
	{
		m_nStringCnt = n;
	}
	void SetFont(LOGFONT lf)
	{
		m_font.CreateFontIndirect(&lf);
		CListBox::SetFont(&m_font, TRUE);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXListBox)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CFont	m_font;
	int		m_nStringCnt;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XLISTBOX_H__2251F25D_7299_4FAA_A108_EB291D7E6625__INCLUDED_)
