// DLG_ISmartSetting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_ISmartSetting.h"
#include "afxdialogex.h"


// CDLG_ISmartSetting 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_ISmartSetting, CDialog)

CDLG_ISmartSetting::CDLG_ISmartSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_ISmartSetting::IDD, pParent)
{
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	DB_Connect = NULL;

	m_bDbClkCheck = NULL;
	m_nSiteCount = 0;
}

CDLG_ISmartSetting::~CDLG_ISmartSetting()
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

void CDLG_ISmartSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_SITE, m_xListSiteList);
}


BEGIN_MESSAGE_MAP(CDLG_ISmartSetting, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CDLG_ISmartSetting::OnBnClickedButtonRegister)
	ON_NOTIFY(NM_DBLCLK, USER_CONTROL_LIST, OnClickedList)
	ON_BN_CLICKED(IDC_RADIO_Y, &CDLG_ISmartSetting::OnBnClickedRadioY)
	ON_BN_CLICKED(IDC_RADIO_N, &CDLG_ISmartSetting::OnBnClickedRadioN)
	ON_BN_CLICKED(IDOK, &CDLG_ISmartSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CDLG_ISmartSetting 메시지 처리기입니다.

TCHAR*	_lpszSiteColumn[] = {"사이트명","접속ID","사용유무"};
BOOL CDLG_ISmartSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	_CreateFont(&m_Font);

	ComposeList(m_xListSiteList, USER_CONTROL_LIST, IDC_LIST_SITE,3,_lpszSiteColumn);
	m_xListSiteList.SetFont(&m_Font);


	CButton *btRadioCheck_USE_YN;
	btRadioCheck_USE_YN = (CButton*)GetDlgItem(IDC_RADIO_N); 
	btRadioCheck_USE_YN->SetCheck(1);

	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(FALSE);

	if(GetSiteSearch() != TRUE)
	{
		AfxMessageBox("DB 접속할 수 없습니다.");
		return FALSE;
	}



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDLG_ISmartSetting::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
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
			lvcolumn.fmt = LVCFMT_CENTER;
			break;
		default:
			lvcolumn.fmt = LVCFMT_LEFT;
			break;
		}

		lvcolumn.pszText = szColumn[nPos];
		lvcolumn.iSubItem = nPos;

		nRight = (rect.right / nColumns);
		if(nPos == 0)
			nRight = nRight - 25;

		lvcolumn.cx = nRight;

		listCtrl.InsertColumn(nPos, &lvcolumn);
	}

	
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;
}


BOOL CDLG_ISmartSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDLG_ISmartSetting::GetSiteSearch()
{
	ST_DBINFO stDBInfo;
	stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(0,0,"DB Setting","");

	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);
	if(DB_Connect->DB_Connection() == TRUE)
	{
		_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, "접속 성공");
		m_nSiteCount = GetSiteList();
		return TRUE;
	}
	else
	{
		_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, "데이터 베이스에 접속할 수 없습니다.");
		return FALSE;
	}
}

