// DLG_TagMapping.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_TagMapping.h"
#include "afxdialogex.h"
#include "DLG_TagMappingSet.h"

// CDLG_TagMapping 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_TagMapping, CDialog)

CDLG_TagMapping::CDLG_TagMapping(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_TagMapping::IDD, pParent)
{

	m_pThread_TagSearch = NULL;
	m_STAllTagList = new ST_ALLTAGLIST;
	m_nPageItemCount = 25;
	m_nItemRowStart = 0;
	DB_Connect = NULL;
}

CDLG_TagMapping::~CDLG_TagMapping()
{
	m_STAllTagList->DeleteItem();
	if(m_STAllTagList != NULL)
	{
		delete m_STAllTagList;
		m_STAllTagList = NULL;
	}
}

void CDLG_TagMapping::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_TagMapping, CDialog)
	ON_BN_CLICKED(IDC_RADIO_UNREGISTERED_TAG, &CDLG_TagMapping::OnBnClickedRadioUnregisteredTag)
	ON_BN_CLICKED(IDC_RADIO_REGISTER_TAG, &CDLG_TagMapping::OnBnClickedRadioRegisterTag)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_BACK_ITEM, &CDLG_TagMapping::OnBnClickedButtonBackItem)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_ITEM, &CDLG_TagMapping::OnBnClickedButtonNextItem)
	ON_NOTIFY(NM_DBLCLK, USER_CONTROL_LIST, OnBLClickedList)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDLG_TagMapping::OnBnClickedButtonDelete)
END_MESSAGE_MAP()

CString CDLG_TagMapping::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "",strErrorID = "",strErrorCode;
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("Position : [%s],Description : [%s], DB Error Code : [%08lx], Code meaning : [%s], Source : [%s]",
		szLogName,(LPCTSTR)bstrDescription,e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource);

	if(m_nDBType == DB_ORACLE)
	{
		strErrorID.Format("%s",(LPCTSTR)bstrDescription);
		strErrorCode = strErrorID.Mid(4,5);//Left(nPos);
	}
	else if(m_nDBType == DB_MSSQL)
	{
		strErrorCode.Format("%08lx",e->Error());
	}

#ifdef _DEBUG
	TRACE("ProcessorName : [TagMapping-Dlg],Position : [%s][%s]\r\n",szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("[TagMapping-Dlg] Position : [DB Com Error..], LogName: [%s], %s",szLogName, strRunlog_E2);

	//	SetWriteLogFile(strRunlog_E2Log);
	//	SysLogOutPut(m_strLogTitle,strRunlog_E2,USER_COLOR_RED);
	Sleep(500);

	return strErrorCode;
}


TCHAR*	_lpszList_Column[] = {"번호","TAG명","그룹","유형","물질","단위","위치","장비-대","장비-중","장비-소","세부","계측타입"};

// CDLG_TagMapping 메시지 처리기입니다.
BOOL CDLG_TagMapping::OnInitDialog()
{
	CDialog::OnInitDialog();
	ControlEnable(FALSE);
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_UNREGISTERED_TAG);
	btRadioCheck->SetCheck(TRUE);
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_REGISTER_TAG);
	btRadioCheck->SetCheck(FALSE);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ComposeList(m_xListTagItems, USER_CONTROL_LIST, IDC_LIST_TAG_LIST,12,_lpszList_Column);
	m_nListPageCheck = 10;
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);

	SetTimer(ADO_CONNECTION,200,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDLG_TagMapping::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
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
		lvcolumn.fmt = LVCFMT_CENTER;

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		nRight = (rcCtrl.right / nColumns);

		if(nPos == 0)
			lvcolumn.cx = 50;
		else if(nPos == 1)
			lvcolumn.cx = 150;
		else
			lvcolumn.cx = 100;
		
		//lvcolumn.cx = nRight;

		listCtrl.InsertColumn(nPos, &lvcolumn);		
	}
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}

BOOL CDLG_TagMapping::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

