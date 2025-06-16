// Thread_FMSAlarmMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_FMSAlarmMain.h"
#include "FormView_AlarmFMS.h"


// CThread_FMSAlarmMain

IMPLEMENT_DYNCREATE(CThread_FMSAlarmMain, CWinThread)

CThread_FMSAlarmMain::CThread_FMSAlarmMain()
{
	m_bEndThread = FALSE;
	m_bStartCheck = FALSE;
	m_bStratSubThread = FALSE;
	DB_Connect = NULL;

	m_pstFMSAlarmList = NULL;
	m_pstFMSAlarmList = new std::list<ST_FMSAlarmList>;
	m_pstUMSSend_UserList = NULL;
	m_pstUMSSend_UserList = new std::list<ST_UMSSend_UserList>;
}

CThread_FMSAlarmMain::~CThread_FMSAlarmMain()
{
}

BOOL CThread_FMSAlarmMain::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_FMSAlarmMain::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	Release_ST_List(ST_LIST_DELETE);
	Release_ST_UMS_List(ST_LIST_DELETE);
	CoUninitialize();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_FMSAlarmMain, CWinThread)
END_MESSAGE_MAP()


// CThread_FMSAlarmMain 메시지 처리기입니다.

void CThread_FMSAlarmMain::Release_ST_List(int nMode)
{
	if(m_pstFMSAlarmList != NULL)
	{
		m_pstFMSAlarmList->clear();

		if(nMode == ST_LIST_DELETE)
		{
			delete m_pstFMSAlarmList;
			m_pstFMSAlarmList =NULL;
		}
	}
}

void CThread_FMSAlarmMain::Release_ST_UMS_List(int nMode)
{
	if(m_pstUMSSend_UserList != NULL)
	{
		m_pstUMSSend_UserList->clear();

		if(nMode == ST_LIST_DELETE)
		{
			delete m_pstUMSSend_UserList;
			m_pstUMSSend_UserList =NULL;
		}
	}
}

CString CThread_FMSAlarmMain::Com_Error(const char *szLogName,_com_error *e)
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
	//20200212 jsh : 에러추가
	else if(m_nDBType == DB_POSTGRE)
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

void CThread_FMSAlarmMain::SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody)
{
	_addSystemMsg(LOG_MESSAGE_6, USER_COLOR_BLUE, m_strLogTitle, crBody, strMsg);
}

void CThread_FMSAlarmMain::SetWriteLogFile(const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_WriteLogFile(g_stProjectInfo.szProjectLogPath,m_strLogTitle,szLogMsg);		
	LeaveCriticalSection(&g_cs);
}

