// DLG_GatherInfoSetting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_GatherInfoSetting.h"
#include "afxdialogex.h"


// CDLG_GatherInfoSetting 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_GatherInfoSetting, CDialog)

CDLG_GatherInfoSetting::CDLG_GatherInfoSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_GatherInfoSetting::IDD, pParent)
{
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시

	DB_Connect = NULL;
	m_nItemTotalCount = 0;
}

CDLG_GatherInfoSetting::~CDLG_GatherInfoSetting()
{
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	memset(m_stDBGatherList,0x00,sizeof(m_stDBGatherList));
	CoUninitialize();
}

void CDLG_GatherInfoSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEICEV_LIST, m_ComboDeicevList);
}


BEGIN_MESSAGE_MAP(CDLG_GatherInfoSetting, CDialog)
	ON_BN_CLICKED(IDC_RADIO_GATHER_TYPE_1, &CDLG_GatherInfoSetting::OnBnClickedRadioGatherType1)
	ON_BN_CLICKED(IDC_RADIO_GATHER_TYPE_2, &CDLG_GatherInfoSetting::OnBnClickedRadioGatherType2)
	ON_BN_CLICKED(IDC_BT_ITEM_ADD, &CDLG_GatherInfoSetting::OnBnClickedBtItemAdd)
	ON_BN_CLICKED(IDC_BT_ITEM_DELETE, &CDLG_GatherInfoSetting::OnBnClickedBtItemDelete)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDLG_GatherInfoSetting 메시지 처리기입니다.

TCHAR*	_lpszSiteList_Column[] = {"사이트명","디바이스명","상태"};

BOOL CDLG_GatherInfoSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//_CreateFont(&m_Font);

	ComposeList(m_xListSiteList, USER_CONTROL_LIST, IDC_STATIC_LIST,3,_lpszSiteList_Column);
	//m_xListSiteList.SetFont(&m_Font);

	GetGatherSetting();

	CButton *btRadioDBType;


	for (int nIndex = 0; nIndex < m_stInfoType.nCount; nIndex++)
	{
		ListInsertItem(m_stInfoType.stGatherUseList[nIndex].szSiteName,m_stInfoType.stGatherUseList[nIndex].szDeviceName,"사용");
	}

	if(m_stInfoType.nGatherType == 2)
	{
		BOOL bRet = SetDBConnect();

		btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_1);
		btRadioDBType->SetCheck(FALSE);
		btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_2);
		btRadioDBType->SetCheck(TRUE);

		if(bRet == FALSE)
			AfxMessageBox("접속 정보가 다르거나 접속 정보등록이 필요합니다.");
		else
			GetDeviceInfo();
			//GetSiteInfo();
	}
	else
	{
		btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_1);
		btRadioDBType->SetCheck(TRUE);
		btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_2);
		btRadioDBType->SetCheck(FALSE);

		GetDlgItem(IDC_COMBO_DEICEV_LIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_ITEM_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_ITEM_DELETE)->EnableWindow(FALSE);
		m_xListSiteList.EnableWindow(FALSE);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDLG_GatherInfoSetting::ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[])
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
		;
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

BOOL CDLG_GatherInfoSetting::SetDBConnect()
{
	BOOL bRet = FALSE;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(0,0,"GatherInfoSet","");
	DB_Connect->DB_ConnectionInfo(m_stDbInfo.szServer,m_stDbInfo.szDB,m_stDbInfo.szID,m_stDbInfo.szPW,m_stDbInfo.unDBType);
	if(DB_Connect->DB_Connection() == TRUE)
	{
		bRet = TRUE;
		_addSystemMsg(0, USER_COLOR_BLUE, "DataGathering - Setting", USER_COLOR_LIME, "DBConnect - 성공");
	}
	else
	{
		bRet = FALSE;
		_addSystemMsg(0, USER_COLOR_RED, "DataGathering - Setting", USER_COLOR_RED, "DBConnect - 실패");
	}

	return bRet;
}

