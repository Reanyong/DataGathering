// FormView_TAGGather.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TAGGather.h"
#include "ChildFrm.h"
#include "DlgTagDic.h"
#include "DlgEMSChart.h"

// CFormView_TAGGather

IMPLEMENT_DYNCREATE(CFormView_TAGGather, CFormView)

CFormView_TAGGather::CFormView_TAGGather()
	: CFormView(CFormView_TAGGather::IDD)
{
	g_nCheckTagThreadRun = 0;
	m_bStartRunCheck = TRUE;
	m_strTitle = "";
	m_pThread_MinGatherMain = NULL;
	m_b50Check = FALSE; // 20210308 ksw 50c초 체크 변수 초기화
}

CFormView_TAGGather::~CFormView_TAGGather()
{
	if (DB_Connect != NULL)
	{
		delete DB_Connect;
	}
}

void CFormView_TAGGather::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CFormView_TAGGather, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_GATHER_START, &CFormView_TAGGather::OnBnClickedButtonGatherStart)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnUserMessage)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(HDN_BEGINTRACK, OnHeaderBeginTrack)
	ON_BN_CLICKED(IDC_BTN_TAGDIC_INSERT, &CFormView_TAGGather::OnBnClickedBtnTagdicInsert)
	ON_BN_CLICKED(IDC_BTN_EMSCHART, &CFormView_TAGGather::OnBnClickedBtnEmschart)
END_MESSAGE_MAP()

// CFormView_TAGGather 진단입니다.

#ifdef _DEBUG
void CFormView_TAGGather::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormView_TAGGather::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CFormView_TAGGather 메시지 처리기입니다.
TCHAR* _lpszProcessorList_Column[] = { "ID","상태","할당수","처리 상태" };
TCHAR* _lpszTagDicList_Column[] = { "태그 이름", "태그 설명", "데이터 타입"};


void CFormView_TAGGather::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CString strTitle;
	g_nActivateDocCheck = FORM_VIEW_ID_2;
	strTitle.LoadStringA(IDS_MY_TREE_VIEW_1);
	GetDocument()->SetTitle(strTitle);

	CChildFrame *pWnd = (CChildFrame *)GetParent();
	pWnd->SetMenu(NULL);
	//pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX |WS_SYSMENU ,WS_BORDER, 0);
	pWnd->ModifyStyle(WS_THICKFRAME  | WS_MAXIMIZEBOX|WS_SYSMENU,WS_MAXIMIZE, 0);
	pWnd->RecalcLayout();
	pWnd->MDIRestore();
	pWnd->SetMenu(NULL);
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	time_t tStartTime = time(NULL); //20210308 ksw 시작 대기 변수
	struct tm *pLocalTime; //20210308 ksw 시작 대기 변수
	pLocalTime = localtime(&tStartTime); // 20210308 ksw 대기시간

	m_nMin = pLocalTime->tm_min;
	m_nSec = pLocalTime->tm_sec;

	_CreateFont(&m_Font);

	ComposeList(m_xListSiteList, USER_CONTROL_LIST, IDC_STATIC_TAG_LISTCTRL2, 4, _lpszProcessorList_Column);
	m_xListSiteList.SetFont(&m_Font);
	ComposeList(m_xListTagDicList, USER_CONTROL_LIST2, IDC_STC_TAGDIC_LISTCTRL, 3, _lpszTagDicList_Column);
	m_xListTagDicList.SetFont(&m_Font);


	GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(FALSE);
	m_strTitle = strTitle;

	m_stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	SetTimer(1, 100, NULL);

	//ON_BN_CLICKED(IDC_BUTTON_GATHER_START, &CFormView_TAGGather::OnBnClickedButtonGatherStart)
	//SendMessage(WM_COMMAND, IDC_BUTTON_GATHER_START, 0);
	// PostMessage(WM_COMMAND, IDC_BUTTON_GATHER_START, 0);
}

BOOL CFormView_TAGGather::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F4 || pMsg->wParam == VK_CONTROL)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}