int CThread_FMSAlarmMain::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//m_strThreadName = "FACILITY Check";
	m_strLogTitle = "Main Thread";

	CTime currentTime;// CTime::GetCurrentTime();
	int nMinuteCheck = -1;
	CString strLogMsg = "";
	BOOL bTimeCheck = FALSE;
	BOOL bStartCheck = FALSE;

	ST_DBINFO stDBInfo;
	ST_GATHERINFO_TYPE stGatherInfo;
	ST_DATABASENAME  stDBName;
	stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	stGatherInfo = _getGatherInfoType(g_stProjectInfo.szProjectIniPath);
	stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	CString strDBName;
	m_nDBType = stDBInfo.unDBType;
	if(m_nDBType == DB_MSSQL)
		strDBName.Format("%s.dbo.",stDBName.szCommonDBName);
	else if(m_nDBType == DB_ORACLE)
		strDBName.Format("%s.",stDBName.szCommonDBName);
	else
		strDBName = "";

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,m_strLogTitle,g_stProjectInfo.szProjectLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);

	do 
	{
		if(m_bEndThread == TRUE)
			break;

		currentTime = CTime::GetCurrentTime();

		if(DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if(bConnectCheck != TRUE)
			{
				Sleep(500);
				continue;
			}
		}
		if(((currentTime.GetMinute() % 1) != 0 || nMinuteCheck == currentTime.GetMinute()) && (bStartCheck != FALSE))
		{
			Sleep(100);
			continue;
		}

		nMinuteCheck = currentTime.GetMinute();
		bStartCheck = TRUE;

		int nFctCount = GetSite_ChangeFCTList();

		CTime timeCompare;
		CString strQuery = "",strAlarmMsg = "";
		timeCompare = CTime(currentTime.GetYear(),currentTime.GetMonth(),currentTime.GetDay(),0,0,0);
		if(nFctCount != 0)
		{
			int nSearchCount = GetFctAlarmSearch();
			std::list<ST_FMSAlarmList>::iterator iter = m_pstFMSAlarmList->begin();
			while(iter != m_pstFMSAlarmList->end())
			{	
				CTimeSpan timeSpan(iter->nChangeAlarmDate,0,0,0); //or timeSpan = CTimeSpan(일,시,분,초);
				CTime timeCheck = iter->timeExptChangeDate - timeSpan;

				ST_ALARMCHECK stAlarmCheck;
				memset(&stAlarmCheck,0x00,sizeof(stAlarmCheck));
				BOOL bRet = GetAlarmOccursFMS_File(iter->szAlarmId,&stAlarmCheck);

				if(bRet == FALSE)
				{
					CString strTimeTemp_1;
					strTimeTemp_1.Format("%s",timeCompare.Format("%Y-%m-%d %H:%M:%S"));
					CString strTimeTemp_2;
					strTimeTemp_2.Format("%s",timeCheck.Format("%Y-%m-%d %H:%M:%S"));
					
					if(timeCompare == timeCheck)
					{
						strAlarmMsg.Format("알람 종류:[설비교체][%d일전]",iter->nChangeAlarmDate);
						strQuery.Format("INSERT INTO %sCM_ALARM_HISTORY "
							"(ALARM_ID,START_TIME,ALARM_OCCURRENCE_INFO,STATION_NAME,ALARM_CHECK_USE_YN,ALARM_KIND,ALARM_TYPE,ALARM_VALUE,ALARM_MESSAGE)"
							" VALUES "
							" ('%s','%s',1,'%s',0,10,10,%d,'%s')",
							strDBName,
							iter->szAlarmId,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szFCT_Name,iter->nChangeAlarmDate,strAlarmMsg);

						SetAlarmOccursFMS_File(iter->szAlarmId,10,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->nChangeAlarmDate);
						SetQueryValue(strQuery,m_strLogTitle,iter->szFCT_Name);

						int nRetLen = strlen(iter->szAlarmLevel);
						if(nRetLen != 0)
						{
							int nRetUms = GetAlarmUMSCheck(iter->szAlarmLevel);
							if(nRetUms == 1)
							{
								std::list<ST_UMSSend_UserList>::iterator iter_UMS = m_pstUMSSend_UserList->begin();
								while(iter_UMS != m_pstUMSSend_UserList->end())
								{
									SetSUMSendOutput(*iter,*iter_UMS,"교체 알람");
									iter_UMS++;
									Sleep(100);
								}
							}
							else
							{

							}
						}
					}
				}
				else
				{
					int nRet = GetAlarmOccursFMS_DB(iter->szAlarmId,stAlarmCheck.szAlarmStartTime);
					if(nRet == 1)
						SetAlarmOccursFMS_ClearFile(iter->szAlarmId);
				}
				
				Sleep(100);
				iter++;
			}
		}

		//_addCurrentstateMsg(0,0, m_strLogTitle, "Processor 정상 처리중..");

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);

	return CWinThread::Run();
}

