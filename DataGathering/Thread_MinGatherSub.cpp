// Thread_MinData.cpp : implementation file
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_MinGatherSub.h"
#include "FormView_TAGGather.h"

/*
//20200225 나정호 메모리 누수 테스트
#include <crtdbg.h>
#if _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif // 몇행에서 메모리 누수가 나는지 알려줌.
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThread_MinGatherSub

IMPLEMENT_DYNCREATE(CThread_MinGatherSub, CWinThread)

CThread_MinGatherSub::CThread_MinGatherSub()
{
	m_bEndThread = FALSE;
	DB_Connect = NULL;

	m_List_ST_Tag = NULL;
	//m_List_ST_Tag = new std::list<ST_TagInfoList>;// 20210310 ksw 주석

	m_bSubThreadPause = TRUE;
	m_bButtonStop = FALSE;

	Release_List_ST_Tag(ST_LIST_CLEAR);
}

CThread_MinGatherSub::~CThread_MinGatherSub()
{
	//20200225 나정호 수정 메모리 누수 체크
	//_CrtSetBreakAlloc(24496);
	//_CrtDumpMemoryLeaks(); // 메모리 누수 체크


}

BOOL CThread_MinGatherSub::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	//CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_MinGatherSub::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	//_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath,"메모리누수Log","SubTread ExitInstance");
	if (DB_Connect != NULL)
	{
		if (DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	Release_List_ST_Tag(ST_LIST_CLEAR);

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_MinGatherSub, CWinThread)
	//{{AFX_MSG_MAP(CThread_MinGatherSub)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThread_MinGatherSub message handlers

void CThread_MinGatherSub::Release_List_ST_Tag(int nMode)
{
	if (m_List_ST_Tag != NULL)
	{
		m_List_ST_Tag->clear();

		if (nMode == 1)
		{
			delete m_List_ST_Tag;
			m_List_ST_Tag = NULL;
		}
	}
}

void CThread_MinGatherSub::LogHistory(CString strLogName, CString strMsg, int nColorType)
{
	EnterCriticalSection(&g_cs);
	CString strMsgBuff = "";
	strMsgBuff.Format("%s-%s", strLogName, strMsg);
	//	((CDLG_LogMin *)(m_pCtrl))->Syslog(m_strThreadName, strMsgBuff, nColorType);
	LeaveCriticalSection(&g_cs);
}

void CThread_MinGatherSub::SetWriteLogFile(const char* szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, LOG_FOLDER_NAME_1, szLogMsg);// _szAlarmSettingPath,strLogMsg);
	LeaveCriticalSection(&g_cs);
}

void CThread_MinGatherSub::ShowGridDataOutPut(CString strStatus, CString strMsg)
{
	EnterCriticalSection(&g_cs);
	((CFormView_TAGGather*)(m_pCtrl))->ShowListMsgUdate(m_nThreadNumber, strStatus, strMsg);
	LeaveCriticalSection(&g_cs);
}

int CThread_MinGatherSub::Run()
{
	// TODO: Add your specialized code here and/or call the base class
	CString str;

	CTime currentTime;
	CString strMsgTemp = "";
	int nTimeTemp = -1;

	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	m_strLogTitle = m_strThreadName;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);
	int nTotalTagCount = 0;


	do
	{
		if (m_bEndThread == TRUE)
			break;

		currentTime = CTime::GetCurrentTime();


		if (DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if (bConnectCheck != TRUE)
			{
				Sleep(20);
				continue;
			}
		}

		try
		{
			if (stDBInfo.unDBType == DB_POSTGRE && m_nProduct == 1) throw 0; //20210805 ksw

			if (m_bEndThread == TRUE)
				break;

			//if((currentTime.GetMinute() % 1) != 0 || nTimeTemp == currentTime.GetMinute())
			if (currentTime.GetMinute() - nTimeTemp < 0) nTimeTemp = -1;

			if ((currentTime.GetMinute() - nTimeTemp) < (m_nInterval + 1) || nTimeTemp == currentTime.GetMinute()) // 20210831 ksw 주기 조절 옵션 추가
			{
				Sleep(500);
				continue;
			}

			nTimeTemp = currentTime.GetMinute();

			int nItemSize = m_List_ST_Tag->size();
			str.Format("Sub Thread nItemSize : %d", nItemSize);
			if (nItemSize <= 0)
			{
				Sleep(100);

#ifdef _DEBUG
				TRACE("%s Item  Size 0 \n", m_strThreadName);
#endif
				continue;
			}

			BOOL bStartCheck = FALSE;
			float fValueCheck = 0;
			CString strValue = "";
			std::list<ST_TagInfoList>::iterator iter = m_List_ST_Tag->begin();
			while (iter != m_List_ST_Tag->end())
			{
				strValue = GetTagValue(iter->szTAG_Name, iter->nTagType);
				if (m_bEndThread == TRUE)
					break;
				if ("Error" == strValue)
				{
					Sleep(5);
					iter++;
					continue;
				}

				if (strValue.Find("[Lower Val!!]", 0) > -1) // 2020830 ksw 너무 낮은 음수값 예외
				{
					strValue.Replace("[Lower Val!!] ", "");
					strMsgTemp.Format("TagName : %s / Value : %s", iter->szTAG_Name, strValue);
					throw 1;
				}

				/*if(iter->nTagType == TYPE_AI)
				{
				fValueCheck = (iter->fValue / atof(strValue) )*100;
				if (fValueCheck > 10)
				{
				}

				//iter->fValue = atof(strValue);
				}*/

				if (strValue != "-")
				{
					int nRowCount; //20210407 ksw 제품분기

					if (m_nProduct == 0)
						nRowCount = GetMinRegCheck(iter->szTAG_Name, currentTime, stDBName.szHMIDBName, stDBInfo.unDBType);
					else
						nRowCount = GetMinRegCheck(iter->szTAG_Id, currentTime, stDBName.szHMIDBName, stDBInfo.unDBType);

					if (nRowCount == 0) //Insert
						SetTagValue(QUERY_TYPE_INSERT, stDBInfo.unDBType, iter->szTAG_Id, iter->szTAG_Name, iter->szGroupName, iter->nTagType, currentTime, strValue, stDBName.szHMIDBName);
					else //Update
						SetTagValue(QUERY_TYPE_UPDATE, stDBInfo.unDBType, iter->szTAG_Id, iter->szTAG_Name, iter->szGroupName, iter->nTagType, currentTime, strValue, stDBName.szHMIDBName);
				}



				if (m_bEndThread == TRUE)
					break;

				Sleep(5);
				iter++;
			}

			if (m_bEndThread == TRUE)
				break;
			CTime NowTime = CTime::GetCurrentTime();
			strMsgTemp.Format("Update Time : End Time (%s : %s)", currentTime.Format("%H:%M:%S"), NowTime.Format("%H:%M:%S"));
