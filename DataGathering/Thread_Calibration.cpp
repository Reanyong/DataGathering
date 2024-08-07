// Thread_Calibration.cpp : ���� �����Դϴ�.
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
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
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

// CThread_Calibration �޽��� ó�����Դϴ�.
int CThread_Calibration::Run()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	//HM_TAG_DIC ���̺� ����
	m_nListCnt = g_pList_ST_TagDivisionListCnt;
	m_pList_ST_TagDivisionList = g_pList_ST_TagDivisionList;

	//DB ���� ����
	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);

	m_nDBType = stDBInfo.unDBType;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, "TagMapping ����", g_stProjectInfo.szDTGatheringLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

	//Source DB ���� ����
	CString strDBName = "";
	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
	if (stDBInfo.unDBType == DB_MSSQL)
	{
		if (strlen(stDBName.szHMIDBName) > 1)
			strDBName.Format("%s.dbo.HM_MINUTE_TREND_HISTORY", stDBName.szHMIDBName);
		else
			strDBName.Format("HM_MINUTE_TREND_HISTORY");
	}
	//20200211 jsh : postgres �߰����� ���ǹ� �߰�
	else if (stDBInfo.unDBType == DB_POSTGRE) //20210702 ksw ��ȸ ���̺� ����
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

	//20200225 ����ȣ ���� tag id ����
	//20210308 ksw BEMS EMS ���� ������ �ٽ� �츲
	CString strTagId = "", strTagName = "", strRecordDate = "", strRecordTime = "", strTData = "", strGroupName = "";
	//CString strTagName ="",strRecordDate ="",strRecordTime ="",strTData ="",strGroupName ="";
	CString strQuery, strMsg, sTDataColName, sCur_vtVals_AI_DateTime, sDBDateTime, sValue, sTmp, sInsertTColNames;
	CString strRealExcuteQuery = "";
	int nExcuteCheck = 0;
	int nQueryType = 0, nFailCnt = 0;
	char szProgress[100] = { 0, };

	strMsg.Format("Start Thread_Calibration ( %s ~ ���� )", m_ctSelTime.Format("%Y/%m/%d 00"));
	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "Calibration ����", USER_COLOR_BLACK, strMsg);

	while (!m_bEndThread)
	{
		//DB ���� / �翬��
		if (DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if (bConnectCheck != TRUE)
			{
				Sleep(500);
				continue;
			}
		}

		//CThread_MinGatherMain::GetTagList ���� HM_TAG_DIC ������ �������Ƿ� ���� �ٽ� �������� �ʰ� �״�� Ȱ��
		//Ȥ�� Ÿ�ֻ̹� ���������� �ٽ� �����´�
		if (m_nListCnt == 0)
		{
			m_nListCnt = g_pList_ST_TagDivisionListCnt;
			m_pList_ST_TagDivisionList = g_pList_ST_TagDivisionList;

			Sleep(500);
			continue;
		}

		try
		{
			//�����Ȳ ǥ�ø� ���� ��ü ī��Ʈ
			unsigned int nTotalCnt = 0, nGroupCnt = 0;
			for (int nI = 0; nI < m_nListCnt; nI++)
			{
				nGroupCnt = m_pList_ST_TagDivisionList[nI]->size();
				nTotalCnt += nGroupCnt;
			}
			nGroupCnt = 0;



			//CThread_MinGatherMain::GetTagList ���� HM_TAG_DIC ���̺� �±� �ܾ�ٰ� m_nListCnt ���� ������ ����
			//�Ѹ���� ��ü �±װ� m_nListCnt���� �׷����� �������� ����
			for (int nI = 0; nI < m_nListCnt; nI++)
			{
				std::list<ST_TagInfoList>::iterator iter = m_pList_ST_TagDivisionList[nI]->begin();

				//�� �׷캰�� �����ϰ� �ִ� �±׸�ŭ
				//HM_MINUTE_TREND_HISTORY ���̺� �±� �� �ð����� ������ ������ ����
				while (iter != m_pList_ST_TagDivisionList[nI]->end())
				{
					//�����Ȳ ǥ��
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

					//20200220 ����ȣ ���� tag id -> tag name
					//RECORD_DATE�� string�̰� RECORD_TIME�� int... ����. RECORD_TIME�� �ð��ε� 1�ڸ� �ð��϶� �񱳿� ������ ��.. �񱳿� ������ ���� ��ȯ�ؼ� SELECT
					//strQuery.Format("SELECT * FROM %s WHERE TAG_ID = '%s' AND RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2) >= '%s' ORDER BY RECORD_DATE+RIGHT('00' + CONVERT(NVARCHAR, LTRIM(str(RECORD_TIME))), 2)"
					//	,strDBName, iter->szTAG_Id, m_ctSelTime.Format("%Y%m%d00"));
					if (m_nProduct == 0)
					{
						if (stDBInfo.unDBType == DB_POSTGRE)	//20200219 ����ȣ ���� PostgreSQL ���̺� ������ ����
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
						//�±׺� HM_MINUTE_TREND_HISTORY ���̺� ������

						/*if (stDBInfo.unDBType == DB_POSTGRE)
						const char* szSerVer = DB_Connect->m_stDBInfo.szServer;
						const char* szDBName = m_stDBInfo.szDB;
						const char* szID = m_stDBInfo.szID;
						const char* szPW =m_stDBInfo.szPW;
						UINT unDBTypeID = m_stDBInfo.unDBTypeID;
						DB_Connect->DB_ConnectionInfo(szSerVer,szDBName, szID, szPW,unDBTypeID);
						*/


						if (stDBInfo.unDBType == DB_POSTGRE)	//20200219 ����ȣ ���� PostgreSQL ���̺� ������ ����
						{
							vtVals_AI.clear();
							//GetHistoricalTagValues( iter->szTAG_Name, CTime::GetCurrentTime() - CTimeSpan( 2, 0, 0, 0 ), CTime::GetCurrentTime(), vtVals_AI );
							GetHistoricalTagValues(iter->szTAG_Name, m_ctSelTime, CTime::GetCurrentTime(), vtVals_AI);

							//�ش� �±��� �����ϴ� HM_MINUTE_TREND_HISTORY ���̺� ���ڵ� �ϳ��� ���ͼ� �ش� ���ڵ������ ������ ä��
							//1�ð� ������ �����ϴ� HM_MINUTE_TREND_HISTORY ���̺� Row �ϳ���
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

										//Easyview Historical ������ �̿�, ���� ���ڵ�(pRs) ���� ����
										for (; vtVals_AICnt < vtVals_AI.size(); vtVals_AICnt++)
										{
											sprintf_s(szProgress, "%d / %d ( %d%% )", vtVals_AICnt, vtVals_AI.size(), (vtVals_AICnt * 100) / (vtVals_AI.size()));
											::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress);

											if (vtVals_AICnt % 60 != 0)	continue;	//60�� (1�ð�) Historical ������ ���� = ���ڵ� 1�� (1�ð� ������)

											sCur_vtVals_AI_DateTime = vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d%H"); //�±��� ����Ͻð�
											if (sCur_vtVals_AI_DateTime == sDBDateTime)	//���� ���ڵ�� ��ġ -> Historical ������ Ȱ�� Update �� Break
											{
												nQueryType = QUERY_TYPE_UPDATE;

												sValue = "";
												for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
												{
													if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History������ ���̸� �ű������

													sTDataColName.Format("T%02d", n1HCnt - vtVals_AICnt);
													DB_Connect->GetFieldValue(pRs, sTDataColName, strTData);

													if (strTData.GetLength() > 0)	continue;	//T00~T59 �� ���� �ִ°� �״�εд�
													if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//History�����͵� ������ �״�εд�	////���� ���ϰ�� -3.4028234663852886e+038 �� ���´�
													if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History������ ���̸� �ű������

													sTmp.Format("%s%s='%.4f'", sValue.GetLength() > 0 ? "," : "", sTDataColName, vtVals_AI[n1HCnt].dbVal);
													sValue += sTmp;
												}

												//Update�� �����Ͱ� �ϳ��� ������ ���� �ʵ���
												if (sValue.GetLength() == 0)
												{
													vtVals_AICnt += 60;
													break;
												}
												if (sInsertTColNames != "")
												{

													//20200220 ����ȣ ���� tag id -> tag name
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
											//Record(Row)�� ������� ������ Historical �� vtVals_AI ���� INSERT
											else	//���� ���ڵ�� ����ġ -> ���ڵ� ����, Historical ������ Ȱ�� Insert �� ��� ���ڵ忡 �´� ������ ã��
											{
												nQueryType = QUERY_TYPE_INSERT;

												sValue = "", sInsertTColNames = "";
												for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
												{
													if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History������ ���̸� �ű������

													if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//���� ���ϰ�� -3.4028234663852886e+038 �� ���´�

													sTmp.Format(",'%.4f'", vtVals_AI[n1HCnt].dbVal);
													sValue += sTmp;

													sTDataColName.Format(",T%02d", n1HCnt - vtVals_AICnt);
													sInsertTColNames += sTDataColName;
												}


												if (sInsertTColNames != "")
												{
													//20200220 ����ȣ ���� tag id -> tag name
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
														sTmp.Format("[%s] Insert ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);
													else
														sTmp.Format("[%s] Update ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);

													//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "���� Error", USER_COLOR_BLACK, sTmp);

													strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
													SetWriteLogFile(strMsg);
													Sleep(50);
												}
											}

											vtVals_AICnt += 59;

											//UPDATE�� ���ڵ带 ã�Ҵٴ� ���̴� �������ڵ��..
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
													sTmp.Format("[%s] Insert ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);
												else
													sTmp.Format("[%s] Update ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);

												//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "���� Error", USER_COLOR_BLACK, sTmp);

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

										if(pRs->GetEndOfFile() && vtVals_AICnt >= vtVals_AI.size())	break;	//���ڵ�� vtVals_AI(Historical ������)�� ���̻� ������
										*/

									}
								}
								else
								{
									sDBDateTime = "";	//if( sCur_vtVals_AI_DateTime == sDBDateTime ) ���� Update�� ������ �ʵ���
								}
							}


						}
						else
						{
							pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
							//strMsg.Format("After query (Rec cnt : %d)( %s )", pRs->RecordCount,strQuery);
							//SetWriteLogFile(strMsg);

							//if(pRs == NULL)	continue;

							//�±׺� Easyview Historical ������, ������ ��¥ 0�� ~ ������� ��� ������
							vtVals_AI.clear();
							//GetHistoricalTagValues( iter->szTAG_Name, CTime::GetCurrentTime() - CTimeSpan( 2, 0, 0, 0 ), CTime::GetCurrentTime(), vtVals_AI );
							GetHistoricalTagValues(iter->szTAG_Name, m_ctSelTime, CTime::GetCurrentTime(), vtVals_AI);

							//�ش� �±��� �����ϴ� HM_MINUTE_TREND_HISTORY ���̺� ���ڵ� �ϳ��� ���ͼ� �ش� ���ڵ������ ������ ä��
							//1�ð� ������ �����ϴ� HM_MINUTE_TREND_HISTORY ���̺� Row �ϳ���
							int vtVals_AICnt = 0;
							while (1)
							{
								//DB���� ���̻� ���ڵ尡 ������ vtVals_AI(Historical ������)�� ������ vtVals_AI ��ŭ ��� ����
								if (!pRs->GetEndOfFile() && pRs->RecordCount > 0)
								{
									//20200220 ����ȣ ���� tag id -> tag name
									//20210308 ksw ��ǰ �б�
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
									sDBDateTime = "";	//if( sCur_vtVals_AI_DateTime == sDBDateTime ) ���� Update�� ������ �ʵ���

								//�������� ��� Query ����
								int nExcuteCheck = 0;
								CString strRealExcuteQuery = "";

								//Easyview Historical ������ �̿�, ���� ���ڵ�(pRs) ���� ����
								for (; vtVals_AICnt < vtVals_AI.size(); vtVals_AICnt++)
								{
									//sprintf(szProgress, "%d / %d ( %d%% )", vtVals_AICnt, vtVals_AI.size()-1, (vtVals_AICnt*100)/(vtVals_AI.size()-1));
									//::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress);

									if (vtVals_AICnt % 60 != 0)	continue;	//60�� (1�ð�) Historical ������ ���� = ���ڵ� 1�� (1�ð� ������)

									sCur_vtVals_AI_DateTime = vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d%H");
									if (sCur_vtVals_AI_DateTime == sDBDateTime)	//���� ���ڵ�� ��ġ -> Historical ������ Ȱ�� Update �� Break
									{
										nQueryType = QUERY_TYPE_UPDATE;

										sValue = "";
										for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
										{
											if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History������ ���̸� �ű������

											sTDataColName.Format("T%02d", n1HCnt - vtVals_AICnt);
											DB_Connect->GetFieldValue(pRs, sTDataColName, strTData);

											if (strTData.GetLength() > 0)	continue;	//T00~T59 �� ���� �ִ°� �״�εд�
											if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//History�����͵� ������ �״�εд�	////���� ���ϰ�� -3.4028234663852886e+038 �� ���´�
											if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History������ ���̸� �ű������

											sTmp.Format("%s%s='%.4f'", sValue.GetLength() > 0 ? "," : "", sTDataColName, vtVals_AI[n1HCnt].dbVal);
											sValue += sTmp;
										}

										//Update�� �����Ͱ� �ϳ��� ������ ���� �ʵ���
										if (sValue.GetLength() == 0)
										{
											vtVals_AICnt += 60;
											break;
										}

										//20200220 ����ȣ ���� tag id -> tag name
										//strQuery.Format("UPDATE %s SET %s WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
										//	, strDBName, sValue, iter->szTAG_Id, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
										if (m_nProduct == 0)
										{
											strQuery.Format("UPDATE %s SET %s WHERE TAG_NAME = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
												, strDBName, sValue, iter->szTAG_Name, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
										}
										else //20210308 ksw ��ǰ �б�
										{
											strQuery.Format("UPDATE %s SET %s WHERE TAG_ID = '%s' AND RECORD_DATE = '%s' and RECORD_TIME = %d"
												, strDBName, sValue, iter->szTAG_Id, vtVals_AI[vtVals_AICnt].tLogTime.Format("%Y%m%d"), vtVals_AI[vtVals_AICnt].tLogTime.GetHour());
										}
									}
									//Record(Row)�� ������� ������ Historical �� vtVals_AI ���� INSERT
									else	//���� ���ڵ�� ����ġ -> ���ڵ� ����, Historical ������ Ȱ�� Insert �� ��� ���ڵ忡 �´� ������ ã��
									{
										nQueryType = QUERY_TYPE_INSERT;

										sValue = "", sInsertTColNames = "";
										for (int n1HCnt = vtVals_AICnt; n1HCnt < vtVals_AICnt + 60; n1HCnt++)
										{
											if (vtVals_AICnt + (n1HCnt - vtVals_AICnt) >= vtVals_AI.size())	break;	//History������ ���̸� �ű������

											if (vtVals_AI[n1HCnt].dbVal < -1000000000.0)	continue;	//���� ���ϰ�� -3.4028234663852886e+038 �� ���´�

											sTmp.Format(",'%.4f'", vtVals_AI[n1HCnt].dbVal);
											sValue += sTmp;

											sTDataColName.Format(",T%02d", n1HCnt - vtVals_AICnt);
											sInsertTColNames += sTDataColName;
										}



										//20200220 ����ȣ ���� tag id -> tag name
										//20210308 ksw ��ǰ �б�
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
												sTmp.Format("[%s] Insert ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);
											else
												sTmp.Format("[%s] Update ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);

											//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "���� Error", USER_COLOR_BLACK, sTmp);

											strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
											SetWriteLogFile(strMsg);
											Sleep(50);
										}
									}

									vtVals_AICnt += 59;

									//UPDATE�� ���ڵ带 ã�Ҵٴ� ���̴� �������ڵ��..
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
											sTmp.Format("[%s] Insert ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);
										else
											sTmp.Format("[%s] Update ���� Log ���� (ErrorCode : %d)", m_strThreadName, nResult);

										//_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "���� Error", USER_COLOR_BLACK, sTmp);

										strMsg.Format("(%s):%s-%s", m_strThreadName, sTmp, strRealExcuteQuery);
										SetWriteLogFile(strMsg);
										Sleep(50);
									}
								}

								//sprintf(szProgress, "%d / %d ( %d%% )", vtVals_AI.size(), vtVals_AI.size(), 100);
								//::PostMessage(m_pOwner->m_hWnd, WM_ADDLIST, 1, (LPARAM)szProgress); 20210308 ksw �ּ�
								if (!pRs->GetEndOfFile())	pRs->MoveNext();

								if (pRs->GetEndOfFile() && vtVals_AICnt >= vtVals_AI.size())	break;	//���ڵ�� vtVals_AI(Historical ������)�� ���̻� ������
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
			strMsg.Format("Run ������ ���� ����");
			SysLogOutPut(m_strLogTitle, strMsg, USER_COLOR_RED);
		}
		if (nFailCnt > 0)
		{
			strMsg.Format("Fail to INSERT/UPDATE (%d) item(s)", nFailCnt);
			_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_RED, "���� Error", USER_COLOR_BLACK, strMsg);
		}
		_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, "Calibration ����", USER_COLOR_BLACK, "Thread_Calibration Finished");
		break;	//��û�� 1ȸ�� ����
	}
	::PostMessage(m_pOwner->m_hWnd, WM_ENABLECONTROL, IDC_BUTTON_CALIBRATION, 1);//20210308 ksw ��Ʈ�� Enable
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