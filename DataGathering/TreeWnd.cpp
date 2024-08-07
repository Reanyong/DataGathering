// TreeWnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "TreeWnd.h"


// CTreeWnd

IMPLEMENT_DYNAMIC(CTreeWnd, CDockablePane)

CTreeWnd::CTreeWnd()
{
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	//_tcscpy( ncm.lfMessageFont.lfFaceName, _T("Courier"));
	_tcscpy_s( ncm.lfMessageFont.lfFaceName,12, _T("���� ���"));
	m_Font.CreateFontIndirect(&ncm.lfMessageFont);
}

CTreeWnd::~CTreeWnd()
{
}


BEGIN_MESSAGE_MAP(CTreeWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_NOTIFY_EX(NM_DBLCLK, ID_MY_VIEW_TREE_CONTROL, OnNotifyDblclk)
END_MESSAGE_MAP()



// CTreeWnd �޽��� ó�����Դϴ�.
int CTreeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// �並 ����ϴ�.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	if (!m_wndTreeView.Create(dwViewStyle, rectDummy, this, ID_MY_VIEW_TREE_CONTROL))
	{
		TRACE0("���� �並 ������ ���߽��ϴ�.\n");
		return -1;    
	}

	// �� �̹����� �ε��մϴ�.
	m_TreeViewImages.Create(IDB_TREE_IMAGE, 16, 0, RGB(0, 255, 0));
	m_wndTreeView.SetImageList(&m_TreeViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* ��� */);

	//OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// ��� ����� �θ� �������� �ƴ� �� ��Ʈ���� ���� ����õ˴ϴ�.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// ���� Ʈ�� �� �����͸� ���� �ڵ�� ä��ϴ�.
	SetTreeItem();
	AdjustLayout();

	return 0;
}


void CTreeWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CTreeWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


BOOL CTreeWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CDockablePane::PreTranslateMessage(pMsg);
}



void CTreeWnd::SetTreeItem()
{
	BOOL bNameValid;
	CString strTitle = "";
	//ȯ�漳��
	m_wndTreeView.SetFont(&m_Font);
	bNameValid = strTitle.LoadString(IDS_MY_TREE_ROOT_SETTING);
	ASSERT(bNameValid);
	HTREEITEM hSettingRoot = m_wndTreeView.InsertItem(strTitle, 11, 11);
	m_wndTreeView.SetItemState(hSettingRoot, TVIS_BOLD, TVIS_BOLD);
	//dataGathering ����
	bNameValid = strTitle.LoadString(IDS_MY_TREE_PROGRAM_SETTING_1);
	ASSERT(bNameValid);
	HTREEITEM hSettingDepth_1 = m_wndTreeView.InsertItem(strTitle, 12, 12, hSettingRoot);
	m_wndTreeView.InsertItem("����", 1, 18, hSettingDepth_1);
	

	//���α׷� �׸� ����Ʈ
	bNameValid = strTitle.LoadString(IDS_MY_TREE_ROOT_PROGRAM);
	ASSERT(bNameValid);
	HTREEITEM hProgramRoot = m_wndTreeView.InsertItem(strTitle, 11, 11);
	m_wndTreeView.SetItemState(hProgramRoot, TVIS_BOLD, TVIS_BOLD);

	bNameValid = strTitle.LoadString(IDS_MY_TREE_MAIN_VIEW);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);
	
	bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_1);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);
	

	//20200227	����ȣ ����	���� ������� �ʴ� ��� ������ �ʵ��� ����
	/*
	bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_2);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);
	
	bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_4);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);

	bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_5);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);

	bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_6);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);
	*/

	/*bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_3);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);

	bNameValid = strTitle.LoadString(IDS_MY_TREE_VIEW_4);
	ASSERT(bNameValid);
	m_wndTreeView.InsertItem(strTitle, 0, 18, hProgramRoot);*/

	m_wndTreeView.Expand(hSettingRoot, TVE_EXPAND);
	m_wndTreeView.Expand(hSettingDepth_1, TVE_EXPAND);
	m_wndTreeView.Expand(hProgramRoot, TVE_EXPAND);
}

void CTreeWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTreeView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CTreeWnd::OnNotifyDblclk(UINT id, NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSelected = m_wndTreeView.GetSelectedItem();
	CWnd *pWnd = AfxGetMainWnd();

	*pResult = 0;

	if (NULL == hSelected)
		return FALSE;

	UpdateData(TRUE);
	BOOL bNameValid;
	DWORD wParam = m_wndTreeView.GetItemData(hSelected);
	DWORD lParam;
	CString strLoadString,strRootTitle,strDepthTitle;
	CString strProjectSetting,strDBSetting,strProgramSetting,strProgramList,strSetting,strSetting_3,strSetting_4,strSetting_5;
	CString strMainView,strView_1,strView_2,strView_3,strView_4,strView_5,strView_6;

	HTREEITEM hSelectedParent = m_wndTreeView.GetParentItem(hSelected);

	strDepthTitle = m_wndTreeView.GetItemText(hSelected);
	strRootTitle = m_wndTreeView.GetItemText(hSelectedParent);

	bNameValid = strLoadString.LoadString(IDS_MY_TREE_ROOT_SETTING);
	ASSERT(bNameValid);

	if(strRootTitle == strLoadString || strRootTitle == "")
		return -1;

	bNameValid = strSetting.LoadString(IDS_MY_TREE_PROGRAM_SETTING_1); //DBȯ�漳��
	ASSERT(bNameValid);

	
	bNameValid = strProgramList.LoadString(IDS_MY_TREE_ROOT_PROGRAM);
	ASSERT(bNameValid);
	

#ifdef _DEBUG
	TRACE("���� %s/%s \n",strRootTitle,strDepthTitle);
#endif

	if (strRootTitle == strSetting)
	{
		lParam = IDS_MY_TREE_PROGRAM_SETTING_1;
	}
	else if (strRootTitle == strProgramList)
	{
		 bNameValid = strMainView.LoadString(IDS_MY_TREE_MAIN_VIEW);
		 ASSERT(bNameValid);
		 bNameValid = strView_1.LoadString(IDS_MY_TREE_VIEW_1);
		 ASSERT(bNameValid);
		 bNameValid = strView_2.LoadString(IDS_MY_TREE_VIEW_2);
		 ASSERT(bNameValid);
		 bNameValid = strView_3.LoadString(IDS_MY_TREE_VIEW_3);
		 ASSERT(bNameValid);

		 bNameValid = strView_4.LoadString(IDS_MY_TREE_VIEW_4);
		 ASSERT(bNameValid);

		 bNameValid = strView_5.LoadString(IDS_MY_TREE_VIEW_5);
		 ASSERT(bNameValid);
		 bNameValid = strView_6.LoadString(IDS_MY_TREE_VIEW_6);
		 ASSERT(bNameValid);

		 if(strDepthTitle == strMainView)
		 {
			 lParam = IDS_MY_TREE_MAIN_VIEW;
		 }
		 else if(strDepthTitle == strView_1)
		 {
			 lParam = IDS_MY_TREE_VIEW_1;
		 }
		 else if(strDepthTitle == strView_2)
		 {
			 lParam = IDS_MY_TREE_VIEW_2;
		 }
		 else if(strDepthTitle == strView_3)
		 {
			 lParam = IDS_MY_TREE_VIEW_3;
		 }
		 else if(strDepthTitle == strView_4)
		 {
			 lParam = IDS_MY_TREE_VIEW_4;
		 }
		 else if(strDepthTitle == strView_5)
		 {
			 lParam = IDS_MY_TREE_VIEW_5;
		 }
		 else if(strDepthTitle == strView_6)
		 {
			 lParam = IDS_MY_TREE_VIEW_6;
		 }
	}

	//CWnd *pWnd = AfxGetMainWnd();

	AfxGetMainWnd()->PostMessage(WM_USER_OPEN_FRAME, (WPARAM)wParam, (LPARAM)lParam);
	return FALSE;
}