#ifdef _DEBUG
			TRACE("MSG(%s) - 처리 경과 (%s) \n", m_strThreadName, strMsgTemp);
#endif
			//LogHistory("분데이터 처리", strMsgTemp, LOG_COLOR_RED);
			ShowGridDataOutPut("정상", strMsgTemp);

		}
		catch (int exp)
		{
			if (exp == 0)
			{
				strMsgTemp.Format("Version Check Please!! ");
				ShowGridDataOutPut("POSTGRE && TAG_ID", strMsgTemp);
				m_bEndThread = TRUE;
			}
			else if (exp == 1)
			{
				LogHistory("Lower Value!!", strMsgTemp, LOG_COLOR_RED);

				ShowGridDataOutPut("음수 범위 Warnning", "범위 Warnning!!");
				_addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, LOG_COLOR_RED, m_strThreadName + " -> " + strMsgTemp);
				CString strRunlog_E2Log;
				strRunlog_E2Log.Format("(%s):%s", m_strThreadName, strMsgTemp);
				SetWriteLogFile(strRunlog_E2Log);

				Sleep(500);
			}

		}
		catch (...)
		{
			strMsgTemp.Format("(%s) - catch Error ", m_strThreadName);
#ifdef _DEBUG
			TRACE("MSG(%s) - catch Error \n", m_strThreadName);
#endif
			LogHistory("Main", strMsgTemp, LOG_COLOR_RED);
			SetWriteLogFile(strMsgTemp);
		}


	} while (!m_bEndThread);
