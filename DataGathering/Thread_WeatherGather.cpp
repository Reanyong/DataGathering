// Thread_WeatherGather.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_WeatherGather.h"
#include "FormView_Weather.h"


// CThread_WeatherGather

IMPLEMENT_DYNCREATE(CThread_WeatherGather, CWinThread)

CThread_WeatherGather::CThread_WeatherGather()
{
	m_bEndCheck = FALSE;
	m_bEndThread = FALSE;
	DB_Connect = NULL;

	m_List_ST_Weather = NULL;
	m_List_ST_Weather = new std::list<ST_DB_WeatherColumn>;

	m_nWeatherLiveMinuteCheck = -1;
	m_bWeatherForecastTimeCheck = FALSE;
	m_nSiteCount = 0;
}

CThread_WeatherGather::~CThread_WeatherGather()
{
}

BOOL CThread_WeatherGather::InitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
	CoInitialize(NULL); //DB-ADO ��Ʈ�� ����
	return TRUE;
}

int CThread_WeatherGather::ExitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� ������ �����մϴ�.
	if(m_bEndCheck == FALSE)
		((CFormView_Weather *)(m_pCtrl))->ShowRun_State(PRO_STOP);

	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	Release_List_ST(ST_LIST_DELETE);
	CoUninitialize();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_WeatherGather, CWinThread)
END_MESSAGE_MAP()

/*!
 * @fn void Release_List_ST(int nMode)
 * @brief  �Լ��� ���� ����. (���� �̻�) ���� Ű���尡 ���ö����� '///' ������ ������ brief�� �������� �ν��Ѵ�.
 * @details  ��� ������, �߰� ������ �ʿ��ϸ� ����϶�� Ű����. (more)�� �������� ������ �ؽ�Ʈ.
 * @param[in]	�Ķ���� �̸�	�Ķ���� ����, [in]�� �Է¿� �Ķ���Ͷ�� ��.
 * @param[out]	�Ķ���� �̸�	�Ķ���� ����, [out]�� �Է¿� �Ķ���Ͷ�� ��.
 * @return	���ϰ��� ���� ����, ���� ���� �����̶� �߰� ������ �ʿ��ϸ� retval�� ����Ѵ�. '\returns'�� ����.
 * @retval	���ϰ�	���� ���� ���� ����.
 * @bug		�˰� �ִ� ���׿� ���ؼ� ���´�.
 * @todo		todo�� ���ؼ� ���´�.
 * @warning	���� ���׿� ���ؼ� ���´�.
 * @see '�ٸ� ���� �̸�'�� ���´�. �ٸ� ���� �����϶�� �Ҷ� ���µ�.. ����  �ẻ���� ���.
*/
/*!
 * @fn void Release_List_ST(int nMode)
 * @brief ����Ʈ ������ �ʱ�ȭ/����
 * @details ������ �ʱ�ȭ �� �޸� ����
 * @param[in] nMode �Է¿� ���ڰ�
 * @return -
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
void CThread_WeatherGather::Release_List_ST(int nMode)
{
	if(m_List_ST_Weather != NULL)
	{
		m_List_ST_Weather->clear();

		if(nMode == ST_LIST_DELETE)
		{
			delete m_List_ST_Weather;
			m_List_ST_Weather =NULL;
		}
	}
}

// CThread_WeatherGather �޽��� ó�����Դϴ�.
//////////////////////////////////////////////////////////////////////////
/*!
 * @fn Com_Error(const char *szLogName,_com_error *e)
 * @brief ���� �޽��� �߻�
 * @details try/catch ���� ���� �߻��� ���� �޽��� �߻� ����
 * @param[in] *szLogName �޽��� ��ġ/�߻� ��Ī
 * @param[in] *e �߻��޽���
 * @return -
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
void CThread_WeatherGather::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "";
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("DB Error Code : [%08lx], Code meaning : [%s], Source : [%s], Description : [%s]",
		e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);

#ifdef _DEBUG
	TRACE("ProcessorName : [%s],Position : [%s][%s]\r\n",m_strThreadName,szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("Position : [%s], LogName: [%s], %s",szLogName, strRunlog_E2);
	SetWriteLogFile("Processor-log : [DB Com Error..],",strRunlog_E2Log);
	//_addCurrentstateMsg(1,0, m_strThreadName, "DB ó�� ���� �󼼷α� Ȯ��");
	Sleep(500);
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn void SetWriteLogFile(const char *sTitle,const char *szLogMsg)
 * @brief �̷� �� �޽��� ���
 * @details ���� �߻��̷� ���� ���Ϸ� ����
 * @param[in] *sTitle ��� ��Ī
 * @param[in] *szLogMsg �α� �޽���
 * @return -
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
void CThread_WeatherGather::SetWriteLogFile(const char *sTitle,const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, sTitle, USER_COLOR_PINK, szLogMsg);
	_WriteLogFile(g_stProjectInfo.szProjectLogPath,m_strThreadName,szLogMsg);
	LeaveCriticalSection(&g_cs);
}

int CThread_WeatherGather::Run()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	//m_strThreadName = "WeatherGather";
	((CFormView_Weather *)(m_pCtrl))->ShowRun_State(PRO_RUN);

	CTime currentTime;// CTime::GetCurrentTime();
	int nMinuteCheck = -1;
	int nSiteCount = 0;
	CString strLogMsg = "";
	BOOL bTimeCheck = FALSE;
	BOOL bStartCheck = FALSE;

	ST_WEATHER_INFO stWeatherInfo[24];
	memset(stWeatherInfo,0x00,sizeof(stWeatherInfo));

	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	//ST_WEATHER_INFO stWeatherInfo = _getInfoWeatherRead(g_stProjectInfo.szProjectIniPath);
	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	if(stGatherInfo.nWeather_GatherType == 0)
	{
		stWeatherInfo[0] = _getInfoWeatherRead(g_stProjectInfo.szProjectIniPath);
		sprintf_s(stWeatherInfo[0].szSite_ID,"�����Ŀ���");
		sprintf_s(stWeatherInfo[0].szSite_Name,"�����Ŀ���");
		nSiteCount = 1;
	}

	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,m_strThreadName,g_stProjectInfo.szProjectLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);

	strLogMsg.Format("�ֱ� ��Ȳ:[30]��,����:[����]");
	//_addCurrentstateMsg(0,0, m_strThreadName, strLogMsg);
	//_addCurrentstateMsg(1,0, m_strThreadName, strLogMsg);
	do
	{
		if(m_bEndThread == TRUE)
			break;

		try
		{
			currentTime = CTime::GetCurrentTime();
			//currentTime = CTime(2017,1,17,16,0,0);
			if(DB_Connect->GetDB_ConnectionStatus() != 1)
			{
				BOOL bConnectCheck = DB_Connect->DB_Connection();
				if(bConnectCheck != TRUE)
				{
					Sleep(500);
					continue;
				}
			}

#ifdef	_TEST_MODE
			_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_BLACK, "log : [TEST_MODE 1Sec Data Read]");
			Sleep(2000);
#else
			//if(((currentTime.GetMinute() % 15) != 0 || nMinuteCheck == currentTime.GetMinute()))
			if(1 == 0)
			{
				Sleep(500);
				continue;
			}
#endif
			nMinuteCheck = currentTime.GetMinute();
			if(m_bEndThread == TRUE)
				break;


			int nResult = 0;


			if(stGatherInfo.nWeather_GatherType == 1)
			{
				nSiteCount = GetWeatherInfo(&stDBInfo,stWeatherInfo,stDBName.szWTDBName);
			}

			for(int nIndex = 0; nIndex < nSiteCount ; nIndex++)
			{
				if(atoi(stWeatherInfo[nIndex].szLiveUSE_YN) == 1)
				{
					nResult = GetWeatherLiveGather_2(currentTime,g_stProjectInfo.szProjectIniPath,&stDBInfo,stDBName.szWTDBName,&stWeatherInfo[nIndex],TRUE);
					if(nResult == THREAD_END)
						break;
					Sleep(100);
				}

				if(atoi(stWeatherInfo[nIndex].szForecastUSE_YN) == 1)
				{

				}
			}


			if(m_bEndThread == TRUE)
				break;

			//nResult = GetWeatherForecastGather(currentTime,g_stProjectInfo.szProjectIniPath,&stDBInfo,&stGatherInfo,TRUE);
			//if(nResult == THREAD_END)
			//	break;

			/*BOOL bResult = DB_Connect->DB_Close();
			if(bResult == 1)
			{
				strLogMsg.Format("log :[Connection Close]");
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE,"Processor-log : [Weather Gather]" , USER_COLOR_BLUE, strLogMsg);
			}*/

			//_addCurrentstateMsg(0,0, m_strThreadName, "Processor ���� ó����..");
		}
		catch (...)
		{
			strLogMsg.Format("(%s) - catch Error ",m_strThreadName);
#ifdef _DEBUG
			TRACE("Thread Result %s - catch Error \n",m_strThreadName);
#endif
			SetWriteLogFile("Processor-log : [Error..],",strLogMsg);
		}

		if(m_bEndThread == TRUE)
			break;

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);

	return CWinThread::Run();
}

