// WndMenubar.cpp : implementation file
//

#include "stdafx.h"
#include "../../DataGathering.h"
#include "WndMenubar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int _nDetail = 0;
CString _szDetail;
LOGFONT _lfItem = { -13, 0, 0, 0, 400, 0, 0, 0, 0, 1, 2, 1, 34, "MS Sans Serif" };

/////////////////////////////////////////////////////////////////////////////
// CWndMenubar

IMPLEMENT_DYNCREATE(CWndMenubar, CWnd)

CWndMenubar::CWndMenubar()
{
	m_nToolbarId = IDR_CUSTOM_TOOLBAR;
	m_szRoot.Format("%s","메뉴");
	
}

CWndMenubar::~CWndMenubar()
{
}


BEGIN_MESSAGE_MAP(CWndMenubar, CWnd)
	//{{AFX_MSG_MAP(CWndMenubar)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP

	ON_NOTIFY_EX(NM_DBLCLK, ID_MENU_LIST, OnNotifyDblclk)
END_MESSAGE_MAP()

BOOL CWndMenubar::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}
//#define MENUBAR_MSG
void CWndMenubar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rcClient, rcToolbar;
	GetClientRect(&rcClient);

#ifdef TREE_TOOLBAR
	m_wndToolBar.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
#endif

#ifdef XTP_THEME_OFFICE2007
	dc.FillSolidRect(rcClient, RGB(255, 255, 255));
#else
	CXTPColorManager* cm = XTPColorManager();
	COLORREF rgb = RGB(202, 202, 202);
	if (!cm->IsLunaColorsDisabled())
		rgb = GetMSO2003Color(XPCOLOR_TOOLBAR_FACE);
	dc.FillSolidRect(rcClient, rgb);
#endif

	/*
#ifdef MENUBAR_MSG
	CDC memDC;
	CBitmap *pOldBitmap;
	memDC.CreateCompatibleDC(&dc);
	pOldBitmap = memDC.SelectObject(&m_bmpInfo);
	dc.StretchBlt(rcClient.left, rcClient.bottom-m_bm.bmHeight, rcClient.Width(), m_bm.bmHeight, &memDC, 0, 0, m_bm.bmWidth, m_bm.bmHeight, SRCCOPY);
	memDC.SelectObject(pOldBitmap);

	rcClient.top = rcClient.bottom - m_bm.bmHeight+8;
	//if (rcClient.top < rcToolbar.bottom)
	//	rcClient.top = rcToolbar.bottom;
	
	dc.SetBkMode(TRANSPARENT);
	CString s = "Info";

	CFont font, *pOldFont;
	font.CreateFontIndirect(&_lfItem);
	pOldFont = dc.SelectObject(&font);

	dc.DrawText(s, s.GetLength(), rcClient, DT_CENTER|DT_WORDBREAK);

	_nDetail = 1;
	_szDetail = "asdfasdf";
	if (_nDetail > 0)
	{
		rcClient.top += 34;
		dc.DrawText(_szDetail, _szDetail.GetLength(), rcClient, DT_CENTER|DT_VCENTER|DT_WORDBREAK);
	}
	
	dc.SelectObject(pOldFont);
#endif*/
}


/////////////////////////////////////////////////////////////////////////////
// CWndMenubar drawing
// ON_NOTIFY_REFLECT를 이용하여 WM_NOTIFY를 파생된 컨트롤클래스에게 되돌려 보낼수 있다.

// Your function must return TRUE if the notification message has been completely handled 
// or FALSE if other objects in the command routing should have a chance to handle the message


