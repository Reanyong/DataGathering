// FormView_TagMapping.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TagMapping.h"
#include "ChildFrm.h"
#include "DLG_TagMapping.h"



// CFormView_TagMapping

IMPLEMENT_DYNCREATE(CFormView_TagMapping, CFormView)

CFormView_TagMapping::CFormView_TagMapping()
	: CFormView(CFormView_TagMapping::IDD)
{
	m_pThreadXmlTagValue = NULL;
	m_bThreadStart = TRUE;
}

CFormView_TagMapping::~CFormView_TagMapping()
{
}

void CFormView_TagMapping::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormView_TagMapping, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_MAPPING_SET, &CFormView_TagMapping::OnBnClickedButtonMappingSet)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_TAGLIST_SET, &CFormView_TagMapping::OnBnClickedButtonTaglistSet)
	ON_BN_CLICKED(IDC_BUTTON_START, &CFormView_TagMapping::OnBnClickedButtonStart)
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnUserMessage)
END_MESSAGE_MAP()


// CFormView_TagMapping �����Դϴ�.

#ifdef _DEBUG
void CFormView_TagMapping::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_TagMapping::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


TCHAR*	_lpszList_History[] = {"�ð�","���"};

// CFormView_TagMapping �޽��� ó�����Դϴ�.
void CFormView_TagMapping::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	g_nActivateDocCheck = FORM_VIEW_ID_5;
	m_strTitle.LoadString(IDS_MY_TREE_VIEW_5);
	GetDocument()->SetTitle(m_strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();
	pWnd->SetMenu(NULL);
	ComposeList(m_xListHistory, USER_CONTROL_LIST, IDC_STATIC_LIST,2,_lpszList_History);

	GetDlgItem(IDC_BUTTON_TAGLIST_SET)->EnableWindow(FALSE);

	m_stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);

	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	if(stGatherInfo.nEngMng_GatherType == 0)
	{
		_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "Auto start", USER_COLOR_RED, "���� ��� �����Դϴ�.");
		if(stGatherInfo.nAutoRun_Check == 1)
		{
			GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
			SetTimer(1,500,NULL);
			m_bThreadStart = FALSE;
			_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "���� ���� : [�ڵ� ����]");
			//_addCurrentstateMsg(0,0, m_strTitle, "�ڵ� ����");
		}
		else
		{
			GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "View log : [Processor..]", USER_COLOR_BLACK, "���� ���� : [���� ����]");
			//_addCurrentstateMsg(0,0, m_strTitle, "���� ����");
		}
	}
	else
		_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "��� �����", USER_COLOR_RED, "���� ����� �����Դϴ�.");

	//SetTimer(1, 500 , NULL);
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

BOOL CFormView_TagMapping::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
{
	int nRight = 0;
	CRect rcCtrl,rect;
	GetDlgItem(nPosListId)->GetWindowRect(&rect);
	GetDlgItem(nPosListId)->GetClientRect(&rcCtrl);
	ScreenToClient(&rect);
	GetDlgItem(nPosListId)->ShowWindow(SW_HIDE);

	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | WS_VSCROLL |WS_BORDER ;
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
		//lvcolumn.fmt = LVCFMT_CENTER;

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		nRight = (rcCtrl.right / nColumns);

		if(nPos == 0)
			lvcolumn.cx = 200;
		else
			lvcolumn.cx = 1200;

		//lvcolumn.cx = nRight;

		listCtrl.InsertColumn(nPos, &lvcolumn);
	}
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}


BOOL CFormView_TagMapping::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}


void CFormView_TagMapping::OnBnClickedButtonMappingSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDLG_TagMapping dlg;
	dlg.DoModal();
}

