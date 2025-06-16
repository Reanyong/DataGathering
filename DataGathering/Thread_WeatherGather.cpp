// Thread_WeatherGather.cpp : 구현 파일입니다.
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
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_WeatherGather::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
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
 * @brief  함수에 대한 설명. (한줄 이상) 다음 키워드가 나올때까지 '///' 다음의 문장을 brief의 문장으로 인식한다.
 * @details  결과 모르지만, 추가 설명이 필요하면 사용하라는 키워드. (more)를 눌렀을때 보여줄 텍스트.
 * @param[in]	파라미터 이름	파라미터 설명, [in]은 입력용 파라미터라는 뜻.
 * @param[out]	파라미터 이름	파라미터 설명, [out]은 입력용 파라미터라는 뜻.
 * @return	리턴값에 대한 설명, 리턴 값이 여럿이라 추가 설명이 필요하면 retval을 사용한다. '\returns'와 같다.
 * @retval	리턴값	리턴 값에 대한 설명.
 * @bug		알고 있는 버그에 대해서 적는다.
 * @todo		todo에 대해서 적는다.
 * @warning	주의 사항에 대해서 적는다.
 * @see '다른 파일 이름'을 적는다. 다른 곳을 참조하라고 할때 쓰는데.. 직접  써본적은 없어서.