int CDLG_TagMapping::ADO_Connection()
{
	CoInitialize(NULL);

	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);

	m_nDBType = stDBInfo.unDBType;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_hWnd,"TagMapping 설정",g_stProjectInfo.szDTGatheringLogPath);	
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);

	if(DB_Connect->GetDB_ConnectionStatus() != 1)
	{
		BOOL bConnectCheck = DB_Connect->DB_Connection();
		if(bConnectCheck != TRUE)
		{
			SetTimer(ADO_CONNECTION,200,NULL);
		}
		else
			return RETURN_SUCCESS;
	}
	return 0;
}

void CDLG_TagMapping::ADO_Close()
{
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}
	CoUninitialize();
}

void CDLG_TagMapping::OnBnClickedRadioUnregisteredTag()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ControlEnable(FALSE);
	//SetTimer(3,50,NULL);
	if(m_pThread_TagSearch != NULL)
		m_pThread_TagSearch->m_nSearchType = 1;
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);
}


void CDLG_TagMapping::OnBnClickedRadioRegisterTag()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ControlEnable(FALSE);
	//SetTimer(4,50,NULL);
	if(m_pThread_TagSearch != NULL)
		m_pThread_TagSearch->m_nSearchType = 2;

	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(TRUE);

}


void CDLG_TagMapping::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer(nIDEvent);
	int nCount = 6;
	switch(nIDEvent)
	{
	case ADO_CONNECTION:
		{
			int nRet = ADO_Connection();
			if(nRet == RETURN_SUCCESS)
			{
				SetTimer(ADO_QUERY_SELECT,100,NULL);
			}
		}
		break;
	case ADO_QUERY_SELECT:
		GetType();
		SetTimer(1,100,NULL);
		break;
	case 1:
		StartThread();
		break;
	case 2:
		{
			m_xListTagItems.DeleteAllItems();
			int nCount = m_STAllTagList->GetSize();
			for(int n = m_nItemRowStart; n < m_nPageItemCount; n++)
			{
				CString strNum;
				strNum.Format("%d",n + 1);
				if(n < nCount)
				{
					ST_TAG_LIST stTagInfo = m_STAllTagList->GetData(n);
					SetListRowTagItems(strNum,&stTagInfo);
				}
				
			}
			UpdateData(FALSE);
			ControlEnable(TRUE);
		}
		break;
	case 3:
		if(m_pThread_TagSearch != NULL)
			m_pThread_TagSearch->m_nSearchType = 1;
		break;
	case 4:
		if(m_pThread_TagSearch != NULL)
			m_pThread_TagSearch->m_nSearchType = 2;

		break;
	default:
		break;
	}
	
	CDialog::OnTimer(nIDEvent);
}


void CDLG_TagMapping::StartThread()
{
	//분테이터 수집 로직
	if(m_pThread_TagSearch == NULL)
	{
		m_pThread_TagSearch = (CThread_TAGSearch *)AfxBeginThread(RUNTIME_CLASS(CThread_TAGSearch),
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED);
		m_pThread_TagSearch->SetLPVOID(this);	
		m_pThread_TagSearch->SetWnd(m_hWnd);
		m_pThread_TagSearch->GetSTTagListData(m_STAllTagList);
		m_pThread_TagSearch->m_nSearchType = 1;
		m_pThread_TagSearch->DB_Connect = DB_Connect;
		m_pThread_TagSearch->m_nDBType = m_nDBType;

		m_pThread_TagSearch->ResumeThread();
	}
}