#ifdef _DEBUG
	TRACE("MSG(%s) - Thread End.. \n", m_strThreadName);
#endif
	if (m_bButtonStop == TRUE)
		_addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strThreadName, USER_COLOR_BLUE, "Message : [프로세서 종료]");

	PostThreadMessage(WM_QUIT, 0, 0);
	return CWinThread::Run();
}

CString CThread_MinGatherSub::GetTagValue(CString strTagName, int nTagTyp)
{
	CString strValue = "";
	int nRet = -1;
	ST_EV_TAG_INFO* pTagInfo = new ST_EV_TAG_INFO;

	nRet = EV_GetTagInfo(strTagName, pTagInfo);

	if (nRet == -5)
	{
		if (pTagInfo)
		{
			delete pTagInfo;
			pTagInfo = NULL;
		}
		return "Error";
	}

	BYTE byValue; //DI
	double dValue; //AI

	if (TYPE_DI == pTagInfo->nTagType)
	{
		EV_GetDiData(pTagInfo->nStnPos, pTagInfo->nTagPos, &byValue); //태그 그룹 위치, DI태그 위치로 Data값 추출
		strValue.Format("%d", byValue);
		if ((atoi(strValue) != 0) && (atoi(strValue) != 1))
			strValue = "-";
	}
	else if (TYPE_AI == pTagInfo->nTagType)
	{
		EV_GetAiData(pTagInfo->nStnPos, pTagInfo->nTagPos, &dValue); //태그 그룹 위치, AI태그 위치로 Data값 추출
		strValue.Format("%0.4f", dValue);
		if (strValue == "1.#INF")
			strValue = "0";

		if (strValue.Find("-", 0) > -1)// 20210830 ksw 9자리 이상 음수값 제한
		{
			CString strTmp = strValue;
			strTmp = strTmp.Mid(1, strTmp.GetLength());
			int nFind = strTmp.Find('.', 0); // 20210924 ksw 소수점 처리..

			if (nFind > -1)
				strTmp = strTmp.Left(nFind);

			if (strTmp.GetLength() > 8)
				strValue = "[Lower Val!!] " + strValue;

		}
	}
	else
		strValue = "0";

	if (pTagInfo)
	{
		delete pTagInfo;
		pTagInfo = NULL;
	}
	if (m_bEndThread == TRUE)
		return "";

	return strValue;
}