void CFormView_TagMapping::OnDestroy()
{
	CFormView::OnDestroy();
	StopThread();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CFormView_TagMapping::StartThread()
{
	if(m_pThreadXmlTagValue == NULL)
	{
		m_pThreadXmlTagValue = (CThread_XmlTagValue *)AfxBeginThread(RUNTIME_CLASS(CThread_XmlTagValue),
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED);
		m_pThreadXmlTagValue->SetLPVOID(this);
		m_pThreadXmlTagValue->SetWnd(m_hWnd);
		m_pThreadXmlTagValue->ResumeThread();
	}
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
}

void CFormView_TagMapping::StopThread()
{
	DWORD dwExitCode;

	if (NULL == m_pThreadXmlTagValue)
		return;

	DWORD dw = GetTickCount();
	m_pThreadXmlTagValue->Stop();
#ifdef _DEBUG
	TRACE("Min Thread Stop \n");
#endif

	int nErr = 0;
	while (1)
	{
		if(GetExitCodeThread(m_pThreadXmlTagValue->m_hThread, &dwExitCode))
		{
			if(dwExitCode != STILL_ACTIVE)
				break;
			if(m_pThreadXmlTagValue->GetStop() != TRUE)
			{
				m_pThreadXmlTagValue->Stop();
			}
			else
				break;
			Sleep(500);
		}
		else
			break;
		Sleep(500);
		if(nErr++ > TIMEWAIT_FILELOG)
		{
			ASSERT(FALSE);
			return;
		}
	}


	m_pThreadXmlTagValue = NULL;
}

void CFormView_TagMapping::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	KillTimer(nIDEvent);

	switch(nIDEvent)
	{
	case 1:
		//GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����..");
		StartThread();
		SetTimer(2,50,NULL);
		break;
	case 2:
		{
			if(m_pThreadXmlTagValue != NULL)
			{
				int nState = m_pThreadXmlTagValue->GetTagInfoListState();
				if(nState == 1)
					GetDlgItem(IDC_BUTTON_TAGLIST_SET)->EnableWindow(TRUE);
				else
					SetTimer(2,50,NULL);
			}
		}
		break;
	default:
		break;
	}
	CFormView::OnTimer(nIDEvent);
}


void CFormView_TagMapping::OnBnClickedButtonTaglistSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//GetDlgItem(IDC_BUTTON_TAGLIST_SET)->EnableWindow(FALSE);
	if(m_pThreadXmlTagValue != NULL)
	{
		m_pThreadXmlTagValue->SetTagInfoListOutput();
	}
	//SetTimer(2,500,NULL);
}

void CFormView_TagMapping::SetTagInfoList()
{
	//GetTagInfoList();
}

void CFormView_TagMapping::OnBnClickedButtonStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strProcessorTitle = "";
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	strProcessorTitle.LoadString(IDS_MY_TREE_VIEW_1);

	if(m_bThreadStart == TRUE)
	{
		m_bThreadStart = FALSE;
		//StartThread();
		SetTimer(1,200,NULL);
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"�����ڿ� ���� ���� ����");
		_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "���� : [�����ڿ� ���� ���� ����]");
	}
	else
	{
		m_bThreadStart = TRUE;
		StopThread();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("���� ����");
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		_WriteLogFile(g_stProjectInfo.szProjectLogPath,strProcessorTitle,"�����ڿ� ���� ���� ����");
		_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "View log : [Processor..]" , USER_COLOR_PINK, "���� : [�����ڿ� ���� ���� ����]");
	}

}


//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : �̺�Ʈ
- ����� �̺�Ʈ �޽���
-LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam)
*/
//////////////////////////////////////////////////////////////////////////
LRESULT CFormView_TagMapping::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	CString sMsgBuffer = "";
	sMsgBuffer = (LPCSTR)(long)wParam;

	//lParam = 0 : SUCCESS
	//lParam = 1 : Error
	int nMsgType = 0;
	nMsgType = lParam;
	COLORREF rgbRet;

	if(nMsgType == LOG_COLOR_BLUE)
		rgbRet = RGB(0, 0, 255);
	else if(nMsgType == LOG_COLOR_RED)
		rgbRet =  RGB(255, 0, 0);

	_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "DB log : [DB Access..]",rgbRet, sMsgBuffer);
	return 0;
}


void CFormView_TagMapping::OutputHistory(const char *szData1,const char *szData2)
{
	int nItem = m_xListHistory.GetItemCount();
	if (nItem > 1024)
	{
		m_xListHistory.DeleteItem(nItem-1);
		nItem = m_xListHistory.GetItemCount();
	}

	nItem = m_xListHistory.InsertItem(0, szData1);
	m_xListHistory.SetItemText(nItem, 1, szData2);

	m_xListHistory.Invalidate(FALSE);
}