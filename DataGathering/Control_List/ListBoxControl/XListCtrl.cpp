#include "stdafx.h"
#include "XListCtrl.h"
#include "CtrlButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl

BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_VK_ESCAPE, OnComboEscape)
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_VK_RETURN, OnComboReturn)
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_KEYDOWN, OnComboKeydown)
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_LBUTTONUP, OnComboLButtonUp)
	ON_WM_CREATE()
	ON_WM_MEASUREITEM()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MEASUREITEM_REFLECT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////
// ctor
CXListCtrl::CXListCtrl()
{
	m_bComboIsClicked       = FALSE;
	m_nComboItem            = -1;
	m_nComboSubItem         = 0;
	m_pListBox              = NULL;
	m_bFontIsCreated        = FALSE;
	m_strInitialComboString = _T("");
	m_bHeaderIsSubclassed   = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXListCtrl::~CXListCtrl()
{
	if(m_pListBox)
		delete m_pListBox;
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	// for Dialog based applications, this is a good place
	// to subclass the header control because the OnCreate()
	// function does not get called.

	SubclassHeaderControl();
}

///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CXListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		ASSERT(FALSE);
		return -1;
	}

	// When the CXListCtrl object is created via a call to Create(), instead
	// of via a dialog box template, we must subclass the header control
	// window here because it does not exist when the PreSubclassWindow()
	// function is called.

	SubclassHeaderControl();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// SubclassHeaderControl
void CXListCtrl::SubclassHeaderControl()
{
	if (m_bHeaderIsSubclassed)
		return;

	// if the list control has a header control window, then
	// subclass it

	// Thanks to Alberto Gattegno and Alon Peleg젨and their article
	// "A Multiline Header Control Inside a CListCtrl" for easy way
	// to determine if the header control exists.

	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader)
	{
		VERIFY(m_HeaderCtrl.SubclassWindow(pHeader->m_hWnd));
		m_bHeaderIsSubclassed = TRUE;
	}
}

void CXListCtrl::SetEmptyMessage(const CString &message)
{
	m_emptyMessage = message;
}

//Set empty message
void CXListCtrl::SetEmptyMessage(UINT ID)
{
	CString temp;
	temp.LoadString(ID);

	SetEmptyMessage(temp);
}

///////////////////////////////////////////////////////////////////////////////
// OnClick
void CXListCtrl::OnClick(NMHDR*, LRESULT* pResult)
{
	UnpressComboButton();
	*pResult = 0;
}

void CXListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nItem = lpDrawItemStruct->itemID;
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *)CListCtrl::GetItemData(nItem);
	COLORREF crText  = ::GetSysColor(COLOR_WINDOWTEXT);
	COLORREF crBkgnd = ::GetSysColor(COLOR_WINDOW);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rect;
	for (int nSubItem = 0; nSubItem < nColumnCount; nSubItem++)
	{
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

		if (pXLCD)
		{
			crText  = pXLCD[nSubItem].crText;
			crBkgnd = pXLCD[nSubItem].crBackground;

			if (!pXLCD[0].bEnabled)
				crText = ::GetSysColor(COLOR_GRAYTEXT);
		}

		if (pXLCD && (pXLCD[nSubItem].bShowProgress))
		{
			DrawProgress(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);
		}
		else if (pXLCD && (pXLCD[nSubItem].bCombo))
		{
			int nSelectedItem = GetCurSel();

			if (nSelectedItem == nItem)
				DrawComboBox(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);
			else
				DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);
		}
		else if (pXLCD && (pXLCD[nSubItem].bButton))
		{
			CButton *pButton = pXLCD[nSubItem].pButton;
			ASSERT(pButton);
			if (pButton)
			{
				rect.left++;
				pButton->MoveWindow(rect);
			}
		}
		else if (pXLCD && (pXLCD[nSubItem].nCheckedState != -1))
		{
			DrawCheckbox(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);
		}
		else
		{
			DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnCustomDraw
void CXListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) pLVCD->nmcd.lItemlParam;

		COLORREF crText  = ::GetSysColor(COLOR_WINDOWTEXT);
		COLORREF crBkgnd = ::GetSysColor(COLOR_WINDOW);

		if (pXLCD)
		{
			crText  = pXLCD[nSubItem].crText;
			crBkgnd = pXLCD[nSubItem].crBackground;

			if (!pXLCD[0].bEnabled)
				crText = ::GetSysColor(COLOR_GRAYTEXT);
		}

		// store the colors back in the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;

		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

		if (pXLCD && (pXLCD[nSubItem].bShowProgress))
		{
			DrawProgress(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if (pXLCD && (pXLCD[nSubItem].bCombo))
		{
			int nSelectedItem = GetCurSel();

			if (nSelectedItem == nItem)
				DrawComboBox(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);
			else
				DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if (pXLCD && (pXLCD[nSubItem].bButton))
		{
			CButton *pButton = pXLCD[nSubItem].pButton;
			ASSERT(pButton);
			if (pButton)
			{
				rect.left++;
				pButton->MoveWindow(rect);
				pButton->Invalidate();
			}

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if (pXLCD && (pXLCD[nSubItem].nCheckedState != -1))
		{
			DrawCheckbox(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else
		{
			DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawProgress
void CXListCtrl::DrawProgress(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	UNUSED_ALWAYS(nItem);

	ASSERT(pDC);
	ASSERT(pXLCD);

	rect.bottom -= 1;
	rect.left += 1;		// leave margin in case row is highlighted
	rect.right -= 2;

	// draw border

	CPen graypen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
	CPen *pOldPen = pDC->SelectObject(&graypen);

	pDC->MoveTo(rect.left, rect.bottom);
	pDC->LineTo(rect.right+1, rect.bottom);

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);

	pDC->MoveTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);

	// fill interior with light gray
	CRect InteriorRect;
	InteriorRect = rect;
	InteriorRect.left += 1;
	InteriorRect.top += 1;
	pDC->FillSolidRect(InteriorRect, RGB(224,224,224));

	// finish drawing border
	CPen blackpen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&blackpen);

	pDC->MoveTo(rect.left+1, rect.top+1);
	pDC->LineTo(rect.right, rect.top+1);

	pDC->MoveTo(rect.left+1, rect.top+1);
	pDC->LineTo(rect.left+1, rect.bottom);

	pDC->SelectObject(pOldPen);

	if (pXLCD[nSubItem].nProgressPercent > 0)
	{
		// draw progress bar and text

		CRect LeftRect, RightRect;
		LeftRect = rect;
		LeftRect.left += 2;
		LeftRect.top += 2;
		RightRect = LeftRect;
		int w = (LeftRect.Width() * pXLCD[nSubItem].nProgressPercent) / 100;
		LeftRect.right = LeftRect.left + w;
		RightRect.left = LeftRect.right + 1;
		pDC->FillSolidRect(LeftRect, ::GetSysColor(COLOR_HIGHLIGHT));

		if (pXLCD[nSubItem].bShowProgressMessage)
		{
			CString str, format;
			format = pXLCD[nSubItem].strProgressMessage;
			if (format.IsEmpty())
				str.Format(_T("%d%%"), pXLCD[nSubItem].nProgressPercent);
			else
				str.Format(format, pXLCD[nSubItem].nProgressPercent);

			pDC->SetBkMode(TRANSPARENT);

			CRect TextRect;
			TextRect = rect;
			TextRect.DeflateRect(1, 1);
			TextRect.top += 1;

			CRgn rgn;
			rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, LeftRect.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->SetTextColor(crBkgnd);
			pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rgn.DeleteObject();
			rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, RightRect.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->SetTextColor(crText);
			pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			rgn.DeleteObject();
			pDC->SelectClipRgn(NULL);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawComboBox
void CXListCtrl::DrawComboBox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	UNUSED_ALWAYS(crText);
	UNUSED_ALWAYS(crBkgnd);

	ASSERT(pDC);
	ASSERT(pXLCD);

	rect.bottom += 1;	// bottom edge is white, so this doesn't matter
	rect.left += 1;		// leave margin in case row is highlighted
	rect.right -= 2;

	// draw border

	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
	CPen *pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rect.left, rect.bottom-2);
	pDC->LineTo(rect.right, rect.bottom-2);

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom-2);

	pDC->MoveTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom-1);

	CPen blackpen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&blackpen);

	// fill interior with white
	CRect InteriorRect;
	InteriorRect = rect;
	InteriorRect.DeflateRect(2, 2);
	pDC->FillSolidRect(InteriorRect, RGB(255,255,255));

	// set arrow rect
	CRect ArrowRect;
	ArrowRect = rect;
	ArrowRect.right += 1;
	ArrowRect.left = ArrowRect.right - ArrowRect.Height();
	ArrowRect.DeflateRect(2, 2);

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (str.IsEmpty())
	{
		// subitem text is empty, try to get from listbox strings
		if (pXLCD[nSubItem].psa)
		{
			int index = 0;
			if ((pXLCD[nSubItem].nInitialComboSel >= 0) &&
				(pXLCD[nSubItem].psa->GetSize() > pXLCD[nSubItem].nInitialComboSel))
			{
				index = pXLCD[nSubItem].nInitialComboSel;
				str = pXLCD[nSubItem].psa->GetAt(index);
				SetItemText(nItem, nSubItem, str);
			}
		}
	}

	if (!str.IsEmpty())
	{
		// draw text
		CRect TextRect;
		TextRect = rect;
		TextRect.top -= 1;
		TextRect.left += 2;
		TextRect.right = ArrowRect.left - 1;

		pDC->SetBkMode(TRANSPARENT);
		COLORREF cr = ::GetSysColor(COLOR_WINDOWTEXT);
		if (!pXLCD[0].bEnabled)
			cr = ::GetSysColor(COLOR_GRAYTEXT);
		pDC->SetTextColor(cr);
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		UINT nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		pDC->DrawText(str, &TextRect, nFormat);
	}

	if (!pXLCD[nSubItem].bComboIsClicked)
	{
		// draw depressed combobox
		pDC->DrawEdge(&ArrowRect, EDGE_RAISED, BF_RECT);
		ArrowRect.DeflateRect(2, 2);
		pDC->FillSolidRect(ArrowRect, ::GetSysColor(COLOR_BTNFACE));

		// draw the downarrow using blackpen
		int x = ArrowRect.left + 1;
		int y = ArrowRect.top + 2;
		int k = 5;
		for (int i = 0; i < 3; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x+k, y);
			x++;
			y++;
			k -= 2;
		}
	}
	else
	{
		// draw normal combobox
		m_rectComboButton = ArrowRect;
		CBrush brush(::GetSysColor(COLOR_3DSHADOW));
		pDC->FrameRect(&ArrowRect, &brush);
		ArrowRect.DeflateRect(1, 1);
		pDC->FillSolidRect(ArrowRect, ::GetSysColor(COLOR_BTNFACE));

		// draw the downarrow using blackpen
		int x = ArrowRect.left + 3;
		int y = ArrowRect.top + 4;
		int k = 5;
		for (int i = 0; i < 3; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x+k, y);
			x++;
			y++;
			k -= 2;
		}

		// show listbox if not already shown
		if (!m_pListBox)
		{
			// create and populate the combo's listbox
			m_pListBox = new CXComboList(this);
			ASSERT(m_pListBox);

			if (m_pListBox)
			{
				m_rectComboList = rect;
				m_rectComboList.right -= 1;
				m_rectComboList.top += rect.Height() - 1;

				m_rectComboList.bottom = m_rectComboList.top +
					(pXLCD[nSubItem].nComboListHeight) * (rect.Height() - 2);
				ClientToScreen(&m_rectComboList);

				CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS,
														  LoadCursor(NULL, IDC_ARROW));

				BOOL bSuccess = m_pListBox->CreateEx(0, szClassName, _T(""),
									WS_POPUP | WS_VISIBLE /*| WS_VSCROLL*/ | WS_BORDER,
									m_rectComboList,
									this, 0, NULL);

				if (!bSuccess)
				{
				}
				else
				{
					m_strInitialComboString = _T("");

					if (!m_bFontIsCreated)
					{
						// use font from list control
						CFont *font = pDC->GetCurrentFont();
						if (font)
						{
							LOGFONT lf;
							font->GetLogFont(&lf);
							m_ListboxFont.CreateFontIndirect(&lf);
							m_bFontIsCreated = TRUE;
						}
					}

					if (m_bFontIsCreated)
						m_pListBox->SetFont(&m_ListboxFont, FALSE);

					if (pXLCD[nSubItem].psa)
					{
						CString s;
						for (int i = 0; i < pXLCD[nSubItem].psa->GetSize(); i++)
						{
							s = pXLCD[nSubItem].psa->GetAt(i);
							if (!s.IsEmpty())
								m_pListBox->AddString(s);
						}
					}

					int index = 0;
					if (str.IsEmpty())
					{
						// str is empty, try to get from first listbox string
						if (m_pListBox->GetCount() > 0)
							m_pListBox->GetText(0, str);

						SetItemText(nItem, nSubItem, str);
					}
					else
					{
						// set listbox selection from subitem text
						index = m_pListBox->FindStringExact(-1, str);
						if (index == LB_ERR)
							index = 0;
					}
					m_pListBox->SetCurSel(index);
					m_pListBox->GetText(index, m_strInitialComboString);
					m_pListBox->SetActive();
				}
			}
		}
	}
	pDC->SelectObject(pOldPen);
}