BOOL CFormView_TAGGather::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
{
	int nRight = 0;
	CRect rcCtrl,rect;
	GetDlgItem(nPosListId)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(nPosListId)->ShowWindow(SW_HIDE);

	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	rcCtrl = CRect(rect.left + 5,rect.top + 5,rect.right - 1,rect.bottom - 1);
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

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCFMT_CENTER;
		/*switch(nPos)
		{
		case 0:
			lvcolumn.fmt = LVCFMT_CENTER;
			break;
		default:
			lvcolumn.fmt = LVCFMT_LEFT;
			break;
		}*/

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		if (nListID == USER_CONTROL_LIST2) // TAG_DIC List 컨트롤 ID 확인
		{
			if (nPos == 0)
				lvcolumn.cx = 440;
			else if (nPos == 1)
				lvcolumn.cx = 450;
			else
				lvcolumn.cx = 120;
		}

		else
		{
			nRight = (rect.right / nColumns);
			if (nPos == 0)
				nRight = nRight - 25;

			lvcolumn.cx = nRight;
		}

		listCtrl.InsertColumn(nPos, &lvcolumn);
	}

	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}


void CFormView_TAGGather::OnBnClickedButtonGatherStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(FALSE);

	if(m_bStartRunCheck == TRUE)
	{
		m_bStartRunCheck = FALSE;
		GetDlgItem(IDC_BUTTON_GATHER_START)->SetWindowText("수집 정지..");

		LoadTagDic();

		StartThread();
		GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(FALSE);

		//SetTimer(1, 100, NULL);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_GATHER_START)->SetWindowText("수집 시작..");
		//GetDlgItem(IDC_BUTTON_CALIBRATION)->EnableWindow(FALSE);
		g_nCheckTagThreadRun = 0;
		m_bStartRunCheck = TRUE;
		SetTimer(2, 100, NULL);
		GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(TRUE);
	}

	Invalidate(TRUE);
}

void CFormView_TAGGather::StartThread()
{
	//분테이터 수집 로직
	m_stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	if(m_pThread_MinGatherMain == NULL)
	{
		m_pThread_MinGatherMain = (CThread_MinGatherMain *)AfxBeginThread(RUNTIME_CLASS(CThread_MinGatherMain),
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED);
		m_pThread_MinGatherMain->SetLPVOID(this);
		m_pThread_MinGatherMain->SetWnd(m_hWnd);
		m_pThread_MinGatherMain->SetProgramName(m_strTitle);
		m_pThread_MinGatherMain->SetProduct(m_stGatherInfo.nProduct_Check);
		m_pThread_MinGatherMain->SetInterval(m_stGatherInfo.nGathe_Interval);

		m_pThread_MinGatherMain->ResumeThread();

		GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(TRUE);
	}
}

void CFormView_TAGGather::StopThread(BOOL bStopType)
{
	ShowText_State("수집 정지");
	ShowText_Count("-");
	ShowText_Processor("-");

	if(bStopType == TRUE)
		m_pThread_MinGatherMain->ButtonStop();

	if(m_pThread_MinGatherMain != NULL)
	{
		while (1)
		{
			if(m_pThread_MinGatherMain->GetStop() != TRUE)
				m_pThread_MinGatherMain->Stop();
			else
				break;
		}
		m_pThread_MinGatherMain = NULL;
	}
}


void CFormView_TAGGather::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nIDCtl==IDC_BUTTON_GATHER_START )         //checking for the button
	{
		COLORREF rgbBkColor,rgbTextColor;
		if(m_bStartRunCheck == FALSE)
		{
			rgbTextColor = RGB(255, 0, 0);
			rgbBkColor = RGB(255, 217, 236);
		}
		else
		{
			rgbTextColor = RGB(0, 0, 0);
			rgbBkColor = RGB(246,246,246);
		}

		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct ->hDC);   // Get the Button DC to CDC

		rect = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));

		dc.FillSolidRect(&rect,rgbBkColor);//Here you can define the required color to appear on the Button.

		UINT state=lpDrawItemStruct->itemState;  //This defines the state of the Push button either pressed or not.

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(rgbBkColor);   //Setting the Text Background color
		dc.SetTextColor(rgbTextColor);     //Setting the Text Color

		TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
		ZeroMemory(buffer,MAX_PATH );     //Intializing the buffer to zero
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH); //Get the Caption of Button Window

		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);//Redraw the  Caption of Button Window

		dc.Detach();  // Detach the Button DC
	}
	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


