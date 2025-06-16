// Thread_Calibration.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_Calibration.h"


// CThread_Calibration

IMPLEMENT_DYNCREATE(CThread_Calibration, CWinThread)

	CThread_Calibration::CThread_Calibration()
{
	m_bEndThread = FALSE;
	m_strThreadName = "CThread_Calibration";
}

CThread_Calibration::~CThread_Calibration()
{
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}
}

BOOL CThread_Calibration::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	return TRUE;
}

int CThread_Calibration::ExitInstance()
{
	if (DB_Connect != NULL)
	{
		if (DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_Calibration, CWinThread)
END_MESSAGE_MAP()

void CThread_Calibration::SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody)
{
	_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "Title", crBody, strMsg);
}

// CThread_Calibration 메시지 처리기입니다.
int CThread_Calibration::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//HM_TAG_DIC 테이블 정보
	m_nListCnt = g_pList_ST_TagDivisionListCnt;
	m_pList_ST_TagDivisionList = g_pList_ST_TagDivisionList;

	//DB 연결 셋팅
	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);

	m_nDBType = stDBInfo.unDBType;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, "TagMapping 설정", g_stProjectInfo.szDTGatheringLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

	//Source DB 정보 셋팅
	CString strDBName = "";
	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
	if (stDBInfo.unDBType == DB_MSSQL)
	{
		if (strlen(stDBName.szHMIDBName) > 1)
			strDBName.Format("%s.dbo.HM_MINUTE_TREND_HISTORY", stDBName.szHMIDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}
	//20200211 jsh : postgres 추가인해 조건문 추가
	else if (stDBInfo.unDBType == DB_POSTGRE) //20210702 ksw 조회 테이블 수정
	{
		if (strlen(stDBName.szHMIDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", stDBName.szHMIDBName);
		else
			strDBName.Format("easy_hmi.HM_MINUTE_TREND_HISTORY");
	}
	else
	{
		if (strlen(stDBName.szHMIDBName) > 1)
			strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", stDBName.szHMIDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}

	_RecordsetPtr pRs = NULL;
	std::vector<CDataAi> vtVals_AI;

	//20200225 나정호 수정 tag id 제거
	//20210308 ksw BEMS EMS 버전 때문에 다시 살림
	CString strTagId = "", strTagName = "", strRecordDate = "", strRecordTime = "", strTData = "", strGroupName = "";
	//CString strTagName ="",strRecordDate ="",strRecordTime ="",strTData ="",strGroupName ="";
	CString strQuery, strMsg, sTDataColName, sCur_vtVals_AI_DateTime, sDBDateTime, sValue, sTmp, sInsertTColNames;
	CString strRealExcuteQuery = "";
	int nExcuteCheck = 0;
	int nQueryType = 0, nFailCnt = 0;
	char szProgress[100] = { 0, };

	strMsg.Format("Start Thread_Calibration ( %s ~ 현재 )", m_ctSelTime.Format("%Y/%m/%d 00"));
	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "Calibration 시작", USER_COLOR_BLACK, strMsg);

	while (!m_bEndThread)
	{
		//DB 연결 / 재연결
		if (DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if (bConnectCheck != TRUE)
			{
				Sleep(500);
				continue;
			}
		}

		//CThread_MinGatherMain::GetTagList 에서 HM_TAG_DIC 정보를 가져오므로 따로 다시 가져오지 않고 그대로 활용
		//혹시 타이밍상 못가져오면 다시 가져온다
		if (m_nListCnt == 0)
		{
			m_nListCnt = g_pList_ST_TagDivisionListCnt;
			m_pList_ST_TagDivisionList = g_pList_ST_TagDivisionList;

			Sleep(500);
			continue;
		}

		try
		{
			//진행상황 표시를 위해 전체 카운트
			unsigned int nTotalCnt = 0, nGroupCnt = 0;
			for (int nI = 0; nI < m_nListCnt; nI++)
			{
				nGroupCnt = m_pList_ST_TagDivisionList[nI]->size();
				nTotalCnt += nGroupCnt;
			}
			nGroupCnt = 0;



			//CThread_MinGatherMain::GetTagList 에서 HM_TAG_DIC 테이블 태그 긁어다가 m_nListCnt 개로 분할해 놨음
			//한마디로 전체 태그가 m_nListCnt개의 그룹으로 나뉘어져 있음
			for (int nI = 0; nI < m_nListCnt; nI++)
			{
				std::list<ST_TagInfoList>::iterator iter = m_pList_ST_TagDivisionList[nI]->begin();

				//각 그룹별로 보유하고 있는 태그만큼
				//HM_MINUTE_TREND_HISTORY 테이블에 태그 및 시간별로 저장한 데이터 보정
				while (iter != m_pList_ST_TagDivisionList[nI]->end())
				{
					//진행상황 표시
					nGroupCnt++;
					sprintf_s(szProgress, "%u / %u ( %d%% )", nGroupCnt, nTotalCnt, (nGroupCnt * 100) / (nTotalCnt));
					::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, NULL, (LPARAM)szProgress);

					/*if( nGroupCnt > 1 )
					{
					Sleep(5);
					iter++;

					continue;
					}*/

					//strMsg.Format("Before query ( %s )",iter->szTAG_Id);
					//SetWriteLogFile(strMsg);

					//20200220 나정호 수정 tag id -> tag name
					//RECORD_DATE는 string이고 RECORD_TIME은 int... 란다. RECORD_TIME이 시간인데 1자리 시간일때 비교와 정렬이 좀.. 비교와 정렬을 위해 변환해서 SELECT
					//strQuery.Format("SELECT * FROM %s WHERE TAG_ID = '%s' AND RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2) >= '%s' ORDER BY RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2)"
					//	,strDBName, iter->szTAG_Id, m_ctSelTime.Format("%Y%m%d00"));
					if (m_nProduct == 0)
					{
						if (stDBInfo.unDBType == DB_POSTGRE)	//20200219 나정호 수정 PostgreSQL 테이블 데이터 수집
						{
							strQuery.Format("SELECT * FROM %s WHERE TAG_NAME = '%s' AND RECORD_DATE || to_char(RECORD_TIME,'FM00') >= '%s' ORDER BY RECORD_DATE || TO_CHAR(RECORD_TIME,'FM00')"
								, strDBName, iter->szTAG_Name, m_ctSelTime.Format("%Y%m%d00"));
						}
						else
						{
							strQuery.Format("SELECT * FROM %s WHERE TAG_NAME = '%s' AND RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2) >= '%s' ORDER BY RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2)"
								, strDBName, iter->szTAG_Name, m_ctSelTime.Format("%Y%m%d00"));
						}
					}
					else
					{
						strQuery.Format("SELECT * FROM %s WHERE TAG_ID = '%s' AND RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2) >= '%s' ORDER BY RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2)"
							, strDBName, iter->szTAG_Id, m_ctSelTime.Format("%Y%m%d00"));
					}
					try
					{
						//태그별 HM_MINUTE_TREND_HISTORY 테이블 데이터

						/*if (stDBInfo.unDBType == DB_POSTGRE)
						const char* szSerVer = DB_Connect->m_stDBInfo.szServer;
						const char* szDBName = m_stDBInfo.szDB;
						const char* szID = m_stDBInfo.szID;
						const char* szPW =m_stDBInfo.szPW;
						UINT unDBTypeID = m_stDBInfo.unDBTypeID;
						DB_Connect->DB_ConnectionInfo(szSerVer,szDBName, szID, szPW,unDBTypeID);
						*/


						if (stDBInfo.unDBType == DB_POSTGRE)	//20200219 나정호 수정 PostgreSQL 테이블 데이터 수집
						{
							vtVals_AI.clear();
							//GetHistoricalTagValues( iter->szTAG_Name, CTime::GetCurrentTime() - CTimeSpan( 2, 0, 0, 0 ), CTime::GetCurrentTime(), vtVals_AI );
							GetHistoricalTagValues(iter->szTAG_Name, m_ctSelTime, CTime::GetCurrentTime(), vtVals_AI);

							//해당 태그의 현존하는 HM_MINUTE_TREND_HISTORY 테이블 레코드 하나씩 빼와서 해당 레코드까지의 데이터 채움
							//1시간 단위로 존재하는 HM_MINUTE_TREND_HISTORY 테이블 Row 하나씩
							int vtVals_AICnt = 0;

							//pRs = DB_Connect->codbc->SQLGetData();
							SQLRETURN retcode;

							retcode = DB_Connect->SetQueryRun(strQuery);

							SQLINTEGER sqlRqwCount;

							DB_Connect->codbc->SQLRowCount(&sqlRqwCount);

							int nCountRow = 0;
							nCountRow = sqlRqwCount;

							int nDivideCount = 0;
							//							nDivideCount = nCountRow / nTheradCount;


							if (isSqlOk(retcode))
							{

								if (nCountRow > 0)
								{
									while (1)
									{
										//int cnt=0;
										//CString t;
										//DB_Connect->codbc->DeleteNonFixedRows();
										retcode = DB_Connect->codbc->COdbc::SQLFetch();
										if (isSqlOk(retcode))
										{
											//SQLINTEGER cbTagId=0;
											SQLINTEGER cbTagName = 0;
											SQLINTEGER cbGroupName = 0;
											SQLINTEGER cbRecordDate = 0;
											SQLINTEGER cbRecordTime = 0;
											//SQLINTEGER cbTagTypeTemp=0;
											//SQLINTEGER cbDbName=0;

											//SQLCHAR szTagId[128] = {0,};
											SQLCHAR szTagName[64] = { 0, };
											SQLCHAR szGroupName[64] = { 0, };
											SQLCHAR szRecordDate[64] = { 0, };
											SQLCHAR szRecordTime[64] = { 0, };

											//SQLCHAR szTagType[64] = {0,};
											//SQLSMALLINT nTagTypeTemp = 0;
											//SQLCHAR szDbName[64] = {0,};

											void* Temp;

											//DB_Connect->codbc->COdbc::SQLGetData( 1,SQL_C_CHAR, szTagId , sizeof(szTagId) ,&cbTagId);
											DB_Connect->codbc->COdbc::SQLGetData(1, SQL_C_CHAR, szTagName, sizeof(szTagName), &cbTagName);
											//DB_Connect->codbc->COdbc::SQLGetData( 2,SQL_C_CHAR, szGroupName , sizeof(szGroupName) ,&cbGroupName);
											DB_Connect->codbc->COdbc::SQLGetData(2, SQL_C_CHAR, szRecordDate, sizeof(szRecordDate), &cbRecordDate);
											DB_Connect->codbc->COdbc::SQLGetData(3, SQL_C_CHAR, szRecordTime, sizeof(szRecordTime), &cbRecordTime);
											//DB_Connect->codbc->COdbc::SQLGetData( 1,SQL_C_CHAR, szTagName , sizeof(szTagName) ,&cbTagName);
											//DB_Connect->codbc->COdbc::SQLGetData( 3,SQL_C_CHAR, szDbName , sizeof(szDbName) ,&cbDbName);

											//ST_TagInfoList stNewTagInfo;
											//memset(&stNewTagInfo,0x00,sizeof(ST_TagInfoList));

											Temp = szTagName;
											strTagName = (const char*)Temp;

											Temp = szGroupName;
											strGroupName = (const char*)Temp;

											Temp = szRecordDate;
											strRecordDate = (const char*)Temp;

											Temp = szRecordTime;
											strRecordTime = (const char*)Temp;


											sDBDateTime.Format("%s%02d", strRecordDate, atoi(strRecordTime));

										}
										else
										{
											break;
										}

										//Easyview Historical 데이터 이용, 현재 레코드(pRs) 까지 보정
										for (; vtVals_AICnt < vtVals_AI.size(); vtVals_AICnt++)
										{
											sprintf_s(szProgress, "%d / %d ( %d%% )", vtVals_AICnt, vtVals_AI.size(), (vtVals_AICnt * 100) / (vtVals_AI.size()));
											::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress);

											if (vtVals_AICnt % 60 != 0)	continue;	//60개 (1시간) Historical 데이터 단위 = 레코드 1개 (1시간 데이터)

											sCur_vtVals_AI_DateTime = vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d%H"); //태그의 년월일시간
											if (sCur_vtVals_AI_DateTime == sDBDateTime)	//현재 레코드와 일치 -> Historical 데이터 활용 Update 후 Break
											{
												nQueryType = QUERY_TYPE_UPDATE;

												sValue = "";
												for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
												{
													if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History데이터 끝이면 거기까지만

													sTDataColName.Format("T%02d", n1HCnt - vtVals_AICnt);
													DB_Connect->GetFieldValue(pRs, sTDataColName, strTData);

													if (strTData.GetLength() > 0)	continue;	//T00~T59 중 값이 있는건 그대로둔다
													if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//History데이터도 없으면 그대로둔다	////없는 값일경우 -3.4028234663852886e+038 로 나온다
													if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History데이터 끝이면 거기까지만

													sTmp.Format("%s%s='%.4f'", sValue.GetLength() > 0 ? "," : "", sTDataColName, vtVals_AI[n1HCnt].dbVal);
													sValue += sTmp;
												}

												//Update할 데이터가 하나도 없을때 하지 않도록
												if (sValue.GetLength() == 0)
												{
													vtVals_AICnt += 60;
													break;
												}
												if (sInsertTColNames != "")
												{

													//20200220 나정호 수정 tag id -> tag name
													//strQuery.Format("UPDATE %s SET %s WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
													//	, strDBName, sValue, iter->szTAG_Id, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
													strQuery.Format("UPDATE %s SET %s WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
														, strDBName, sValue, iter->szTAG_Name, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
												}
												else
												{
													nExcuteCheck++;

													strQuery = "";
												}
											}
											//Record(Row)가 없을경우 가져온 Historical 값 vtVals_AI 으로 INSERT
											else	//현재 레코드와 불일치 -> 레코드 없음, Historical 데이터 활용 Insert 후 계속 레코드에 맞는 데이터 찾기
											{
												nQueryType = QUERY_TYPE_INSERT;

												sValue = "", sInsertTColNames = "";
												for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
												{
													if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History데이터 끝이면 거기까지만

													if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//없는 값일경우 -3.4028234663852886e+038 로 나온다

													sTmp.Format(",'%.4f'", vtVals_AI[n1HCnt].dbVal);
													sValue += sTmp;

													sTDataColName.Format(",T%02d", n1HCnt - vtVals_AICnt);
													sInsertTColNames += sTDataColName;
												}


												if (sInsertTColNames != "")
												{
													//20200220 나정호 수정 tag id -> tag name
													//strQuery.Format("INSERT INTO %s(TAG_ID, GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME, T00 ,T01 ,T02 ,T03 ,T04 ,T05 ,T06 ,T07 ,T08 ,T09 ,T10 ,T11 ,T12 ,T13 ,T14 ,T15 ,T16 ,T17 ,T18 ,T19 ,T20 ,T21 ,T22 ,T23 ,T24 ,T25 ,T26 ,T27 ,T28 ,T29 ,T30 ,T31 ,T32 ,T33 ,T34 ,T35 ,T36 ,T37 ,T38 ,T39 ,T40 ,T41 ,T42 ,T43 ,T44 ,T45 ,T46 ,T47 ,T48 ,T49 ,T50 ,T51 ,T52 ,T53 ,T54 ,T55 ,T56 ,T57 ,T58 ,T59 ) "
													//	CString text;
													//	text.Format("%s%d %s",vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"),vtVals_AI[vtVals_AICnt].tLogTime.GetHour(),sValue);

													strQuery.Format("INSERT INTO %s(GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME %s ) "
														" VALUES ('%s','%s','%s',%d %s)",
														strDBName, sInsertTColNames, iter->szGroupName, iter->szTAG_Name, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour(), sValue);
												}
												else
												{
													nExcuteCheck++;

													strQuery = "";
												}

											}
											if (strcmp(iter->szTAG_Name, "test") == 0)
											{
												int a = strQuery.GetLength();
												a = strRealExcuteQuery.GetLength();
												a++;
											}

											if (strQuery.GetLength() > 0)
											{
												if (strRealExcuteQuery)
													strRealExcuteQuery += strQuery + ";\r\n";
												nExcuteCheck++;

												strQuery = "";
											}
											if (nExcuteCheck > 50)
											{
												int a = 0;
												a++;
											}

											if (nExcuteCheck > 50 && strRealExcuteQuery.GetLength() > 0)
											{
												int nResult = DB_Connect->SetQueryRun(strRealExcuteQuery);

												nExcuteCheck = 0;
												strRealExcuteQuery = "";

												if (nResult < 1)
												{
#ifdef _DEBUG
													TRACE("MSG(%s) %s Query-Error (%s)(ErrorCode : %d) \n", m_strThreadName, strDBName, strRealExcuteQuery, nResult);
#endif
													nFailCnt++;

													if (nQueryType == QUERY_TYPE_INSERT)
														sTmp.Format("[%s] Insert 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);
													else
														sTmp.Format("[%s] Update 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);

													//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "생성 Error", USER_COLOR_BLACK, sTmp);

													strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
													SetWriteLogFile(strMsg);
													Sleep(50);
												}
											}

											vtVals_AICnt += 59;

											//UPDATE는 레코드를 찾았다는 뜻이니 다음레코드로..
											if (nQueryType == QUERY_TYPE_UPDATE)
											{

												vtVals_AICnt++;
												break;
											}
										}

										if (nExcuteCheck > 0 && strRealExcuteQuery.GetLength() > 0)
										{
											int nResult = DB_Connect->SetQueryRun(strRealExcuteQuery);

											nExcuteCheck = 0;
											strRealExcuteQuery = "";

											if (nResult < 1)
											{
#ifdef _DEBUG
												TRACE("MSG(%s) %s Query-Error (%s)(ErrorCode : %d) \n", m_strThreadName, strDBName, strRealExcuteQuery, nResult);
#endif
												nFailCnt++;

												if (nQueryType == QUERY_TYPE_INSERT)
													sTmp.Format("[%s] Insert 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);
												else
													sTmp.Format("[%s] Update 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);

												//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "생성 Error", USER_COLOR_BLACK, sTmp);

												strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
												SetWriteLogFile(strMsg);
												Sleep(50);
											}
										}
										sprintf_s(szProgress, "%d / %d ( %d%% )", vtVals_AI.size(), vtVals_AI.size(), 100);
										::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress);
										/*
										if(!pRs->GetEndOfFile())
										pRs->MoveNext();

										if(pRs->GetEndOfFile() && vtVals_AICnt >= vtVals_AI.size())	break;	//레코드와 vtVals_AI(Historical 데이터)가 더이상 없을때
										*/

									}
								}
								else
								{
									sDBDateTime = "";	//if( sCur_vtVals_AI_DateTime == sDBDateTime ) 에서 Update로 빠지지 않도록
								}
							}


						}
						else
						{
							pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
							//strMsg.Format("After query (Rec cnt : %d)( %s )", pRs->RecordCount,strQuery);
							//SetWriteLogFile(strMsg);

							//if(pRs == NULL)	continue;

							//태그별 Easyview Historical 데이터, 지정한 날짜 0시 ~ 현재까지 모두 가져옴
							vtVals_AI.clear();
							//GetHistoricalTagValues( iter->szTAG_Name, CTime::GetCurrentTime() - CTimeSpan( 2, 0, 0, 0 ), CTime::GetCurrentTime(), vtVals_AI );
							GetHistoricalTagValues(iter->szTAG_Name, m_ctSelTime, CTime::GetCurrentTime(), vtVals_AI);

							//해당 태그의 현존하는 HM_MINUTE_TREND_HISTORY 테이블 레코드 하나씩 빼와서 해당 레코드까지의 데이터 채움
							//1시간 단위로 존재하는 HM_MINUTE_TREND_HISTORY 테이블 Row 하나씩
							int vtVals_AICnt = 0;
							while (1)
							{
								//DB에는 더이상 레코드가 없지만 vtVals_AI(Historical 데이터)는 있으면 vtVals_AI 만큼 계속 보정
								if (!pRs->GetEndOfFile() && pRs->RecordCount > 0)
								{
									//20200220 나정호 수정 tag id -> tag name
									//20210308 ksw 제품 분기
									if (m_nProduct == 1) {
										DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);
										DB_Connect->GetFieldValue(pRs, "GROUP_NAME", strGroupName);
									}
									DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);
									DB_Connect->GetFieldValue(pRs, "RECORD_DATE", strRecordDate);
									DB_Connect->GetFieldValue(pRs, "RECORD_TIME", strRecordTime);

									sDBDateTime.Format("%s%02d", strRecordDate, atoi(strRecordTime));
								}
								else
									sDBDateTime = "";	//if( sCur_vtVals_AI_DateTime == sDBDateTime ) 에서 Update로 빠지지 않도록

								//여러개씩 묶어서 Query 실행
								int nExcuteCheck = 0;
								CString strRealExcuteQuery = "";

								//Easyview Historical 데이터 이용, 현재 레코드(pRs) 까지 보정
								for (; vtVals_AICnt < vtVals_AI.size(); vtVals_AICnt++)
								{
									//sprintf(szProgress, "%d / %d ( %d%% )", vtVals_AICnt, vtVals_AI.size()-1, (vtVals_AICnt*100)/(vtVals_AI.size()-1));
									//::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress);

									if (vtVals_AICnt % 60 != 0)	continue;	//60개 (1시간) Historical 데이터 단위 = 레코드 1개 (1시간 데이터)

									sCur_vtVals_AI_DateTime = vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d%H");
									if (sCur_vtVals_AI_DateTime == sDBDateTime)	//현재 레코드와 일치 -> Historical 데이터 활용 Update 후 Break
									{
										nQueryType = QUERY_TYPE_UPDATE;

										sValue = "";
										for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
										{
											if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History데이터 끝이면 거기까지만

											sTDataColName.Format("T%02d", n1HCnt - vtVals_AICnt);
											DB_Connect->GetFieldValue(pRs, sTDataColName, strTData);

											if (strTData.GetLength() > 0)	continue;	//T00~T59 중 값이 있는건 그대로둔다
											if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//History데이터도 없으면 그대로둔다	////없는 값일경우 -3.4028234663852886e+038 로 나온다
											if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History데이터 끝이면 거기까지만

											sTmp.Format("%s%s='%.4f'", sValue.GetLength() > 0 ? "," : "", sTDataColName, vtVals_AI[n1HCnt].dbVal);
											sValue += sTmp;
										}

										//Update할 데이터가 하나도 없을때 하지 않도록
										if (sValue.GetLength() == 0)
										{
											vtVals_AICnt += 60;
											break;
										}

										//20200220 나정호 수정 tag id -> tag name
										//strQuery.Format("UPDATE %s SET %s WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
										//	, strDBName, sValue, iter->szTAG_Id, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
										if (m_nProduct == 0)
										{
											strQuery.Format("UPDATE %s SET %s WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
												, strDBName, sValue, iter->szTAG_Name, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
										}
										else //20210308 ksw 제품 분기
										{
											strQuery.Format("UPDATE %s SET %s WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
												, strDBName, sValue, iter->szTAG_Id, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
										}
									}
									//Record(Row)가 없을경우 가져온 Historical 값 vtVals_AI 으로 INSERT
									else	//현재 레코드와 불일치 -> 레코드 없음, Historical 데이터 활용 Insert 후 계속 레코드에 맞는 데이터 찾기
									{
										nQueryType = QUERY_TYPE_INSERT;

										sValue = "", sInsertTColNames = "";
										for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
										{
											if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History데이터 끝이면 거기까지만

											if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//없는 값일경우 -3.4028234663852886e+038 로 나온다

											sTmp.Format(",'%.4f'", vtVals_AI[n1HCnt].dbVal);
											sValue += sTmp;

											sTDataColName.Format(",T%02d", n1HCnt - vtVals_AICnt);
											sInsertTColNames += sTDataColName;
										}



										//20200220 나정호 수정 tag id -> tag name
										//20210308 ksw 제품 분기
										//strQuery.Format("INSERT INTO %s(TAG_ID, GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME, T00 ,T01 ,T02 ,T03 ,T04 ,T05 ,T06 ,T07 ,T08 ,T09 ,T10 ,T11 ,T12 ,T13 ,T14 ,T15 ,T16 ,T17 ,T18 ,T19 ,T20 ,T21 ,T22 ,T23 ,T24 ,T25 ,T26 ,T27 ,T28 ,T29 ,T30 ,T31 ,T32 ,T33 ,T34 ,T35 ,T36 ,T37 ,T38 ,T39 ,T40 ,T41 ,T42 ,T43 ,T44 ,T45 ,T46 ,T47 ,T48 ,T49 ,T50 ,T51 ,T52 ,T53 ,T54 ,T55 ,T56 ,T57 ,T58 ,T59 ) "
										if (m_nProduct == 0)
										{
											strQuery.Format("INSERT INTO %s( GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME %s ) VALUES ('%s','%s','%s',%d %s)",
												strDBName, sInsertTColNames, iter->szGroupName, iter->szTAG_Name, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour(), sValue);
										}
										else
										{
											strQuery.Format("INSERT INTO %s(TAG_ID, GROUP_NAME, TAG_NAME, RECORD_DATE, RECORD_TIME %s ) "
												" VALUES ('%s','%s','%s','%s',%d %s)",
												strDBName, sInsertTColNames, iter->szTAG_Id, iter->szGroupName, iter->szTAG_Name, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour(), sValue);
										}
									}

									if (strQuery.GetLength() > 0)
									{
										strRealExcuteQuery += strQuery + "\r\n";
										nExcuteCheck++;

										strQuery = "";
									}

									if (nExcuteCheck > 50 && strRealExcuteQuery.GetLength() > 0)
									{
										int nResult = DB_Connect->SetQueryRun(strRealExcuteQuery);

										nExcuteCheck = 0;
										strRealExcuteQuery = "";

										if (nResult < 1)
										{
#ifdef _DEBUG
											TRACE("MSG(%s) %s Query-Error (%s)(ErrorCode : %d) \n", m_strThreadName, strDBName, strRealExcuteQuery, nResult);
#endif
											nFailCnt++;

											if (nQueryType == QUERY_TYPE_INSERT)
												sTmp.Format("[%s] Insert 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);
											else
												sTmp.Format("[%s] Update 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);

											//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "생성 Error", USER_COLOR_BLACK, sTmp);

											strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
											SetWriteLogFile(strMsg);
											Sleep(50);
										}
									}

									vtVals_AICnt += 59;

									//UPDATE는 레코드를 찾았다는 뜻이니 다음레코드로..
									if (nQueryType == QUERY_TYPE_UPDATE)
									{
										vtVals_AICnt++;
										break;
									}
								}

								if (nExcuteCheck > 0 && strRealExcuteQuery.GetLength() > 0)
								{
									int nResult = DB_Connect->SetQueryRun(strRealExcuteQuery);

									nExcuteCheck = 0;
									strRealExcuteQuery = "";

									if (nResult < 1)
									{
#ifdef _DEBUG
										TRACE("MSG(%s) %s Query-Error (%s)(ErrorCode : %d) \n", m_strThreadName, strDBName, strRealExcuteQuery, nResult);
#endif
										nFailCnt++;

										if (nQueryType == QUERY_TYPE_INSERT)
											sTmp.Format("[%s] Insert 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);
										else
											sTmp.Format("[%s] Update 실패 Log 저장 (ErrorCode : %d)", m_strThreadName, nResult);

										//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "생성 Error", USER_COLOR_BLACK, sTmp);

										strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
										SetWriteLogFile(strMsg);
										Sleep(50);
									}
								}

								//sprintf(szProgress, "%d / %d ( %d%% )", vtVals_AI.size(), vtVals_AI.size(), 100);
								//::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress); 20210308 ksw 주석
								if (!pRs->GetEndOfFile())	pRs->MoveNext();

								if (pRs->GetEndOfFile() && vtVals_AICnt >= vtVals_AI.size())	break;	//레코드와 vtVals_AI(Historical 데이터)가 더이상 없을때
							}
						}

					}
					catch (const _com_error& e)
					{
						CString strMsg;
						strMsg.Format("COM Error: %s", e.ErrorMessage());
						SetWriteLogFile(strMsg);
					}
					catch (const std::exception& e)
					{
						CString strMsg;
						strMsg.Format("Standard Exception: %s", e.what());
						SetWriteLogFile(strMsg);
					}
					catch (...)
					{
						CString strMsg;
						strMsg.Format("Unknown error occurred in Run()");
						SetWriteLogFile(strMsg);
					}
					Sleep(5);
					iter++;
				}
			}

			if (m_bEndThread)
				break;
		}
		catch (...)
		{
			strMsg.Format("Run 스레드 내부 오류");
			SysLogOutPut(m_strLogTitle, strMsg, USER_COLOR_RED);
		}
		if (nFailCnt > 0)
		{
			strMsg.Format("Fail to INSERT/UPDATE (%d) item(s)", nFailCnt);
			_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "생성 Error", USER_COLOR_BLACK, strMsg);
		}
		_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "Calibration 종료", USER_COLOR_BLACK, "Thread_Calibration Finished");
		break;	//요청시 1회만 수행
	}
	::PostMessage(m_pOwner->m_hWnd, WM_ENABLECONTROL, IDC_BUTTON_CALIBRATION, 1);//20210308 ksw 컨트롤 Enable
	PostThreadMessage(WM_QUIT, 0, 0);
	return CWinThread::Run();
}

void CThread_Calibration::SetWriteLogFile(const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath,LOG_FOLDER_NAME_1,szLogMsg);// _szAlarmSettingPath,strLogMsg);
	LeaveCriticalSection(&g_cs);
}

bool CThread_Calibration::GetHistoricalTagValues(LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataDi>& vtVals)
{
	HANDLE hAccess = EV_QueryLog_MI(tStart.GetTime(), tEnd.GetTime(), szTagNm);

	if (hAccess == NULL)
		return false;

	vtVals.clear();

	int nDataCount = 0;

	double* pData = EV_AccessLogData_MI(hAccess, 0, &nDataCount);

	if (nDataCount <= 0)	return true;

	if (pData == NULL)	return false;

	try
	{
		for (size_t i = 0; i < nDataCount; i++)
		{
			CDataDi di;
			di.tLogTime = tStart + CTimeSpan(0, 0, i, 0);
			di.bVal = pData[i] == 0.0f ? false : true;
			vtVals.push_back(di);
		}

		EV_CloseLog_MI(hAccess);

		hAccess = NULL;
	}
	catch (...)
	{
		hAccess = NULL;
		return false;
	}

	return true;
}

bool CThread_Calibration::GetHistoricalTagValues(LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataAi>& vtVals)
{
	HANDLE hAccess = EV_QueryLog_MI(tStart.GetTime(), tEnd.GetTime(), szTagNm);

	if (hAccess == NULL)
		return false;

	vtVals.clear();

	int nDataCount = 0;

	double* pData = EV_AccessLogData_MI(hAccess, 0, &nDataCount);

	if (nDataCount <= 0)
		return true;

	if (pData == NULL)
		return false;

	try
	{
		for (size_t i = 0; i < nDataCount; i++)
		{
			CDataAi ai;
			ai.tLogTime = tStart + CTimeSpan(0, 0, i, 0);
			ai.dbVal = pData[i];
			vtVals.push_back(ai);
		}

		EV_CloseLog_MI(hAccess);

		hAccess = NULL;
	}
	catch (...)
	{
		hAccess = NULL;
		return false;
	}

	return true;
}