//////////////////////////////////////////////////////////////////////////
/*
- ȣ�� ��� : ȣ��
- ����Ʈ�� �������� ��뿩�� ��ȸ
-int GetWeatherList(int nQueryType)
/*!
 * @fn int GetWeatherList(int nQueryType)
 * @brief �������� ���� ��ȸ
 * @details ��Ȳ/���� ���� ������� ��ȸ
 * @param[in] *nQueryType 0: ��Ȳ ���� ��ȸ, 1: ���� ���� ��ȸ
 * @return - ��ȸ�� Count
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
int CThread_WeatherGather::GetWeatherList(int nQueryType)
{
	memset(m_stWeatherList,0x00,sizeof(m_stWeatherList));

	CString strMsgTitle = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strQuery = "",strRunlog_E2 = "",strQueryType = "";
	if(nQueryType == 0)
	{
		strMsgTitle = "Select-GetLiveList";
		strQueryType = "and wtcfg.WEATHER_LIVE_USE_YN = 1 ";
	}
	else
	{
		strMsgTitle = strMsgTitle = "Select-GetForecastList";
		strQueryType = "and wtcfg.WEATHER_FORECAST_USE_YN = 1 ";
	}

	strQuery.Format("SELECT wtcfg.SITE_ID "
							",siteinfo.SITE_NAME "
							",wtcfg.WEATHER_LIVE_SEVICE_KEY "
							",addlist.REGION_LATITUDE "
							",addlist.REGION_LONGITUDE "
				"FROM CM_SITE_INFO siteinfo, CM_WEATHER_CFG wtcfg, CM_ADDRESS_LIST addlist "
				"where wtcfg.ADDRESS_ID= addlist.ADDRESS_ID "
						"and siteinfo.SITE_ID = wtcfg.SITE_ID "
						"and siteinfo.USE_YN = 1 "
						"%s"
					"order by wtcfg.STATE_PROVINCE_ID,wtcfg.CITY_ID,wtcfg.ADDRESS_ID asc ",strQueryType);

	CString strSiteid,strSiteName,strSiteSeviceKey;
	float flat = 0,flon = 0;
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				for(int nCnt = 0; nCnt < nCountRow ; nCnt++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strSiteName);
					DB_Connect->GetFieldValue(pRs, "WEATHER_LIVE_SEVICE_KEY", strSiteSeviceKey);

					DB_Connect->GetFieldValue(pRs, "REGION_LATITUDE", dbValue);
					flat = (float)dbValue;
					DB_Connect->GetFieldValue(pRs, "REGION_LONGITUDE", dbValue);
					flon = (float)dbValue;

					sprintf_s(m_stWeatherList[nCnt].szSiteId,"%s",strSiteid);
					sprintf_s(m_stWeatherList[nCnt].szSiteName,"%s",strSiteName);
					sprintf_s(m_stWeatherList[nCnt].szLiveSeviceKey,"%s",strSiteSeviceKey);
					m_stWeatherList[nCnt].fLat = flat;
					m_stWeatherList[nCnt].fLon = flon;
					pRs->MoveNext();
				}
			}
			// ���ڵ� Get �� �����Ѵ�.
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
			strRunlog_E2.Format("Position : [%s], log : [DB ���� ���� �õ�..]",strMsgTitle);
			SetWriteLogFile("Processor-log : [_com_error..],",strRunlog_E2);

			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB ���� ����!]",strMsgTitle);
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

		strRunlog_E2.Format("Position : [%s], log : [WeatherLive Event][%s]",strMsgTitle,strQuery);
		SetWriteLogFile("Processor-log : [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}

	return nCountRow;
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn int GetWeatherLiveGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck)
 * @brief ��Ȳ ���� ���� ���� ������ ����
 * @details �������� ������ ���� ���� ����
 * @param[in] currentTime ���� �ð�
 * @param[in] szSavePath ��ȸ�� ������ �ӽ� ���� ��ġ
 * @param[in] stDBInfo ���ӵ� DB ����
 * @param[in] bStartCheck ������
 * @return 1: ���� ,THREAD_END: ������ ����
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
int CThread_WeatherGather::GetWeatherLiveGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck)
{
#ifdef	_TEST_MODE
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live Gather]", USER_COLOR_BLACK, "log : [_TEST_MODE 1Sec Data Read]");
	Sleep(1000);
#else
	/*if((currentTime.GetMinute() % 40) != 0 || m_nWeatherLiveMinuteCheck == currentTime.GetMinute())
	{
		Sleep(100);
		return 0;
	}*/
#endif

	m_nWeatherLiveMinuteCheck = currentTime.GetMinute();
	CString strLogMsg = "";
	//��� ���� live ���� ���� �κ�
	//ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);

	//00�� ���� 20�б��� �� ������ �� ����.
	CTimeSpan timeSpan(0,1,0,0); //or timeSpan = CTimeSpan(��,��,��,��);
	CTime CheckTime;
	int nMinuteCheck = currentTime.GetMinute();
	if(nMinuteCheck <= 20)
		CheckTime = currentTime - timeSpan;
	else
		CheckTime = currentTime;

	int nLiveList = 1;//GetWeatherList(WEATHER_LIVE);
	if(nLiveList > 0)
	{
		for(int nI = 0; nI < nLiveList; nI++)
		{
			//strLogMsg.Format("%s - ��Ȳ���� ó��",m_stWeatherList[nI].szSiteName);
			//_addCurrentstateMsg(1,0, m_strThreadName, strLogMsg);

			if(m_bEndThread == TRUE)
				break;
			//BOOL bResult = GetWeatherLive_URL(CheckTime,stGatherInfo->szUrlLive,m_stWeatherList[nI],g_stProjectInfo.szWeatherLivePath);
			BOOL bResult = GetWeatherLive_URL(CheckTime,stGatherInfo->szUrlLive,m_stWeatherList[nI],g_stProjectInfo.szWeatherLivePath);
			if(bResult == TRUE)
			{
				int nTotalLiveSize = m_List_ST_Weather->size();
				if(nTotalLiveSize != 0)
				{
					std::list<ST_DB_WeatherColumn>::iterator iter = m_List_ST_Weather->begin();
					CString strQuery = "",strColumn ="",strValue = "",strTemp ="";
					int nCount = 0;

					if(stDBInfo->unDBType == DB_MSSQL)
						strValue.Format(" VALUES ('%s','%s',",m_stWeatherList[nI].szSiteId,currentTime.Format("%Y-%m-%d %H:00"));
					else if(stDBInfo->unDBType == DB_ORACLE)
						strValue.Format(" VALUES ('%s',TO_DATE('%s','YYYY-MM-DD HH24:MI'),",m_stWeatherList[nI].szSiteId,currentTime.Format("%Y-%m-%d %H:00"));
					else
					{
						_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_RED, "log : [MY-SQL ���� ���������� ���� ���� ����!]");
						m_bEndThread = TRUE;
						if (m_bEndThread == TRUE)
							break;
					}

					while(iter != m_List_ST_Weather->end())
					{
						if(nCount != 0)
						{
							strColumn += ", ";
							strValue += ", ";
						}
						//strTemp.Format("%s",iter->szColumnName);
						strTemp.Format("%s",iter->szCategory);
						strColumn += strTemp;
						strTemp.Format("'%s'",iter->szValue);
						strValue += strTemp;

						if (m_bEndThread == TRUE)
							break;

						Sleep(2);
						iter++;
						nCount++;
					}

					if(m_bEndThread == TRUE)
						break;

					strValue += ")";
					strQuery.Format("INSERT INTO CM_WEATHER_LIVE_HISTORY (SITE_ID,REGISTER_DATE,%s)%s",strColumn,strValue);
#ifdef _DEBUG
					TRACE("Weather live insert Query - %s) \n",strQuery);
#endif
					Release_List_ST(ST_LIST_CLEAR);

					int nRet = SetQueryValue(strQuery,"Weather Live",m_stWeatherList[nI].szSiteId);
					if(nRet == THREAD_END && m_bEndThread == TRUE)
						return THREAD_END;
					else
					{
						strLogMsg.Format("Position : [Weather Live][%s], log : [SetQuery Success],",m_stWeatherList[nI].szSiteName);
						_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_BLACK, strLogMsg);
#ifdef _DEBUG
						TRACE("Weather live insert Query Success) \n");
#endif
					}
					if(m_bEndThread == TRUE)
						break;
				}
			}
			if(m_bEndThread == TRUE)
				break;
		}
	}
	else
	{
		strLogMsg.Format("Position : [Weather Live],log : [SetQuery Success][������ ����Ʈ������ �����ϴ�.]");
		_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_BLACK, strLogMsg);

		//_addCurrentstateMsg(1,0, m_strThreadName, strLogMsg);
	}
	if(m_bEndThread == TRUE)
		return THREAD_END;

	return TRUE;
}