int CDLG_ISmartSetting::GetSiteList()
{
	CString strMsgTitle = "GetSiteList Search";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strRetSiteid,strRetSiteName,strUseYNMsg;

	strQuery.Format("SELECT SITE_ID"
		" ,SITE_NAME "
		" FROM CM_SITE_INFO "
		" WHERE USE_YN = 1");
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		memset(m_stSiteInterlock,0x00,sizeof(m_stSiteInterlock));
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nI = 0; nI < nCountRow ; nI++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strRetSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strRetSiteName);

					CString strRetInfo = GetSiteSettingInfo(strRetSiteid);
					
					strcpy_s(m_stSiteInterlock[nI].szSiteId,strRetSiteid);
					strcpy_s(m_stSiteInterlock[nI].szSiteName,strRetSiteName);
					m_stSiteInterlock[nI].nRowIndex = nI + 1;

					if(strRetInfo == "-")
						ListInsertItem(strRetSiteName,"미설정","미지정");
					else
					{
						CString strTempBuff = "";
						int nPos = strRetInfo.Find(",");
						if(nPos < 0)
							return FALSE;
						strTempBuff = strRetInfo.Left(nPos);
						strRetInfo.Delete(0,nPos + 1);
						strcpy_s(m_stSiteInterlock[nI].szID,strTempBuff);		

						nPos = strRetInfo.Find(",");
						strTempBuff = strRetInfo.Left(nPos);
						strRetInfo.Delete(0,nPos + 1);
						strcpy_s(m_stSiteInterlock[nI].szPW,strTempBuff);

						m_stSiteInterlock[nI].nUSE_YN = atoi(strRetInfo);
						
						if(m_stSiteInterlock[nI].nUSE_YN == 1)
							strUseYNMsg = "사용";
						else
							strUseYNMsg = "미사용";
												
						ListInsertItem(strRetSiteName,m_stSiteInterlock[nI].szID,strUseYNMsg);
					}

					pRs->MoveNext();					
				}
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}

			return nCountRow;
		}
	}
	catch (_com_error &e)
	{
		Com_Error(strMsgTitle,&e);
		//CString strDescription = e.Description();
		if(0x80004005 == e.Error())
		{
			strRunlog_E2.Format("Position : [%s], log : [DB 접속 접속 시도..]",strMsgTitle);
			SetWriteLogFile("Processor-log : [_com_error..],",strRunlog_E2);

			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB 접속 실패!]",strMsgTitle);
				SetWriteLogFile("Processor-log : [_com_error..],",strRunlog_E2);

				return ERROR_DB_RECONNECTION;
			}
		}
		return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}

		strRunlog_E2.Format("Position : [%s], log : [catch Event][%s]",strMsgTitle,strQuery);
		SetWriteLogFile("Processor-log : [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}
	return 0;
}

void CDLG_ISmartSetting::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "";
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("DB Error Code : [%08lx], Code meaning : [%s], Source : [%s], Description : [%s]",
		e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);

#ifdef _DEBUG
	TRACE("ProcessorName : [ISmart 설정],Position : [%s][%s]\r\n",szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("Position : [ISmart 설정], LogName: [%s], %s",szLogName, strRunlog_E2);
	SetWriteLogFile("Processor-log : [DB Com Error..],",strRunlog_E2Log);
	Sleep(500);
}

void CDLG_ISmartSetting::SetWriteLogFile(const char *sTitle,const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, sTitle, USER_COLOR_PINK, szLogMsg);
	_WriteLogFile(g_stProjectInfo.szProjectLogPath,"Setting",szLogMsg);		
	LeaveCriticalSection(&g_cs);
}

void CDLG_ISmartSetting::ListInsertItem(const char *szData1,const char *szData2,const char *szData3)
{
	int nItem = m_xListSiteList.GetItemCount();

	m_xListSiteList.InsertItem(nItem, szData1);
	m_xListSiteList.SetItemText(nItem, 1, szData2);
	m_xListSiteList.SetItemText(nItem, 2, szData3);

	m_xListSiteList.Invalidate(FALSE);
}

void CDLG_ISmartSetting::OnClickedList(NMHDR* pNMHDR, LRESULT* pResult)
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
		CString strText = m_xListSiteList.GetItemText(nRow, nColumn);

		//m_stSiteInterlock[nRow].szSiteId;
		GetDlgItem(IDC_EDIT_SITE_NAME)->SetWindowText(strText);
		GetDlgItem(IDC_EDIT_ID)->SetWindowText(m_stSiteInterlock[nRow].szID);
		GetDlgItem(IDC_EDIT_PW)->SetWindowText(m_stSiteInterlock[nRow].szPW);
		
		CButton *btRadioCheck_USE_Y = (CButton*)GetDlgItem(IDC_RADIO_Y);
		CButton *btRadioCheck_USE_N = (CButton*)GetDlgItem(IDC_RADIO_N);

		if(m_stSiteInterlock[nRow].nUSE_YN == 1)
		{
			btRadioCheck_USE_Y->SetCheck(1);
			btRadioCheck_USE_N->SetCheck(0);
		}
		else
		{
			btRadioCheck_USE_Y->SetCheck(0);
			btRadioCheck_USE_N->SetCheck(1);
		}

		GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(TRUE);
		m_bDbClkCheck = TRUE;
	}

	*pResult = 0;
}