void CDLG_TagMapping::StopThread()
{
	DWORD dwExitCode;

	if (NULL == m_pThread_TagSearch) 
		return;

	DWORD dw = GetTickCount();
	m_pThread_TagSearch->Stop();
#ifdef _DEBUG
	TRACE("Min Thread Stop \n");
#endif

	int nErr = 0;
	while (1)
	{
		if(GetExitCodeThread(m_pThread_TagSearch->m_hThread, &dwExitCode))
		{
			if(dwExitCode != STILL_ACTIVE)
				break;
			if(m_pThread_TagSearch->GetStop() != TRUE)
			{
				m_pThread_TagSearch->Stop();
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


	m_pThread_TagSearch = NULL;
}

void CDLG_TagMapping::OnDestroy()
{
	CDialog::OnDestroy();

	StopThread();

	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

int CDLG_TagMapping::GetSearchType()
{
	int nSearchType = 0;
	BOOL bCheck = FALSE;
	CButton *btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_UNREGISTERED_TAG);
	bCheck = btRadioCheck->GetCheck();
	if(bCheck == TRUE)
		nSearchType = 1;
	btRadioCheck = (CButton*)GetDlgItem(IDC_RADIO_REGISTER_TAG);
	bCheck = btRadioCheck->GetCheck();
	if(bCheck == TRUE)
		nSearchType = 2;

	return nSearchType;
}

void CDLG_TagMapping::DataShowTemp()
{
	ControlEnable(FALSE);
	SetTimer(2,500,NULL);
}

//{"번호","TAG명","그룹","유형","물질","단위","위치","장비-대","장비-중","장비-소","세부","계측타입"};
void CDLG_TagMapping::SetListRowTagItems(const char *szNum,ST_TAG_LIST *pstTagInfo)
{
	CString strType;
	int nItem = m_xListTagItems.GetItemCount();
	m_xListTagItems.InsertItem(nItem, szNum);
	m_xListTagItems.SetItemText(nItem, 1, pstTagInfo->szTAG_NAME);
	m_xListTagItems.SetItemText(nItem, 2, pstTagInfo->szNEW_TAG_TP_GRP_NM);
	m_xListTagItems.SetItemText(nItem, 3, pstTagInfo->szNEW_TAG_TP_NM);
	m_xListTagItems.SetItemText(nItem, 4, pstTagInfo->szNEW_MTAL_NM);
	m_xListTagItems.SetItemText(nItem, 5, pstTagInfo->szNEW_UNIT_NM);
	m_xListTagItems.SetItemText(nItem, 6, pstTagInfo->szNEW_LOC_FLR_NM);
	m_xListTagItems.SetItemText(nItem, 7, pstTagInfo->szNEW_DV_CLSF_01_NM);
	m_xListTagItems.SetItemText(nItem, 8, pstTagInfo->szNEW_DV_CLSF_02_NM);
	m_xListTagItems.SetItemText(nItem, 9, pstTagInfo->szNEW_DV_CLSF_03_NM);
	m_xListTagItems.SetItemText(nItem, 10, pstTagInfo->szNEW_DV_CLSF_LOC_NM);
	
	if(pstTagInfo->nTAG_TYPE== TYPE_AI)
		strType = "AI";
	else if(pstTagInfo->nTAG_TYPE== TYPE_DI)
		strType = "DI";
	m_xListTagItems.SetItemText(nItem, 11, strType);
	//m_xListScheduleList.SetItemText(nItem, 5, szData6);

	//UpdateData(FALSE);
}

void CDLG_TagMapping::ListRowAdd_Test(const char *szData,const char *szData1)
{
	CString strType;
	int nItem = m_xListTagItems.GetItemCount();
	m_xListTagItems.InsertItem(nItem, szData);
	m_xListTagItems.SetItemText(nItem, 1,szData1);
	m_xListTagItems.SetItemText(nItem, 2,"");
	m_xListTagItems.SetItemText(nItem, 3,"");
	m_xListTagItems.SetItemText(nItem, 4,"");
	m_xListTagItems.SetItemText(nItem, 5,"");
	m_xListTagItems.SetItemText(nItem, 6,"");
	m_xListTagItems.SetItemText(nItem, 7,"");
	m_xListTagItems.SetItemText(nItem, 8,"");
	m_xListTagItems.SetItemText(nItem, 9,"");
	m_xListTagItems.SetItemText(nItem, 10,"");
	m_xListTagItems.SetItemText(nItem, 11, "");
}

void CDLG_TagMapping::OnBnClickedButtonBackItem()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ControlEnable(FALSE);
	if(25 >= m_nPageItemCount )
	{
		ControlEnable(TRUE);
		return;
	}
	m_nPageItemCount = m_nPageItemCount - 25;
	m_nItemRowStart = m_nPageItemCount - 25;
	SetTimer(2,500,NULL);
}


void CDLG_TagMapping::OnBnClickedButtonNextItem()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ControlEnable(FALSE);
	int nTotalTagList = m_STAllTagList->GetSize();
	if(nTotalTagList < m_nPageItemCount)
	{
		ControlEnable(TRUE);
		AfxMessageBox("더이상 없습니다.");
		return;
	}
	m_nItemRowStart = m_nPageItemCount;
	m_nPageItemCount = m_nPageItemCount + 25;

	SetTimer(2,500,NULL);
}

void CDLG_TagMapping::OnBLClickedList(NMHDR* pNMHDR, LRESULT* pResult)
{

	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nRow = -1;
	int nColumn = -1;
	if (pNMIA)
	{
		nRow = pNMIA->iItem;
		nColumn = pNMIA->iSubItem;
	}

	if (nRow >= 0 && nColumn >= 0)
	{
		CString strRow = m_xListTagItems.GetItemText(nRow, 0);
		SetTagMapping_PopUp(m_nItemRowStart + nRow);
	}

	*pResult = 0;
}

void CDLG_TagMapping::ControlEnable(BOOL bEnable)
{
	GetDlgItem(IDC_RADIO_UNREGISTERED_TAG)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_REGISTER_TAG)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_BACK_ITEM)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_NEXT_ITEM)->EnableWindow(bEnable);
}