int CThread_FMSAlarmMain::GetSite_ChangeFCTList()
{
	CString strMsgTitle = "SiteDevice Check";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strRetSiteid,strRetSiteName,strFacCount;

	strQuery.Format("  SELECT siteInfo.SITE_ID "
		",siteInfo.SITE_NAME "
		",facList.facCount "
		"FROM "
		"(SELECT "
				"SITE_ID "
				",COUNT(*) AS facCount "
				"FROM EASY_COMMON.dbo.CM_FACILITY_LIST "
				"WHERE FCT_CHANGE_ALARM_USE_YN = 1 "
				"GROUP BY SITE_ID "
				"HAVING COUNT(*) > 0 "
		") AS facList "
		"INNER JOIN EASY_COMMON.dbo.CM_SITE_INFO AS siteInfo ON (facList.SITE_ID = siteInfo.SITE_ID) " );

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{				
				((CFormView_AlarmFMS *)(m_pCtrl))->ListRemoveItem_Site();

				for(int nI = 0; nI < nCountRow ; nI++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strRetSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strRetSiteName);
					DB_Connect->GetFieldValue(pRs, "facCount", strFacCount);

					((CFormView_AlarmFMS *)(m_pCtrl))->ListInsertItem_Site(strRetSiteName,strFacCount,"알람 처리 중..");
					
					pRs->MoveNext();					
				}

				m_nDeviceTotal = nCountRow;
				m_bStartCheck = TRUE;
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
		CString strErrorCode = Com_Error(strMsgTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMsgTitle);
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
				strRunlog_E2.Format("%s - DB 접속 실패!",strMsgTitle);
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
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMsgTitle);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

int CThread_FMSAlarmMain::GetFctAlarmSearch()
{
	CString strMsgTitle = "GetFctAlarmSearch";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strRetSiteid,strRetSiteName,strRetFctId,strRetFctName,strRetChangeData,strRetExptDate,strRetAlarmId,strRetAlarmLeveId;

	strQuery.Format("SELECT siteInfo.SITE_ID "
						" ,siteInfo.SITE_NAME "
						" ,fctList.FCT_ID "
						" ,fctList.FCT_NAME "
						" ,fctList.FCT_CHANGE_ALARM_DATE "
						" ,fctList.FCT_EXPT_CHANGE_DATE "
						" ,alrmList.ALARM_ID "
						" ,alrmList.ALARM_LEVEL_ID "
				" FROM [EASY_COMMON].[dbo].CM_SITE_INFO siteInfo,[EASY_COMMON].[dbo].CM_FACILITY_LIST fctList ,[EASY_FMS].[dbo].FM_ALARM_LIST alrmList "
				" WHERE siteInfo.SITE_ID = fctList.SITE_ID "
					" AND alrmList.FCT_ID = fctList.FCT_ID "
				//	" AND fctList.FCT_ID ='FCT_1459138345666' "
					" AND fctList.FCT_CHANGE_ALARM_USE_YN = 1");

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;
		
		Release_ST_List(ST_LIST_CLEAR);
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{			
				COleDateTime dt;
				SYSTEMTIME st;

				for(int nI = 0; nI < nCountRow ; nI++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strRetSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strRetSiteName);
					DB_Connect->GetFieldValue(pRs, "FCT_ID", strRetFctId);
					DB_Connect->GetFieldValue(pRs, "FCT_NAME", strRetFctName);
					DB_Connect->GetFieldValue(pRs, "FCT_CHANGE_ALARM_DATE", strRetChangeData);
					DB_Connect->GetFieldValue(pRs, "FCT_EXPT_CHANGE_DATE", strRetExptDate);
					DB_Connect->GetFieldValue(pRs, "ALARM_ID", strRetAlarmId);
					DB_Connect->GetFieldValue(pRs, "ALARM_LEVEL_ID", strRetAlarmLeveId);

					//여기 해야함
					ST_FMSAlarmList stFmaAlarm;
					memset(&stFmaAlarm,0x00,sizeof(stFmaAlarm));

					strcpy_s(stFmaAlarm.szSite_Id,strRetSiteid);
					strcpy_s(stFmaAlarm.szSite_Name,strRetSiteName);
					strcpy_s(stFmaAlarm.szFCT_ID,strRetFctId);
					strcpy_s(stFmaAlarm.szFCT_Name,strRetFctName);
					strcpy_s(stFmaAlarm.szAlarmId,strRetAlarmId);
					strcpy_s(stFmaAlarm.szAlarmLevel,strRetAlarmLeveId);
					
					dt.ParseDateTime(strRetExptDate);
					dt.GetAsSystemTime(st);
					stFmaAlarm.timeExptChangeDate = CTime(st).GetTime();
					
					stFmaAlarm.nChangeAlarmDate = atoi(strRetChangeData);
					m_pstFMSAlarmList->push_back(stFmaAlarm);
					pRs->MoveNext();			
				}

				m_nDeviceTotal = nCountRow;
				m_bStartCheck = TRUE;
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
		CString strErrorCode = Com_Error(strMsgTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMsgTitle);
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
				strRunlog_E2.Format("%s - DB 접속 실패!",strMsgTitle);
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
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMsgTitle);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