int CThread_WeatherGather::GetWeatherInfo(ST_DBINFO *stDBInfo,ST_WEATHER_INFO *stWeatherInfo,const char *szDBName)
{
	//strcat_s(stWeatherInfo->szServiceKey,"ddddd");
	CString strMsgTitle = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strQuery,strRunlog_E2,strQueryType;
	CString strDBName;

	if(stDBInfo->unDBType == DB_MSSQL)
	{
		if(strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.",szDBName);
		else
			strDBName.Format("");
	}
	else
	{
		if(strlen(szDBName) > 1)
			strDBName.Format("%s.",szDBName);
		else
			strDBName.Format("");
	}

	strQuery.Format("SELECT wtcfg.SITE_ID"
		",siteInfo.SITE_NAME"
		",wtcfg.WEATHER_LIVE_USE_YN"
		",wtcfg.WEATHER_LIVE_SEVICE_KEY"
		",wtcfg.WEATHER_FORECAST_USE_YN"
		",(SELECT REGION_LATITUDE From %sCM_ADDRESS_LIST WHERE ADDRESS_ID = wtcfg.ADDRESS_ID) as Lat"
		",(SELECT REGION_LONGITUDE From %sCM_ADDRESS_LIST WHERE ADDRESS_ID = wtcfg.ADDRESS_ID) as Lon"
		" FROM %sCM_WEATHER_CFG wtcfg,%sCM_SITE_INFO siteInfo"
		" where siteInfo.SITE_ID = wtcfg.SITE_ID",
		strDBName,strDBName,strDBName,strDBName);

	CString strSiteid,strSiteName,strSiteSeviceKey,strLiveUse_YN,strForecastUse_YN;
	CString strLat,strLon;
	float flat = 0,flon = 0;
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{

				for(int nCnt = 0; nCnt < nCountRow ; nCnt++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strSiteName);
					DB_Connect->GetFieldValue(pRs, "WEATHER_LIVE_SEVICE_KEY", strSiteSeviceKey);
					DB_Connect->GetFieldValue(pRs, "WEATHER_LIVE_USE_YN", strLiveUse_YN);
					DB_Connect->GetFieldValue(pRs, "WEATHER_FORECAST_USE_YN", strForecastUse_YN);
					DB_Connect->GetFieldValue(pRs, "Lat", strLat);
					DB_Connect->GetFieldValue(pRs, "Lon", strLon);

					sprintf_s(stWeatherInfo[nCnt].szSite_ID,"%s",strSiteid);
					sprintf_s(stWeatherInfo[nCnt].szSite_Name,"%s",strSiteName);
					sprintf_s(stWeatherInfo[nCnt].szServiceKey,"%s",strSiteSeviceKey);

					sprintf_s(stWeatherInfo[nCnt].szLiveUSE_YN,"%d",atoi(strLiveUse_YN));
					sprintf_s(stWeatherInfo[nCnt].szForecastUSE_YN,"%d",atoi(strForecastUse_YN));

					sprintf_s(stWeatherInfo[nCnt].szLiveLat,"%s",strLat);
					sprintf_s(stWeatherInfo[nCnt].szLiveLon,"%s",strLon);
					sprintf_s(stWeatherInfo[nCnt].szForecastLat,"%s",strLat);
					sprintf_s(stWeatherInfo[nCnt].szForecastLon,"%s",strLon);


					pRs->MoveNext();
				}
			}
			// ���ڵ� Get �� �����Ѵ�.
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
			strRunlog_E2.Format("Position : [%s], log : [DB ���� ���� �õ�..]",strMsgTitle);
			SetWriteLogFile("Processor-log : [_com_error..],",strRunlog_E2);

			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB ���� ����!]",strMsgTitle);
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

		strRunlog_E2.Format("Position : [%s], log : [WeatherLive Event][%s]",strMsgTitle,strQuery);
		SetWriteLogFile("Processor-log : [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}

	return nCountRow;
}

int CThread_WeatherGather::GetWeatherLiveGather_2(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,const char *szDBName,ST_WEATHER_INFO *stWeatherInfo,BOOL bStartCheck)
{
#ifdef	_TEST_MODE
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live Gather]", USER_COLOR_BLACK, "log : [_TEST_MODE 1Sec Data Read]");
	Sleep(1000);
#else
	/*if((currentTime.GetMinute() % 40) != 0 || m_nWeatherLiveMinuteCheck == currentTime.GetMinute())
	{
		Sleep(100);
		return 0;
	}*/
#endif

	m_nWeatherLiveMinuteCheck = currentTime.GetMinute();
	CString strLogMsg = "",strDBName;
	CString strSiteId = stWeatherInfo->szSite_Name;

	//��� ���� live ���� ���� �κ�
	//00�� ���� 20�б��� �� ������ �� ����.
	CTimeSpan timeSpan(0,1,0,0); //or timeSpan = CTimeSpan(��,��,��,��);
	CTime CheckTime;
	int nMinuteCheck = currentTime.GetMinute();
	if(nMinuteCheck <= 30)
		CheckTime = currentTime - timeSpan;
	else
		CheckTime = currentTime;

	if(stDBInfo->unDBType == DB_MSSQL)
	{
		if(strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.",szDBName);
		else
			strDBName.Format("");
	}
	else
	{
		if(strlen(szDBName) > 1)
			strDBName.Format("%s.",szDBName);
		else
			strDBName.Format("");
	}

	if(m_bEndThread == TRUE)
		return THREAD_END;

	BOOL bResult = GetWeatherLive_URL_2(CheckTime,"",stWeatherInfo,g_stProjectInfo.szWeatherLivePath);
	if(bResult == TRUE)
	{
		int nTotalLiveSize = m_List_ST_Weather->size();
		if(nTotalLiveSize != 0)
		{
			std::list<ST_DB_WeatherColumn>::iterator iter = m_List_ST_Weather->begin();
			CString strQuery = "",strColumn ="",strValue = "",strTemp ="";
			int nCount = 0;

			if(stDBInfo->unDBType == DB_MSSQL)
				strValue.Format(" VALUES ('%s','%s',",stWeatherInfo->szSite_ID,currentTime.Format("%Y-%m-%d %H:00"));
			else if(stDBInfo->unDBType == DB_ORACLE)
			{
				// -> �����Ŀ��ؿ��� ��� SITE ID ������ //strValue.Format(" VALUES (TO_DATE('%s','YYYY-MM-DD HH24:MI'),",currentTime.Format("%Y-%m-%d %H:%M"));
				strValue.Format(" VALUES ('%s',TO_DATE('%s','YYYY-MM-DD HH24:MI'),",stWeatherInfo->szSite_ID,currentTime.Format("%Y-%m-%d %H:00"));
			}
			else
			{
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_RED, "log : [My-SQL ���� ���������� ���� ���� ����!]");
				m_bEndThread = TRUE;
				if(m_bEndThread == TRUE)
					return THREAD_END;
			}

			while(iter != m_List_ST_Weather->end())
			{
				if(nCount != 0)
				{
					strColumn += ", ";
					strValue += ", ";
				}
				//strTemp.Format("%s",iter->szColumnName);
				strTemp.Format("%s",iter->szCategory);
				strColumn += strTemp;
				strTemp.Format("'%s'",iter->szValue);
				strValue += strTemp;

				if (m_bEndThread == TRUE)
					break;

				Sleep(2);
				iter++;
				nCount++;
			}

			if(m_bEndThread == TRUE)
				return THREAD_END;

			strValue += ")";
			strQuery.Format("INSERT INTO %sCM_WEATHER_LIVE_HISTORY (SITE_ID,REGISTER_DATE,%s)%s",strDBName,strColumn,strValue);
			//-> �����Ŀ��� ���� ����� ������//strQuery.Format("INSERT INTO %sCM_WEATHER_LIVE_HISTORY (REGISTER_DATE,%s)%s",strDBName,strColumn,strValue);
#ifdef _DEBUG
			TRACE("Weather live insert Query - %s) \n",strQuery);
#endif
			Release_List_ST(ST_LIST_CLEAR);

			int nRet = SetQueryValue(strQuery,"Weather Live",strSiteId);
			if(nRet == THREAD_END && m_bEndThread == TRUE)
				return THREAD_END;
			else
			{
				strLogMsg.Format("Position : [Weather Live][%s], log : [SetQuery Success],",strSiteId);
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_BLACK, strLogMsg);
#ifdef _DEBUG
				TRACE("Weather live insert Query Success) \n");
#endif
			}
			if(m_bEndThread == TRUE)
				return THREAD_END;
		}
	}

	/*else
	{
		strLogMsg.Format("Position : [Weather Live],log : [SetQuery Success][������ ����Ʈ������ �����ϴ�.]");
		_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_BLACK, strLogMsg);

		//_addCurrentstateMsg(1,0, m_strThreadName, strLogMsg);
	}*/
	if(m_bEndThread == TRUE)
		return THREAD_END;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