void CDLG_GatherInfoSetting::GetDeviceInfo()
{
	_RecordsetPtr pRs = NULL;
	CString strMsgTitle = "GetDevice Select";
	CString strQuery = "";
	CString strRunlog_E2 ="",strTempBuffer = "",strDvId,strDvName,strSiId,strSiName;
	int nCountRow = 0;

	memset(m_stDBGatherList,0x00,sizeof(m_stDBGatherList));
	m_ComboDeicevList.ResetContent();

	//strQuery.Format("SELECT DEVICE_ID, DEVICE_NAME	FROM HM_DEVICE_CFG WHERE SITE_ID = '%s'");
	strQuery.Format("SELECT Device.DEVICE_ID "
							",Device.DEVICE_NAME "
							",Siteinfo.SITE_ID "
							",Siteinfo.SITE_NAME "
							"FROM EASY_HMI.dbo.HM_DEVICE_CFG Device, EASY_COMMON.dbo.CM_SITE_INFO Siteinfo "
							" where Device.SITE_ID = Siteinfo.SITE_ID");
	//memset(m_stDeviceList,0x00,sizeof(m_stDeviceList));
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nIndex = 0; nIndex < nCountRow;nIndex++)
				{
					DB_Connect->GetFieldValue(pRs, "DEVICE_ID", strDvId);
					DB_Connect->GetFieldValue(pRs, "DEVICE_NAME", strDvName);
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strSiId);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strSiName);

					strTempBuffer.Format("%s/%s",strSiName,strDvName);
					m_ComboDeicevList.AddString(strTempBuffer);

					strcat_s(m_stDBGatherList[nIndex].szSiteId,strSiId);
					strcat_s(m_stDBGatherList[nIndex].szSiteName,strSiName);
					strcat_s(m_stDBGatherList[nIndex].szDeviceId,strDvId);
					strcat_s(m_stDBGatherList[nIndex].szDeviceName,strDvName);

					pRs->MoveNext();
				}

				m_nItemTotalCount = nCountRow;
				m_ComboDeicevList.SetCurSel(0);
				m_ComboDeicevList.EnableWindow(TRUE);
				GetDlgItem(IDC_BT_ITEM_ADD)->EnableWindow(TRUE);
			}
			else
			{
				m_ComboDeicevList.AddString("없음");
				m_ComboDeicevList.SetCurSel(0);
				m_ComboDeicevList.EnableWindow(FALSE);
				GetDlgItem(IDC_BT_ITEM_ADD)->EnableWindow(FALSE);
			}

			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}
		}
	}
	catch (_com_error &e)
	{
		int nOraCode = Com_Error(strMsgTitle,&e);

		switch(nOraCode)
		{
		case 942:
			{
				//strRunlog_E2.Format("%s",strMSGTitle);
				//SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
				//_addSystemMsg(0,USER_COLOR_BLUE, "DataGathering - Setting",USER_COLOR_BLUE,strRunlog_E2Log);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ;
		case 3113:
			{
				int nResult = DB_Connect->DB_ReConnection();
				if(nResult == 0)
				{
					//strRunlog_E2.Format("%s - DB 접속 실패!",strMSGTitle);
					//SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
				}
			}
			return ;//ERROR_DB_COM_ERROR;
		}
		return ;//ERROR_DB_COM_ERROR;

	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}

		strRunlog_E2.Format("Position : [%s], log : [catch Event][%s]",strMsgTitle,strQuery);
		_addSystemMsg(0,USER_COLOR_RED, "DataGathering - Setting",USER_COLOR_RED,strRunlog_E2);

		return;//ERROR_DB_COM_ERROR;
	}
}

void CDLG_GatherInfoSetting::OnBnClickedRadioGatherType1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_COMBO_DEICEV_LIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_ITEM_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_ITEM_DELETE)->EnableWindow(FALSE);
	m_xListSiteList.EnableWindow(FALSE);
}