BOOL CThread_FMSAlarmMain::GetAlarmOccursFMS_File(const char *szAlarmId,ST_ALARMCHECK *stAlarmCheck)
{
	char szBuff[64];
	memset(szBuff,0x00,sizeof(szBuff));
	CString strAlarmSettingPath = "";
	CString strTotalBuff = "",strTempBuff = "";
	//strAlarmSettingPath.Format("%s\\Alarm\\Fms\\OccursAlarm.ini",g_stProjectInfo.szProjectPath);//
	strAlarmSettingPath.Format("%s\\OccursAlarm.ini",g_stProjectInfo.szProjectFMSLogPath);
	GetPrivateProfileString("AckValue", szAlarmId,"0", szBuff,sizeof(szBuff), strAlarmSettingPath);

	if(!_strcmpi(szBuff,"0"))
	{
		return FALSE;
	}
	else
	{
		strTotalBuff = szBuff;

		int nPos = strTotalBuff.Find(",");
		if(nPos < 0)
			return FALSE;
		strTempBuff = strTotalBuff.Left(nPos);
		strTotalBuff.Delete(0,nPos + 1);
		strcpy_s(stAlarmCheck->szAlarmStartTime,strTempBuff);		 

		nPos = strTotalBuff.Find(",");
		strTempBuff = strTotalBuff.Left(nPos);
		strTotalBuff.Delete(0,nPos + 1);
		stAlarmCheck->nOldAlarmAckType = (int)atoi(strTempBuff);
		stAlarmCheck->fAckValue = (float)atof(strTotalBuff);
	}

	return TRUE;
}

BOOL CThread_FMSAlarmMain::SetAlarmOccursFMS_File(const char *szAlarmId, int nAlarmType ,const char *szStartTime, int nAckValue)
{
	CString strAlarmSettingPath = "";
	CString strTempBuff = "";
	//strAlarmSettingPath.Format("%s\\Alarm\\Fms\\OccursAlarm.ini",g_stProjectInfo.szProjectPath);
	strAlarmSettingPath.Format("%s\\OccursAlarm.ini",g_stProjectInfo.szProjectFMSLogPath);

	strTempBuff.Format("%s,%d,%f",szStartTime,nAlarmType,nAckValue);
	WritePrivateProfileString("AckValue", szAlarmId, strTempBuff,strAlarmSettingPath);
	return TRUE;
}

BOOL CThread_FMSAlarmMain::SetAlarmOccursFMS_ClearFile(const char *szAlarmId)
{
	char szBuff[26];
	memset(szBuff,0x00,sizeof(szBuff));
	CString strTotalBuff = "",strTempBuff = "";
	CString strAlarmSettingPath = "";
	//strAlarmSettingPath.Format("%s\\Alarm\\Fms\\OccursAlarm.ini",g_stProjectInfo.szProjectPath);
	strAlarmSettingPath.Format("%s\\OccursAlarm.ini",g_stProjectInfo.szProjectFMSLogPath);

	WritePrivateProfileString("AckValue", szAlarmId, NULL,strAlarmSettingPath);
	return TRUE;
}


int CThread_FMSAlarmMain::SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos)
{
	CString strRunlog_E2 = "";

	int nResult = DB_Connect->SetQueryRun(strQuery);

	if(nResult < 1)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [Query Fail],[%s]",szLogTitle,szLogPos,strQuery);
		SetWriteLogFile(strRunlog_E2);

#ifdef _DEBUG
		TRACE("Set Query Error-ProcessorName : [%s][%s]\r\n",m_strLogTitle,szLogTitle,strRunlog_E2);
#endif
		return -1;
	}	

	if (m_bEndThread == TRUE)
		return THREAD_END;

	return 0;
}