/*!
 * @fn BOOL GetWeatherLive_URL(CTime time,const char *szUrl,ST_WEATHER_LIST stWeatherInfoList,const char *szSavePath)
 * @brief ������ ȣ�� �� ������ �Ľ�
 * @details ��Ȳ ������ URL ���� ��ȸ�� ������ �Ľ�
 * @param[in] time ��ȸ�� �ð�
 * @param[in] szUrl ���� URL
 * @param[in] stWeatherInfoList ��Ȳ ������ ��ġ ����
 * @param[in] szSavePath �ӽ� ���� ������ġ
 * @return TRUE: ���� ,FALSE: ����
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
BOOL CThread_WeatherGather::GetWeatherLive_URL(CTime time,const char *szUrl,ST_WEATHER_LIST stWeatherInfoList,const char *szSavePath)
{
	CString strMsgTitle = "WeatherLive";
	CString strUrl = "",strBuff = "",strMsgTemp = "",strRunlog_E2 = "",strSiteName ="";
	CXmlDocumentWrapper xmlDocW;
	CString strXmlSavePath = "";
	double dx = 0;
	double dy = 0;

	ST_WEATHERINFO stWeatherUrl;
	memset(&stWeatherUrl,0x00,sizeof(stWeatherUrl));

	strSiteName = stWeatherInfoList.szSiteName;
	stWeatherUrl.XY_Conv(stWeatherInfoList.fLat,stWeatherInfoList.fLon,&dx,&dy);

	//strUrl = stWeatherUrl.GetUrl(szUrl,stWeatherInfoList.szLiveSeviceKey,FRC_GRID,time,(int)dx,(int)dy);
	strUrl = stWeatherUrl.GetUrl("http://newsky2.kma.go.kr",stWeatherInfoList.szLiveSeviceKey,FRC_GRID,time,(int)dx,(int)dy);

#ifdef _DEBUG
	TRACE("WeatherLive Xml Url : [%s][%s] \n",strSiteName,strUrl);
#endif

	//////////////////////////////////////////////////////////////////////////
	//RUL ���� XML Load
#ifdef	_TEST_MODE
	strXmlSavePath.Format("%s\\WeatherLive-2016-02-19 1145.xml",szSavePath);
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather XmlLoad]", USER_COLOR_BLACK, "Log : [_TEST_MODE Load]");
#else
	if(xmlDocW.Load(strUrl) == FALSE)
	{
		strMsgTemp.Format("Position : [%s][%s], log : [���� Fail][%s]",strMsgTitle,strSiteName,strUrl);
		SetWriteLogFile("Processor-log : [Weather UrlXmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml Url Load Error.. (%s) \n",strMsgTemp);
#endif
		return FALSE;
	}
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather UrlXmlLoad]", USER_COLOR_BLACK, "log : [Url Xml Load Success]");

	strXmlSavePath.Format("%s\\%s-%04d-%02d-%02d %02d%02d.xml",szSavePath,strSiteName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute());
	xmlDocW.Save(strXmlSavePath);
#endif

	//////////////////////////////////////////////////////////////////////////
	//XML ���� Load
	TiXmlDocument ToiXmlDoc;
	ToiXmlDoc.Clear();
	if(ToiXmlDoc.LoadFile(strXmlSavePath) == FALSE)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [Xml File Load Fail][%s]",strMsgTitle,strSiteName,strXmlSavePath);
		SetWriteLogFile("Processor-log : [Weather XmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml File Load Error.. (%s) \n",strRunlog_E2);
#endif
		return FALSE;
	}
	strRunlog_E2.Format("Position : [%s][%s], log : [File Xml Load Success]",strMsgTitle,strSiteName);
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather XmlLoad]", USER_COLOR_BLACK, strRunlog_E2);

	int nDataCnt = 0;
	CString strDataCnt = "";
	CString strGetText ="",strTreeText = "", strTextTemp = "";

	TiXmlNode* pNode = ToiXmlDoc.FirstChild("response")->FirstChild("body")->FirstChild("items")->FirstChild("item");

	while(pNode)
	{
		pNode->ToElement()->Attribute("item"); // ù��° ����� name�� �н��ϴ�

		pNode = pNode->NextSibling();  // �ι�°�� �̵��մϴ�.
		nDataCnt++;
	}

	pNode = ToiXmlDoc.FirstChild("response")->FirstChild("body")->FirstChild("items")->FirstChild("item");
	CString strCategory ="", strColumnName = "";
	CString strObsrValue ="";
	CString strValBuff ="";

	//TCHAR*	_lpszLiveGrid[] = {"Site","�µ�","������","�ϴ��ڵ�","����","�����ڵ�","ǳ��"};
	CString strT1H,strRN1,strSKY,strREH,strPTY,strWSD;

	for(int nI = 0; nI < nDataCnt; nI++)
	{
		try
		{
			strCategory = GetElementText(pNode,"category",strMsgTitle,strSiteName); //�׸�
			//strColumnName = GetColumnName(strCategory); //�׸񰪰� ��ġ�ϴ� �÷� �� �����´�.
			strColumnName = strCategory; //XML �÷� ������ �״�� ���̺� �÷� ���
			strObsrValue = GetElementText(pNode,"obsrValue",strMsgTitle,strSiteName); //Missing
			strValBuff = stWeatherUrl.GetForecastGrib_TYP(strCategory,strObsrValue);
			if(strValBuff == "Error")
			{
				pNode = pNode->NextSibling();  // ���� Item �̵� �մϴ�.
				continue;
			}

			ST_DB_WeatherColumn  stNewWeatherinfo;
			strcpy_s(stNewWeatherinfo.szColumnName,strColumnName);
			strcpy_s(stNewWeatherinfo.szCategory,strCategory);

			CString strTempText = "";
			if(strCategory == "T1H")
			{
				strTempText.Format("%.1f",atof(strValBuff));
				strValBuff = strTempText;
			}

			strcpy_s(stNewWeatherinfo.szValue,strValBuff);
			m_List_ST_Weather->push_back(stNewWeatherinfo);

			if(strCategory == "T1H")
				strT1H = strValBuff;
			else if(strCategory == "RN1")
				strRN1 = strValBuff;
			else if(strCategory == "SKY")
				strSKY = strValBuff;
			else if(strCategory == "REH")
				strREH = strValBuff;
			else if(strCategory == "PTY")
				strPTY = strValBuff;
			else if(strCategory == "WSD")
				strWSD = strValBuff;
#ifdef _DEBUG
			TRACE("WeatherLive Xml File Load category : [%s], obsrValue : [%s]) \n",strCategory,strObsrValue);
#endif

			pNode = pNode->NextSibling();  // ���� Item �̵� �մϴ�.
		}
		catch (_com_error &e)
		{
			strRunlog_E2.Format("Position : [%s][%s], log : [%s]",strMsgTitle,strSiteName,(LPCTSTR)(_bstr_t)e.Description());
			SetWriteLogFile("Processor-log : [Weather parsing _com_error..],",strRunlog_E2);
#ifdef _DEBUG
			TRACE("Weather parsing _com_error - %s \n",strRunlog_E2);
#endif
			ToiXmlDoc.Clear();
			return FALSE;
		}
		catch (...)
		{
			strRunlog_E2.Format("Position : [%s][%s], log : [XML Parsing Error]",strMsgTitle,strSiteName);
			SetWriteLogFile("Processor-log : [Weather parsing Error..],",strRunlog_E2);
#ifdef _DEBUG
			TRACE("Weather parsing Error (XML Parsing Error) \n");
#endif
			ToiXmlDoc.Clear();
			return FALSE;
		}
	}

	ToiXmlDoc.Clear();

	if(nDataCnt != 0)
		((CFormView_Weather *)(m_pCtrl))->OutputWeatherLiveHistory(stWeatherInfoList.szSiteName,time.Format("%m/%d %H:%M"),strT1H,strRN1,strSKY,strREH,strPTY,strWSD);
	else
		return FALSE;


	return TRUE;
}


BOOL CThread_WeatherGather::GetWeatherLive_URL_2(CTime time,const char *szUrl,ST_WEATHER_INFO *stWeatherInfo,const char *szSavePath)
{
	CString strMsgTitle = "WeatherLive";
	CString strUrl = "",strBuff = "",strMsgTemp = "",strRunlog_E2 = "",strSiteName ="";
	CXmlDocumentWrapper xmlDocW;
	CString strXmlPath = "",strXmlFileName = "";
	float fLat = 0;
	float fLon = 0;
	double dx = 0;
	double dy = 0;

	ST_WEATHERINFO stWeatherUrl;
	memset(&stWeatherUrl,0x00,sizeof(stWeatherUrl));

	fLat = (float)atof(stWeatherInfo->szLiveLat);
	fLon = (float)atof(stWeatherInfo->szLiveLon);


	strSiteName = stWeatherInfo->szSite_Name;
	stWeatherUrl.XY_Conv(fLat,fLon,&dx,&dy);

	//strUrl = stWeatherUrl.GetUrl(szUrl,stWeatherInfoList.szLiveSeviceKey,FRC_GRID,time,(int)dx,(int)dy);
	strUrl = stWeatherUrl.GetUrl("http://newsky2.kma.go.kr",stWeatherInfo->szServiceKey,FRC_GRID,time,(int)dx,(int)dy);

#ifdef _DEBUG
	TRACE("WeatherLive Xml Url : [%s][%s] \n",strSiteName,strUrl);
#endif

	//////////////////////////////////////////////////////////////////////////
	//RUL ���� XML Load

	if(xmlDocW.Load(strUrl) == FALSE)
	{
		strMsgTemp.Format("Position : [%s][%s], log : [���� Fail][%s]",strMsgTitle,strSiteName,strUrl);
		SetWriteLogFile("Processor-log : [Weather UrlXmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml Url Load Error.. (%s) \n",strMsgTemp);
#endif
		return FALSE;
	}
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather UrlXmlLoad]", USER_COLOR_BLACK, "log : [Url Xml Load Success]");

	//--������
	//strXmlPath.Format("%s\\%s-%04d-%02d-%02d %02d%02d.xml",szSavePath,strSiteName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute());

	strXmlFileName.Format("%s-%04d-%02d-%02d %02d%02d.xml",strSiteName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute());
	strXmlPath.Format("%s\\%s",szSavePath,strXmlFileName);
	xmlDocW.Save(strXmlPath);

	//�׽�Ʈ�Ҷ��� ���
	//strXmlPath.Format("%s\\Error-TestXML.xml",szSavePath);


	//////////////////////////////////////////////////////////////////////////
	//XML ���� Load
	TiXmlDocument ToiXmlDoc;
	ToiXmlDoc.Clear();
	if(ToiXmlDoc.LoadFile(strXmlPath) == FALSE)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [Xml File Load Fail][%s]",strMsgTitle,strSiteName,strXmlPath);
		SetWriteLogFile("Processor-log : [Weather XmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml File Load Error.. (%s) \n",strRunlog_E2);
#endif
		return FALSE;
	}
	strRunlog_E2.Format("Position : [%s][%s], log : [File Xml Load Success]",strMsgTitle,strSiteName);
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather XmlLoad]", USER_COLOR_BLACK, strRunlog_E2);

	int nDataCnt = 0;
	CString strDataCnt = "";
	CString strGetText ="",strTreeText = "", strTextTemp = "";

	TiXmlNode* pNodeHeaderCheck = ToiXmlDoc.FirstChild("response")->FirstChild("header");

	CString strHeaderCheck = "";
	try
	{
		strHeaderCheck = GetElementText(pNodeHeaderCheck,"resultMsg",strMsgTitle,strSiteName); //�׸�

		if(strHeaderCheck != "OK")
		{
			ToiXmlDoc.Clear();
			//strXmlPath.Format("%s\\�����Ŀ���-2017-01-17 1650.xml",szSavePath);
			if(m_strXmlOldFileName.IsEmpty() == TRUE)
			{
				strRunlog_E2.Format("Position : [%s][%s], log : [���û ���� Ȯ�� �� XML ���� Ȯ�� �ʿ�][%s]",strMsgTitle,strSiteName,strXmlPath);
				SetWriteLogFile("Processor-log : [Weather XmlLoad Error..Sub],",strRunlog_E2);

				ToiXmlDoc.Clear();
				return FALSE;
			}

			strXmlPath.Format("%s\\%s",szSavePath,m_strXmlOldFileName);
			if(ToiXmlDoc.LoadFile(strXmlPath) == FALSE)
			{
				strRunlog_E2.Format("Position : [%s][%s], log : [Xml File Load Fail][%s]",strMsgTitle,strSiteName,strXmlPath);
				SetWriteLogFile("Processor-log : [Weather XmlLoad Error..Sub],",strRunlog_E2);
#ifdef _DEBUG
				TRACE("WeatherLive Xml File Load Error.. (%s) \n",strRunlog_E2);
#endif
				return FALSE;
			}

			strRunlog_E2.Format("Position : [%s][%s], log : [���û���� ���� ����(�������� ��������(%s)�� ������Ʈ)][%s]",strMsgTitle,strSiteName,m_strXmlOldFileName,strXmlPath);
			SetWriteLogFile("Processor-log : [Weather Server Error..Sub],",strRunlog_E2);
		}
	}
	catch (...)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [XML Parsing Error]",strMsgTitle,strSiteName);
		SetWriteLogFile("Processor-log : [Weather parsing Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("Weather parsing Error (XML Parsing Error) \n");
#endif
		ToiXmlDoc.Clear();
		return FALSE;
	}

	m_strXmlOldFileName = strXmlFileName;

	TiXmlNode* pNode = ToiXmlDoc.FirstChild("response")->FirstChild("body")->FirstChild("items")->FirstChild("item");

	while(pNode)
	{
		pNode->ToElement()->Attribute("item"); // ù��° ����� name�� �н��ϴ�

		pNode = pNode->NextSibling();  // �ι�°�� �̵��մϴ�.
		nDataCnt++;
	}

	pNode = ToiXmlDoc.FirstChild("response")->FirstChild("body")->FirstChild("items")->FirstChild("item");
	CString strCategory ="", strColumnName = "";
	CString strObsrValue ="";
	CString strValBuff ="";

	//TCHAR*	_lpszLiveGrid[] = {"Site","�µ�","������","�ϴ��ڵ�","����","�����ڵ�","ǳ��"};
	CString strT1H,strRN1,strSKY,strREH,strPTY,strWSD;

	for(int nI = 0; nI < nDataCnt; nI++)
	{
		try
		{
			strCategory = GetElementText(pNode,"category",strMsgTitle,strSiteName); //�׸�
			//strColumnName = GetColumnName(strCategory); //�׸񰪰� ��ġ�ϴ� �÷� �� �����´�.
			strColumnName = strCategory; //XML �÷� ������ �״�� ���̺� �÷� ���
			strObsrValue = GetElementText(pNode,"obsrValue",strMsgTitle,strSiteName); //Missing
			strValBuff = stWeatherUrl.GetForecastGrib_TYP(strCategory,strObsrValue);
			if(strValBuff == "Error")
			{
				pNode = pNode->NextSibling();  // ���� Item �̵� �մϴ�.
				continue;
			}

			ST_DB_WeatherColumn  stNewWeatherinfo;
			strcpy_s(stNewWeatherinfo.szColumnName,strColumnName);
			strcpy_s(stNewWeatherinfo.szCategory,strCategory);

			CString strTempText = "";
			if(strCategory == "T1H")
			{
				strTempText.Format("%.1f",atof(strValBuff));
				strValBuff = strTempText;
			}

			strcpy_s(stNewWeatherinfo.szValue,strValBuff);
			m_List_ST_Weather->push_back(stNewWeatherinfo);

			if(strCategory == "T1H")
				strT1H = strValBuff;
			else if(strCategory == "RN1")
				strRN1 = strValBuff;
			else if(strCategory == "SKY")
				strSKY = strValBuff;
			else if(strCategory == "REH")
				strREH = strValBuff;
			else if(strCategory == "PTY")
				strPTY = strValBuff;
			else if(strCategory == "WSD")
				strWSD = strValBuff;
#ifdef _DEBUG
			TRACE("WeatherLive Xml File Load category : [%s], obsrValue : [%s]) \n",strCategory,strObsrValue);
#endif

			pNode = pNode->NextSibling();  // ���� Item �̵� �մϴ�.
		}
		catch (_com_error &e)
		{
			strRunlog_E2.Format("Position : [%s][%s], log : [%s]",strMsgTitle,strSiteName,(LPCTSTR)(_bstr_t)e.Description());
			SetWriteLogFile("Processor-log : [Weather parsing _com_error..],",strRunlog_E2);
#ifdef _DEBUG
			TRACE("Weather parsing _com_error - %s \n",strRunlog_E2);
#endif
			ToiXmlDoc.Clear();
			return FALSE;
		}
		catch (...)
		{
			strRunlog_E2.Format("Position : [%s][%s], log : [XML Parsing Error]",strMsgTitle,strSiteName);
			SetWriteLogFile("Processor-log : [Weather parsing Error..],",strRunlog_E2);
#ifdef _DEBUG
			TRACE("Weather parsing Error (XML Parsing Error) \n");
#endif
			ToiXmlDoc.Clear();
			return FALSE;
		}
	}

	ToiXmlDoc.Clear();

	if(nDataCnt != 0)
		((CFormView_Weather *)(m_pCtrl))->OutputWeatherLiveHistory(strSiteName,time.Format("%m/%d %H:%M"),strT1H,strRN1,strSKY,strREH,strPTY,strWSD);
	else
		return FALSE;


	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
/*!
 * @fn int GetWeatherForecastGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck)
 * @brief ���������� ����
 * @details 48�ð� ���������� ����
 * @param[in] currentTime ����ð�
 * @param[in] szSavePath �ӽ� ���� ������ġ
 * @param[in] stDBInfo DB ���� ����
 * @param[in] stGatherInfo �������� ��ġ ����
 * @param[in] bStartCheck ������
 * @return TRUE: ���� ,THREAD_END: ������ ����
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
int CThread_WeatherGather::GetWeatherForecastGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck)
{
	int nHour = 0;

	nHour = currentTime.GetHour();
	//nHour = 8;

	CString strLogMsg = "";

	if((nHour == 2 || nHour == 5 || nHour == 8 || nHour == 11 || nHour == 14 || nHour == 17 || nHour == 20 || nHour == 23) )
	{
		//if(((currentTime.GetMinute() >= 30) && m_bWeatherForecastTimeCheck == FALSE))
		if(m_bWeatherForecastTimeCheck == FALSE)
		{
			int nForecastList = GetWeatherList(WEATHER_FORECAST);
			if(nForecastList > 0)
			{
				for(int nI = 0; nI < nForecastList; nI++)
				{
					strLogMsg.Format("%s - 48�ð� �������� ó��",m_stWeatherList[nI].szSiteName);
					//_addCurrentstateMsg(1,0, m_strThreadName, strLogMsg);

					if(m_bEndCheck == TRUE)
						break;
					BOOL bResult = GetWeatherForecast_URL(currentTime,stGatherInfo->szUrlForecast,m_stWeatherList[nI],g_stProjectInfo.szWeatherForecastPath);
					if(bResult == TRUE)
					{
						SetWeatherForecastSpaceData(currentTime, nI,m_stWeatherList[nI].szSiteId,stDBInfo);
						strLogMsg.Format("Position : [Weather Forecast][%s], log : [SetQuery Success],",m_stWeatherList[nI].szSiteName);
						_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Forecast]", USER_COLOR_BLACK, strLogMsg);
#ifdef _DEBUG
						TRACE("Weather live insert Query Success) \n");
#endif
					}
					else
					{
						strLogMsg.Format("Position : [Weather Forecast][%s], log : [SetQuery Fail],",m_stWeatherList[nI].szSiteName);
						_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Forecast]", USER_COLOR_BLACK, strLogMsg);
#ifdef _DEBUG
						TRACE("WeatherForecast live XML Data Load Fail) \n");
#endif
					}
				}
				if(m_bEndCheck == TRUE)
					return THREAD_END;
			}
			else
			{
				strLogMsg.Format("Position : [Weather Forecast],log : [SetQuery Success][������ ����Ʈ������ �����ϴ�.]");
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Forecast]", USER_COLOR_BLACK, strLogMsg);
				//_addCurrentstateMsg(1,0, m_strThreadName, strLogMsg);
			}
		}
		m_bWeatherForecastTimeCheck = TRUE;
	}
	else
		m_bWeatherForecastTimeCheck = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn BOOL GetWeatherForecast_URL(CTime time,const char *szUrl,ST_WEATHER_LIST stWeatherInfoList,const char *szSavePath)
 * @brief ���������� ����
 * @details ���� ���� XML ������ ȣ�� �� ������ �Ľ�
 * @param[in] time ��ȸ�� �ð�
 * @param[in] szUrl ���� URL
 * @param[in] stDBInfo DB ���� ����
 * @param[in] stWeatherInfoList ��ȸ�� ��ġ�� ��ǥ
 * @param[in] szSavePath �ӽ� ������ġ
 * @return TRUE: ���� ,FLASE: ��ȸ ����
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
BOOL CThread_WeatherGather::GetWeatherForecast_URL(CTime time,const char *szUrl,ST_WEATHER_LIST stWeatherInfoList,const char *szSavePath)
{
	CString strMsgTitle = "WeatherForecast";
	CString strUrl = "",strBuff = "",strMsgTemp = "",strRunlog_E2 = "",strSiteName ="";
	CXmlDocumentWrapper xmlDocW;
	CString strXmlSavePath = "";
	double dx = 0;
	double dy = 0;

	ST_WEATHERINFO stWeatherUrl;
	memset(&stWeatherUrl,0x00,sizeof(stWeatherUrl));
	strSiteName = stWeatherInfoList.szSiteName;
	stWeatherUrl.XY_Conv(stWeatherInfoList.fLat,stWeatherInfoList.fLon,&dx,&dy);

	strUrl = stWeatherUrl.GetUrl(szUrl,"",FRC_TIMEDATA,time,(int)dx,(int)dy);

#ifdef _DEBUG
	TRACE("WeatherForecast Xml Url : [%s][%s] \n",strSiteName,strUrl);
#endif

	//////////////////////////////////////////////////////////////////////////
	//RUL ���� XML Load
	if(xmlDocW.Load(strUrl) == FALSE)
	{
		strMsgTemp.Format("Position : [%s][%s], log : [���� Fail][%s]",strMsgTitle,strSiteName,strUrl);
		SetWriteLogFile("Processor-log : [Weather UrlXmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml Url Load Error.. (%s) \n",strMsgTemp);
#endif
		return FALSE;
	}
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather UrlXmlLoad]", USER_COLOR_BLACK, "log : [Url Xml Load Success]");

	strXmlSavePath.Format("%s\\%s-%04d-%02d-%02d %02d%02d.xml",szSavePath,strSiteName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute());
	xmlDocW.Save(strXmlSavePath);


	//////////////////////////////////////////////////////////////////////////
	//XML ���� Load
	TiXmlDocument ToiXmlDoc;
	ToiXmlDoc.Clear();
	if(ToiXmlDoc.LoadFile(strXmlSavePath) == FALSE)
	{
		strRunlog_E2.Format("Position : [%s][%s], alog : [Xml File Load Fail][%s]",strMsgTitle,strSiteName,strXmlSavePath);
		SetWriteLogFile("Processor-log : [Weather XmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml File Load Error.. (%s) \n",strRunlog_E2);
#endif
		return FALSE;
	}
	strRunlog_E2.Format("Position : [%s][%s], log : [File Xml Load Success]",strMsgTitle,strSiteName);
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather XmlLoad]", USER_COLOR_BLACK, strRunlog_E2);

	int nDataCnt = 0;
	CString strDataCnt = "";
	CString strGetText ="",strTreeText = "", strTextTemp = "";

	TiXmlNode* pNode = ToiXmlDoc.FirstChild("wid")->FirstChild("body")->FirstChild("data");

	while(pNode)
	{
		pNode->ToElement()->Attribute("seq"); // ù��° ����� name �д´�

		pNode = pNode->NextSibling();  // �ι�°�� �̵�
		nDataCnt++;
	}

	pNode = ToiXmlDoc.FirstChild("wid")->FirstChild("header");
	CString strTM = GetElementText(pNode,"tm",strMsgTitle,strSiteName); //1��°��(����/����/�� �� ?)

	m_stWeatherForecast.nTotalCnt = nDataCnt;
	m_stWeatherForecast.init(strTM);

	pNode = ToiXmlDoc.FirstChild("wid")->FirstChild("body")->FirstChild("data");
	CString strCategory ="", strColumnName = "";
	CString strObsrValue ="";
	CString strValBuff ="";

	for(int nI = 0; nI < nDataCnt; nI++)
	{
		try
		{
			int nReadHour = -1;
			strGetText = GetElementText(pNode,"day",strMsgTitle,strSiteName); //1��°��(����/����/�� �� ?)
			m_stWeatherForecast.pStWTData[nI].nDay = atoi(strGetText);
			if(atoi(strGetText) == 0)
				strTextTemp = "����";
			else if(atoi(strGetText) == 1)
				strTextTemp = "����";
			else if(atoi(strGetText) == 2)
				strTextTemp = "��";

			strGetText = GetElementText(pNode,"hour",strMsgTitle,strSiteName); //�����ð�
			nReadHour = atoi(strGetText);
			m_stWeatherForecast.pStWTData[nI].nHour = atoi(strGetText);

			strGetText = GetElementText(pNode,"temp",strMsgTitle,strSiteName); //���� �ð� �µ�
			m_stWeatherForecast.pStWTData[nI].fTemp = (float)atof(strGetText);

			strGetText = GetElementText(pNode,"tmx",strMsgTitle,strSiteName); //�ְ�µ� (-999.0 : ���� ���� ���)
			m_stWeatherForecast.pStWTData[nI].fTempMx = (float)atof(strGetText);

			strGetText = GetElementText(pNode,"tmn",strMsgTitle,strSiteName); //���� �µ� (-999.0 : ���� ���� ���)
			m_stWeatherForecast.pStWTData[nI].fTempMn = (float)atof(strGetText);

			strGetText = GetElementText(pNode,"sky",strMsgTitle,strSiteName); //�ϴû��� �ڵ� 1:����,2:��������,3:��������,4:�帲
			m_stWeatherForecast.pStWTData[nI].nSky = atoi(strGetText);

			strGetText = GetElementText(pNode,"pty",strMsgTitle,strSiteName); //�������� �ڵ� 0:����,1��,2:��/��,3:��/��,4:��
			m_stWeatherForecast.pStWTData[nI].nPty = atoi(strGetText);

			CString strRetBuff = GetElementText(pNode,"wfKor",strMsgTitle,strSiteName); //�����ѱ��� ����,��������,��������,�帲,��,��/��,��
			//�ʿ��ϸ� �����ڵ带 �ٽ� �Ƚ�(MBCS)��
			wchar_t utf8[64] = {0,};
			char ansi[64] = {0,};
			//�����ڵ��
			int re1 = MultiByteToWideChar(CP_UTF8,0,(LPCTSTR)strRetBuff,strRetBuff.GetLength()+1,utf8,1000);
			int re2 = WideCharToMultiByte(CP_ACP,0,utf8,wcslen(utf8)+1,ansi,1000,0,NULL);
			sprintf_s(m_stWeatherForecast.pStWTData[nI].szWfkor,"%s",ansi);

			strGetText = GetElementText(pNode,"pop",strMsgTitle,strSiteName); //������ %
			m_stWeatherForecast.pStWTData[nI].nPop = atoi(strGetText);

			strGetText = GetElementText(pNode,"ws",strMsgTitle,strSiteName); //ǳ��(m/s)
			m_stWeatherForecast.pStWTData[nI].nWs = atoi(strGetText);

			strGetText = GetElementText(pNode,"wd",strMsgTitle,strSiteName); //ǳ�� �ڵ� (0:��,1:�ϵ�,2:��,3:����,4:��,5:����,6:��,7:�ϼ�
			m_stWeatherForecast.pStWTData[nI].nWd = atoi(strGetText);

			strRetBuff = GetElementText(pNode,"wdKor",strMsgTitle,strSiteName); //ǳ�� �ѱ���
			memset(utf8,0x00,sizeof(utf8));
			memset(ansi,0x00,sizeof(ansi));
			//�����ڵ��
			re1 = MultiByteToWideChar(CP_UTF8,0,(LPCTSTR)strRetBuff,strRetBuff.GetLength() + 1, utf8, 1000);
			re2 = WideCharToMultiByte(CP_ACP,0,utf8,wcslen(utf8)+1,ansi,1000,0,NULL);
			sprintf_s(m_stWeatherForecast.pStWTData[nI].szWdkor, "%s", ansi);

			strGetText = GetElementText(pNode,"reh",strMsgTitle,strSiteName); //����(%)
			m_stWeatherForecast.pStWTData[nI].nReh = atoi(strGetText);

			pNode = pNode->NextSibling();  // ���� Item �̵� �մϴ�.

			CString strTime = "";
			strTime.Format("%s/%d",strTextTemp,nReadHour);
			CString strData1,strData2,strData3,strData4;
			strData1.Format("%.1f",m_stWeatherForecast.pStWTData[nI].fTemp);
			strData2.Format("%d",m_stWeatherForecast.pStWTData[nI].nReh);
			strData3.Format("%f",m_stWeatherForecast.pStWTData[nI].fTempMx);
			strData4.Format("%f",m_stWeatherForecast.pStWTData[nI].fTempMn);
			((CFormView_Weather *)(m_pCtrl))->OutputWeatherForecastHistory(strSiteName,strTime,strData1,strData2,strData3,strData4,
				m_stWeatherForecast.pStWTData[nI].szWfkor,m_stWeatherForecast.pStWTData[nI].szWdkor);
		}
		catch (_com_error &e)
		{
			strRunlog_E2.Format("Position : [%s][%s], log : [%s]",strMsgTitle,strSiteName,(LPCTSTR)(_bstr_t)e.Description());
			SetWriteLogFile("Processor-log : [Weather parsing _com_error..],",strRunlog_E2);
#ifdef _DEBUG
			TRACE("Weather parsing _com_error - %s \n",strRunlog_E2);
#endif
			ToiXmlDoc.Clear();
			return FALSE;
		}
		catch (...)
		{
			strRunlog_E2.Format("Position : [%s][%s], log : [XML Parsing Error]",strMsgTitle,strSiteName);
			SetWriteLogFile("Processor-log : [Weather parsing Error..],",strRunlog_E2);
#ifdef _DEBUG
			TRACE("Weather parsing Error (XML Parsing Error) \n");
#endif
			ToiXmlDoc.Clear();
			return FALSE;
		}
	}
	/*
	strQuery.Format("INSERT INTO CM_WEATHER_FORECAST_HISTORY(SITE_ID,REGISTER_DATE,TEMP,REH,TMX,TMN,SKY,WFKOR,PTY,POP,WS,WD,WDKOR) VALUES ("
	"'%s', '%s', %0.1f, %d, %0.1f, %0.1f, %d, '%s', %d, %d, %d, %d, '%s')",
	szSiteId,strMsr_Tim,m_stWeatherForecast.pStWTData[nI].fTemp,m_stWeatherForecast.pStWTData[nI].nReh,
	m_stWeatherForecast.pStWTData[nI].fTempMx,m_stWeatherForecast.pStWTData[nI].fTempMn,m_stWeatherForecast.pStWTData[nI].nSky,m_stWeatherForecast.pStWTData[nI].szWfkor,
	m_stWeatherForecast.pStWTData[nI].nPty,m_stWeatherForecast.pStWTData[nI].nPop,m_stWeatherForecast.pStWTData[nI].nWs,m_stWeatherForecast.pStWTData[nI].nWd,m_stWeatherForecast.pStWTData[nI].szWdkor)
	*/

	ToiXmlDoc.Clear();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn CString GetElementText(TiXmlNode* pNode, const char* szChildName,const char *szLogTitle,const char *szLogPos)
 * @brief XML �����ͺ� ����
 * @details XML ������ �Ľ�
 * @param[in] pNode XML ������ ��Ʈ
 * @param[in] szChildName ��ȸ�� �����͸�
 * @param[in] stDBInfo DB ���� ����
 * @param[in] szLogTitle �α������ ��Ī
 * @param[in] szLogPos �α� ��ġ
 * @return Text ��ȸ�� ������ ���
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
CString CThread_WeatherGather::GetElementText(TiXmlNode* pNode, const char* szChildName,const char *szLogTitle,const char *szLogPos)
{
	CString strText = "";
	CString strRunlog_E2 = "";

	try
	{
		strText = pNode->FirstChild(szChildName)->ToElement()->GetText();
	}
	catch (_com_error &e)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [Xml Element _com_error], NodeNAme:[%s],[%s]",szLogTitle,szLogPos,szChildName,(LPCTSTR)(_bstr_t)e.Description());
		SetWriteLogFile("Processor-log : [Weather Xml Element Load Error..],",strRunlog_E2);

#ifdef _DEBUG
		TRACE("Weather - Xml Element _com_error (%s) \n",strRunlog_E2);
#endif
		return "0";
	}
	catch(...)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [XML Parsing Error]",szLogTitle,szLogPos);
		SetWriteLogFile("Processor-log : [Weather Xml Element Load Error..],",strRunlog_E2);