///////////////////////////////////////////////////////////////////////////////
// DrawCheckbox
void CXListCtrl::DrawCheckbox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	ASSERT(pDC);
	ASSERT(pXLCD);

	int nSelectedItem = GetCurSel();

	if (nSelectedItem == nItem)
	{
		// selected?  if so, draw highlight background
		crText  = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		crBkgnd = ::GetSysColor(COLOR_HIGHLIGHT);

		// has focus?  if not, draw gray background
		if (m_hWnd != ::GetFocus())
		{
			crText  = ::GetSysColor(COLOR_WINDOWTEXT);
			crBkgnd = ::GetSysColor(COLOR_BTNFACE);
		}
	}

	pDC->FillSolidRect(&rect, crBkgnd);

	CRect chkboxrect;
	chkboxrect = rect;
	chkboxrect.bottom -= 1;
	chkboxrect.left += 9;		// line up checkbox with header checkbox
	chkboxrect.right = chkboxrect.left + chkboxrect.Height();	// width = height

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (str.IsEmpty())
	{
		// center the checkbox

		chkboxrect.left = rect.left + rect.Width()/2 - chkboxrect.Height()/2 - 1;
		chkboxrect.right = chkboxrect.left + chkboxrect.Height();
	}

	// fill rect around checkbox with white
	pDC->FillSolidRect(&chkboxrect, ::GetSysColor(COLOR_WINDOW));

	chkboxrect.left += 1;
	chkboxrect.right += 2;

	// draw border
	pDC->DrawEdge(&chkboxrect, EDGE_SUNKEN, BF_RECT);

	if (pXLCD[nSubItem].nCheckedState == 1)
	{
		CPen *pOldPen = NULL;

		CPen graypen(PS_SOLID, 1, ::GetSysColor(COLOR_GRAYTEXT));
		CPen blackpen(PS_SOLID, 1, RGB(0,0,0));

		if (pXLCD[0].bEnabled)
			pOldPen = pDC->SelectObject(&blackpen);
		else
			pOldPen = pDC->SelectObject(&graypen);

		// draw the checkmark
		int x = chkboxrect.left + 9;
		ASSERT(x < chkboxrect.right);
		int y = chkboxrect.top + 3;
		int i;
		for (i = 0; i < 4; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y++;
		}
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y--;
		}

		if (pOldPen)
			pDC->SelectObject(pOldPen);
	}

	if (!str.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		CRect textrect;
		textrect = rect;
		textrect.left = chkboxrect.right + 4;

		pDC->DrawText(str, &textrect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawText
void CXListCtrl::DrawText(int nItem,
						  int nSubItem,
						  CDC *pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect& rect,
						  XLISTCTRLDATA *pXLCD)
{
	ASSERT(pDC);
//	ASSERT(pXLCD);

	int nSelectedItem = GetCurSel();

	int nState = GetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED);

	//if (nSelectedItem == nItem)
	//if (nState & (LVIS_SELECTED|LVIS_FOCUSED))
	if (nState & (LVIS_SELECTED))
	{
		// selected?  if so, draw highlight background
		crText  = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		crBkgnd = ::GetSysColor(COLOR_HIGHLIGHT);

		// has focus?  if not, draw gray background
		if (m_hWnd != ::GetFocus())
		{
			crText  = ::GetSysColor(COLOR_WINDOWTEXT);
			crBkgnd = ::GetSysColor(COLOR_BTNFACE);
		}
	}

	pDC->FillSolidRect(&rect, crBkgnd);

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (!str.IsEmpty())
	{
		// get text justification
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		m_HeaderCtrl.GetItem(nSubItem, &hditem);
		int nFmt = hditem.fmt & HDF_JUSTIFYMASK;
		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if (nFmt == HDF_CENTER)
			nFormat |= DT_CENTER;
		else if (nFmt == HDF_LEFT)
			nFormat |= DT_LEFT;
		else
			nFormat |= DT_RIGHT;

		CFont *pOldFont = NULL;
		CFont boldfont;

		CFont *font = pDC->GetCurrentFont();
		LOGFONT lf;
		font->GetLogFont(&lf);

		// check if bold specified for subitem
		if (pXLCD && pXLCD[nSubItem].bBold)
		{
			if (font)
			{
				LOGFONT lf;
				font->GetLogFont(&lf);
				lf.lfWeight = FW_BOLD;
				boldfont.CreateFontIndirect(&lf);
				pOldFont = pDC->SelectObject(&boldfont);
			}
		}
		else
		{
			if (font)
				pOldFont = pDC->SelectObject(font);
		}
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		rect.left += 3;
		pDC->DrawText(str, &rect, nFormat);
		if (pOldFont)
			pDC->SelectObject(pOldFont);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetSubItemRect
BOOL CXListCtrl::GetSubItemRect(int nItem,
								int nSubItem,
								int nArea,
								CRect& rect)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL bRC = CListCtrl::GetSubItemRect(nItem, nSubItem, nArea, rect);

	// if nSubItem == 0, the rect returned by CListCtrl::GetSubItemRect
	// is the entire row, so use left edge of second subitem

	if (nSubItem == 0)
	{
		if (GetColumns() > 1)
		{
			CRect rect1;
			bRC = GetSubItemRect(nItem, 1, LVIR_BOUNDS, rect1);
			rect.right = rect1.left;
		}
	}

	return bRC;
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nItem = HitTest(point, NULL);
	if (nItem == -1)
	{
		OnComboEscape(0, 0);
	}
	else
	{

#if 0  // -----------------------------------------------------------
		SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED,
			LVIS_SELECTED | LVIS_FOCUSED);
#endif // -----------------------------------------------------------

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			CListCtrl::OnLButtonDown(nFlags, point);
			return;
		}

		if (!pXLCD[0].bEnabled)
		{
			CListCtrl::OnLButtonDown(nFlags, point);
			return;
		}

		CRect rect;

		int nSubItem = -1;

		// check if a subitem checkbox was clicked

		/*
		int nState = GetItemState(nItem, 0xFFFFFFFF);
		if (nState & LVIS_SELECTED)
		{
			nState &= ~LVIS_SELECTED;
			SetItemState(nItem, nState, LVIS_SELECTED);
		}
		*/

		for (int i = 0; i < GetColumns(); i++)
		{
			GetSubItemRect(nItem, i, LVIR_BOUNDS, rect);
			if (rect.PtInRect(point))
			{
				nSubItem = i;
				break;
			}
		}

		if (nSubItem == -1)
		{
			if (m_pListBox)
			{
				OnComboEscape(0, 0);
			}
		}
		else
		{
			// -1 = no checkbox for this subitem
			if (pXLCD[nSubItem].nCheckedState >= 0 && pXLCD[nSubItem].nCheckEnable)
			{
				int nChecked = pXLCD[nSubItem].nCheckedState;

				nChecked = (nChecked == 0) ? 1 : 0;

				/*
				if(nChecked)
				{
					for(int n = 0 ; n < GetItemCount(); n++)
					{
						if(n != nItem)
						{
							SetCheckbox(n, nSubItem, 0);
						}
					}
				}
				*/

				pXLCD[nSubItem].nCheckedState = nChecked;

				UpdateSubItem(nItem, nSubItem);

				// now update checkbox in header

				// -1 = no checkbox in column header
				if (GetHeaderCheckedState(nSubItem) != -1)
				{
					int nCheckedCount = CountCheckedItems(nSubItem);

					if (nCheckedCount == GetItemCount())
						SetHeaderCheckedState(nSubItem, 1);
					else
						SetHeaderCheckedState(nSubItem, 0);
				}
			}
			else if (pXLCD[nSubItem].bCombo)
			{
				if (m_pListBox)
				{
					m_pListBox->DestroyWindow();
					delete m_pListBox;
					m_pListBox = NULL;
				}

				rect.left = rect.right - rect.Height();
				if (point.x >= rect.left && point.y <= rect.right)
				{
					pXLCD[nSubItem].bComboIsClicked = TRUE;
					m_bComboIsClicked = TRUE;
					m_nComboItem = nItem;
					m_nComboSubItem = nSubItem;
					UpdateSubItem(nItem, nSubItem);
					SetTimer(1, 100, NULL);
				}
			}
			else if (m_pListBox)
			{
				OnComboEscape(0, 0);
			}
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXListCtrl::OnPaint()
{
    Default();
	if (GetItemCount() <= 0)
	{
		CDC* pDC = GetDC();
		int nSavedDC = pDC->SaveDC();

		CRect rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);
		CHeaderCtrl* pHC = GetHeaderCtrl();
		if (pHC != NULL)
		{
			CRect rcH;
			pHC->GetItemRect(0, &rcH);
			rc.top += rcH.bottom;
		}
		rc.top += 10;
		CString strText;


		/*
		CFont	font;
		if(!font.CreateFont(9,0, 0,0,FW_NORMAL,FALSE,FALSE,FALSE,
						DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						DEFAULT_PITCH,"Arial"))
		{
			font.DeleteObject();
			return;
		}
		CFont* ptrFont = pDC->SelectObject(&font);
		*/
		CFont* ptrFont = pDC->SelectObject(GetFont());

		strText = m_emptyMessage;

		COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT);
		COLORREF crBkgnd = ::GetSysColor(COLOR_WINDOW);

		CBrush brush(crBkgnd);
		pDC->FillRect(rc, &brush);

		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->SelectStockObject(ANSI_VAR_FONT);
		pDC->DrawText(strText, -1, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

		pDC->SelectObject(ptrFont);
		//font.DeleteObject();
		pDC->RestoreDC(nSavedDC);
		ReleaseDC(pDC);
	}
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(const LVITEM* pItem)
{
	return CListCtrl::InsertItem(pItem);
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	ASSERT(nItem >= 0);
	if (nItem < 0)
		return -1;

	return InsertItem(nItem,
					  lpszItem,
					  ::GetSysColor(COLOR_WINDOWTEXT),
					  ::GetSysColor(COLOR_WINDOW));
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(int nItem,
						   LPCTSTR lpszItem,
						   COLORREF crText,
						   COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	if (nItem < 0)
		return -1;

	int index = CListCtrl::InsertItem(nItem, lpszItem);

	if (index < 0)
		return index;

	XLISTCTRLDATA *pXLCD = new XLISTCTRLDATA [GetColumns()];
	ASSERT(pXLCD);
	if (!pXLCD)
		return -1;

	pXLCD[0].crText       = crText;
	pXLCD[0].crBackground = crBackground;

	CListCtrl::SetItemData(index, (DWORD) pXLCD);

	return index;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemText
BOOL CXListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
//	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL rc = CListCtrl::SetItemText(nItem, nSubItem, lpszText);

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemText
//
// This function will set the text and colors for a subitem.  If lpszText
// is NULL, only the colors will be set.  If a color value is -1, the display
// color will be set to the default Windows color.
//
BOOL CXListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText,
					COLORREF crText, COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	BOOL rc = TRUE;

	if (nItem < 0)
		return FALSE;

	if (lpszText)
		rc = CListCtrl::SetItemText(nItem, nSubItem, lpszText);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].crText       = (crText == -1) ? ::GetSysColor(COLOR_WINDOWTEXT) : crText;
		pXLCD[nSubItem].crBackground = (crBackground == -1) ? ::GetSysColor(COLOR_WINDOW) : crBackground;
	}

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteItem
BOOL CXListCtrl::DeleteItem(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
		delete [] pXLCD;
	CListCtrl::SetItemData(nItem, 0);
	return CListCtrl::DeleteItem(nItem);
}

///////////////////////////////////////////////////////////////////////////////
// DeleteAllItems
BOOL CXListCtrl::DeleteAllItems()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(i);
		if (pXLCD)
			delete [] pXLCD;
		CListCtrl::SetItemData(i, 0);
	}

	return CListCtrl::DeleteAllItems();
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXListCtrl::OnDestroy()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(i);
		if (pXLCD)
			delete [] pXLCD;
		CListCtrl::SetItemData(i, 0);
	}

	CListCtrl::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// SetProgress
//
// This function creates a progress bar in the specified subitem.  The
// UpdateProgress function may then be called to update the progress
// percent.  If bShowProgressText is TRUE, either the default text
// of "n%" or the custom percent text (lpszProgressText) will be
// displayed.  If bShowProgressText is FALSE, only the progress bar
// will be displayed, with no text.
//
// Note that the lpszProgressText string should include the format
// specifier "%d":  e.g., "Pct %d%%"
//
BOOL CXListCtrl::SetProgress(int nItem,
							 int nSubItem,
							 BOOL bShowProgressText /*= TRUE*/,
							 LPCTSTR lpszProgressText /*= NULL*/)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].bShowProgress        = TRUE;
	pXLCD[nSubItem].nProgressPercent     = 0;
	pXLCD[nSubItem].bShowProgressMessage = bShowProgressText;
	pXLCD[nSubItem].strProgressMessage   = lpszProgressText;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteProgress
void CXListCtrl::DeleteProgress(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return;
	}

	pXLCD[nSubItem].bShowProgress = FALSE;
	pXLCD[nSubItem].nProgressPercent = 0;

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// UpdateProgress
void CXListCtrl::UpdateProgress(int nItem, int nSubItem, int nPercent)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	ASSERT(nPercent >= 0 && nPercent <= 100);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return;
	}

	pXLCD[nSubItem].nProgressPercent = nPercent;

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// SetCheckbox
BOOL CXListCtrl::SetCheckBoxEnable(int nItem, int nSubItem, int nEnable)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].nCheckEnable = nEnable;

	return TRUE;
}

BOOL CXListCtrl::SetCheckbox(int nItem, int nSubItem, int nCheckedState)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;
	ASSERT(nCheckedState == 0 || nCheckedState == 1 || nCheckedState == -1);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}
	if (pXLCD[nSubItem].nCheckEnable == 0)
		return FALSE;

	// update checkbox in subitem

	pXLCD[nSubItem].nCheckedState = nCheckedState;

	UpdateSubItem(nItem, nSubItem);

	// now update checkbox in column header

	// -1 = no checkbox in column header
	if (GetHeaderCheckedState(nSubItem) != -1)
	{
		int nCheckedCount = CountCheckedItems(nSubItem);

		if (nCheckedCount == GetItemCount())
			SetHeaderCheckedState(nSubItem, 1);
		else
			SetHeaderCheckedState(nSubItem, 0);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetCheckbox
int CXListCtrl::GetCheckbox(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return -1;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return -1;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return -1;
	}

	return pXLCD[nSubItem].nCheckedState;
}

