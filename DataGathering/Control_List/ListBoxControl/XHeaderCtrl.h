#ifndef XHEADERCTRL_H
#define XHEADERCTRL_H

#include <afxtempl.h>
#include <tchar.h>
#include "MyMemDC.h"

#define FLATHEADER_TEXT_MAX	80

///////////////////////////////////////////////////////////////////////////////
// CXHeaderCtrl window

class CXHeaderCtrl;

#define FH_PROPERTY_SPACING			1
#define FH_PROPERTY_ARROW			2
#define FH_PROPERTY_STATICBORDER	3
#define FH_PROPERTY_DONTDROPCURSOR	4
#define FH_PROPERTY_DROPTARGET		5

#define HDF_EX_AUTOWIDTH	0x0001
#define HDF_EX_INCLUDESORT	0x0002
#define HDF_EX_FIXEDWIDTH	0x0004

typedef struct _HDITEMEX
{
	UINT	nStyle;
	int		iMinWidth;
	int		iMaxWidth;
	int		iImageIndex;

	_HDITEMEX() : nStyle(0), iMinWidth(0), iMaxWidth(-1), iImageIndex(-1) {};

} HDITEMEX, FAR* LPHDITEMEX;

class CXHeaderCtrl : public CHeaderCtrl
{
    DECLARE_DYNCREATE(CXHeaderCtrl)

// Construction
public:
	CXHeaderCtrl();
	virtual ~CXHeaderCtrl();

// Attributes
public:
	BOOL ModifyProperty(WPARAM wParam, LPARAM lParam);

// Overrides
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void DrawItem(CDC* pDC, CRect rect, LPHDITEM lphdi);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bDoubleBuffer;
	int m_iSpacing;
	SIZE m_sizeImage;
	SIZE m_sizeArrow;
	BOOL m_bStaticBorder;
	UINT m_nDontDropCursor;
	BOOL m_bResizing;
	UINT m_nClickFlags;
	CPoint m_ptClickPoint;

	CArray<HDITEMEX, HDITEMEX> m_arrayHdrItemEx;

	COLORREF m_cr3DHighLight;
	COLORREF m_cr3DShadow;
	COLORREF m_cr3DFace;
	COLORREF m_crText;

	void DrawCtrl(CDC* pDC);
	int DrawImage(CDC* pDC, CRect rect, LPHDITEM hdi, BOOL bRight);
	int DrawBitmap(CDC* pDC, CRect rect, LPHDITEM hdi, CBitmap* pBitmap, BITMAP* pBitmapInfo, BOOL bRight);
	int DrawText (CDC* pDC, CRect rect, LPHDITEM lphdi);

// Generated message map functions
protected:
	//{{AFX_MSG(CXHeaderCtrl)
	afx_msg LRESULT OnInsertItem(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnDeleteItem(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnSetImageList(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnLayout(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XHEADERCTRL_H
