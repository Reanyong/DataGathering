// FormView_Main.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_Main.h"
#include "ChildFrm.h"



// CFormView_Main

IMPLEMENT_DYNCREATE(CFormView_Main, CFormView)

CFormView_Main::CFormView_Main()
	: CFormView(CFormView_Main::IDD)
{
	m_pThreadMain = NULL;
	m_dwRowCnt = 0;
}

CFormView_Main::~CFormView_Main()
{

	StopThread();
}

void CFormView_Main::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormView_Main, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CFormView_Main �����Դϴ�.

#ifdef _DEBUG
void CFormView_Main::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_Main::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormView_Main �޽��� ó�����Դϴ�.
char *szColumn_Info[]={"Processor Name","� Message"};
char *szColumn_SubInfo[]={"Num","Processor Name","Message"};

void CFormView_Main::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CString strTitle;
	g_nActivateDocCheck = FORM_VIEW_ID_1;
	strTitle.LoadString(IDS_MY_TREE_MAIN_VIEW);
	GetDocument()->SetTitle(strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();
	pWnd->SetMenu(NULL);
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	/*CString strName = "";
	strName.Format("EVDataGathering-D");
	SetWindowText(strName);*/

	_CreateFont(&m_Font);

	ComposeList(m_xListProcessorList, USER_CONTROL_LIST, IDC_STATIC_PROCESSOR,2,szColumn_Info);
	m_xListProcessorList.SetFont(&m_Font);

	ComposeList(m_xListProcessorMsgList, USER_CONTROL_LIST+1, IDC_STATIC_PROCESSOR_MSG,3,szColumn_SubInfo);
	m_xListProcessorMsgList.SetFont(&m_Font);

	ShowSettingInfo(g_stProjectInfo.szProjectIniPath);
	/*
	���� ������ ó�� �ʿ�
	*/
	SetTimer(1, 500, NULL);
}


BOOL CFormView_Main::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
{
	int nRight = 0;
	CRect rcCtrl,rect;
	GetDlgItem(nPosListId)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(nPosListId)->ShowWindow(SW_HIDE);

	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | WS_VSCROLL ;
	if (!listCtrl.Create(dwStyle, rect, this, nListID))
	{
		TRACE(_T("Failed to create output window.\n"));
		return FALSE;
	}

	for(int nPos = 0; nPos< nColumns; nPos++)
	{
		if(szColumn[nPos] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		switch(nPos)
		{
		case 0:
			lvcolumn.fmt = LVCFMT_LEFT;//LVCFMT_CENTER;
			break;
		default:
			lvcolumn.fmt = LVCFMT_LEFT;
			break;
		}

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		nRight = (rect.right / nColumns);

		if(listCtrl == m_xListProcessorList)
		{
			if(nPos == 0)
				nRight = nRight - 350;
			else
				nRight = 700;
		}
		else
		{
			if(nPos == 0)
				nRight = nRight - 280;
			else if(nPos == 1)
				nRight = nRight - 150;
			else
				nRight = 500;
		}
		lvcolumn.cx = nRight;
		listCtrl.InsertColumn(nPos, &lvcolumn);
	}
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}

int CFormView_Main::ListInsertItem_Info(const char *szData1,const char *szData2,const char *szData3)
{
	int nItem = m_xListProcessorList.GetItemCount();

	BOOL bDataCheck = FALSE;
	for(int nI = 0; nI < nItem ; nI++)
	{
		CString strRetData1 = m_xListProcessorList.GetComboText(nI,0);
		if(strRetData1 == szData1)
		{
			CString strRetData2 = m_xListProcessorList.GetComboText(nI,0);
			if(strRetData2 != szData2)
			{
				//m_xListProcessorList.SetItemText(nI, 1, szData2);
				m_xListProcessorList.SetItemText(nI, 1, szData2, USER_COLOR_BLUE, RGB(255, 255, 255));

				bDataCheck = TRUE;
				break;
			}
		}
	}

	if(bDataCheck == FALSE)
	{
		m_xListProcessorList.InsertItem(nItem, szData1);
		m_xListProcessorList.SetItemText(nItem, 1, szData2);
	}

	m_xListProcessorList.Invalidate(FALSE);

	return nItem;
}

int CFormView_Main::ListInsertItem_Msg(const char *szData1,const char *szData2,const char *szData3)
{
	CString strNum = "";
	int nItem = m_xListProcessorMsgList.GetItemCount();
	if (nItem > 512)
	{
		m_xListProcessorMsgList.DeleteItem(nItem-1);
		nItem = m_xListProcessorMsgList.GetItemCount();
		m_dwRowCnt = 0;
	}
	m_dwRowCnt++;

	strNum.Format("%03d",m_dwRowCnt + 1);

	nItem = m_xListProcessorMsgList.InsertItem(0, strNum);
	m_xListProcessorMsgList.SetItemText(nItem, 1, szData1);
	m_xListProcessorMsgList.SetItemText(nItem, 2, szData2);

	m_xListProcessorMsgList.Invalidate(FALSE);

	return nItem;
}

BOOL CFormView_Main::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}