int CThread_FMSAlarmMain::GetAlarmOccursFMS_DB(const char *szAlarmId,const char *szStartTime)
{
	CString strMsgTitle = "GetAlarmOccursFMS_DB";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	int nSelectCount;

	strQuery.Format("SELECT COUNT(ALARM_ID) as Occurs "
							" FROM CM_ALARM_HISTORY "
					" WHERE ALARM_ID = '%s' "
					" AND START_TIME = '%s' "
					" AND ALARM_CHECK_USE_YN = 1",szAlarmId,szStartTime);
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				DB_Connect->GetFieldValue(pRs, "Occurs", dbValue);
				nSelectCount = (int)dbValue;
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}

			if(nSelectCount != 0)
				return 1;
			else
				return 0;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMsgTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMsgTitle);
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
				strRunlog_E2.Format("%s - DB 접속 실패!",strMsgTitle);
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
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMsgTitle);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

int CThread_FMSAlarmMain::GetAlarmUMSCheck(const char *szAlsrmLevel_ID)
{
	CString strMsgTitle = "GetAlarmUMSCheck";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strUSER_NAME = "",strUSER_TEL = "",strUSER_EMAIL ="",strALARM_LEVEL_ID = "",ALARM_LEVEL_DESC ="";
	int nSmsSend =0,nEmailSend =0;

	strQuery.Format("SELECT userInfo.USER_NAME "
		",userInfo.USER_TEL "
		",userInfo.USER_EMAIL "
		",levelinfo.ALARM_LEVEL_ID "
		",levelinfo.ALARM_LEVEL_DESC "
		",levelinfo.SMS_USE_YN  "
		",levelinfo.EMAIL_USE_YN  "
		",levelinfo.POPUP_USE_YN  "
		"FROM CM_ALARM_LEVEL_INFO levelinfo,CM_USER_INFO userInfo "
		"WHERE levelinfo.ALARM_LEVEL_ID = userInfo.ALARM_LEVEL_ID "
		"AND levelinfo.ALARM_LEVEL_ID = '%s' "
		"AND userInfo.USE_YN = 1",szAlsrmLevel_ID);

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		Release_ST_UMS_List(ST_LIST_CLEAR);
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				for(int nRow = 0; nRow < nCountRow; nRow++ )
				{
					DB_Connect->GetFieldValue(pRs, "USER_NAME", strUSER_NAME);
					DB_Connect->GetFieldValue(pRs, "USER_TEL", strUSER_TEL);
					DB_Connect->GetFieldValue(pRs, "USER_EMAIL", strUSER_EMAIL);
					DB_Connect->GetFieldValue(pRs, "ALARM_LEVEL_ID", strALARM_LEVEL_ID);
					DB_Connect->GetFieldValue(pRs, "ALARM_LEVEL_DESC", ALARM_LEVEL_DESC);

					DB_Connect->GetFieldValue(pRs, "SMS_USE_YN", dbValue);
					nSmsSend = (int)dbValue;
					DB_Connect->GetFieldValue(pRs, "EMAIL_USE_YN", dbValue);
					nEmailSend = (int)dbValue;

					ST_UMSSend_UserList stUmsUser;
					memset(&stUmsUser,0x00,sizeof(stUmsUser));

					strcpy_s(stUmsUser.szUserName,strUSER_NAME);
					strcpy_s(stUmsUser.szUserTEL,strUSER_TEL);
					strcpy_s(stUmsUser.szUserEmail,strUSER_EMAIL);
					strcpy_s(stUmsUser.szAlarmleveldesc,ALARM_LEVEL_DESC);

					stUmsUser.nSMS_SendCheck = nSmsSend;
					stUmsUser.nEmail_SendCheck = nEmailSend;

					m_pstUMSSend_UserList->push_back(stUmsUser);

					pRs->MoveNext();
				}
			}
			else
			{
				if(pRs != NULL)
				{
					pRs->Close();
					pRs = NULL;
				}

				return 0;
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}
			return 1;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMsgTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMsgTitle);
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
				strRunlog_E2.Format("%s - DB 접속 실패!",strMsgTitle);
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
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMsgTitle);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

		return ERROR_DB_QUERY_FAIL1;
	}
	return 0;
}