///////////////////////////////////////////////////////////////////////////////
// GetEnabled
//
// Note that GetEnabled and SetEnabled only Get/Set the enabled flag from
// subitem 0, since this is a per-row flag.
//
BOOL CXListCtrl::GetEnabled(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	return pXLCD[0].bEnabled;
}

///////////////////////////////////////////////////////////////////////////////
// SetEnabled
BOOL CXListCtrl::SetEnabled(int nItem, BOOL bEnable)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[0].bEnabled = bEnable;

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);
	InvalidateRect(&rect);
	UpdateWindow();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetBold
BOOL CXListCtrl::SetBold(int nItem, int nSubItem, BOOL bBold)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update bold flag
	pXLCD[nSubItem].bBold = bBold;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetBold
BOOL CXListCtrl::GetBold(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update bold flag
	return pXLCD[nSubItem].bBold;
}

///////////////////////////////////////////////////////////////////////////////
// SetComboBox
//
// Note:  SetItemText may also be used to set the initial combo selection.
//
BOOL CXListCtrl::SetComboBox(int nItem,
							 int nSubItem,
							 BOOL bFlag,
							 CStringArray *psa,
							 int nComboListHeight,
							 int nInitialComboSel)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update flag
	pXLCD[nSubItem].bCombo = bFlag;

	if (bFlag)
	{
		pXLCD[nSubItem].psa = psa;
		pXLCD[nSubItem].nComboListHeight = nComboListHeight;
		pXLCD[nSubItem].nInitialComboSel = nInitialComboSel;

		if (pXLCD[nSubItem].psa)
		{
			int index = 0;
			if ((pXLCD[nSubItem].nInitialComboSel >= 0) &&
				(pXLCD[nSubItem].psa->GetSize() > pXLCD[nSubItem].nInitialComboSel))
			{
				index = pXLCD[nSubItem].nInitialComboSel;
				CString str;
				str = pXLCD[nSubItem].psa->GetAt(index);
				SetItemText(nItem, nSubItem, str);
			}
		}
	}

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// GetComboText
//
// Actually this does nothing more than GetItemText()
//
CString	CXListCtrl::GetComboText(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return _T("");
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return _T("");

	CString str;
	str = _T("");

	str = GetItemText(nItem, nSubItem);

	return str;
}