void CDLG_TagMapping::GetType()
{
	int nRowCount = SelectCode_Count("01_NEW_TAG_TP_GRP");
	ST_CODE *stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("01_NEW_TAG_TP_GRP",stCodeList,"");
	m_stGrp.init(nRowCount);
	m_stGrp.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("03_NEW_MTAL");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("03_NEW_MTAL",stCodeList,"");
	m_stMtal.init(nRowCount);
	m_stMtal.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("04_NEW_UNIT");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("04_NEW_UNIT",stCodeList,"");
	m_stUnit.init(nRowCount);
	m_stUnit.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}
	
	nRowCount = SelectCode_Count("05_NEW_LOC_FLR");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("05_NEW_LOC_FLR",stCodeList,"order by CODE_NAME asc");
	m_stFlr.init(nRowCount);
	m_stFlr.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}
	
	nRowCount = SelectCode_Count("06_NEW_DV_CLSF_01");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("06_NEW_DV_CLSF_01",stCodeList,"");
	m_stClsf_Lv1.init(nRowCount);
	m_stClsf_Lv1.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("07_NEW_DV_CLSF_02");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("07_NEW_DV_CLSF_02",stCodeList,"");
	m_stClsf_Lv2.init(nRowCount);
	m_stClsf_Lv2.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("08_NEW_DV_CLSF_03");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("08_NEW_DV_CLSF_03",stCodeList,"");
	m_stClsf_Lv3.init(nRowCount);
	m_stClsf_Lv3.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("09_NEW_DV_CLSF_LOC");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("09_NEW_DV_CLSF_LOC",stCodeList,"");
	m_stClsf_Loc.init(nRowCount);
	m_stClsf_Loc.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("10_NEW_VIRT_TAG");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("10_NEW_VIRT_TAG",stCodeList,"");
	m_stVirt.init(nRowCount);
	m_stVirt.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("11_NEW_MEAU_CYCLE");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("11_NEW_MEAU_CYCLE",stCodeList,"");
	m_stCycle.init(nRowCount);
	m_stCycle.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("12_NEW_MEAU_CYCLE_UNIT");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("12_NEW_MEAU_CYCLE_UNIT",stCodeList,"");
	m_stCycle_Unit.init(nRowCount);
	m_stCycle_Unit.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}

	nRowCount = SelectCode_Count("BEMS_BLD_LIST");
	stCodeList = new ST_CODE[nRowCount];
	memset(stCodeList ,0x00,sizeof(ST_CODE)*nRowCount);
	SelectCode("BEMS_BLD_LIST",stCodeList,"");
	m_stBld_List.init(nRowCount);
	m_stBld_List.SetDataAdd(stCodeList);
	if(stCodeList != NULL)
	{
		delete []stCodeList;
		stCodeList = NULL;
	}
	
	SelectSite();

}