#ifdef _DEBUG
		TRACE("Weather - Xml Element Error (%s) \n",strRunlog_E2);
#endif
		return "0";
	}

	return strText;
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn int SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos)
 * @brief DB ���� ����
 * @details (Insert/Update) ���� �� ����
 * @param[in] strQuery ������ ���ڿ�
 * @param[in] szLogTitle �α� ��Ī
 * @param[in] szLogPos �α� ��ġ
 * @return 1: ����,-1:����
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
int CThread_WeatherGather::SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos)
{
	CString strRunlog_E2 = "";

	int nResult = DB_Connect->SetQueryRun(strQuery);

	if(nResult < 1)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [Query Fail],[%s]",szLogTitle,szLogPos,strQuery);
		SetWriteLogFile("Processor-log : [Weather Set Query Error..],",strRunlog_E2);

#ifdef _DEBUG
		TRACE("Weather - Set Query Error (%s) \n",strRunlog_E2);
#endif
		return -1;
	}

	if (m_bEndThread == TRUE)
		return THREAD_END;

	return 0;
}


//////////////////////////////////////////////////////////////////////////
/*!
 * @fn void SetWeatherForecastSpaceData(CTime time, int nIndex,const char *szSiteId,ST_DBINFO *stDBInfo)
 * @brief ���� ������ DB������ ����
 * @details (Insert/Update) ������ ����
 * @param[in] time ����� �ð�
 * @param[in] szSiteId ����Ʈ(�ǹ�) ID
 * @param[in] stDBInfo DB ���� ����
 * @return -
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
void CThread_WeatherGather::SetWeatherForecastSpaceData(CTime time, int nIndex,const char *szSiteId,ST_DBINFO *stDBInfo)
{
	int nTotalCnt = m_stWeatherForecast.nTotalCnt;
	CString strQuery = "",strMsr_Tim = "";
	CTime HeadTime = m_stWeatherForecast.HeadTime;
	CTime TimeTemp;

	for(int nI = 0; nI < nTotalCnt; nI++)
	{
		int nDayCheck = m_stWeatherForecast.pStWTData[nI].nDay;
		int nHour = m_stWeatherForecast.pStWTData[nI].nHour;
		CTimeSpan timeSpan(nDayCheck,nHour,0,0);

		TimeTemp = HeadTime + timeSpan;

		if(stDBInfo->unDBType == DB_MSSQL)
			strMsr_Tim = TimeTemp.Format("%Y-%m-%d %H:00");
		else if(stDBInfo->unDBType == DB_ORACLE)
			strMsr_Tim.Format("TO_DATE('%s','YYYY-MM-DD HH24:MI'),",TimeTemp.Format("%Y-%m-%d %H:00"));
		else
		{
		}

		int nCheck = GetWeatherForecast_UseCheck(szSiteId,strMsr_Tim);

		if(nCheck == 0)
		{
																						//�µ�,����, �ְ�, �ּ�,�ϴ�, ����, ����,����, ǳ��,ǳ��, ǳ���ѱ�
			strQuery.Format("INSERT INTO CM_WEATHER_FORECAST_HISTORY(SITE_ID,REGISTER_DATE,TEMP,REH,TMX,TMN,SKY,WFKOR,PTY,POP,WS,WD,WDKOR) VALUES ("
				"'%s', '%s', %0.1f, %d, %0.1f, %0.1f, %d, '%s', %d, %d, %d, %d, '%s')",
				szSiteId,strMsr_Tim,m_stWeatherForecast.pStWTData[nI].fTemp,m_stWeatherForecast.pStWTData[nI].nReh,
				m_stWeatherForecast.pStWTData[nI].fTempMx,m_stWeatherForecast.pStWTData[nI].fTempMn,m_stWeatherForecast.pStWTData[nI].nSky,m_stWeatherForecast.pStWTData[nI].szWfkor,
				m_stWeatherForecast.pStWTData[nI].nPty,m_stWeatherForecast.pStWTData[nI].nPop,m_stWeatherForecast.pStWTData[nI].nWs,m_stWeatherForecast.pStWTData[nI].nWd,m_stWeatherForecast.pStWTData[nI].szWdkor);
			SetQueryValue(strQuery,"WeatherForecast-Insert",szSiteId);
			//InsertWeat_For(strQueryValue);
		}
		else if (nCheck == 1)
		{
			CString strQueryValue = "";
			strQueryValue.Format("TEMP =%0.1f, REH = %d, TMX = %0.1f, TMN = %0.1f, SKY = %d, WFKOR = '%s', PTY = %d, POP = %d, WS = %d, WD = %d, WDKOR = '%s'",m_stWeatherForecast.pStWTData[nI].fTemp,m_stWeatherForecast.pStWTData[nI].nReh,
				m_stWeatherForecast.pStWTData[nI].fTempMx,m_stWeatherForecast.pStWTData[nI].fTempMn,m_stWeatherForecast.pStWTData[nI].nSky,m_stWeatherForecast.pStWTData[nI].szWfkor,
				m_stWeatherForecast.pStWTData[nI].nPty,m_stWeatherForecast.pStWTData[nI].nPop,m_stWeatherForecast.pStWTData[nI].nWs,m_stWeatherForecast.pStWTData[nI].nWd,m_stWeatherForecast.pStWTData[nI].szWdkor);

			strQuery.Format("UPDATE CM_WEATHER_FORECAST_HISTORY SET %s WHERE SITE_ID ='%s' AND REGISTER_DATE = '%s'",
				strQueryValue,szSiteId,strMsr_Tim);

			SetQueryValue(strQuery,"WeatherForecast-Update",szSiteId);
			//UpdateWeat_For(strSiteOid, strMsr_Tim, strQueryValue);
		}
		else
		{

		}
	}

	if(nTotalCnt <= 0)
		m_stWeatherForecast.DeleteItme();
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn int GetWeatherForecast_UseCheck(const char *szSiteid, const char *szMsrTime)
 * @brief ���� ���� DB ���忩�� üũ
 * @details ���� ���� DB ���忩�� üũ
 * @param[in] szSiteId ����Ʈ(�ǹ�) ID
 * @param[in] szMsrTime ��ȸ�� ��¥�ð� colum data Format : xxxx-xx-xx xx:xx
 * @return -
 * @bug -
 * @warning -
 * @see -
 * @ref -
 */
