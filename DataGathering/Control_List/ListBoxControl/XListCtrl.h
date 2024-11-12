#ifndef XLISTCTRL_H
#define XLISTCTRL_H

#include "XHeaderCtrl.h"
#include "XComboList.h"

///////////////////////////////////////////////////////////////////////////////
// CXListCtrl window

struct XLISTCTRLDATA
{
	// ctor
	XLISTCTRLDATA()
	{
		bEnabled             = TRUE;
		crText               = ::GetSysColor(COLOR_WINDOWTEXT);
		crBackground         = ::GetSysColor(COLOR_WINDOW);
		bShowProgress        = FALSE;
		nProgressPercent     = 0;
		strProgressMessage   = _T("");
		bShowProgressMessage = TRUE;
		nCheckedState        = -1;
		nCheckEnable		 = 1;
		bBold                = FALSE;
		bCombo               = FALSE;
		bComboIsClicked      = FALSE;
		nComboListHeight     = 10;
		nInitialComboSel     = 0;
		psa                  = NULL;
		dwItemData           = 0;

		bButton = FALSE;
		pButton = NULL;
	}

	BOOL			bEnabled;				// TRUE = enabled, FALSE = disabled (gray text)
	BOOL			bBold;					// TRUE = display bold text

	// for combo
	BOOL			bCombo;					// TRUE = display combobox
	BOOL			bComboIsClicked;		// TRUE = downarrow is clicked
	CStringArray *	psa;					// pointer to string array for combo listbox
	int				nComboListHeight;		// combo listbox height (in rows)
	int				nInitialComboSel;		// initial combo listbox selection (0 = first)

	BOOL			bButton;

	// for color
	COLORREF	crText;
	COLORREF	crBackground;
	CButton		*pButton;

	// for progress
	BOOL		bShowProgress;				// true = show progress control
	int			nProgressPercent;			// 0 - 100
	CString		strProgressMessage;			// custom message for progress indicator -
											// MUST INCLUDE %d
	BOOL		bShowProgressMessage;		// TRUE = display % message, or custom message
											// if one is supplied
	// for checkbox
	int			nCheckedState;				// -1 = don't show, 0 = unchecked, 1 = checked
	int			nCheckEnable;

	DWORD		dwItemData;					// pointer to app's data
};


class CXListCtrl : public CListCtrl
{
// Construction
public:
	CXListCtrl();
	virtual ~CXListCtrl();

// Attributes
public:
	CEdit* m_pEdit;  // Edit control 포인터
	int m_nEditItem; // 편집중인 아이템
	int m_nEditSubItem; // 편집중인 서브 아이템


// Operations
public:
	int		CountCheckedItems(int nSubItem);
	BOOL	DeleteAllItems();
	BOOL	DeleteItem(int nItem);
	void	DeleteProgress(int nItem, int nSubItem);
	BOOL	GetBold(int nItem, int nSubItem);
	int		GetCheckbox(int nItem, int nSubItem);
	int		GetColumns();
	CString	GetComboText(int iItem, int iSubItem);
	int		GetCurSel();
	BOOL	GetEnabled(int nItem);
	int		GetHeaderCheckedState(int nSubItem);
	DWORD	GetItemData(int nItem);
	BOOL	GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& rect);
	int		InsertItem(int nItem, LPCTSTR lpszItem);
	int		InsertItem(int nItem,
					   LPCTSTR lpszItem,
					   COLORREF crText,
					   COLORREF crBackground);
	int		InsertItem(const LVITEM* pItem);
	BOOL	SetBold(int nItem, int nSubItem, BOOL bBold);
	BOOL	SetComboBox(int nItem,
						int nSubItem,
						BOOL bFlag,
						CStringArray *psa,
						int nComboListHeight,
						int nInitialComboSel);
	BOOL	SetCheckBoxEnable(int nItem, int nSubItem, int nEnable);
	BOOL	SetCheckbox(int nItem, int nSubItem, int nCheckedState);
	BOOL	SetButton(int nItem, int nSubItem, int bFlag, BOOL bEnable);
	BOOL	SetCurSel(int nItem);
	BOOL	SetEnabled(int nItem, BOOL bEnable);
	BOOL	SetHeaderCheckedState(int nSubItem, int nCheckedState);
	BOOL	SetItemData(int nItem, DWORD dwData);
	BOOL	SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	BOOL	SetItemText(int nItem,
						int nSubItem,
						LPCTSTR lpszText,
						COLORREF crText,
						COLORREF crBackground);
	BOOL	SetProgress(int nItem,
						int nSubItem,
						BOOL bShowProgressText = TRUE,
						LPCTSTR lpszProgressText = NULL);
	void	UpdateProgress(int nItem, int nSubItem, int nPercent);
	void	UpdateSubItem(int nItem, int nSubItem);

	//Set the message that will be shown if the list is empty
	void SetEmptyMessage(UINT ID);
	void SetEmptyMessage(const CString& message);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrl)
	public:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	//}}AFX_VIRTUAL

// Implementation
public:
	CXHeaderCtrl	m_HeaderCtrl;
	CImageList		m_cImageList;	// Image list for the header control

protected:
	void DrawCheckbox(int nItem,
					  int nSubItem,
					  CDC *pDC,
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect,
					  XLISTCTRLDATA *pCLD);
	void DrawComboBox(int nItem,
					  int nSubItem,
					  CDC *pDC,
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect,
					  XLISTCTRLDATA *pCLD);
	void DrawProgress(int nItem,
					  int nSubItem,
					  CDC *pDC,
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect,
					  XLISTCTRLDATA *pCLD);
	void DrawText(int nItem,
				  int nSubItem,
				  CDC *pDC,
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect& rect,
				  XLISTCTRLDATA *pCLD);
	void SubclassHeaderControl();
	void UnpressComboButton();

	BOOL			m_bHeaderIsSubclassed;
	BOOL			m_bComboIsClicked;
	int				m_nComboItem;
	int				m_nComboSubItem;
	CRect			m_rectComboButton;
	CRect			m_rectComboList;
	CXComboList *	m_pListBox;
	CFont			m_ListboxFont;
	BOOL			m_bFontIsCreated;
	CString			m_strInitialComboString;
	CString m_emptyMessage;

	// Generated message map functions
protected:
	//{{AFX_MSG(CXListCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnComboEscape(UINT, LONG);
	afx_msg LONG OnComboReturn(UINT, LONG);
	afx_msg LONG OnComboKeydown(UINT, LONG);
	afx_msg LONG OnComboLButtonUp(UINT, LONG);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XLISTCTRL_H