BOOL CXListCtrl::SetButton(int nItem, int nSubItem, int bFlag, BOOL bEnable)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update flag
	pXLCD[nSubItem].bButton = bFlag;

	if (bFlag)
	{
		if (pXLCD[nSubItem].pButton == NULL)
		{
			CRect rect;
			GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
			//pXLCD[nSubItem].pButton = (CButton*)new CCtrlButton(this, rect, IDC_INPLACE_CONTROL, nItem, nSubItem);
			pXLCD[nSubItem].pButton = (CButton*)new CCtrlButton(this, rect, 30000, nItem, nSubItem);
		}
		pXLCD[nSubItem].pButton->EnableWindow(bEnable);
	}
	else
	{
		if (pXLCD[nSubItem].pButton)
		{
			pXLCD[nSubItem].pButton->DestroyWindow();
			pXLCD[nSubItem].pButton = NULL;
		}
	}
	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetCurSel
BOOL CXListCtrl::SetCurSel(int nItem)
{
	return SetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED,
		LVIS_FOCUSED | LVIS_SELECTED);
}

///////////////////////////////////////////////////////////////////////////////
// GetCurSel - returns selected item number, or -1 if no item selected
//
// Note:  for single-selection lists only
//
int CXListCtrl::GetCurSel()
{
	POSITION pos = GetFirstSelectedItemPosition();
	int nSelectedItem = -1;
	if (pos != NULL)
		nSelectedItem = GetNextSelectedItem(pos);
	return nSelectedItem;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateSubItem
void CXListCtrl::UpdateSubItem(int nItem, int nSubItem)
{
//	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return;

	CRect rect;
	if (nSubItem == -1)
	{
		GetItemRect(nItem, &rect, LVIR_BOUNDS);
	}
	else
	{
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
	}

	InvalidateRect(&rect);
	UpdateWindow();
}

///////////////////////////////////////////////////////////////////////////////
// GetColumns
int CXListCtrl::GetColumns()
{
	return GetHeaderCtrl()->GetItemCount();
}

///////////////////////////////////////////////////////////////////////////////
// GetItemData
//
// The GetItemData and SetItemData functions allow for app-specific data
// to be stored, by using an extra field in the XLISTCTRLDATA struct.
//
DWORD CXListCtrl::GetItemData(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return 0;
	}

	return pXLCD->dwItemData;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemData
BOOL CXListCtrl::SetItemData(int nItem, DWORD dwData)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD->dwItemData = dwData;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetHeaderCheckedState
//
// The GetHeaderCheckedState and SetHeaderCheckedState may be used to toggle
// the checkbox in a column header.
//     0 = unchecked
//     1 = checked
//    -1 = no checkbox
//
int CXListCtrl::GetHeaderCheckedState(int nSubItem)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return -1;

	HDITEM hditem;

	// use the image index (0 or 1) to indicate the checked status
	hditem.mask = HDI_IMAGE;
	m_HeaderCtrl.GetItem(nSubItem, &hditem);
	return hditem.iImage;
}