BOOL CWndMenubar::OnNotifyDblclk(UINT id, NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSelected = m_TreeCtrl.GetSelectedItem();
	CWnd *pWnd = AfxGetMainWnd();

	*pResult = 0;

	if (NULL == hSelected)
		return FALSE;

	UpdateData(TRUE);
	DWORD wParam = m_TreeCtrl.GetItemData(hSelected);
	DWORD lParam;

	CString s,s1;
	s = m_TreeCtrl.GetItemText(hSelected);

	HTREEITEM hSelectedParent = m_TreeCtrl.GetParentItem(hSelected);

	s1 = m_TreeCtrl.GetItemText(hSelected);
	
	if(s1 == "좌석위치 등록")
		lParam = 1;
	else if(s1 == "구성원 등록")
		lParam = 2;
	else if(s1 == "스케줄 등록")
		lParam = 9;
	else if(s1 == "New 스케줄 등록")
		lParam = 3;
	else if(s1 == "공조기연동 등록")
		lParam = 5;
	else if(s1 == "전력요금연동 등록")
		lParam = 6;

	if(wParam<1)
		return -1;

	//AfxGetMainWnd()->PostMessage(WM_OPEN_FRAME, (WPARAM)wParam, (LPARAM)lParam);
	PostMessage(WM_OPEN_FRAME, (WPARAM)wParam, (LPARAM)lParam);
	return FALSE;

}

/*
void CWndMenubar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hSelected = m_TreeCtrl.GetSelectedItem();
	CWnd *pWnd = AfxGetMainWnd();
	if (NULL == hSelected)
		return;
	DWORD lParam = m_TreeCtrl.GetItemData(hSelected);
	int a = m_TreeCtrl.ItemHasChildren(hSelected);
	CString s;
	s = m_TreeCtrl.GetItemText(hSelected) ;
	if (lParam == 0)
		return;
/*	int nDevice=lParam-1;
	switch(_pStation[nDevice].nType)
	{
	case 0:
		m_pt.PropertyEth(nDevice);
		break;
	case 1:
		m_pt.PropertySio(nDevice);
		break;
	case 2:
		m_pt.PropertyDb(nDevice);
		break;
	}

	AfxGetMainWnd()->PostMessage(WM_OPEN_PROPERTY, (WPARAM)lParam, 0);
	PostMessage(WM_OPEN_PROPERTY, (WPARAM)lParam, 0);
	CWnd::OnLButtonDown(nFlags, point);
	
}
*/
void CWndMenubar::OnDraw(CDC* /*pDC*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CWndMenubar diagnostics

#ifdef _DEBUG
void CWndMenubar::AssertValid() const
{
	CWnd::AssertValid();
}

void CWndMenubar::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG
#define TREE_TOOLBAR
/////////////////////////////////////////////////////////////////////////////
// CWndMenubar message handlers

void CWndMenubar::OnInitialUpdate()
{

	_hTreeMenu = GetSafeHwnd();
#ifdef TREE_TOOLBAR
	VERIFY(m_wndToolBar.CreateToolBar(WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));
	VERIFY(m_wndToolBar.LoadToolBar(m_nToolbarId));

	CXTPControlButton* pControl = (CXTPControlButton*)m_wndToolBar.GetControls()->FindControl(IDM_CUSMENU_ADD);
	if (pControl)
		pControl->SetStyle(xtpButtonIconAndCaption);
	pControl = (CXTPControlButton*)m_wndToolBar.GetControls()->FindControl(IDM_CUSMENU_DEL);
	if (pControl)
		pControl->SetStyle(xtpButtonIconAndCaption);
#endif
	CRect rc;
	int lParam = 1;

	GetClientRect(&rc);

	//if (!m_TreeCtrl.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_SHOWSELALWAYS|TVS_LINESATROOT|TVS_HASBUTTONS, rc, this, ID_MENU_LIST))
	//if (!m_TreeCtrl.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_SHOWSELALWAYS|TVS_LINESATROOT|TVS_HASBUTTONS, rc, this, ID_MENU_LIST))
	if (!m_TreeCtrl.Create(WS_CHILD|WS_VISIBLE|TVS_SHOWSELALWAYS|TVS_LINESATROOT, rc, this, ID_MENU_LIST))
	{
		TRACE0("Failed to create tree control.\n");
		return;
	}

	//m_bmpInfo.LoadBitmap(IDB_MENU_INFO);
	//m_bmpInfo.GetBitmap(&m_bm);
	
	m_ImageList.Create(IDB_TREE_IMAGE, 16, 0, RGB(0, 255, 0));
	m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	TV_INSERTSTRUCT tvInsert;

	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM ;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.cchTextMax = 30;
	tvInsert.item.iImage = 11;
	tvInsert.item.iSelectedImage = 11;
	tvInsert.item.cChildren = 0;
	tvInsert.item.lParam = 0;

	tvInsert.item.pszText = (char*)(const char*)m_szRoot;
	m_hRoot = m_TreeCtrl.InsertItem(&tvInsert);

	memset(&tvInsert, 0, sizeof(tvInsert));
	tvInsert.hParent = m_hRoot;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_PARAM ;//TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM ;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.cchTextMax = 30;
	tvInsert.item.iImage = 13;
	tvInsert.item.iSelectedImage = 13;
	tvInsert.item.cChildren = 0;
	tvInsert.item.lParam = 0;


	tvInsert.hParent =m_hRoot;
	tvInsert.item.iImage = 15;
	tvInsert.item.iSelectedImage = 15;
	

	int nCheck = 0;

/*	if(g_bProjectExclude == FALSE )
	{
		tvInsert.item.lParam = lParam++;
		tvInsert.item.pszText = _T("구성원 등록");
		m_hBranch1 = m_TreeCtrl.InsertItem(&tvInsert);
		
		tvInsert.item.pszText =  _T("좌석위치 등록");
		m_hBranch2 = m_TreeCtrl.InsertItem(&tvInsert);

		if(_bAdminCheck == TRUE)
		{
		//	tvInsert.item.pszText =  _T("스케줄 등록");
		//	m_hBranch3 = m_TreeCtrl.InsertItem(&tvInsert);

			tvInsert.item.pszText =  _T("New 스케줄 등록");
			m_hBranch3 = m_TreeCtrl.InsertItem(&tvInsert);


			if(g_bVacExclude == FALSE)
			{
				tvInsert.item.pszText =  _T("공조기연동 등록");
				m_hBranch4 = m_TreeCtrl.InsertItem(&tvInsert);
			}
			if(g_bElecExclude == FALSE)
			{
				tvInsert.item.pszText =  _T("전력요금연동 등록");
				m_hBranch5 = m_TreeCtrl.InsertItem(&tvInsert);
			}
		}
	}
	else
	{*/
	
	
		tvInsert.item.pszText =  _T("New 스케줄 등록");
		m_hBranch3 = m_TreeCtrl.InsertItem(&tvInsert);
		lParam = 3;
//	}

	
	m_TreeCtrl.Expand(m_hRoot, TVE_EXPAND);
	CString s1,s;
	HTREEITEM hFirst = m_TreeCtrl.GetRootItem();
	HTREEITEM item = m_TreeCtrl.GetRootItem();
	
	PostMessage(WM_OPEN_FRAME, 2, (LPARAM)lParam);
}

