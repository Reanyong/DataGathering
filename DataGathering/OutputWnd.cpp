
#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	IDC_OUTPUT_LIST             100
/////////////////////////////////////////////////////////////////////////////
// COutputBar
static CString strTabs[] =
{
	"System",
	"Tag설정",
	"Row 데이터 수집",
	"Row 정보 삭제 처리"
};

TCHAR*	_lpszHeaders[] = { _T("발생시간"),	_T("구분"),	_T("내용")};
int		_pnHeaders[] = 	{ 128,	200, 1800};


COutputWnd::COutputWnd()
{
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	_tcscpy_s( ncm.lfMessageFont.lfFaceName,12, _T("맑은 고딕"));
	m_Font.CreateFontIndirect(&ncm.lfMessageFont);
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 탭 창을 만듭니다.
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D_VS2005, rectDummy, this, 1))
	{
		TRACE0("출력 탭 창을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | WS_VSCROLL ;

	CString strTabName;
	//BOOL bNameValid;
	//int nListCount = sizeof(m_OutputList)/sizeof(m_OutputList[0]);
	m_nOutPutTabCount = 2;
	for( int iList = 0; iList < m_nOutPutTabCount; iList++)
	{
		if (!m_OutputList[iList].Create(dwStyle, rectDummy, &m_wndTabs, IDC_OUTPUT_LIST+iList))
		{
			TRACE(_T("Failed to create output window.\n"));
			return -1;
		}
		m_OutputList[iList].SetFont(&m_Font);

		//하단 Outputbar 여기2 (탭 컨트롤러 생성)
		if (iList == FORM_VIEW_ID_SYSTEM)
		{
			strTabName.Format("System Log");
			InitListCtrl_SysTime(m_OutputList[iList]);
		}
		else if (iList == LOG_MESSAGE_2)
		{
			strTabName.LoadString(IDS_MY_TREE_VIEW_1);
			InitListCtrl_Common(m_OutputList[iList]);
		}
		else if (iList == LOG_MESSAGE_3)
		{
			strTabName.LoadString(IDS_MY_TREE_VIEW_2);
			InitListCtrl_Common(m_OutputList[iList]);
		}
		else if (iList == LOG_MESSAGE_4)
		{
			strTabName.LoadString(IDS_MY_TREE_VIEW_4);
			InitListCtrl_Common(m_OutputList[iList]);
		}
		else if (iList == LOG_MESSAGE_5)
		{
			strTabName.LoadString(IDS_MY_TREE_VIEW_5);
			InitListCtrl_Common(m_OutputList[iList]);
		}
		else if (iList == LOG_MESSAGE_6)
		{
			strTabName.LoadString(IDS_MY_TREE_VIEW_6);
			InitListCtrl_Common(m_OutputList[iList]);
		}
		else if (iList == LOG_MESSAGE_7)
		{
			strTabName.LoadString(IDS_MY_TREE_VIEW_7);
			InitListCtrl_Common(m_OutputList[iList]);
		}
		m_wndTabs.AddTab(&m_OutputList[iList], strTabName, (UINT)0);
	}
		
	SelectTabView(0);

	return 0;
}

void COutputWnd::InitListCtrl_SysTime(CXListCtrl &list)
{
	CRect rect;
	list.GetWindowRect(&rect);

	int i;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	for(i = 0; i<3; i++)
	{
		if(_lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		switch(i)
		{
			case 0:
				lvcolumn.fmt = LVCFMT_CENTER;
				break;
			default:
				lvcolumn.fmt = LVCFMT_LEFT;
				break;
		}

		lvcolumn.pszText = _lpszHeaders[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = _pnHeaders[i];

		list.InsertColumn(i, &lvcolumn);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	for (i = 0;i<3 ; i++)
	{
		if (_lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		
		/*
		if (i == 1)
			lvcolumn.fmt = LVCFMT_LEFT;
		else
			lvcolumn.fmt = LVCFMT_CENTER;
		*/

		lvcolumn.iSubItem = i;
		list.SetColumn(i, &lvcolumn);
	}

	list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void COutputWnd::InitListCtrl_Common(CXListCtrl &list)
{
	CRect rect;
	list.GetWindowRect(&rect);

	int i;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	for(i = 0; i<3; i++)
	{
		if(_lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		switch(i)
		{
			case 0:
				lvcolumn.fmt = LVCFMT_CENTER;
				break;
			default:
				lvcolumn.fmt = LVCFMT_LEFT;
				break;
		}

		lvcolumn.pszText = _lpszHeaders[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = _pnHeaders[i];

		list.InsertColumn(i, &lvcolumn);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	for (i = 0;i<3 ; i++)
	{
		if (_lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		
		/*
		if (i == 1)
			lvcolumn.fmt = LVCFMT_LEFT;
		else
			lvcolumn.fmt = LVCFMT_CENTER;
		*/

		lvcolumn.iSubItem = i;
		list.SetColumn(i, &lvcolumn);
	}

	list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

int COutputWnd::InsertMessage(int nListCtrl, COLORREF crText1, const char *szMsg1, COLORREF crText2, const char *szMsg2)
{
	CXListCtrl& list = m_OutputList[nListCtrl];
	CString s;
	time_t tCurrent;
	tm tmCurrent;

	time(&tCurrent);
	//tmCurrent = *localtime_s(&tCurrent);
	localtime_s(&tmCurrent,&tCurrent);

	int nItem = list.GetItemCount();
	if (nItem > 512)
	{
		list.DeleteItem(nItem-1);
		nItem = list.GetItemCount();
	}

	s.Format("%04d/%02d/%02d %02d:%02d:%02d", tmCurrent.tm_year+1900, tmCurrent.tm_mon+1, tmCurrent.tm_mday, tmCurrent.tm_hour, tmCurrent.tm_min, tmCurrent.tm_sec);
	nItem = list.InsertItem(0, s);

	list.SetItemText(nItem, 1, szMsg1, crText1, RGB(255, 255, 255));
	list.SetItemText(nItem, 2, szMsg2, crText2, RGB(255, 255, 255));
	//nItem = list.InsertItem(nItem, szMsg2, crText, ::GetSysColor(COLOR_WINDOW));

	list.Invalidate(FALSE);

	return nItem;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}


void COutputWnd::SelectTab(int nTab)
{
	SelectTabView(nTab);
	m_wndTabs.SetActiveTab(nTab);
}

void COutputWnd::SelectTabView(int nTab)
{
	//int nListCount = sizeof(m_OutputList)/sizeof(m_OutputList[0]);
	for( int iList = 0; iList < m_nOutPutTabCount; ++iList) {
		m_OutputList[iList].ShowWindow(SW_HIDE);
	}

	m_OutputList[nTab].ShowWindow(SW_SHOW);
}


void COutputWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rc;
	GetClientRect(&rc);
	CRect rect;

	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DDKSHADOW),
		::GetSysColor(COLOR_3DDKSHADOW));
}


/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_MY_VIEW_OUTPUT_WND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 메시지 처리기

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	/*CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();*/

}

void COutputList::OnEditCopy()
{
	MessageBox(_T("출력 복사"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("출력 지우기"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}