BOOL CThread_FMSAlarmMain::SetSUMSendOutput(ST_FMSAlarmList &stAlrmInfo,ST_UMSSend_UserList &stUmsSend,const char *szAlarmKind)
{
	CString strAlarmMsg = "",strSMSSendUseYN = "",strEmailSendUseYN = "";
	CString strSMSMsg = "",strEmailMsg = "",strId = "";
	CString strMsg = "",strSendValue = "";
	CString strQuery = "";

	EnterCriticalSection(&g_cs);
	strId = _IDCreated();
	LeaveCriticalSection(&g_cs);


	strMsg.Format("알람발생:[%s/%s],[%s],조건:[%s],값:[%d일전]",stAlrmInfo.szAlarmLevel,stUmsSend.szAlarmleveldesc,stAlrmInfo.szFCT_Name,szAlarmKind,stAlrmInfo.nChangeAlarmDate);

	if((stUmsSend.nSMS_SendCheck == 1) && (stUmsSend.nEmail_SendCheck == 1))
	{
		//strSMSEmailSend_UseYN = "'Y','Y'";
		strSMSSendUseYN = "Y";
		strEmailSendUseYN = "Y";
		strSMSMsg = strEmailMsg = strMsg;
	}
	else if((stUmsSend.nSMS_SendCheck == 1) && (stUmsSend.nEmail_SendCheck == 0))
	{
		//strSMSEmailSend_UseYN = "'Y','N'";
		strSMSSendUseYN = "Y";
		strEmailSendUseYN = "N";
		strSMSMsg = strMsg;
	}
	else if((stUmsSend.nSMS_SendCheck == 0) && (stUmsSend.nEmail_SendCheck == 1))
	{
		//strSMSEmailSend_UseYN = "'N','Y'";
		strSMSSendUseYN = "N";
		strEmailSendUseYN = "Y";
		strEmailMsg = strMsg;
	}
	else
		return FALSE;

	strQuery.Format("INSERT INTO UM_UMS_TRANSMIT_MSG (MSG_ID, TRANSMIT_SYSTEM_NAME, SMS_TARGET_YN,EMAIL_TARGET_YN,"
		"SMS_NUMBER_LIST, SMS_TARGET_NAME_LIST,"
		"EMAIL_ADDRESS_LIST, EMAIL_TARGET_NAME_LIST, SMS_MSG, "
		"EMAIL_MSG, REQUIRE_RESPONSE_YN, MSG_DELETE_POLICY, UMS_READ_YN) "
		//"VALUES (%s, 'MBMS시스템', %s,"
		"VALUES (%s, 'BICMS시스템', '%s','%s',"
		"'%s', '%s','%s', '%s',"
		"'%s', '%s', 'N', 0, 'N')",
		//strId,strSMSEmailSend_UseYN,
		strId,strSMSSendUseYN,strEmailSendUseYN,
		stUmsSend.szAlarmleveldesc,stUmsSend.szUserName,stUmsSend.szUserEmail,stUmsSend.szUserName,
		strMsg,strMsg);

	int nRet = SetQueryValue(strQuery,"FMS Alarm UMS Send",stAlrmInfo.szFCT_Name);
	if(nRet == 0)
		strAlarmMsg.Format("UMS 전송:[성공],[%s],태그명:[%s],알람타입:[%s],사용자:[%s],SMS/Email:[%s/%s],"
		,stAlrmInfo.szAlarmLevel,stAlrmInfo.szFCT_Name,szAlarmKind,stUmsSend.szUserName,strSMSSendUseYN,strEmailSendUseYN);
	else	
		strAlarmMsg.Format("UMS 전송:[실패][%s],태그명:[%s],알람타입:[%s],사용자:[%s],SMS/Email:[%s/%s],"
		,stAlrmInfo.szAlarmLevel,stAlrmInfo.szFCT_Name,szAlarmKind,stUmsSend.szUserName,strSMSSendUseYN,strEmailSendUseYN);

	_addSystemMsg(FORM_VIEW_ID_4, USER_COLOR_BLUE, m_strLogTitle, USER_COLOR_PINK, strAlarmMsg);

	return FALSE;
}


