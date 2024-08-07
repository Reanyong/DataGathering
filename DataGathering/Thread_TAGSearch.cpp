// Thread_TAGSearch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_TAGSearch.h"
#include "DLG_TagMapping.h"


// CThread_TAGSearch

IMPLEMENT_DYNCREATE(CThread_TAGSearch, CWinThread)

	CThread_TAGSearch::CThread_TAGSearch()
{
	m_bEndThread = FALSE;
	DB_Connect = NULL;
	m_nSearchType = 0;
}

CThread_TAGSearch::~CThread_TAGSearch()
{
	/*m_tempTagList->DeleteItem();
	if(m_tempTagList !=  NULL)
	{
	delete m_tempTagList;
	m_tempTagList = NULL;
	}*/
}

BOOL CThread_TAGSearch::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	//CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_TAGSearch::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	/*	if(DB_Connect != NULL)
	{
	if(DB_Connect->GetDB_ConnectionStatus() == 1)
	DB_Connect->DB_Close();

	delete DB_Connect;
	DB_Connect = NULL;
	}
	CoUninitialize();
	*/
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_TAGSearch, CWinThread)
END_MESSAGE_MAP()


CString CThread_TAGSearch::Com_Error(const char *szLogName,_com_error *e)
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
	TRACE("ProcessorName : [%s],Position : [%s][%s]\r\n",m_strLogTitle,szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("[%s] Position : [DB Com Error..], LogName: [%s], %s",m_strLogTitle,szLogName, strRunlog_E2);

	SetWriteLogFile(strRunlog_E2Log);
	SysLogOutPut(m_strLogTitle,strRunlog_E2,USER_COLOR_RED);
	Sleep(500);

	return strErrorCode;
}

void CThread_TAGSearch::SetWriteLogFile(const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath,LOG_FOLDER_NAME_4,szLogMsg);
	LeaveCriticalSection(&g_cs);
}

void CThread_TAGSearch::SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody)
{
	_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "Title", crBody, strMsg);
}


// CThread_TAGSearch 메시지 처리기입니다.
int CThread_TAGSearch::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	int nTimeTemp = -1;
	CTime currentTime;
	CString strDeviceItem,strDBName;

	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
	ST_GATHERINFO_TYPE stGatherInfo = _getGatherInfoType(g_stProjectInfo.szProjectIniPath);

	if(stGatherInfo.nGatherType != 1)
	{
		strDeviceItem.Format("WHERE DEVICE_ID = '");
		for(int nIndex = 0; nIndex < stGatherInfo.nCount; nIndex++)
		{
			strDeviceItem += stGatherInfo.stGatherUseList[nIndex].szDeviceId;

			if(nIndex != (stGatherInfo.nCount -1) && stGatherInfo.nCount != 1)
				strDeviceItem += "' or DEVICE_ID = '";
			else
				strDeviceItem += "' AND ";
		}
	}
	else
		strDeviceItem.Format("WHERE ");

	BOOL bTempTestCheck =FALSE;
	do
	{
		if(m_bEndThread == TRUE)
			break;

		try
		{
			if(m_bEndThread)
				break;
			int nCheck = m_nSearchType;//((CDLG_TagMapping *)(m_pCtrl))->GetSearchType();
			if(nCheck == 1)
			{
				if(bTempTestCheck == FALSE)
				{
					int nRet = GetUnregisteredTag(stDBInfo.unDBType,strDeviceItem, stDBName.szHMIDBName);
					if(nRet == THREAD_END)
						break;
					bTempTestCheck = TRUE;
					((CDLG_TagMapping *)(m_pCtrl))->DataShowTemp();
				}
				if(m_bEndThread)
					break;
			}
			else if(nCheck == 2)
				if(bTempTestCheck == TRUE)
				{
					int nRet = GetRegisterTag(stDBInfo.unDBType,strDeviceItem, stDBName.szHMIDBName);//GetRegisterTag()
					if(nRet == THREAD_END)
						break;
					bTempTestCheck = FALSE;
					((CDLG_TagMapping *)(m_pCtrl))->DataShowTemp();
				}
				if(m_bEndThread)
					break;
				else
				{
					Sleep(1000);
					continue;
				}
				if(m_bEndThread)
					break;


		}
		catch (...)
		{
#ifdef _DEBUG
			TRACE("메인 스레드 catch ....\n");
#endif
		}

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);
	return CWinThread::Run();
}