*/
/*!
 * @fn void Release_List_ST(int nMode)
 * @brief 리스트 구초제 초기화/삭제
 * @details 구초제 초기화 및 메모리 해제
 * @param[in] nMode 입력용 숫자값
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

// CThread_WeatherGather 메시지 처리기입니다.
//////////////////////////////////////////////////////////////////////////
/*!
 * @fn Com_Error(const char *szLogName,_com_error *e)
 * @brief 에러 메시지 발생
 * @details try/catch 에서 오류 발생시 오류 메시지 발생 정보
 * @param[in] *szLogName 메시지 위치/발생 명칭
 * @param[in] *e 발생메시지
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
	//_addCurrentstateMsg(1,0, m_strThreadName, "DB 처리 오류 상세로그 확인");
	Sleep(500);
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn void SetWriteLogFile(const char *sTitle,const char *szLogMsg)
 * @brief 이력 및 메시지 출력
 * @details 오류 발생이력 정보 파일로 저장
 * @param[in] *sTitle 출력 명칭
 * @param[in] *szLogMsg 로그 메시지
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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
		sprintf_s(stWeatherInfo[0].szSite_ID,"현대파워텍");
		sprintf_s(stWeatherInfo[0].szSite_Name,"현대파워텍");
		nSiteCount = 1;
	}

	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,m_strThreadName,g_stProjectInfo.szProjectLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);

	strLogMsg.Format("주기 실황:[30]분,예보:[정시]");
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

			//_addCurrentstateMsg(0,0, m_strThreadName, "Processor 정상 처리중..");
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
- 호출 방법 : 호출
- 사이트별 날씨수집 사용여부 조회
-int GetWeatherList(int nQueryType)
/*!
 * @fn int GetWeatherList(int nQueryType)
 * @brief 날씨연동 정보 조회
 * @details 실황/예보 수집 사용유무 조회
 * @param[in] *nQueryType 0: 실황 정보 조회, 1: 예보 정보 조회
 * @return - 조회된 Count
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
			// 레코드 Get 후 종료한다.
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

		strRunlog_E2.Format("Position : [%s], log : [WeatherLive Event][%s]",strMsgTitle,strQuery);
		SetWriteLogFile("Processor-log : [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}

	return nCountRow;
}

//////////////////////////////////////////////////////////////////////////
/*!
 * @fn int GetWeatherLiveGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck)
 * @brief 실황 날씨 정보 쿼리 데이터 조합
 * @details 날씨정보 데이터 쿼리 정보 조합
 * @param[in] currentTime 현재 시간
 * @param[in] szSavePath 조회된 데이터 임시 저장 위치
 * @param[in] stDBInfo 접속된 DB 정보
 * @param[in] bStartCheck 사용안함
 * @return 1: 성공 ,THREAD_END: 쓰레드 종료
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
	//기상 정보 live 정보 수집 부분
	//ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);

	//00분 부터 20분까지 는 데이터 가 없다.
	CTimeSpan timeSpan(0,1,0,0); //or timeSpan = CTimeSpan(일,시,분,초);
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
			//strLogMsg.Format("%s - 실황날씨 처리",m_stWeatherList[nI].szSiteName);
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
						_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_RED, "log : [MY-SQL 쿼리 미지정으로 인한 수접 종류!]");
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
		strLogMsg.Format("Position : [Weather Live],log : [SetQuery Success][수집할 사이트정보가 없습니다.]");
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
			// 레코드 Get 후 종료한다.
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

	//기상 정보 live 정보 수집 부분
	//00분 부터 20분까지 는 데이터 가 없다.
	CTimeSpan timeSpan(0,1,0,0); //or timeSpan = CTimeSpan(일,시,분,초);
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
				// -> 현대파워텍에서 사용 SITE ID 사용안함 //strValue.Format(" VALUES (TO_DATE('%s','YYYY-MM-DD HH24:MI'),",currentTime.Format("%Y-%m-%d %H:%M"));
				strValue.Format(" VALUES ('%s',TO_DATE('%s','YYYY-MM-DD HH24:MI'),",stWeatherInfo->szSite_ID,currentTime.Format("%Y-%m-%d %H:00"));
			}
			else
			{
				_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather Live]", USER_COLOR_RED, "log : [My-SQL 쿼리 미지정으로 인한 수접 종류!]");
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
			//-> 현대파워텍 에서 사용한 쿼리문//strQuery.Format("INSERT INTO %sCM_WEATHER_LIVE_HISTORY (REGISTER_DATE,%s)%s",strDBName,strColumn,strValue);
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
		strLogMsg.Format("Position : [Weather Live],log : [SetQuery Success][수집할 사이트정보가 없습니다.]");
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
 * @brief 데이터 호출 및 데이터 파싱
 * @details 실황 데이터 URL 접속 조회된 데이터 파싱
 * @param[in] time 조회할 시간
 * @param[in] szUrl 접속 URL
 * @param[in] stWeatherInfoList 실황 데이터 위치 정보
 * @param[in] szSavePath 임시 파일 저장위치
 * @return TRUE: 성공 ,FALSE: 실패
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
	//RUL 접속 XML Load
#ifdef	_TEST_MODE
	strXmlSavePath.Format("%s\\WeatherLive-2016-02-19 1145.xml",szSavePath);
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather XmlLoad]", USER_COLOR_BLACK, "Log : [_TEST_MODE Load]");
#else
	if(xmlDocW.Load(strUrl) == FALSE)
	{
		strMsgTemp.Format("Position : [%s][%s], log : [접속 Fail][%s]",strMsgTitle,strSiteName,strUrl);
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
	//XML 파일 Load
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
		pNode->ToElement()->Attribute("item"); // 첫번째 노드의 name을 읽습니다

		pNode = pNode->NextSibling();  // 두번째로 이동합니다.
		nDataCnt++;
	}

	pNode = ToiXmlDoc.FirstChild("response")->FirstChild("body")->FirstChild("items")->FirstChild("item");
	CString strCategory ="", strColumnName = "";
	CString strObsrValue ="";
	CString strValBuff ="";

	//TCHAR*	_lpszLiveGrid[] = {"Site","온도","강수량","하늘코드","습도","강수코드","풍속"};
	CString strT1H,strRN1,strSKY,strREH,strPTY,strWSD;

	for(int nI = 0; nI < nDataCnt; nI++)
	{
		try
		{
			strCategory = GetElementText(pNode,"category",strMsgTitle,strSiteName); //항목값
			//strColumnName = GetColumnName(strCategory); //항목값과 일치하는 컬럼 명 가져온다.
			strColumnName = strCategory; //XML 컬럼 데이터 그대로 테이블 컬럼 사용
			strObsrValue = GetElementText(pNode,"obsrValue",strMsgTitle,strSiteName); //Missing
			strValBuff = stWeatherUrl.GetForecastGrib_TYP(strCategory,strObsrValue);
			if(strValBuff == "Error")
			{
				pNode = pNode->NextSibling();  // 다음 Item 이동 합니다.
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

			pNode = pNode->NextSibling();  // 다음 Item 이동 합니다.
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
	//RUL 접속 XML Load

	if(xmlDocW.Load(strUrl) == FALSE)
	{
		strMsgTemp.Format("Position : [%s][%s], log : [접속 Fail][%s]",strMsgTitle,strSiteName,strUrl);
		SetWriteLogFile("Processor-log : [Weather UrlXmlLoad Error..],",strRunlog_E2);
#ifdef _DEBUG
		TRACE("WeatherLive Xml Url Load Error.. (%s) \n",strMsgTemp);
#endif
		return FALSE;
	}
	_addSystemMsg(LOG_MESSAGE_3, USER_COLOR_BLUE, "Processor-log : [Weather UrlXmlLoad]", USER_COLOR_BLACK, "log : [Url Xml Load Success]");

	//--사용안함
	//strXmlPath.Format("%s\\%s-%04d-%02d-%02d %02d%02d.xml",szSavePath,strSiteName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute());

	strXmlFileName.Format("%s-%04d-%02d-%02d %02d%02d.xml",strSiteName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute());
	strXmlPath.Format("%s\\%s",szSavePath,strXmlFileName);
	xmlDocW.Save(strXmlPath);

	//테스트할때만 사용
	//strXmlPath.Format("%s\\Error-TestXML.xml",szSavePath);


	//////////////////////////////////////////////////////////////////////////
	//XML 파일 Load
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
		strHeaderCheck = GetElementText(pNodeHeaderCheck,"resultMsg",strMsgTitle,strSiteName); //항목값

		if(strHeaderCheck != "OK")
		{
			ToiXmlDoc.Clear();
			//strXmlPath.Format("%s\\현대파워텍-2017-01-17 1650.xml",szSavePath);
			if(m_strXmlOldFileName.IsEmpty() == TRUE)
			{
				strRunlog_E2.Format("Position : [%s][%s], log : [기상청 정보 확인 및 XML 파일 확인 필요][%s]",strMsgTitle,strSiteName,strXmlPath);
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

			strRunlog_E2.Format("Position : [%s][%s], log : [기상청응답 세션 문제(누락방지 전데이터(%s)로 업데이트)][%s]",strMsgTitle,strSiteName,m_strXmlOldFileName,strXmlPath);
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
		pNode->ToElement()->Attribute("item"); // 첫번째 노드의 name을 읽습니다

		pNode = pNode->NextSibling();  // 두번째로 이동합니다.
		nDataCnt++;
	}

	pNode = ToiXmlDoc.FirstChild("response")->FirstChild("body")->FirstChild("items")->FirstChild("item");
	CString strCategory ="", strColumnName = "";
	CString strObsrValue ="";
	CString strValBuff ="";

	//TCHAR*	_lpszLiveGrid[] = {"Site","온도","강수량","하늘코드","습도","강수코드","풍속"};
	CString strT1H,strRN1,strSKY,strREH,strPTY,strWSD;

	for(int nI = 0; nI < nDataCnt; nI++)
	{
		try
		{
			strCategory = GetElementText(pNode,"category",strMsgTitle,strSiteName); //항목값
			//strColumnName = GetColumnName(strCategory); //항목값과 일치하는 컬럼 명 가져온다.
			strColumnName = strCategory; //XML 컬럼 데이터 그대로 테이블 컬럼 사용
			strObsrValue = GetElementText(pNode,"obsrValue",strMsgTitle,strSiteName); //Missing
			strValBuff = stWeatherUrl.GetForecastGrib_TYP(strCategory,strObsrValue);
			if(strValBuff == "Error")
			{
				pNode = pNode->NextSibling();  // 다음 Item 이동 합니다.
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

			pNode = pNode->NextSibling();  // 다음 Item 이동 합니다.
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
 * @brief 예보데이터 수집
 * @details 48시간 예보데이터 수집
 * @param[in] currentTime 현재시간
 * @param[in] szSavePath 임시 파일 저장위치
 * @param[in] stDBInfo DB 접속 정보
 * @param[in] stGatherInfo 예보정보 위치 정보
 * @param[in] bStartCheck 사용안함
 * @return TRUE: 성공 ,THREAD_END: 쓰레드 종료
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
					strLogMsg.Format("%s - 48시간 날씨예보 처리",m_stWeatherList[nI].szSiteName);
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
				strLogMsg.Format("Position : [Weather Forecast],log : [SetQuery Success][수집할 사이트정보가 없습니다.]");
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
 * @brief 예보데이터 수집
 * @details 예보 날씨 XML 데이터 호출 및 데이터 파싱
 * @param[in] time 조회할 시간
 * @param[in] szUrl 접속 URL
 * @param[in] stDBInfo DB 접속 정보
 * @param[in] stWeatherInfoList 조회할 위치및 좌표
 * @param[in] szSavePath 임시 저자위치
 * @return TRUE: 성공 ,FLASE: 조회 실패
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
	//RUL 접속 XML Load
	if(xmlDocW.Load(strUrl) == FALSE)
	{
		strMsgTemp.Format("Position : [%s][%s], log : [접속 Fail][%s]",strMsgTitle,strSiteName,strUrl);
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
	//XML 파일 Load
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
		pNode->ToElement()->Attribute("seq"); // 첫번째 노드의 name 읽는다

		pNode = pNode->NextSibling();  // 두번째로 이동
		nDataCnt++;
	}

	pNode = ToiXmlDoc.FirstChild("wid")->FirstChild("header");
	CString strTM = GetElementText(pNode,"tm",strMsgTitle,strSiteName); //1번째날(오늘/내일/모레 중 ?)

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
			strGetText = GetElementText(pNode,"day",strMsgTitle,strSiteName); //1번째날(오늘/내일/모레 중 ?)
			m_stWeatherForecast.pStWTData[nI].nDay = atoi(strGetText);
			if(atoi(strGetText) == 0)
				strTextTemp = "금일";
			else if(atoi(strGetText) == 1)
				strTextTemp = "내일";
			else if(atoi(strGetText) == 2)
				strTextTemp = "모레";

			strGetText = GetElementText(pNode,"hour",strMsgTitle,strSiteName); //예보시간
			nReadHour = atoi(strGetText);
			m_stWeatherForecast.pStWTData[nI].nHour = atoi(strGetText);

			strGetText = GetElementText(pNode,"temp",strMsgTitle,strSiteName); //현재 시각 온도
			m_stWeatherForecast.pStWTData[nI].fTemp = (float)atof(strGetText);

			strGetText = GetElementText(pNode,"tmx",strMsgTitle,strSiteName); //최고온도 (-999.0 : 값이 없을 경우)
			m_stWeatherForecast.pStWTData[nI].fTempMx = (float)atof(strGetText);

			strGetText = GetElementText(pNode,"tmn",strMsgTitle,strSiteName); //최저 온도 (-999.0 : 값이 없을 경우)
			m_stWeatherForecast.pStWTData[nI].fTempMn = (float)atof(strGetText);

			strGetText = GetElementText(pNode,"sky",strMsgTitle,strSiteName); //하늘상태 코드 1:맑음,2:구름조금,3:구름많음,4:흐림
			m_stWeatherForecast.pStWTData[nI].nSky = atoi(strGetText);

			strGetText = GetElementText(pNode,"pty",strMsgTitle,strSiteName); //강수상태 코드 0:없음,1비,2:비/눈,3:눈/비,4:눈
			m_stWeatherForecast.pStWTData[nI].nPty = atoi(strGetText);

			CString strRetBuff = GetElementText(pNode,"wfKor",strMsgTitle,strSiteName); //날씨한국어 맑음,구름조금,구름많음,흐림,비,눈/비,눈
			//필요하면 유니코드를 다시 안시(MBCS)로
			wchar_t utf8[64] = {0,};
			char ansi[64] = {0,};
			//유니코드로
			int re1 = MultiByteToWideChar(CP_UTF8,0,(LPCTSTR)strRetBuff,strRetBuff.GetLength()+1,utf8,1000);
			int re2 = WideCharToMultiByte(CP_ACP,0,utf8,wcslen(utf8)+1,ansi,1000,0,NULL);
			sprintf_s(m_stWeatherForecast.pStWTData[nI].szWfkor,"%s",ansi);

			strGetText = GetElementText(pNode,"pop",strMsgTitle,strSiteName); //강수량 %
			m_stWeatherForecast.pStWTData[nI].nPop = atoi(strGetText);

			strGetText = GetElementText(pNode,"ws",strMsgTitle,strSiteName); //풍속(m/s)
			m_stWeatherForecast.pStWTData[nI].nWs = atoi(strGetText);

			strGetText = GetElementText(pNode,"wd",strMsgTitle,strSiteName); //풍량 코드 (0:북,1:북동,2:동,3:남동,4:남,5:남서,6:서,7:북서
			m_stWeatherForecast.pStWTData[nI].nWd = atoi(strGetText);

			strRetBuff = GetElementText(pNode,"wdKor",strMsgTitle,strSiteName); //풍량 한국어
			memset(utf8,0x00,sizeof(utf8));
			memset(ansi,0x00,sizeof(ansi));
			//유니코드로
			re1 = MultiByteToWideChar(CP_UTF8,0,(LPCTSTR)strRetBuff,strRetBuff.GetLength() + 1, utf8, 1000);
			re2 = WideCharToMultiByte(CP_ACP,0,utf8,wcslen(utf8)+1,ansi,1000,0,NULL);
			sprintf_s(m_stWeatherForecast.pStWTData[nI].szWdkor, "%s", ansi);

			strGetText = GetElementText(pNode,"reh",strMsgTitle,strSiteName); //습도(%)
			m_stWeatherForecast.pStWTData[nI].nReh = atoi(strGetText);

			pNode = pNode->NextSibling();  // 다음 Item 이동 합니다.

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
 * @brief XML 데이터별 추출
 * @details XML 데이터 파싱
 * @param[in] pNode XML 데이터 노트
 * @param[in] szChildName 조회할 데이터명
 * @param[in] stDBInfo DB 접속 정보
 * @param[in] szLogTitle 로그저장시 명칭
 * @param[in] szLogPos 로그 위치
 * @return Text 조회된 데이터 결과
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
 * @brief DB 쿼리 실행
 * @details (Insert/Update) 쿼리 문 실행
 * @param[in] strQuery 쿼리문 문자열
 * @param[in] szLogTitle 로그 명칭
 * @param[in] szLogPos 로그 위치
 * @return 1: 성공,-1:실패
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
 * @brief 예보 데이터 DB쿼리문 생성
 * @details (Insert/Update) 쿼리문 생성
 * @param[in] time 기록할 시간
 * @param[in] szSiteId 사이트(건물) ID
 * @param[in] stDBInfo DB 접속 정보
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
																						//온도,습도, 최고, 최소,하늘, 날씨, 강수,강수, 풍속,풍량, 풍량한국
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
 * @brief 예보 정보 DB 저장여부 체크
 * @details 예보 정보 DB 저장여부 체크
 * @param[in] szSiteId 사이트(건물) ID
 * @param[in] szMsrTime 조회된 날짜시간 colum data Format : xxxx-xx-xx xx:xx
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

		strRunlog_E2.Format("Position : [%s], log : [WeatherForecast Event][%s]",strMsgTitle,strQuery);
		SetWriteLogFile("Processor-log : [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}
}