///////////////////////////////////////////////////////////////////////////////
// SetHeaderCheckedState
BOOL CXListCtrl::SetHeaderCheckedState(int nSubItem, int nCheckedState)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return FALSE;
	ASSERT(nCheckedState == 0 || nCheckedState == 1 || nCheckedState == -1);

	HDITEM hditem;

	hditem.mask = HDI_IMAGE;
	hditem.iImage = nCheckedState;
	m_HeaderCtrl.SetItem(nSubItem, &hditem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// OnColumnClick
void CXListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pnmlv = (NMLISTVIEW*)pNMHDR;

	int nSubItem = pnmlv->iSubItem;

	int nCheckedState = GetHeaderCheckedState(nSubItem);

	// -1 = no checkbox
	if (nCheckedState != - 1)
	{
		nCheckedState = (nCheckedState == 0) ? 1 : 0;
		SetHeaderCheckedState(nSubItem, nCheckedState);

		m_HeaderCtrl.UpdateWindow();

		for (int nItem = 0; nItem < GetItemCount(); nItem++)
		{
			XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
			if (!pXLCD)
			{
				continue;
			}

			if (pXLCD[nSubItem].nCheckedState != -1)
			{
				pXLCD[nSubItem].nCheckedState = nCheckedState;
				UpdateSubItem(nItem, nSubItem);
			}
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// CountCheckedItems
int CXListCtrl::CountCheckedItems(int nSubItem)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return 0;

	int nCount = 0;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			continue;
		}

		if (pXLCD[nSubItem].nCheckedState == 1)
			nCount++;
	}

	return nCount;
}