void CDLG_GatherInfoSetting::OnBnClickedRadioGatherType2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bRet = SetDBConnect();
	if(bRet == FALSE)
	{
		CButton *btRadioDBType;
		btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_1);
		btRadioDBType->SetCheck(TRUE);
		btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_2);
		btRadioDBType->SetCheck(FALSE);
	}
	else
	{
		//GetDeviceInfo();
		GetDlgItem(IDC_COMBO_DEICEV_LIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_ITEM_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_ITEM_DELETE)->EnableWindow(TRUE);
		m_xListSiteList.EnableWindow(TRUE);
	}
}

void CDLG_GatherInfoSetting::SetGatherSetting()
{
	BOOL bCheck = FALSE;
	CButton *btRadioDBType;
	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_1);
	bCheck = btRadioDBType->GetCheck();
	if(bCheck == TRUE)
		m_stInfoType.nGatherType = 1;

	btRadioDBType = (CButton*)GetDlgItem(IDC_RADIO_GATHER_TYPE_2);
	bCheck = btRadioDBType->GetCheck();
	if(bCheck == TRUE)
		m_stInfoType.nGatherType = 2;

	_setGatherInfoType(&m_stInfoType,g_stProjectInfo.szProjectIniPath);
}

void CDLG_GatherInfoSetting::GetGatherSetting()
{
	memset(&m_stDbInfo,0x00,sizeof(m_stDbInfo));
	memset(&m_stInfoType,0x00,sizeof(m_stInfoType));
	m_stInfoType = _getGatherInfoType(g_stProjectInfo.szProjectIniPath);

	m_stDbInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
}


int CDLG_GatherInfoSetting::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "",strErrorID = "",strOraCode;
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("Position : [%s],Description : [%s], DB Error Code : [%08lx], Code meaning : [%s], Source : [%s]",
		szLogName,(LPCTSTR)bstrDescription,e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource);

	strErrorID.Format("%s",(LPCTSTR)bstrDescription);
	strOraCode = strErrorID.Mid(4,5);//Left(nPos);

#ifdef _DEBUG
	TRACE("ProcessorName : [GatherTypeSetting],Position : [%s][%s]\r\n",szLogName,strRunlog_E2);
#endif

	//strRunlog_E2Log.Format("[GatherTypeSetting] Position : [DB Com Error..], LogName: [%s], %s",szLogName, strRunlog_E2);
	_addSystemMsg(0,USER_COLOR_BLUE, "DataGathering - Setting",USER_COLOR_BLUE,strRunlog_E2);
	Sleep(500);

	return atoi(strOraCode);
}