int CThread_MinGatherSub::GetMinRegCheck(CString strTagName, CTime currentTime, const char* szDBName, int nDBType)
{
	CString strMSGTitle = "GetMinRegCheck";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "", strRunlog_E2 = "", strDBName = "";
	int nCountRow = 0;

	CString strDayTyp = "";
	CString strHour = "";
	CString strMin = "";
	strDayTyp.Format("%s", currentTime.Format("%Y%m%d"));
	strHour.Format("%s", currentTime.Format("%H"));
	strMin.Format("%s", currentTime.Format("%M"));


	if (nDBType == DB_MSSQL)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}
	//20200212 jsh : postgre 추가
	else if (nDBType == DB_POSTGRE) //20210702 ksw 조회 테이블 수정
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("Easy_Hmi.HM_MINUTE_TREND_HISTORY");
	}
	else
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}


	//20200220 나정호 수정 TAG_ID -> TAG_NAME 변경
	//20210407 ksw 제품분기
	if (m_nProduct == 1)
	{
		strQuery.Format("SELECT COUNT(TAG_ID) as cnt FROM %s WHERE RECORD_DATE ='%s' AND RECORD_TIME =%s AND TAG_ID ='%s'",
			strDBName,
			strDayTyp, strHour, strTagName);
	}
	else
	{
		strQuery.Format("SELECT COUNT(TAG_NAME) as cnt FROM %s WHERE RECORD_DATE ='%s' AND RECORD_TIME =%s AND TAG_NAME ='%s'",
			strDBName,
			strDayTyp, strHour, strTagName);
	}

	//SetWriteLogFile(strQuery); // ksw

	//20200220 나정호 수정 PostgreSQL odbc 부분 추가
	if (nDBType == DB_POSTGRE)
	{
		try
		{
			if (m_nProduct == 1) throw 0; // 20210805 ksw DB : Postgre, Key : TAG_ID

			SQLRETURN retcode;
			//DB_Connect->codbc->COdbc::SQLFreeStmtHandle();
			DB_Connect->codbc->COdbc::SQLAllocStmtHandle();
			retcode = DB_Connect->SetQueryRun(strQuery);

			//COdbc codbcTemp("postgres",(_bstr_t)(DB_Connect->m_stDBInfo.szID),(_bstr_t)(DB_Connect->m_stDBInfo.szPW));
			//void DB_ConnectionInfo(const char* szSerVer,const char* szDBName,const char* szID,const char* szPW,UINT unDBTypeID);


			if (isSqlOk(retcode))
			{

				int cnt = 0;
				CString t;
				//DB_Connect->codbc->DeleteNonFixedRows();
				retcode = DB_Connect->codbc->COdbc::SQLFetch();
				//retcode = SQLFetch();

				if (isSqlOk(retcode))
				{
					SQLINTEGER cbData = 0;
					SQLSMALLINT nData = 0;

					DB_Connect->codbc->COdbc::SQLGetData(1, SQL_C_SSHORT, &nData, sizeof(nData), &cbData);

					//dbValue = atoi(szData);

					//DB_Connect->GetFieldValue(pRs, "cnt", dbValue);
					nCountRow = nData;
				}
			}
			else
			{
				if (DB_Connect->codbc != NULL)
				{
					delete DB_Connect->codbc;
					DB_Connect->codbc = NULL;
				}
				if (pRs != NULL)
				{
					pRs->Close();
					pRs = NULL;
				}
			}
			return nCountRow;
		}
		catch (_com_error& e)
		{
			CString strErrorCode = Com_Error(strMSGTitle, &e);

			if (strErrorCode == "942")
			{

#ifdef _DEBUG
				TRACE("GetSchedule()/catch com error - %s\n", strRunlog_E2);
#endif
				return ERROR_DB_NO_TABLE;
			}
			else if (strErrorCode == "3113" || strErrorCode == "80004005")
			{
				int nResult = DB_Connect->DB_ReConnection();
				if (nResult == 0)
				{
					strRunlog_E2.Format("ESS-Schedule - DB 접속 실패!");
					LogHistory(strMSGTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetSchedule()/catch com error - %s\n", strRunlog_E2);
#endif
				}
				return ERROR_DB_RECONNECTION;
			}
			else
				return ERROR_DB_COM_ERROR;
		}
		catch (...)
		{
			if (DB_Connect->codbc != NULL)
			{
				delete DB_Connect->codbc;
				DB_Connect->codbc = NULL;
			}
			if (pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}

			strRunlog_E2.Format("SELECT 실패 Event Error : %s", strQuery);
			LogHistory(strMSGTitle, strRunlog_E2, LOG_COLOR_RED);
			ShowGridDataOutPut("catch", strRunlog_E2);

			strRunlog_E2.Format("%s:%s - SELECT 실패 Event Error : %s", m_strThreadName, strMSGTitle, strQuery);
			SetWriteLogFile(strRunlog_E2);
			return ERROR_DB_QUERY_FAIL1;
		}


	}
	else
	{
		try
		{
			if (m_bEndThread == TRUE)
				return THREAD_END;

			pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);

			if (pRs != NULL)
			{
				if (!pRs->GetEndOfFile())
				{
					double dbValue;
					DB_Connect->GetFieldValue(pRs, "cnt", dbValue);
					nCountRow = (int)dbValue;
				}
				if (pRs != NULL)
				{
					pRs->Close();
					pRs = NULL;
				}


#ifdef _DEBUG
				TRACE("MSG(%s) - Select Tag Name - %s,Count - %d \n", m_strThreadName, strTagName, nCountRow);
#endif
				return nCountRow;
			}
		}
		catch (_com_error& e)
		{
			CString strErrorCode = Com_Error(strMSGTitle, &e);

			if (strErrorCode == "942")
			{

#ifdef _DEBUG
				TRACE("GetSchedule()/catch com error - %s\n", strRunlog_E2);
#endif
				return ERROR_DB_NO_TABLE;
			}
			else if (strErrorCode == "3113" || strErrorCode == "80004005")
			{
				int nResult = DB_Connect->DB_ReConnection();
				if (nResult == 0)
				{
					strRunlog_E2.Format("ESS-Schedule - DB 접속 실패!");
					LogHistory(strMSGTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetSchedule()/catch com error - %s\n", strRunlog_E2);
#endif
				}
				return ERROR_DB_RECONNECTION;
			}
			else
				return ERROR_DB_COM_ERROR;
		}
		catch (...)
		{
			if (pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}

			strRunlog_E2.Format("SELECT 실패 Event Error : %s", strQuery);
			LogHistory(strMSGTitle, strRunlog_E2, LOG_COLOR_RED);
			ShowGridDataOutPut("catch", strRunlog_E2);

			strRunlog_E2.Format("%s:%s - SELECT 실패 Event Error : %s", m_strThreadName, strMSGTitle, strQuery);
			SetWriteLogFile(strRunlog_E2);
			return ERROR_DB_QUERY_FAIL1;
		}

	}



	return nCountRow;
}