void CFormView_Main::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CWnd *pWnd_1 = AfxGetMainWnd();
	KillTimer(nIDEvent);

	switch(nIDEvent)
	{
	case 1:
		//������ ����
		//StartThread();
		SetTimer(2, 100, NULL);
		//SetTimer(6, 100, NULL);
		break;
	case 2:
		pWnd_1->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_VIEW_1); //Data Gathering
		SetTimer(3, 100, NULL);
		break;

		//20200227 ����ȣ ���� ������� �ʴ� ��� ������ �ʰ� ����
		/*
	case 3:
		pWnd_1->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_VIEW_2); //Weather Gathering
		SetTimer(4, 100, NULL);
		break;
	case 4:
		pWnd_1->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_VIEW_4);//�����������ͼ���
		SetTimer(5, 100, NULL);
		break;
	case 5:
		pWnd_1->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_VIEW_5);//�������������� ����
		SetTimer(6, 100, NULL);
		break;
	case 6:
		pWnd_1->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_VIEW_6);//����ü�˶�üũ
		SetTimer(7, 100, NULL);
		break;
	case 7:
		pWnd_1->PostMessage(WM_USER_OPEN_FRAME, 0, (LPARAM)IDS_MY_TREE_VIEW_7);//����ü�˶�üũ
		//SetTimer(5, 100, NULL);
		break;
		*/
	}

	CFormView::OnTimer(nIDEvent);
}

void CFormView_Main::StartThread()
{
	if(m_pThreadMain == NULL)
	{
		m_pThreadMain=(CThread_Main*)AfxBeginThread(RUNTIME_CLASS(CThread_Main),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
		m_pThreadMain->SetLPVOID(this);
		m_pThreadMain->SetWnd(m_hWnd);
		m_pThreadMain->ResumeThread();
		//GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");
	}
}

void CFormView_Main::StopThread()
{
	DWORD dwExitCode;

	if (NULL == m_pThreadMain)
		return;

	DWORD dw = GetTickCount();
	m_pThreadMain->Stop();
#ifdef _DEBUG
	TRACE("DataGathering Main Thread Stop \n");
#endif

	int nErr = 0;
	while (1)
	{
		if(GetExitCodeThread(m_pThreadMain->m_hThread, &dwExitCode))
		{
			if(dwExitCode != STILL_ACTIVE)
				break;
		}
		else break;
		Sleep(100);
		if(nErr++ > TIMEWAIT_FILELOG)
		{
			ASSERT(FALSE);
			return;
		}
	}

	m_pThreadMain = NULL;
}

void CFormView_Main::ShowSettingInfo(CString strIniPath)
{
	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(strIniPath);
	ST_DBINFO stDbInfo = _getInfoDBRead(strIniPath);

	CString strTemp = "";
	GetDlgItem(IDC_STATIC_PROJECT_NAME)->SetWindowText(g_stProjectInfo.szProjectName);
	if(stGatherInfo.nDemp_Check == 1)
		strTemp.Format("���");
	else
		strTemp.Format("��� ����");
	GetDlgItem(IDC_STATIC_DEMP_USE)->SetWindowText(strTemp);

	if(stGatherInfo.nAutoRun_Check == 1)
		strTemp.Format("�ڵ� ����");
	else
		strTemp.Format("���� ����");
	GetDlgItem(IDC_STATIC_GATHE_USE)->SetWindowText(strTemp);

	if(stGatherInfo.nGathe_Interval == 0)
		strTemp.Format("���� 1��");
	else if(stGatherInfo.nGathe_Interval == 1)
		strTemp.Format("���� 5��");
	else if(stGatherInfo.nGathe_Interval == 2)
		strTemp.Format("���� 10��");
	else if(stGatherInfo.nGathe_Interval == 3)
		strTemp.Format("���� 15��");

	GetDlgItem(IDC_STATIC_GATHE_TIME)->SetWindowText(strTemp);

	if(stGatherInfo.nDelete_Interval == 0)
		strTemp.Format("2�� ����");
	else if(stGatherInfo.nDelete_Interval == 1)
		strTemp.Format("5�� ����");
	else if(stGatherInfo.nDelete_Interval == 2)
		strTemp.Format("1���� ����");
	else if(stGatherInfo.nDelete_Interval == 3)
		strTemp.Format("2���� ����");
	GetDlgItem(IDC_STATIC_DELETE_INTERVAL)->SetWindowText(strTemp);

	if(stGatherInfo.nDetailsLogCheck == 1)
		strTemp.Format("���");
	else
		strTemp.Format("��� ����");

	GetDlgItem(IDC_STATIC_LOG_USE)->SetWindowText(strTemp);

	GetDlgItem(IDC_STATIC_DBSERVER)->SetWindowText(stDbInfo.szServer);
	GetDlgItem(IDC_STATIC_DBNAME)->SetWindowText(stDbInfo.szDB);
	GetDlgItem(IDC_STATIC_DBID)->SetWindowText(stDbInfo.szID);
}

void CFormView_Main::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED); //--> �����찡 �����ǰ� �Ϸ�ǰ� �� �ڿ�
}