void CDLG_TagMapping::SetTagMapping_PopUp(int nPos)
{
	ST_TAG_LIST stTag = m_STAllTagList->GetData(nPos);
	CDLG_TagMappingSet dlg;
	dlg.DB_Connect = DB_Connect;
	dlg.m_nDBType = m_nDBType;

	dlg.m_stTagInfo = stTag;
	dlg.m_stGrp = m_stGrp;
	dlg.m_stMtal = m_stMtal;
	dlg.m_stUnit = m_stUnit;
	dlg.m_stFlr = m_stFlr;
	dlg.m_stClsf_Lv1 = m_stClsf_Lv1;
	dlg.m_stClsf_Lv2 = m_stClsf_Lv2;
	dlg.m_stClsf_Lv3 = m_stClsf_Lv3;
	dlg.m_stClsf_Loc = m_stClsf_Loc;
	dlg.m_stVirt = m_stVirt;
	dlg.m_stCycle = m_stCycle;
	dlg.m_stCycle_Unit = m_stCycle_Unit;
	dlg.m_stBld_List = m_stBld_List;
	dlg.m_stSiteInfo = m_stSiteInfo;
	dlg.m_pCtrl = this;
	dlg.m_nRowPos = nPos;
	dlg.DoModal();
}

void CDLG_TagMapping::SetListUpdate(int nRowPos,ST_TAG_LIST *pstTagInfo)
{

	CString strType;
	m_xListTagItems.SetItemText(nRowPos, 1, pstTagInfo->szTAG_NAME);
	m_xListTagItems.SetItemText(nRowPos, 2, pstTagInfo->szNEW_TAG_TP_GRP_NM);
	m_xListTagItems.SetItemText(nRowPos, 3, pstTagInfo->szNEW_TAG_TP_NM);
	m_xListTagItems.SetItemText(nRowPos, 4, pstTagInfo->szNEW_MTAL_NM);
	m_xListTagItems.SetItemText(nRowPos, 5, pstTagInfo->szNEW_UNIT_NM);
	m_xListTagItems.SetItemText(nRowPos, 6, pstTagInfo->szNEW_LOC_FLR_NM);
	m_xListTagItems.SetItemText(nRowPos, 7, pstTagInfo->szNEW_DV_CLSF_01_NM);
	m_xListTagItems.SetItemText(nRowPos, 8, pstTagInfo->szNEW_DV_CLSF_02_NM);
	m_xListTagItems.SetItemText(nRowPos, 9, pstTagInfo->szNEW_DV_CLSF_03_NM);
	m_xListTagItems.SetItemText(nRowPos, 10, pstTagInfo->szNEW_DV_CLSF_LOC_NM);

	if(pstTagInfo->nTAG_TYPE== TYPE_AI)
		strType = "AI";
	else if(pstTagInfo->nTAG_TYPE== TYPE_DI)
		strType = "DI";
	else 
		strType ="-";
	m_xListTagItems.SetItemText(nRowPos, 11, strType);
	m_xListTagItems.Invalidate(FALSE);
}

int CDLG_TagMapping::SelectCode_Count(const char *szTableName)
{
	CString strMSGTitle = "SelectCode";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="";
	CString strCount,strCodeName;
	int nTotalRow = 0;

	//오라클 타입 쿼리 필요

	//MSSQL 타입 쿼리
	strQuery.Format("SELECT count(CODE) as RowCounr FROM [ENERGY_MANAGE].[dbo].[%s]",szTableName);

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nTotalRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nRow = 0; nRow < nTotalRow ; nRow++)
				{
					DB_Connect->GetFieldValue(pRs, "RowCounr", strCount);				//TAG id
					
					pRs->MoveNext();
				}
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}	
			return atoi(strCount);
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			//			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB 접속 실패!",strMSGTitle);
				//				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMSGTitle);
		//		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