void CDLG_ISmartSetting::OnBnClickedButtonRegister()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strID,strPW,strUseYNMsg;
	int nRadioCheck_USE_YN = 0;
	if(m_bDbClkCheck == TRUE)
	{
		int nListRow = m_xListSiteList.GetCurSel();
		GetDlgItem(IDC_EDIT_ID)->GetWindowText(strID);
		GetDlgItem(IDC_EDIT_PW)->GetWindowText(strPW);

		CButton *btRadioCheck_USE_YN;
		btRadioCheck_USE_YN= (CButton*)GetDlgItem(IDC_RADIO_Y);
		BOOL bRet = btRadioCheck_USE_YN->GetCheck();
		if(bRet == TRUE)
			btRadioCheck_USE_YN= (CButton*)GetDlgItem(IDC_RADIO_Y);
		else
			btRadioCheck_USE_YN = (CButton*)GetDlgItem(IDC_RADIO_N); 


		CButton *btRadioCheck_USE_Y = (CButton*)GetDlgItem(IDC_RADIO_Y);
		if(btRadioCheck_USE_Y->GetCheck() == 1)
		{
			nRadioCheck_USE_YN = 1;
			strUseYNMsg = "사용";
		}
		CButton *btRadioCheck_USE_N = (CButton*)GetDlgItem(IDC_RADIO_N);
		if(btRadioCheck_USE_N->GetCheck() == 1)
		{
			nRadioCheck_USE_YN = 0;
			strUseYNMsg = "미사용";
		}

		m_xListSiteList.SetItemText(nListRow, 1, strID);
		m_xListSiteList.SetItemText(nListRow, 2, strUseYNMsg);

		m_stSiteInterlock[nListRow].nUSE_YN = nRadioCheck_USE_YN;
		if(strID.IsEmpty() == FALSE)
		{
			strcpy_s(m_stSiteInterlock[nListRow].szID,strID);
			strcpy_s(m_stSiteInterlock[nListRow].szPW,strPW);
			m_stSiteInterlock[nListRow].nEdit_Check = 1;
		}
		
		m_xListSiteList.Invalidate(FALSE);
		m_bDbClkCheck = FALSE;
	}
	else
		AfxMessageBox("사이트 정보를 선택하십시오");


	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(FALSE);
}

void CDLG_ISmartSetting::OnBnClickedRadioY()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *btRadioCheck_USE_Y = (CButton*)GetDlgItem(IDC_RADIO_Y);
	btRadioCheck_USE_Y->SetCheck(1);
	CButton *btRadioCheck_USE_N = (CButton*)GetDlgItem(IDC_RADIO_N);
	btRadioCheck_USE_N->SetCheck(0);
}

void CDLG_ISmartSetting::OnBnClickedRadioN()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *btRadioCheck_USE_Y = (CButton*)GetDlgItem(IDC_RADIO_Y);
	btRadioCheck_USE_Y->SetCheck(0);
	CButton *btRadioCheck_USE_N = (CButton*)GetDlgItem(IDC_RADIO_N);
	btRadioCheck_USE_N->SetCheck(1);
}

void CDLG_ISmartSetting::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	for(int nI = 0; nI < m_nSiteCount ; nI++)
	{
		CString strBuffer = "";
		CString strSettingPath = "";
		if(m_stSiteInterlock[nI].nEdit_Check == 1)
		{
			strSettingPath.Format("%s\\SiteList.ini",g_stProjectInfo.szProjectPath);
			strBuffer.Format("%s,%s,%d",m_stSiteInterlock[nI].szID,m_stSiteInterlock[nI].szPW,m_stSiteInterlock[nI].nUSE_YN);
			WritePrivateProfileString("SiteList",m_stSiteInterlock[nI].szSiteId,strBuffer,strSettingPath);
			
			strBuffer.Format("%s 등록 되었습니다.",m_stSiteInterlock[nI].szSiteName);
			_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_PINK, strBuffer);
		}
	}

	CDialog::OnOK();
}

CString CDLG_ISmartSetting::GetSiteSettingInfo(const char *szSiteID)
{
	char szBuffer[64];
	memset(szBuffer,0x00,sizeof(szBuffer));
	CString strSettingPath = "";
	strSettingPath.Format("%s\\SiteList.ini",g_stProjectInfo.szProjectPath);
	GetPrivateProfileString("SiteList", szSiteID, "-", szBuffer, sizeof(szBuffer), strSettingPath);

	return szBuffer;
}