int CThread_TAGSearch::GetUnregisteredTag(int nDBType,const char *szDeviceItem,const char *szDBName)
{
	CString strMSGTitle = "GetTagList";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="";
	CString strTagId,strTagName,strTagDESC,strTagType,strGroupName,strDBName;
	int nCountRow = 0;

	/*
	switch(nDBType)
	{
	case DB_MSSQL:
	{
	if(strlen(szDBName) > 1)
	strDBName.Format("%s.dbo.HM_TAG_DIC",szDBName);
	else
	strDBName.Format("HM_TAG_DIC");
	}
	break;
	case DB_ORACLE:
	{
	if(strlen(szDBName) > 1)
	strDBName.Format("%s.HM_TAG_DIC",szDBName);
	else
	strDBName.Format("HM_TAG_DIC");
	}
	break;
	case DB_POSTGRE:
	{
	if(strlen(szDBName) > 1)
	strDBName.Format("%s.Easy_Hmi.HM_TAG_DIC",szDBName);
	else
	strDBName.Format("HM_TAG_DIC");
	}
	break;
	default:
	break;
	}

	strQuery.Format("SELECT HMTagDic.TAG_ID,HMTagDic.TAG_NAME,HMTagDic.GROUP_NAME,HMTagDic.TAG_TYPE,HMTagDic.TAG_DESC "
	" FROM %s HMTagDic ,ENERGY_MANAGE.dbo.TAG_MAPPING_LIST ENGTagList "
	" WHERE  (HMTagDic.TAG_NAME != ENGTagList.TAG_NAME )"
	" and (HMTagDic.GATHER_USE_YN = 0 or HMTagDic.GATHER_USE_YN is null) "
	,strDBName);*/

	//20200212 jsh : postgre 사용할 때 쿼리 쪽 에러나는거 같으니 수정 필요
	if(nDBType == DB_POSTGRE)
	{
		if(strlen(szDBName) > 1)
			strDBName.Format("%s.HM_TAG_DIC",szDBName);
		else
			strDBName.Format("Easy_Hmi.HM_TAG_DIC");
		//strDBName.Format("%s.Easy_Hmi.HM_TAG_DIC",szDBName);
		strQuery.Format("SELECT TAG_NAME,GROUP_NAME,TAG_TYPE,TAG_DESC FROM * %s", strDBName);
	}
	else
	{
		strQuery.Format("SELECT TAG_NAME,GROUP_NAME,TAG_TYPE,TAG_DESC "
						" FROM ENERGY_MANAGE.dbo.View_Unrgistered_Tag ");
		//strQuery.Format("SELECT TAG_NAME,TAG_TYPE,TAG_DESC "
		//	" FROM EASY_HMI.dbo.HM_TAG_DIC ");
	}


	//20200221 나정호 수정 POSTGRE 부분에 ODBC 적용
	if(nDBType == DB_POSTGRE)
	{
		try
		{
			SQLRETURN retcode;
			retcode = DB_Connect->SetQueryRun(strQuery);
			if (isSqlOk(retcode))
			{
				m_tempTagList->init(nCountRow);
				int nSelecRowCnt = 0;
				int nTempCount = 0;
				int nSTListCnt = 0;

				while (1)
				{
					retcode = DB_Connect->codbc->COdbc::SQLFetch();

					if (isSqlOk(retcode))
					{

						SQLINTEGER cbTagName=0;
						SQLINTEGER cbTagTypeTemp=0;
						//SQLINTEGER cbGroupName=0;
						SQLINTEGER cbTagDesc=0;
						//SQLCHAR szTagId[128] = {0,};
						SQLCHAR szTagName[64] = {0,};
						//SQLCHAR szTagType[64] = {0,};
						SQLSMALLINT nTagTypeTemp = 0;
						//SQLCHAR szGroupName[64] = {0,};
						SQLCHAR szTagDesc[64] = {0,};
						void* Temp;

						DB_Connect->codbc->COdbc::SQLGetData(1,SQL_C_CHAR,szTagName,sizeof(szTagName),&cbTagName);
						//DB_Connect->codbc->COdbc::SQLGetData(2,SQL_C_CHAR,szGroupName,sizeof(szGroupName),&cbGroupName);
						DB_Connect->codbc->COdbc::SQLGetData(2,SQL_C_SSHORT,&nTagTypeTemp,sizeof(nTagTypeTemp),&cbTagTypeTemp);
						DB_Connect->codbc->COdbc::SQLGetData(3,SQL_C_SSHORT,szTagDesc,sizeof(szTagDesc),&cbTagDesc);


						ST_TAG_LIST stTagInfo;
						memset(&stTagInfo,0x00,sizeof(ST_TAG_LIST));

						//strcpy_s(stTagInfo.szHmiTag_ID,strTagId);

						Temp=szTagName;
						strcpy_s(stTagInfo.szTAG_NAME,(const char*)Temp);

						Temp=szTagDesc;
						strcpy_s(stTagInfo.szTAG_DESC,(const char*)Temp);

						stTagInfo.nTAG_TYPE = nTagTypeTemp;

						m_tempTagList->SetDataAdd(stTagInfo);


#ifdef _DEBUG
						TRACE("STListCnt = %d,TagCnt= %d, Value = %s,%s,%s,%d\n",nSTListCnt,nSelecRowCnt,strTagId,strTagName,strGroupName,atoi(strTagType));

						nSTListCnt ++;
#endif

					}
					else
					{
						break;
					}
				}

			}



		}
		catch (_com_error &e)
		{
			CString strErrorCode = Com_Error(strMSGTitle,&e);

			if(strErrorCode == "942")
			{
				strRunlog_E2.Format("%s",strMSGTitle);
				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
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
					SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
				}
				return ERROR_DB_RECONNECTION;
			}
			else
				return ERROR_DB_COM_ERROR;
		}
		catch(...)
		{
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}
			strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMSGTitle);
			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
			TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_QUERY_FAIL1;
		}
	}
	else
	{

	}




	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				m_tempTagList->init(nCountRow);
				int nSelecRowCnt = 0;
				int nTempCount = 0;
				for(int nSTListCnt = 0; nSTListCnt < nCountRow ; nSTListCnt++)
				{
					DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);				//TAG id
					DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);			//TAG 명
					DB_Connect->GetFieldValue(pRs, "GROUP_NAME", strGroupName);		//TAG 그룹명
					DB_Connect->GetFieldValue(pRs, "TAG_TYPE", strTagType);			//TAG 타입
					DB_Connect->GetFieldValue(pRs, "TAG_DESC", strTagDESC);			//TAG 설명

					ST_TAG_LIST stTagInfo;
					memset(&stTagInfo,0x00,sizeof(ST_TAG_LIST));
					strcpy_s(stTagInfo.szHmiTag_ID,strTagId);
					strcpy_s(stTagInfo.szTAG_NAME,strTagName);
					strcpy_s(stTagInfo.szTAG_DESC,strTagDESC);
					stTagInfo.nTAG_TYPE = atoi(strTagType);
					m_tempTagList->SetDataAdd(stTagInfo);