//////////////////////////////////////////////////////////////////////////
int CThread_WeatherGather::GetWeatherForecast_UseCheck(const char *szSiteid, const char *szMsrTime)
{
	CString strMsgTitle = "WeatherForecast UseCheck";
	CString strQuery = "",strRunlog_E2 = "";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;
	int nCnt = 0;

	strQuery.Format("SELECT COUNT(*) as Cnt FROM CM_WEATHER_FORECAST_HISTORY WHERE SITE_ID ='%s' AND REGISTER_DATE = '%s'",szSiteid, szMsrTime);

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				DB_Connect->GetFieldValue(pRs, "Cnt", dbValue);
				nCnt = (int)dbValue;
			}
		}

		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}


		if(nCnt == 0)
			return 0;
		else
			return 1;
	}
	catch (_com_error &e)
	{
		Com_Error(strMsgTitle,&e);
		//CString strDescription = e.Description();
		if(0x80004005 == e.Error())
		{
			strRunlog_E2.Format("Position : [%s], log : [DB ���� ���� �õ�..]",strMsgTitle);
			SetWriteLogFile("Processor-log : [_com_error..],",strRunlog_E2);

			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB ���� ����!]",strMsgTitle);
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

		strRunlog_E2.Format("Position : [%s], log : [WeatherForecast Event][%s]",strMsgTitle,strQuery);
		SetWriteLogFile("Processor-log : [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}
}