int CThread_MinGatherSub::SetTagValue(int nQueryType, int nDBType, ST_TagInfoList stList, CTime currentTime, CString strValue, const char* szDBName)
{
	CString strQuery = "", strDBName = "";
	CString strDayTyp = "";
	CString strHour = "";
	CString strMin = "";
	CString strRunlog_E2 = "", strRunlog_E2Log = "";
	strDayTyp.Format("%s", currentTime.Format("%Y%m%d"));
	strHour.Format("%s", currentTime.Format("%H"));
	strMin.Format("%s", currentTime.Format("%M"));


	if (nDBType == DB_MSSQL)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}
	//20200212 jsh : postgre 추가
	else if (nDBType == DB_POSTGRE) //20210702 ksw 조회 테이블 수정
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("Easy_Hmi.HM_MINUTE_TREND_HISTORY");
	}
	else
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}
	// 20210407 ksw 제품 분기
	////////////////////
	if (nQueryType == QUERY_TYPE_INSERT)
	{
		if (nDBType == DB_POSTGRE) //20210310 ksw DB분기
		{
			strQuery.Format("INSERT INTO %s(GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
				" VALUES ('%s','%s','%s',%s,%s)",
				strDBName, strMin,
				stList.szGroupName, stList.szTAG_Name, strDayTyp, strHour, strValue);
#ifdef _DEBUG
			TRACE("MSG(%s) %s(INSERT)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
		else //20210310 ksw DB분기
		{//20210310 ksw 제품 분기 추가
			if (m_nProduct == 0) //BEMS
			{
				strQuery.Format("INSERT INTO %s(TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
					" VALUES ('%s','%s',%s,%s)",
					strDBName, strMin, stList.szTAG_Name, strDayTyp, strHour, strValue);
			}
			else //EMS
			{
				strQuery.Format("INSERT INTO %s(TAG_ID, GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
					" VALUES ('%s','%s','%s','%s',%s,%s)",
					strDBName, strMin,
					stList.szTAG_Id, stList.szGroupName, stList.szTAG_Name, strDayTyp, strHour, strValue);
			}
#ifdef _DEBUG
			TRACE("MSG(%s) %s(INSERT)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
	}
	else
	{
		// 20210407 ksw 제품 분기
		if (nDBType == DB_POSTGRE) //20210310 ksw DB분기
		{
			strQuery.Format("UPDATE %s SET T%s = %s "
				" WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
				strDBName, strMin, strValue,
				stList.szTAG_Name, strDayTyp, strHour);
#ifdef _DEBUG
			TRACE("MSG(%s) %s(UPDATE)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
		else //20210310 ksw DB분기
		{
			if (m_nProduct == 0)
			{
				strQuery.Format("UPDATE %s SET T%s = %s "
					" WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
					strDBName, strMin, strValue,
					stList.szTAG_Name, strDayTyp, strHour);
			}
			else
			{
				strQuery.Format("UPDATE %s SET T%s = %s "
					" WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
					strDBName, strMin, strValue,
					stList.szTAG_Id, strDayTyp, strHour);
			}
#ifdef _DEBUG
			TRACE("MSG(%s) %s(UPDATE)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
	}
	//////////////////////

//	if(nQueryType == QUERY_TYPE_INSERT)
//	{
//		//20200220 나정호 수정 Coloum 변경으로 인한 TAG_ID 삭제
//		strQuery.Format("INSERT INTO %s(GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
//			" VALUES ('%s','%s','%s',%s,%s)",
//			strDBName,strMin,
//			stList.szGroupName,stList.szTAG_Name,strDayTyp,strHour,strValue);
//#ifdef _DEBUG
//		TRACE("MSG(%s) %s(INSERT)  = %s\n",m_strThreadName,strDBName,strQuery);
//#endif
//	}
//	else
//	{
//		strQuery.Format("UPDATE %s SET T%s = %s "
//			" WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
//			strDBName,strMin,strValue,
//			stList.szTAG_Name,strDayTyp,strHour);
//#ifdef _DEBUG
//		TRACE("MSG(%s) %s(UPDATE)  = %s\n",m_strThreadName,strDBName,strQuery);
//#endif
//	}

	int nResult = DB_Connect->SetQueryRun(strQuery);

	if (nResult < 1)
	{

#ifdef _DEBUG
		TRACE("MSG(%s) %s Query-Error (%s) \n", m_strThreadName, strDBName, strQuery);
#endif
		strRunlog_E2.Format("생성 실패 (%s)", strQuery);

		LogHistory("SetTagValue", strRunlog_E2, LOG_COLOR_RED);
		if (nQueryType == QUERY_TYPE_INSERT)
			strRunlog_E2.Format("Insert 실패 Log 저장");
		else
			strRunlog_E2.Format("Update 실패 Log 저장");

		ShowGridDataOutPut("생성 Error", strRunlog_E2);

		strRunlog_E2Log.Format("(%s):%s-%s", m_strThreadName, strRunlog_E2, strQuery);
		SetWriteLogFile(strRunlog_E2Log);
		Sleep(500);
	}

	return 0;
}

int CThread_MinGatherSub::SetTagValue(int nQueryType, int nDBType, CString strTagId, CString strTagName, CString strGroupName, int nTagTyp, CTime currentTime, CString strValue, const char* szDBName)
{
	CString strQuery = "", strDBName = "";
	CString strDayTyp = "";
	CString strHour = "";
	CString strMin = "";
	CString strRunlog_E2 = "", strRunlog_E2Log = "";
	strDayTyp.Format("%s", currentTime.Format("%Y%m%d"));
	strHour.Format("%s", currentTime.Format("%H"));
	strMin.Format("%s", currentTime.Format("%M"));

	if (nDBType == DB_MSSQL)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}
	//20200212 jsh : postgre
	else if (nDBType == DB_POSTGRE)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("Easy_Hmi.HM_MINUTE_TREND_HISTORY");
	}
	else
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", szDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}

	if (nQueryType == QUERY_TYPE_INSERT)
	{
		if (nDBType == DB_POSTGRE) //20210310 ksw DB분기
		{
			strQuery.Format("INSERT INTO %s(GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
				" VALUES ('%s','%s','%s',%s,%s)",
				strDBName, strMin,
				strGroupName, strTagName, strDayTyp, strHour, strValue);
#ifdef _DEBUG
			TRACE("MSG(%s) %s(INSERT)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
		else //20210310 ksw DB분기
		{//20210310 ksw 제품 분기 추가
			if (m_nProduct == 0) //BEMS
			{
				strQuery.Format("INSERT INTO %s(TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
					" VALUES ('%s','%s',%s,%s)",
					strDBName, strMin, strTagName, strDayTyp, strHour, strValue);
			}
			else //EMS
			{
				strQuery.Format("INSERT INTO %s(TAG_ID, GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME,T%s ) "
					" VALUES ('%s','%s','%s','%s',%s,%s)",
					strDBName, strMin,
					strTagId, strGroupName, strTagName, strDayTyp, strHour, strValue);
			}
#ifdef _DEBUG
			TRACE("MSG(%s) %s(INSERT)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
	}

	else
	{
		// 20210407 ksw 제품 분기
		if (nDBType == DB_POSTGRE) //20210310 ksw DB분기
		{
			strQuery.Format("UPDATE %s SET T%s = %s "
				" WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
				strDBName, strMin, strValue,
				strTagName, strDayTyp, strHour);
#ifdef _DEBUG
			TRACE("MSG(%s) %s(UPDATE)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
		else //20210310 ksw DB분기
		{
			if (m_nProduct == 0)
			{
				strQuery.Format("UPDATE %s SET T%s = %s "
					" WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
					strDBName, strMin, strValue,
					strTagName, strDayTyp, strHour);
			}
			else
			{
				strQuery.Format("UPDATE %s SET T%s = %s "
					" WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %s",
					strDBName, strMin, strValue,
					strTagId, strDayTyp, strHour);
			}
#ifdef _DEBUG
			TRACE("MSG(%s) %s(UPDATE)  = %s\n", m_strThreadName, strDBName, strQuery);
#endif
		}
	}
	EnterCriticalSection(&g_cs);
	int nResult = 0;

	if (m_nProduct == 1 && nDBType == DB_POSTGRE) // 20210805 ksw DB : PostgreSQL , Key : TAG_ID 예외 처리
	{
		nResult = -1;
	}
	else
	{
		nResult = DB_Connect->SetQueryRun(strQuery);
	}

	if (nResult < 1)
	{

#ifdef _DEBUG
		TRACE("MSG(%s) %s Query-Error (%s) \n", m_strThreadName, strDBName, strQuery);
#endif
		strRunlog_E2.Format("생성 실패 (%s)", strQuery);

		LogHistory("SetTagValue", strRunlog_E2, LOG_COLOR_RED);
		if (nQueryType == QUERY_TYPE_INSERT)
			strRunlog_E2.Format("Insert 실패 Log 저장");
		else
			strRunlog_E2.Format("Update 실패 Log 저장");

		ShowGridDataOutPut("생성 Error", strRunlog_E2);

		strRunlog_E2Log.Format("(%s):%s-%s", m_strThreadName, strRunlog_E2, strQuery);
		SetWriteLogFile(strRunlog_E2Log);
		Sleep(500);
	}
	LeaveCriticalSection(&g_cs);
	return 0;
}

CString CThread_MinGatherSub::Com_Error(const char* szLogName, _com_error* e)
{
	CString strRunlog_E2 = "", strRunlog_E2Log = "", strErrorCode, strErrorID;
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("DB Error Code = %08lx Code meaning = %s Source = %s Description = %s",
		e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);

	if (m_nDBType == DB_ORACLE)
	{
		strErrorID.Format("%s", (LPCTSTR)bstrDescription);
		strErrorCode = strErrorID.Mid(4, 5);//Left(nPos);
	}
	else if (m_nDBType == DB_MSSQL)
	{
		strErrorCode.Format("%08lx", e->Error());
	}

#ifdef _DEBUG
	TRACE("MSG(%s) %s-catch Com Error = %s\n", m_strThreadName, szLogName, strRunlog_E2);
#endif
	LogHistory(szLogName, strRunlog_E2, LOG_COLOR_RED);
	ShowGridDataOutPut("Com Error", strRunlog_E2);
	strRunlog_E2Log.Format("(%s) %s-catch Com Error = %s", m_strThreadName, szLogName, strRunlog_E2);
	SetWriteLogFile(strRunlog_E2Log);
	Sleep(500);

	return strErrorCode;
}