#ifdef _DEBUG
					TRACE("STListCnt = %d,TagCnt= %d, Value = %s,%s,%s,%d\n",nSTListCnt,nSelecRowCnt,strTagId,strTagName,strGroupName,atoi(strTagType));
#endif
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
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
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
				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
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
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}
	return nCountRow;
}

int CThread_TAGSearch::GetRegisterTag(int nDBType,const char *szDeviceItem,const char *szDBName)
{
	CString strMSGTitle = "RegisterTag";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;


	CString strQuery = "",strRunlog_E2 ="";
	CString strHmiTagId,strDvId,strTagId,strNEW_TAG_TP_GRP_Code,strNEW_TAG_TP_GRP_Name,strNEW_TAG_TP_Code,strNEW_TAG_TP_Name;
	CString strNEW_MTAL_Code,strNEW_MTAL_Name,strNEW_UNIT_Code,strNEW_UNIT_Name,strNEW_LOC_FLR_Code,strNEW_LOC_FLR_Name,strNEW_DV_CLSF_01_Code,strNEW_DV_CLSF_01_Name;
	CString strNEW_DV_CLSF_02_Code,strNEW_DV_CLSF_02_Name,strNEW_DV_CLSF_03_Code,strNEW_DV_CLSF_03_Name,strNEW_DV_CLSF_LOC_Code,strNEW_DV_CLSF_LOC_Name;
	CString strNEW_VIRT_TAG_Code,strNEW_VIRT_TAG_Name,strNEW_MEAU_CYCLE_Code,strNEW_MEAU_CYCLE_Name,strNEW_MEAU_CYCLE_UNIT_Code,strNEW_MEAU_CYCLE_UNIT_Name,strBld_Bund;
	CString strTagName,strTagDESC,strTagType;
	int nCountRow = 0;
	strQuery.Format("SELECT HMI_TAG_ID,DV_ID,TAG_ID"
		",(select code FROM ENERGY_MANAGE.dbo.[01_NEW_TAG_TP_GRP] WHERE code = NEW_TAG_TP_GRP) as NEW_TAG_TP_GRP_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[01_NEW_TAG_TP_GRP] WHERE code = NEW_TAG_TP_GRP) as NEW_TAG_TP_GRP_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[02_NEW_TAG_TP] WHERE GRP_CODE = NEW_TAG_TP_GRP and CODE = NEW_TAG_TP) as NEW_TAG_TP_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[02_NEW_TAG_TP] WHERE GRP_CODE = NEW_TAG_TP_GRP and CODE = NEW_TAG_TP) as NEW_TAG_TP_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[03_NEW_MTAL] WHERE CODE = NEW_MTAL) as NEW_MTAL_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[03_NEW_MTAL] WHERE CODE = NEW_MTAL) as NEW_MTAL_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[04_NEW_UNIT] WHERE CODE = NEW_UNIT) as NEW_UNIT_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[04_NEW_UNIT] WHERE CODE = NEW_UNIT) as NEW_UNIT_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[05_NEW_LOC_FLR] WHERE CODE = NEW_LOC_FLR) as NEW_LOC_FLR_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[05_NEW_LOC_FLR] WHERE CODE = NEW_LOC_FLR) as NEW_LOC_FLR_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[06_NEW_DV_CLSF_01] WHERE CODE = NEW_DV_CLSF_01) as NEW_DV_CLSF_01_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[06_NEW_DV_CLSF_01] WHERE CODE = NEW_DV_CLSF_01) as NEW_DV_CLSF_01_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[07_NEW_DV_CLSF_02] WHERE CODE = NEW_DV_CLSF_02) as NEW_DV_CLSF_02_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[07_NEW_DV_CLSF_02] WHERE CODE = NEW_DV_CLSF_02) as NEW_DV_CLSF_02_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[08_NEW_DV_CLSF_03] WHERE CODE = NEW_DV_CLSF_03) as NEW_DV_CLSF_03_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[08_NEW_DV_CLSF_03] WHERE CODE = NEW_DV_CLSF_03) as NEW_DV_CLSF_03_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[09_NEW_DV_CLSF_LOC] WHERE CODE = NEW_DV_CLSF_LOC) as NEW_DV_CLSF_LOC_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[09_NEW_DV_CLSF_LOC] WHERE CODE = NEW_DV_CLSF_LOC) as NEW_DV_CLSF_LOC_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[10_NEW_VIRT_TAG] WHERE CODE = NEW_VIRT_TAG) as NEW_VIRT_TAG_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[10_NEW_VIRT_TAG] WHERE CODE = NEW_VIRT_TAG) as NEW_VIRT_TAG_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[11_NEW_MEAU_CYCLE] WHERE CODE = NEW_MEAU_CYCLE) as NEW_MEAU_CYCLE_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[11_NEW_MEAU_CYCLE] WHERE CODE = NEW_MEAU_CYCLE) as NEW_MEAU_CYCLE_Name"
		",(select code FROM ENERGY_MANAGE.dbo.[12_NEW_MEAU_CYCLE_UNIT] WHERE CODE = NEW_MEAU_CYCLE_UNIT) as NEW_MEAU_CYCLE_UNIT_Code"
		",(select code_name FROM ENERGY_MANAGE.dbo.[12_NEW_MEAU_CYCLE_UNIT] WHERE CODE = NEW_MEAU_CYCLE_UNIT) as NEW_MEAU_CYCLE_UNIT_Name"
		",NEW_BLD_BUND"
		",TAG_NAME"
		",TAG_DESC"
		//",HIGH_LIMIT_VALUE"
		//",LOW_LIMIT_VALUE"
		",TAG_TYPE"
		" FROM [ENERGY_MANAGE].[dbo].[TAG_MAPPING_LIST]"
		" where HMI_TAG_ID != 'test' ");

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				m_tempTagList->init(nCountRow);
				int nSelecRowCnt = 0;
				int nTempCount = 0;
				for(int nSTListCnt = 0; nSTListCnt < nCountRow ; nSTListCnt++)
				{
					DB_Connect->GetFieldValue(pRs, "HMI_TAG_ID", strHmiTagId);				//TAG id
					DB_Connect->GetFieldValue(pRs, "DV_ID", strDvId);			//TAG 명
					DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);		//TAG 그룹명
					DB_Connect->GetFieldValue(pRs, "NEW_TAG_TP_GRP_Code", strNEW_TAG_TP_GRP_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_TAG_TP_GRP_Name", strNEW_TAG_TP_GRP_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_TAG_TP_Code", strNEW_TAG_TP_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_TAG_TP_Name", strNEW_TAG_TP_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_MTAL_Code", strNEW_MTAL_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_MTAL_Name", strNEW_MTAL_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_UNIT_Code", strNEW_UNIT_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_UNIT_Name", strNEW_UNIT_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_LOC_FLR_Code", strNEW_LOC_FLR_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_LOC_FLR_Name", strNEW_LOC_FLR_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_01_Code", strNEW_DV_CLSF_01_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_01_Name", strNEW_DV_CLSF_01_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_02_Code", strNEW_DV_CLSF_02_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_02_Name", strNEW_DV_CLSF_02_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_03_Code", strNEW_DV_CLSF_03_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_03_Name", strNEW_DV_CLSF_03_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_LOC_Code", strNEW_DV_CLSF_LOC_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_LOC_Name", strNEW_DV_CLSF_LOC_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_VIRT_TAG_Code", strNEW_VIRT_TAG_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_VIRT_TAG_Name", strNEW_VIRT_TAG_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_MEAU_CYCLE_Code", strNEW_MEAU_CYCLE_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_MEAU_CYCLE_Name", strNEW_MEAU_CYCLE_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_MEAU_CYCLE_UNIT_Code", strNEW_MEAU_CYCLE_UNIT_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_MEAU_CYCLE_UNIT_Name", strNEW_MEAU_CYCLE_UNIT_Name);
					DB_Connect->GetFieldValue(pRs, "NEW_BLD_BUND", strBld_Bund);
					DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);			//TAG 설명
					DB_Connect->GetFieldValue(pRs, "TAG_DESC", strTagDESC);			//TAG 설명
					DB_Connect->GetFieldValue(pRs, "TAG_TYPE", strTagType);			//TAG 설명

					ST_TAG_LIST stTagInfo;
					memset(&stTagInfo,0x00,sizeof(ST_TAG_LIST));
					strcpy_s(stTagInfo.szHmiTag_ID,strHmiTagId);
					strcpy_s(stTagInfo.szDV_ID,strDvId);
					strcpy_s(stTagInfo.szTAG_ID,strTagId);
					strcpy_s(stTagInfo.szNEW_TAG_TP_GRP_code,strNEW_TAG_TP_GRP_Code);
					strcpy_s(stTagInfo.szNEW_TAG_TP_GRP_NM,strNEW_TAG_TP_GRP_Name);
					strcpy_s(stTagInfo.szNEW_TAG_TP_code,strNEW_TAG_TP_Code);
					strcpy_s(stTagInfo.szNEW_TAG_TP_NM,strNEW_TAG_TP_Name);
					strcpy_s(stTagInfo.szNEW_MTAL_code,strNEW_MTAL_Code);
					strcpy_s(stTagInfo.szNEW_MTAL_NM,strNEW_MTAL_Name);
					strcpy_s(stTagInfo.szNEW_UNIT_code,strNEW_UNIT_Code);
					strcpy_s(stTagInfo.szNEW_UNIT_NM,strNEW_UNIT_Name);
					strcpy_s(stTagInfo.szNEW_LOC_FLR_code,strNEW_LOC_FLR_Code);
					strcpy_s(stTagInfo.szNEW_LOC_FLR_NM,strNEW_LOC_FLR_Name);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_01_code,strNEW_DV_CLSF_01_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_01_NM,strNEW_DV_CLSF_01_Name);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_02_code,strNEW_DV_CLSF_02_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_02_NM,strNEW_DV_CLSF_02_Name);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_03_code,strNEW_DV_CLSF_03_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_03_NM,strNEW_DV_CLSF_03_Name);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_LOC_code,strNEW_DV_CLSF_LOC_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_LOC_NM,strNEW_DV_CLSF_LOC_Name);
					strcpy_s(stTagInfo.szNEW_VIRT_TAG_code,strNEW_VIRT_TAG_Code);
					strcpy_s(stTagInfo.szNEW_VIRT_TAG_NM,strNEW_VIRT_TAG_Name);
					strcpy_s(stTagInfo.szNEW_MEAU_CYCLE_code,strNEW_MEAU_CYCLE_Code);
					strcpy_s(stTagInfo.szNEW_MEAU_CYCLE_NM,strNEW_MEAU_CYCLE_Name);
					strcpy_s(stTagInfo.szNEW_MEAU_CYCLE_UNIT_code,strNEW_MEAU_CYCLE_UNIT_Code);
					strcpy_s(stTagInfo.szNEW_MEAU_CYCLE_UNIT_NM,strNEW_MEAU_CYCLE_UNIT_Name);
					strcpy_s(stTagInfo.szNEW_BLD_BUND_NM,strBld_Bund);
					strcpy_s(stTagInfo.szTAG_NAME,strTagName);
					strcpy_s(stTagInfo.szTAG_DESC,strTagDESC);
					stTagInfo.nTAG_TYPE = atoi(strTagType);

					m_tempTagList->SetDataAdd(stTagInfo);
#ifdef _DEBUG
					TRACE("STListCnt = %d,TagCnt= %d, Value = %s,%s,%d\n",nSTListCnt,nSelecRowCnt,strTagId,strTagName,atoi(strTagType));
#endif
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
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
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
				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
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
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}
	return nCountRow;
}
