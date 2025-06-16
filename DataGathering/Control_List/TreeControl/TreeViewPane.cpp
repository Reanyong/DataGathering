// TreeViewPane.cpp : implementation file
//

#include "stdafx.h"
#include "../../datagathering.h"
#include "TreeViewPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewPane

CTreeViewPane::CTreeViewPane()
{
}

CTreeViewPane::~CTreeViewPane()
{
}


BEGIN_MESSAGE_MAP(CTreeViewPane, CWnd)
	//{{AFX_MSG_MAP(CTreeViewPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREEVIEW,OnDblclk)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTreeViewPane message handlers
static UINT toolbar[] =
{
	ID_PROJECT_ADDEXISTINGITEM
};
int CTreeViewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//	VERIFY(m_wndToolBar.CreateToolBar(WS_TABSTOP|WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));
	//	m_wndToolBar.SetButtons(toolbar, 1);
	
	
	//SetCapture();
	if (m_wndClassView.GetSafeHwnd() == 0)
	{
		if (!m_wndClassView.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
			CRect(0,0,0,0), this, IDC_TREEVIEW ))
		{
			TRACE0( "Unable to create tree control.\n" );
			return 0;
		}
		//	m_wndClassView.ModifyStyleEx(0, WS_EX_STATICEDGE);
		//m_wndClassView.SetCapture();
		//	if (!CreateImageList(m_ilClassView, IDB_CLASSTREE))
		//		return 0;
		
		// Set the image list for the tree control.
		
		m_wndClassView.SetImageList( &m_ilClassView, TVSIL_NORMAL );
		
		// Add the parent item
		HTREEITEM htItem = m_wndClassView.InsertItem(_T("설정"));
		
		
		m_wndClassView.SetItemState( htItem, TVIS_BOLD, TVIS_BOLD );
		
		// Add children
		HTREEITEM hti = m_wndClassView.InsertItem(_T("데이터베이스 설정"), 3, 3, htItem);
		
		
		hti = m_wndClassView.InsertItem(_T("태그 생성 설정"), 3, 3, htItem);
		
		
		hti = m_wndClassView.InsertItem(_T("Raw 데이터 수집설정"), 3, 3, htItem);
		
		
		hti = m_wndClassView.InsertItem(_T("프로젝트 생성"), 3, 3, htItem);
		
		hti = m_wndClassView.InsertItem(_T("Raw Column 설정"), 3, 3, htItem);
		
		HTREEITEM htItem2 = m_wndClassView.InsertItem(_T("상태"));
		m_wndClassView.SetItemState( htItem2, TVIS_BOLD, TVIS_BOLD );
		
		hti = m_wndClassView.InsertItem(_T("태그 상태"), 3, 3, htItem2);
		
		
		hti = m_wndClassView.InsertItem(_T("Raw 데이터 상태"), 3, 3, htItem2);
	
		m_wndClassView.Expand(htItem, TVE_EXPAND);

		m_wndClassView.Expand(htItem2, TVE_EXPAND);
		
		
	}
	
	
	return 0;
}

void CTreeViewPane::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int nTop = 0;

	if (m_wndToolBar.GetSafeHwnd())
	{
		CSize sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, nTop, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
		nTop += sz.cy;
	}

	if (m_wndClassView.GetSafeHwnd())
	{
		m_wndClassView.MoveWindow(0, nTop, cx, cy - nTop);
		m_wndClassView.Invalidate(FALSE);
	}
}

void CTreeViewPane::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_wndClassView.SetFocus();
}


/*
void CTreeViewPane::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD lParam=0;
	//CTreeCtrl &m_TreeCtrl =  GetTreeCtrl();
	HTREEITEM hSelected = m_wndClassView.GetSelectedItem();
	HTREEITEM hChild = m_wndClassView.GetChildItem(hSelected);
	
	if(hChild != NULL)
	{
		*pResult = 0;
		return;
	}
	
	if (NULL == hSelected)
		return ;
	
	UpdateData(TRUE);
	CString str  = m_wndClassView.GetItemText(hSelected);
	DWORD  wParam = m_wndClassView.GetItemData(hSelected);
	
	//왼쪽 트리 추가
	//트리 여기2
	
	
	if(m_wndClassView.GetItemText(hSelected) == "태그 상태")
		lParam = VIEW_TAG;
	else if(m_wndClassView.GetItemText(hSelected) == "Raw 데이터 상태")
		lParam = VIEW_ROWDATA;
	else if(m_wndClassView.GetItemText(hSelected) == "태그 생성 설정")
		lParam = DLG_SETTING_TAG;
	else if(m_wndClassView.GetItemText(hSelected) == "Raw 데이터 수집설정")
		lParam = DLG_SETTING_RAW;
	else if(m_wndClassView.GetItemText(hSelected) == "데이터베이스 설정")
		lParam = DLG_SETTING_DB;
	else if(m_wndClassView.GetItemText(hSelected) == "프로젝트 생성")
		lParam = DLG_SETTING_PROJECT;
	
	if(lParam == 0)
	{
		AfxMessageBox("사용하지 않는 프로그램 입니다.");
		return;
	}
	//UpdateImage(hSelected, TRUE);
	CWnd *pWnd = AfxGetMainWnd();
	
	pWnd->PostMessage(WM_OPENVIEW, (WPARAM)wParam, (LPARAM)lParam);
	
	*pResult = 0;
}
*/


void CTreeViewPane::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD lParam=0;
	//CTreeCtrl &m_TreeCtrl =  GetTreeCtrl();
	HTREEITEM hSelected = m_wndClassView.GetSelectedItem();
	HTREEITEM hChild = m_wndClassView.GetChildItem(hSelected);
	
	if(hChild != NULL)
	{
		*pResult = 0;
		return;
	}
	
	if (NULL == hSelected)
		return ;
	
	UpdateData(TRUE);
	CString str  = m_wndClassView.GetItemText(hSelected);
	DWORD  wParam = m_wndClassView.GetItemData(hSelected);
	
	//왼쪽 트리 추가
	//트리 여기2
	
	
	if(m_wndClassView.GetItemText(hSelected) == "태그 상태")
		lParam = VIEW_TAG;
	/*else if(m_wndClassView.GetItemText(hSelected) == "Raw 데이터 상태")
		lParam = VIEW_ROWDATA;
	else if(m_wndClassView.GetItemText(hSelected) == "태그 생성 설정")
		lParam = DLG_SETTING_TAG;
	else if(m_wndClassView.GetItemText(hSelected) == "Raw 데이터 수집설정")
		lParam = DLG_SETTING_RAW;
	else if(m_wndClassView.GetItemText(hSelected) == "데이터베이스 설정")
		lParam = DLG_SETTING_DB;
	else if(m_wndClassView.GetItemText(hSelected) == "프로젝트 생성")
		lParam = DLG_SETTING_PROJECT;
	else if(m_wndClassView.GetItemText(hSelected) == "Raw Column 설정")
		lParam = DLG_SETTING_COLUMN;
	*/
	if(lParam == 0)
	{
		AfxMessageBox("사용하지 않는 프로그램 입니다.");
		return;
	}
	//UpdateImage(hSelected, TRUE);
	CWnd *pWnd = AfxGetMainWnd();
	
	pWnd->PostMessage(WM_OPENVIEW, (WPARAM)wParam, (LPARAM)lParam);
	
	*pResult = 0;
}