HBRUSH CFormView_TAGGather::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	UINT nID = pWnd->GetDlgCtrlID();
	COLORREF colorreStart = RGB(255,234,234);
	COLORREF colorreStop = RGB(235,247,255);


	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:   /// 다이얼로그 배경색을 white로.
		{
			if(m_bStartRunCheck == FALSE)
				hbr = CreateSolidBrush(colorreStart);
			else
				hbr = CreateSolidBrush(colorreStop);
			break;
		}
	case CTLCOLOR_STATIC:
		{
			if(nID == IDC_STATIC)
			{
				if(m_bStartRunCheck == FALSE)
					pDC->SetBkColor(colorreStart);   // static text 배경색 투명
				else
					pDC->SetBkColor(colorreStop);   // static text 배경색 투명
				break;

				return (HBRUSH)::GetStockObject(NULL_BRUSH);
			}
		}
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

LRESULT CFormView_TAGGather::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	CString sMsgBuffer = "";
	sMsgBuffer = (LPCSTR)(long)wParam;

	//lParam = 0 : SUCCESS
	//lParam = 1 : Error
	int nMsgType = 0;
	nMsgType = lParam;
	COLORREF rgbRet{};

	if(nMsgType == LOG_COLOR_BLUE)
		rgbRet = RGB(0, 0, 255);
	else if(nMsgType == LOG_COLOR_RED)
		rgbRet =  RGB(255, 0, 0);

	_addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, "DB log : [DB Access..]", rgbRet, sMsgBuffer);
	return 0;
}


void CFormView_TAGGather::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	switch(nIDEvent)
	{
	case 1:
		{
			KillTimer(nIDEvent);
			int nStartRun = m_stGatherInfo.nAutoRun_Check;
			if(nStartRun == 1)
			{
				if(m_bWait == TRUE)
				{
					GetDlgItem(IDC_BUTTON_GATHER_START)->SetWindowText("수집 정지..");
					GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(FALSE);
					m_bStartRunCheck = FALSE;
					StartThread();
					LoadTagDic();
				}
				else
				{
					SetTimer(4,1000,NULL);
				}
			}
			else
			{
				GetDlgItem(IDC_BUTTON_GATHER_START)->SetWindowText("수집 시작..");
				m_bStartRunCheck = TRUE;
				GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(TRUE);
			}
			Invalidate(TRUE);
		}
		break;
	case 2: //버튼으로 정지
		KillTimer(nIDEvent);
		StopThread(TRUE);

		GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(TRUE);

		break;
	case 3: //시스템 자동 정지
		KillTimer(nIDEvent);
		StopThread(FALSE);

		GetDlgItem(IDC_BUTTON_GATHER_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(TRUE);
		break;
	case 4: // 자동시작시 다음 정분까지 대기

		m_bStartRunCheck = TRUE;
		CString str;

		time_t tCurTime = time(NULL);
		struct tm *pCurTime = NULL;
		pCurTime = localtime(&tCurTime);

		if((m_nSec >= 55 && m_b50Check) || m_nSec < 55) //20210405 ksw 55초 에서 59초 사이에 게더링 시작시 1분 더 기다림 ex) 03분55초에 프로그램 시작 -> 5분에 수집 시작
		{
			str.Format("수집 준비..\r\n %02d : %02d",m_nMin - pCurTime->tm_min , 59 - pCurTime->tm_sec);
			if(m_nMin +1 == pCurTime->tm_min)
			{
				m_bWait = TRUE;
				KillTimer(nIDEvent);
				SetTimer(1,100,NULL);
				GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_BTN_TAGDIC_INSERT)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_GATHER_START)->SetWindowText(str);
			}

		}
		else
		{
			m_b50Check = TRUE;
			m_nMin++;
			if(m_nMin == 60)
				m_nMin = 0;
			if((m_nMin == 59) && (pCurTime->tm_min == 0))
			{
				m_nMin = 60;
				pCurTime->tm_min = 59;
			}
			str.Format("수집 준비..\r\n %02d : %02d",m_nMin - pCurTime->tm_min , 59 - pCurTime->tm_sec); //59 0 분
			GetDlgItem(IDC_BUTTON_GATHER_START)->SetWindowText(str);
		}
		CRect rect;
		GetDlgItem(IDC_BUTTON_GATHER_START)->GetWindowRect(&rect);

		InvalidateRect(&rect,TRUE);
		break;
	}

	CFormView::OnTimer(nIDEvent);
}