void CWndMenubar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CSize sz(0);
#ifdef TREE_TOOLBAR
//	if (m_wndToolBar.GetSafeHwnd())
//	{
	//	sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

//		m_wndToolBar.MoveWindow(0, 0, sz.cx, sz.cy);
//		m_wndToolBar.Invalidate(FALSE);
//	}
#endif
	if (m_TreeCtrl.GetSafeHwnd())
	{
#ifdef MENUBAR_MSG
		m_TreeCtrl.MoveWindow(0, sz.cy, cx, cy - sz.cy - m_bm.bmHeight);
#else
		m_TreeCtrl.MoveWindow(0, sz.cy, cx, cy - sz.cy);
#endif
	}

	Invalidate(FALSE);
}

//WS_CLIPCHILDREN  -  차일드가 위치한 영역은 그리기 영역에서 제외된다.
//WS_CLIPSIBLINGS  -  차일드끼리 상호 겹친 영역은 그리기 영역에서 제외된다.
BOOL CWndMenubar::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL b = CWnd::PreCreateWindow(cs);

	cs.style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	return b;
}

void CWndMenubar::OnSetFocus(CWnd* /*pOldWnd*/)
{
	if (m_TreeCtrl.GetSafeHwnd())
		m_TreeCtrl.SetFocus();
}

void CWndMenubar::RemoveTreeItrem(HTREEITEM hItem)
{
	HTREEITEM hSub = m_TreeCtrl.GetChildItem(hItem);
	while (hSub)
	{
		HTREEITEM hDelete = hSub;
		hSub = m_TreeCtrl.GetNextItem(hSub, TVGN_NEXT);
		m_TreeCtrl.DeleteItem(hDelete);
	}
	m_TreeCtrl.DeleteItem(hItem);
}