void CDLG_GatherInfoSetting::OnBnClickedBtItemAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strMainBuffer = "",strSubBuffer = "";
	int nItem = m_xListSiteList.GetItemCount();
	int nComboPos = m_ComboDeicevList.GetCurSel();
	BOOL nCheck = FALSE;
	for(int nTotal = 0; nTotal < m_nItemTotalCount; nTotal++)
	{
		if(nCheck == TRUE)
			break;
		strMainBuffer = m_stDBGatherList[nComboPos].szSiteId;
		if(m_stInfoType.nCount != 0)
		{
			for(int nItemIndex = 0; nItemIndex < m_stInfoType.nCount ; nItemIndex++)
			{
				strSubBuffer = m_stInfoType.stGatherUseList[nItemIndex].szSiteId;
				if(strMainBuffer != strSubBuffer)
				{
					strcat_s(m_stInfoType.stGatherUseList[nItemIndex].szSiteId,m_stDBGatherList[nComboPos].szSiteId);
					strcat_s(m_stInfoType.stGatherUseList[nItemIndex].szSiteName,m_stDBGatherList[nComboPos].szSiteName);
					strcat_s(m_stInfoType.stGatherUseList[nItemIndex].szDeviceId,m_stDBGatherList[nComboPos].szDeviceId);
					strcat_s(m_stInfoType.stGatherUseList[nItemIndex].szDeviceName,m_stDBGatherList[nComboPos].szDeviceName);
					m_stInfoType.stGatherUseList[nItemIndex].nUse_YN = 1;
					ListInsertItem(m_stDBGatherList[nComboPos].szSiteName,m_stDBGatherList[nComboPos].szDeviceName,"사용");
					m_stInfoType.nCount++;
				}
				else
				{
					AfxMessageBox("이미 등록되어 있습니다.");
					nCheck = TRUE;
					break;
				}
			}
		}
		else
		{
			strcat_s(m_stInfoType.stGatherUseList[0].szSiteId,m_stDBGatherList[nComboPos].szSiteId);
			strcat_s(m_stInfoType.stGatherUseList[0].szSiteName,m_stDBGatherList[nComboPos].szSiteName);
			strcat_s(m_stInfoType.stGatherUseList[0].szDeviceId,m_stDBGatherList[nComboPos].szDeviceId);
			strcat_s(m_stInfoType.stGatherUseList[0].szDeviceName,m_stDBGatherList[nComboPos].szDeviceName);
			m_stInfoType.stGatherUseList[0].nUse_YN = 1;
			ListInsertItem(m_stDBGatherList[nComboPos].szSiteName,m_stDBGatherList[nComboPos].szDeviceName,"사용");
			m_stInfoType.nCount++;
		}
	}
	/*
	for(int nIndex = 0; nIndex < m_nItemTotalCount; nIndex++)
	{
		strMainBuffer = m_stDBGatherList[nIndex].szSiteId;
		strSubBuffer = m_stInfoType.stGatherUseList[nIndex].szSiteId;

		if(strMainBuffer == strSubBuffer)
		{
			AfxMessageBox("이미 등록되어 있습니다.");
			break;
		}
		else
		{
			strcat_s(m_stInfoType.stGatherUseList[nIndex].szSiteId,m_stDBGatherList[nIndex].szSiteId);
			strcat_s(m_stInfoType.stGatherUseList[nIndex].szSiteName,m_stDBGatherList[nIndex].szSiteName);
			strcat_s(m_stInfoType.stGatherUseList[nIndex].szDeviceId,m_stDBGatherList[nIndex].szDeviceId);
			strcat_s(m_stInfoType.stGatherUseList[nIndex].szDeviceName,m_stDBGatherList[nIndex].szDeviceName);
			m_stInfoType.stGatherUseList[nIndex].nUse_YN = 1;
			ListInsertItem(m_stDBGatherList[m_ComboDeicevList.GetCurSel()].szSiteName,m_stDBGatherList[m_ComboDeicevList.GetCurSel()].szDeviceName,"사용");
			m_stInfoType.nCount++;
		}
	}	*/
}

int CDLG_GatherInfoSetting::ListInsertItem(const char *szData1,const char *szData2,const char *szData3)
{
	int nItem = m_xListSiteList.GetItemCount();

	m_xListSiteList.InsertItem(nItem, szData1);
	m_xListSiteList.SetItemText(nItem, 1, szData2);
	m_xListSiteList.SetItemText(nItem, 2, szData3);

	m_xListSiteList.Invalidate(FALSE);

	return nItem;
}

void CDLG_GatherInfoSetting::OnBnClickedBtItemDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_xListSiteList.GetFirstSelectedItemPosition();
	if (NULL == pos)
		return;
	int nItem = m_xListSiteList.GetNextSelectedItem(pos);

	char szBuffer[48];
	memset(szBuffer,0x00,sizeof(szBuffer));
	m_xListSiteList.SetItemText(nItem, 2, "삭제");
	m_stInfoType.stGatherUseList[nItem].nUse_YN = 0;
	m_xListSiteList.Invalidate(FALSE);
}


void CDLG_GatherInfoSetting::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnClose();
}