int CDLG_TagMapping::SelectCode(const char *szTableName,ST_CODE *stCodeList, const char *szOrder)
{
	CString strMSGTitle = "SelectCode";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="";
	CString strCode,strCodeName;
	int nTotalRow = 0;

	//오라클 타입 쿼리 필요

	//MSSQL 타입 쿼리
	strQuery.Format("SELECT CODE,CODE_NAME	FROM [ENERGY_MANAGE].[dbo].[%s] %s",szTableName,szOrder);

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nTotalRow  = pRs->RecordCount;

		//stCodeList[0] = new ST_CODE[nTotalRow];
		//memset(stCodeList,0x00,sizeof(ST_CODE)*nTotalRow);

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nRow = 0; nRow < nTotalRow ; nRow++)
				{
					DB_Connect->GetFieldValue(pRs, "CODE", strCode);				//TAG id
					DB_Connect->GetFieldValue(pRs, "CODE_NAME", strCodeName);			//TAG 명
					
					strcpy_s(stCodeList[nRow].szCode,strCode);
					strcpy_s(stCodeList[nRow].szCodeName,strCodeName);
#ifdef _DEBUG
					TRACE("ItemNum= %d, Value = %s,%s\n",nRow,strCode,strCodeName);
#endif
					pRs->MoveNext();
				}
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}	
			return nTotalRow;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
//			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB 접속 실패!",strMSGTitle);
//				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMSGTitle);
//		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

int CDLG_TagMapping::SelectSite()
{
	CString strMSGTitle = "SelectSite";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="";
	CString strSiteName,strBEMSID,strBELID;
	int nTotalRow = 0;

	//오라클 타입 쿼리 필요

	//MSSQL 타입 쿼리
	strQuery.Format("SELECT SITE_NAME,BEMS_ID,BLD_ID FROM [ENERGY_MANAGE].[dbo].[BEMS_SITE_INFO] WHERE USE_YN = 1");

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nTotalRow  = pRs->RecordCount;

		memset(&m_stSiteInfo,0x00,sizeof(ST_SITE_INFO));
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nRow = 0; nRow < nTotalRow ; nRow++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strSiteName);				//TAG id
					DB_Connect->GetFieldValue(pRs, "BEMS_ID", strBEMSID);			//TAG 명
					DB_Connect->GetFieldValue(pRs, "BLD_ID", strBELID);			//TAG 명

					strcpy_s(m_stSiteInfo.szSiteName,strSiteName);
					strcpy_s(m_stSiteInfo.szBEMS_ID,strBEMSID);
					strcpy_s(m_stSiteInfo.szBEL_ID,strBELID);
#ifdef _DEBUG
					TRACE("Value = %s,%s,%s\n",strSiteName,strBEMSID,strBELID);
#endif
					pRs->MoveNext();
				}
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}	
			return nTotalRow;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			//			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB 접속 실패!",strMSGTitle);
				//				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMSGTitle);
		//		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

void CDLG_TagMapping::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nRow = m_xListTagItems.GetCurSel();
	
	if(nRow < 0)
	{
		AfxMessageBox("TAG리스트에서 선택이필요합니다.");
		return;
	}

	if(MessageBox("삭제 하시겠습니까?", "확인", MB_YESNO)== IDNO)
		return;
	
	DeleteRegisterTagItem(nRow);
}

void CDLG_TagMapping::DeleteRegisterTagItem(int nPos)
{
	ST_TAG_LIST stTag = m_STAllTagList->GetData(nPos);
	CString strQuery,strRunlog_E2;
	strQuery.Format("DELETE [ENERGY_MANAGE].[dbo].[TAG_MAPPING_LIST] WHERE HMI_TAG_ID = '%s' ",stTag.szHmiTag_ID);

	int nResult = DB_Connect->SetQueryRun(strQuery);

	if(nResult < 1)
	{
		strRunlog_E2.Format("Position : [등록된 TAG][삭제], log : [Query Fail],[%s]",strQuery);
		//SetWriteLogFile(": [Set Query Error..],",strRunlog_E2);

#ifdef _DEBUG
		TRACE("Set Query Error-ProcessorName : [%s]\r\n",strRunlog_E2);
#endif
		return;
	}	
	else
		strRunlog_E2.Format("등록된 Tag:[%s] 삭제..",stTag.szTAG_NAME);
	
	_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "TagMapping", USER_COLOR_BLUE, strRunlog_E2);

	if(m_pThread_TagSearch != NULL)
		m_pThread_TagSearch->m_nSearchType = 2;
}