///////////////////////////////////////////////////////////////////////////////
// UnpressComboButton
void CXListCtrl::UnpressComboButton()
{
	static BOOL bFlag = FALSE;
	if (bFlag)
		return;
	bFlag = TRUE;

	if (m_bComboIsClicked)
	{
		if (m_nComboItem >= 0 && m_nComboItem < GetItemCount())
		{
			XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(m_nComboItem);

			if (pXLCD)
			{
				if (m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns())
				{
					pXLCD[m_nComboSubItem].bComboIsClicked = FALSE;

					UpdateSubItem(m_nComboItem, m_nComboSubItem);
				}
			}
		}
	}
	m_bComboIsClicked = FALSE;
	bFlag = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
//
// Timer usage:
//    1 - used to check if combo button needs to be unpressed,set in
//        OnLButtonDown (when combo button is clicked)
//    2 - used to close combo listbox, set in OnComboEscape (user hits Escape
//        or listbox loses focus)
//    3 - used to get combo listbox selection, then close combo listbox,
//        set in OnComboReturn and OnComboLButtonUp (user hits Enter
//        or clicks on item in listbox)
//    4 - used to get combo listbox selection, set in OnComboKeydown (for
//        example, user hits arrow key in listbox)
//
void CXListCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)			// timer set when combo button is clicked
	{
		if (m_bComboIsClicked)
		{
			POINT point;
			::GetCursorPos(&point);

			ScreenToClient(&point);

			if (!m_rectComboButton.PtInRect(point))
			{
				UnpressComboButton();
			}
		}
		else if (m_pListBox)
		{
			m_pListBox->SetActive();
		}
		else
		{
			KillTimer(nIDEvent);
		}
	}
	else if (nIDEvent == 2)		// close combo listbox
	{
		KillTimer(nIDEvent);

		if (m_pListBox)
		{
			m_pListBox->DestroyWindow();
			delete m_pListBox;
		}
		m_pListBox = NULL;
	}
	else if (nIDEvent == 3)		// get combo listbox selection, then close combo listbox
	{
		KillTimer(nIDEvent);

		if (m_pListBox)
		{
			CString str;
			int i = m_pListBox->GetCurSel();
			if (i != LB_ERR)
			{
				m_pListBox->GetText(i, str);

				if ((m_nComboItem >= 0 && m_nComboItem < GetItemCount()) &&
					(m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns()))
				{
					SetItemText(m_nComboItem, m_nComboSubItem, str);

					UpdateSubItem(m_nComboItem, m_nComboSubItem);
				}
			}

			m_pListBox->DestroyWindow();
			delete m_pListBox;
		}
		m_pListBox = NULL;

		if (m_nComboItem >= 0 && m_nComboItem < GetItemCount())
		{
			SetFocus();
		}
	}
	else if (nIDEvent == 4)		// get combo listbox selection
	{
		KillTimer(nIDEvent);

		if (m_pListBox)
		{
			CString str;
			int i = m_pListBox->GetCurSel();
			if (i != LB_ERR)
			{
				m_pListBox->GetText(i, str);

				if ((m_nComboItem >= 0 && m_nComboItem < GetItemCount()) &&
					(m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns()))
				{
					SetItemText(m_nComboItem, m_nComboSubItem, str);

					UpdateSubItem(m_nComboItem, m_nComboSubItem);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnComboEscape
LONG CXListCtrl::OnComboEscape(UINT, LONG)
{
	KillTimer(1);
	SetTimer(2, 50, NULL);

	if (m_nComboItem >= 0 && m_nComboItem < GetItemCount() &&
		m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns())
	{
		CString currentText = GetItemText(m_nComboItem, m_nComboSubItem);
		if (currentText != m_strInitialComboString)
		{
			SetItemText(m_nComboItem, m_nComboSubItem, m_strInitialComboString);
			UpdateSubItem(m_nComboItem, m_nComboSubItem);
		}
	}

	/* Original Code
	// restore original string
	SetItemText(m_nComboItem, m_nComboSubItem, m_strInitialComboString);

	UpdateSubItem(m_nComboItem, m_nComboSubItem);
	*/

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnComboReturn
LONG CXListCtrl::OnComboReturn(UINT, LONG)
{
	KillTimer(1);
	SetTimer(3, 50, NULL);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnComboLButtonUp
LONG CXListCtrl::OnComboLButtonUp(UINT, LONG)
{
	KillTimer(1);
	SetTimer(3, 50, NULL);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnComboKeydown
LONG CXListCtrl::OnComboKeydown(UINT, LONG)
{
	SetTimer(4, 50, NULL);
	return 0;
}

BOOL CXListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CListCtrl::PreTranslateMessage(pMsg);
}

void CXListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lpMeasureItemStruct->itemHeight = abs(lf.lfHeight) + 3;

	m_HeaderCtrl.SetFont(GetFont());
}

LRESULT CXListCtrl::OnSetFont(WPARAM hFont, LPARAM lParam)
{
	CRect rc;
	WINDOWPOS wp;

	LRESULT res = Default();

	GetWindowRect(rc);

	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);

	m_HeaderCtrl.SetFont(GetFont());

	return res;
}

void CXListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CXListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnLButtonDblClk(nFlags, point);
}