void CFormView_TAGGather::ListInitialization()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_xListSiteList.DeleteAllItems();
	m_xListSiteList.Invalidate(FALSE);

	//m_xListTagDicList.DeleteAllItems();
	//m_xListTagDicList.Invalidate(FALSE);
}


void CFormView_TAGGather::ShowListData(const char* szData1,const char* szData2,const char* szData3,const char* szData4)
{
	int nItem = m_xListSiteList.GetItemCount();
	m_xListSiteList.InsertItem(nItem, szData1);

	m_xListSiteList.SetItemText(nItem, 1, szData2);
	m_xListSiteList.SetItemText(nItem, 2, szData3);
	m_xListSiteList.SetItemText(nItem, 3, szData4);

	m_xListSiteList.Invalidate(FALSE);
}

void CFormView_TAGGather::ShowListMsgUdate(int nRow, const char *szStatus,const char *szMsg)
{
	m_xListSiteList.SetItemText(nRow - 1, 1, szStatus);
	m_xListSiteList.SetItemText(nRow - 1, 3, szMsg);

	m_xListSiteList.Invalidate(FALSE);
}

void CFormView_TAGGather::ShowText_State(const char* szData1)
{
	GetDlgItem(IDC_STATIC_RUN_STATE)->SetWindowText(szData1);
}

void CFormView_TAGGather::ShowText_Count(const char* szData1)
{
	GetDlgItem(IDC_STATIC_TOTAL_INPUT)->SetWindowText(szData1);
}

void CFormView_TAGGather::ShowText_Processor(const char* szData1)
{
	GetDlgItem(IDC_STATIC_PROCESSOR_COUNT)->SetWindowText(szData1);
}

void CFormView_TAGGather::OnDestroy()
{
	//CFormView::OnDestroy();
		StopThread(FALSE);
	int time = GetTickCount64();
	while(1) //20210310 ksw 쓰레드 시간벌이
	{
		if(GetTickCount64() - time>=1000)
		{
			break;
		}
		Sleep(1);
	}

	CFormView::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CFormView_TAGGather::OnBnClickedBtnTagdicInsert()
{
	CDlgTagDic dlg(this);
	dlg.DoModal();
}

void CFormView_TAGGather::OnHeaderBeginTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if (phdr->hdr.idFrom == IDC_STC_TAGDIC_LISTCTRL) // TAG_DIC List 컨트롤 ID 확인
	{
		*pResult = TRUE; // 크기 조정 방지
	}
	else
	{
		*pResult = FALSE; // 다른 리스트 컨트롤의 크기 조정 허용
	}
}

void CFormView_TAGGather::LoadTagDic()
{
	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	ST_DATABASENAME stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	m_strLogTitle = "TAG_DIC";

	m_nDBType = stDBInfo.unDBType;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

	CString strQuery;

	if (!DB_Connect->DB_Connection()) {
		AfxMessageBox(_T("DB 연결 실패"));
		return;
	}

	if (m_nDBType == DB_MSSQL)
	{
		m_xListTagDicList.DeleteAllItems();

		strQuery.Format(_T("SELECT TAG_ID, TAG_DESC, DATA_TYPE FROM HM_TAG_DIC"));

		_RecordsetPtr pRecordset = DB_Connect->DB_OpenRecordSet(strQuery);

		// 레코드셋에서 데이터 읽기
		while (!pRecordset->EndOfFile) {
			CString strTagName = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_ID"]->Value;
			CString strTagDesc = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_DESC"]->Value;
			CString strDataType = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["DATA_TYPE"]->Value;

			CString strDataTypeDisplay;

			if (strDataType == _T("0")) { strDataTypeDisplay = _T("순시"); }
			else if (strDataType == _T("1")) { strDataTypeDisplay = _T("가동제어"); }
			else if (strDataType == _T("3")) { strDataTypeDisplay = _T("센서"); }
			else if (strDataType == _T("5")) { strDataTypeDisplay = _T("적산"); }
			else { strDataTypeDisplay = _T("Unknown"); }

			int nItem = m_xListTagDicList.InsertItem(m_xListTagDicList.GetItemCount(), strTagName);
			m_xListTagDicList.SetItemText(nItem, 1, strTagDesc);
			m_xListTagDicList.SetItemText(nItem, 2, strDataTypeDisplay);

			pRecordset->MoveNext();
		}

		pRecordset->Close();
		DB_Connect->DB_Close();
	}

	else if (m_nDBType == DB_POSTGRE)
	{
		strQuery.Format(_T("SELECT tag_name, tag_desc, data_type FROM easy_hmi.hm_tag_dic"));

		SQLRETURN retcode;
		if (DB_Connect->codbc->SQLAllocStmtHandle() != SQL_SUCCESS)
		{
			AfxMessageBox(_T("SQLAllocStmtHandle 실패"));
			return;
		}

		retcode = DB_Connect->codbc->SQLExecDirect(strQuery);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			AfxMessageBox(_T("SQLExecDirect 실패"));
			return;
		}

		m_xListTagDicList.DeleteAllItems();

		while (true)
		{
			retcode = DB_Connect->codbc->SQLFetch();
			if (retcode == SQL_NO_DATA) {
				break; // 더 이상 데이터가 없으면 루프 종료
			}
			else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				AfxMessageBox(_T("SQLFetch 에러"));
				break;
			}

			SQLCHAR tagID[256], tagDesc[256], dataType[256];
			SQLLEN tagIDLen, tagDescLen, dataTypeLen;

			DB_Connect->codbc->SQLGetData(1, SQL_C_CHAR, tagID, sizeof(tagID), &tagIDLen);
			DB_Connect->codbc->SQLGetData(2, SQL_C_CHAR, tagDesc, sizeof(tagDesc), &tagDescLen);
			DB_Connect->codbc->SQLGetData(3, SQL_C_CHAR, dataType, sizeof(dataType), &dataTypeLen);

			CString strTagID(tagID), strTagDesc(tagDesc), strDataType(dataType);
			CString strDataTypeDisplay;

			if (strDataType == _T("0")) { strDataTypeDisplay = _T("순시"); }
			else if (strDataType == _T("1")) { strDataTypeDisplay = _T("가동제어"); }
			else if (strDataType == _T("3")) { strDataTypeDisplay = _T("센서"); }
			else if (strDataType == _T("5")) { strDataTypeDisplay = _T("적산"); }
			else { strDataTypeDisplay = _T("Unknown"); }

			int nItem = m_xListTagDicList.InsertItem(m_xListTagDicList.GetItemCount(), strTagID);
			m_xListTagDicList.SetItemText(nItem, 1, strTagDesc);
			m_xListTagDicList.SetItemText(nItem, 2, strDataTypeDisplay);
		}

		DB_Connect->codbc->Close();
	}
}

void CFormView_TAGGather::OnBnClickedBtnEmschart()
{
	CDlgEMSChart dlg(this);
	dlg.DoModal();
}
