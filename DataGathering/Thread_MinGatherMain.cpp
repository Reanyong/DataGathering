
#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_MinGatherMain.h"
#include "FormView_TAGGather.h"

/////////////////////////////////////////////////////////////////////////////
// CThread_MinGatherMain

IMPLEMENT_DYNCREATE(CThread_MinGatherMain, CWinThread)

CThread_MinGatherMain::CThread_MinGatherMain()
{
	m_strLogTitle = "";
	m_bEndThread = FALSE;
	DB_Connect = NULL;

	m_pThread_Delete = NULL;
	DeleteThreadCount = 0;

	m_nTagListCount = 0;
	m_pList_ST_TagDivisionList = NULL;
	m_pThread_MinGahterSub = NULL;

	m_bButtonStop = FALSE;
	m_nThteadCount = 0;

	g_pList_ST_TagDivisionListCnt = 0;
	g_pList_ST_TagDivisionList = NULL;
}

CThread_MinGatherMain::~CThread_MinGatherMain()
{

}

BOOL CThread_MinGatherMain::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_MinGatherMain::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	//_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath,"메모리누수Log","MainTread ExitInstance");
	if (m_bButtonStop == TRUE)
	{
		_addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, USER_COLOR_BLUE, "Message : [사용자 정지]");
	}
	StopSubThread();		// SubThread 삭제
	StopDeleteThread();		// DeleteThread 삭제
#ifdef _DEBUG
	TRACE("Stop - 메인스레드 sub 스레드  종료 \n");
#endif

	if (DB_Connect != NULL)
	{
		if (DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}
	if (m_bButtonStop == TRUE)
	{
		_addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, USER_COLOR_BLUE, "Message : [ADO 접속 해제 및 소멸]");
	}
#ifdef _DEBUG
	TRACE("ADO 접속 해제 및 소멸 \n");
#endif
	CoUninitialize();

	Release_List_ST_DB_Tag(ST_LIST_RELEASE);
#ifdef _DEBUG
	TRACE("Stop - 리스트 삭제 \n");
#endif

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_MinGatherMain, CWinThread)
	//{{AFX_MSG_MAP(CThread_MinGatherMain)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString CThread_MinGatherMain::Com_Error(const char* szLogName, _com_error* e)
{
	CString strRunlog_E2 = "", strRunlog_E2Log = "", strErrorID = "", strErrorCode;
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("Position : [%s],Description : [%s], DB Error Code : [%08lx], Code meaning : [%s], Source : [%s]",
		szLogName, (LPCTSTR)bstrDescription, e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource);

	if (m_nDBType == DB_ORACLE)
	{
		strErrorID.Format("%s", (LPCTSTR)bstrDescription);
		strErrorCode = strErrorID.Mid(4, 5);//Left(nPos);
	}
	else if (m_nDBType == DB_MSSQL)
	{
		strErrorCode.Format("%08lx", e->Error());
	}
	else if (m_nDBType == DB_POSTGRE)
	{
		strErrorCode.Format("%08lx", e->Error());
	}

#ifdef _DEBUG
	TRACE("ProcessorName : [%s],Position : [%s][%s]\r\n", m_strLogTitle, szLogName, strRunlog_E2);
#endif

	strRunlog_E2Log.Format("[%s] Position : [DB Com Error..], LogName: [%s], %s", m_strLogTitle, szLogName, strRunlog_E2);

	SetWriteLogFile(strRunlog_E2Log);
	SysLogOutPut(m_strLogTitle, strRunlog_E2, USER_COLOR_RED);
	Sleep(500);

	return strErrorCode;
}

/////////////////////////////////////////////////////////////////////////////
// CThread_MinGatherMain message handlers

int CThread_MinGatherMain::Run()
{
	// TODO: Add your specialized code here and/or call the base class
	int nErrorCheck = 0;
	int nTimeTemp = -1;

	int lastCheckedDay = -1;
	int targetDeleteDay = 1;  // 매월 1일에 삭제 수행

	CTime currentTime;

	BOOL bSubThreadStartCheck = TRUE;

	CString strSysLogMsg = "";
	CString strDeviceItem = "";
	m_strLogTitle = "Main Thread";

	m_pList_ST_TagDivisionList = new std::list<ST_TagInfoList>*[THREAD_CREADTE_MAX];
	Release_List_ST_DB_Tag(ST_LIST_INITIAL);

	ST_GATHERINFO_TYPE stGatherInfo;
	stGatherInfo = _getGatherInfoType(g_stProjectInfo.szProjectIniPath);

	if (stGatherInfo.nGatherType != 1)
	{
		strDeviceItem.Format("WHERE DEVICE_ID = '");
		for (int nIndex = 0; nIndex < stGatherInfo.nCount; nIndex++)
		{
			strDeviceItem += stGatherInfo.stGatherUseList[nIndex].szDeviceId;

			if (nIndex != (stGatherInfo.nCount - 1) && stGatherInfo.nCount != 1)
				strDeviceItem += "' or DEVICE_ID = '";
			else
				strDeviceItem += "' AND ";
		}
	}
	else
		strDeviceItem.Format("WHERE ");


	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	ST_DATABASENAME  stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	m_nDBType = stDBInfo.unDBType;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

	StopSubThread();

	ST_SUBTHREAD_INFO stSubTherdInfo[THREAD_CREADTE_MAX];
	memset(&stSubTherdInfo, 0x00, sizeof(stSubTherdInfo));
	for (int nI = 0; nI < THREAD_CREADTE_MAX; nI++)
	{
		stSubTherdInfo[nI].init();
	}

	((CFormView_TAGGather*)(m_pCtrl))->ShowText_State("수집중....");

	do
	{
		if (DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if (bConnectCheck != TRUE)
			{
				((CFormView_TAGGather*)(m_pCtrl))->ShowText_State("접속 오류 : 10초후 재접속시도..");
				((CFormView_TAGGather*)(m_pCtrl))->ShowText_Count("-");
				((CFormView_TAGGather*)(m_pCtrl))->ShowText_Processor("-");
				Sleep(10000);
				continue;
			}
		}

		if (nErrorCheck == ERROR_DB_QUERY_FAIL1)
		{
			((CFormView_TAGGather*)(m_pCtrl))->ShowText_State("SELECT 쿼리 오류");
			((CFormView_TAGGather*)(m_pCtrl))->ShowText_Count("-");
			((CFormView_TAGGather*)(m_pCtrl))->ShowText_Processor("-");
			Sleep(1000);
			continue;
		}

		try
		{
			if (m_bEndThread)
				break;

			currentTime = CTime::GetCurrentTime();

			// 매월 1일인지 확인 (하루에 한 번만 체크하도록)
			if (currentTime.GetDay() == targetDeleteDay && lastCheckedDay != targetDeleteDay) {
				lastCheckedDay = targetDeleteDay;

				// Delete 스레드가 실행 중인지 확인
				if (m_pThread_Delete != NULL) {
					// 이미 생성되어 있다면 삭제 요청
					m_pThread_Delete->RequestDeleteData();
					SysLogOutPut(m_strLogTitle, _T("매월 정기 데이터 삭제 요청됨"), LOG_COLOR_BLUE);
				}
				else {
					// Delete 스레드 생성 및 시작
					StartDeleteThread();
					if (m_pThread_Delete != NULL) {
						m_pThread_Delete->RequestDeleteData();
						SysLogOutPut(m_strLogTitle, _T("매월 정기 데이터 삭제 요청됨"), LOG_COLOR_BLUE);
					}
				}
			}
			else if (currentTime.GetDay() != targetDeleteDay) {
				// 1일이 아닌 경우 다시 체크할 수 있도록 설정
				lastCheckedDay = -1;
			}



			if ((currentTime.GetMinute() % 1) != 0 || nTimeTemp == currentTime.GetMinute()) // 20210831 ksw
			{
				Sleep(500);
				continue;
			}
			nTimeTemp = currentTime.GetMinute();

			int nTagToalCount = GetTagTotalCountCheck(strDeviceItem, stDBInfo.unDBType, stDBName.szHMIDBName);//GetSecTagList();

			if (nTagToalCount < 0)
			{
				Sleep(500);
				continue;
			}
			if (nTagToalCount != m_nTagListCount)
			{
				int nTheradCount = 1;

				if (nTagToalCount >= 0 && nTagToalCount <= THREAD_LEVEL_1_LIMIT)
					nTheradCount = THREAD_LEVEL_1;
				else if ((nTagToalCount >= (THREAD_LEVEL_1_LIMIT + 1)) && (nTagToalCount <= THREAD_LEVEL_2_LIMIT))
					nTheradCount = THREAD_LEVEL_2;
				else if ((nTagToalCount >= (THREAD_LEVEL_2_LIMIT + 1)) && (nTagToalCount <= THREAD_LEVEL_3_LIMIT))
					nTheradCount = THREAD_LEVEL_3;
				else if ((nTagToalCount >= (THREAD_LEVEL_3_LIMIT + 1)) && (nTagToalCount <= THREAD_LEVEL_4_LIMIT))
					nTheradCount = THREAD_LEVEL_4;
				else if ((nTagToalCount >= (THREAD_LEVEL_4_LIMIT + 1)) && (nTagToalCount <= THREAD_LEVEL_5_LIMIT))
					nTheradCount = THREAD_LEVEL_5;
				else if (nTagToalCount > THREAD_LEVEL_5_LIMIT)
					nTheradCount = THREAD_LEVEL_6;

				//nTheradCount = 1;
				m_nThteadCount = nTheradCount;

				if (bSubThreadStartCheck == FALSE)
				{
					((CFormView_TAGGather*)(m_pCtrl))->ShowText_State("TAG 변경 재수집");
					((CFormView_TAGGather*)(m_pCtrl))->ShowText_Count("-");
					((CFormView_TAGGather*)(m_pCtrl))->ShowText_Processor("-");
					StopSubThread();
					bSubThreadStartCheck = TRUE;
				}

				char szTempBuffer[24];
				memset(szTempBuffer, 0x00, sizeof(szTempBuffer));
				((CFormView_TAGGather*)(m_pCtrl))->ShowText_State("수집중...");
				sprintf_s(szTempBuffer, "%d", nTagToalCount);
				((CFormView_TAGGather*)(m_pCtrl))->ShowText_Count(szTempBuffer);
				sprintf_s(szTempBuffer, "%d", nTheradCount);
				((CFormView_TAGGather*)(m_pCtrl))->ShowText_Processor("-");

				nErrorCheck = GetTagList(strDeviceItem, nTheradCount, stDBInfo.unDBType, stDBName.szHMIDBName);
				if (nErrorCheck == ERROR_DB_COM_ERROR)
					continue;
				else if (nErrorCheck == THREAD_END)
					break;
				else if (nErrorCheck > 0)
					g_nCheckTagThreadRun = 1; // 20210301 ksw 캘리브레이션 버튼 활성화 플래그

				m_nTagListCount = nTagToalCount;

				strSysLogMsg.Format("TAG Total Count : [%d]", m_nTagListCount);
				SysLogOutPut(m_strLogTitle, strSysLogMsg, LOG_COLOR_BLUE);

				CString strCount = "";
				CString strProcessorName = "";
				int nTagCount = 0;
				((CFormView_TAGGather*)(m_pCtrl))->ListInitialization();

				for (int nI = 0; nI < nTheradCount; nI++)
				{
					nTagCount = m_pList_ST_TagDivisionList[nI]->size();
					//if(stSubTherdInfo[nI].nThreadDataCount != nTagCount) //이거 필요없는거 같음..
					{
						if (m_bEndThread == TRUE)
							break;

						strCount.Format("%d", nTagCount);
						strProcessorName.Format("Processor_%02d", nI + 1);
						((CFormView_TAGGather*)(m_pCtrl))->ShowListData(strProcessorName, "시작중", strCount, "리스트 수집 중..");

						stSubTherdInfo[nI].bDataChangeCheck = TRUE;
						if (stSubTherdInfo[nI].nThreadNumber == -1)
							stSubTherdInfo[nI].nThreadNumber = nI + 1;
						if (stSubTherdInfo[nI].nThreadDataCount == -1)
							stSubTherdInfo[nI].nThreadDataCount = nTagCount;

						stSubTherdInfo[nI].nProduct = m_nProduct; //20210308 ksw
						stSubTherdInfo[nI].nInterval = m_nInterval; //20210831 ksw

						sprintf_s(stSubTherdInfo[nI].szThreadName, "%s", strProcessorName);
#ifdef _DEBUG
						TRACE("%s - %d\n", strProcessorName, nTagCount);
#endif
						strSysLogMsg.Format("%s : [%d]", strProcessorName, nTagCount);
						SysLogOutPut(m_strLogTitle, strSysLogMsg, LOG_COLOR_BLUE);

						sprintf_s(szTempBuffer, "%d", nI + 1);
						((CFormView_TAGGather*)(m_pCtrl))->ShowText_Processor(szTempBuffer);
					}
				}
				if (m_bEndThread == TRUE)
					break;

				if (bSubThreadStartCheck == TRUE)
					StartSubThread(stSubTherdInfo, nTheradCount);
				else
				{
#ifdef _DEBUG
					TRACE("Main Thead 초단위TAG 항목 변경 %d \n", nTagToalCount);
#endif
					SetReSTListItem();
				}

				bSubThreadStartCheck = FALSE;
			}

			if (m_bEndThread == TRUE)
				break;
		}
		catch (...)
		{
#ifdef _DEBUG
			TRACE("메인 스레드 catch ....\n");
#endif
		}

#ifdef _DEBUG
		TRACE("Delete Thread Check...\n");
#endif

		/*
		if (DeleteThreadCount == 0 || DeleteThreadCount == 5)
		{
			StartDeleteThread();
			if (DeleteThreadCount == 5)
				DeleteThreadCount = 0;
		}
		DeleteThreadCount++;
		*/

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);
	return CWinThread::Run();
}

void CThread_MinGatherMain::SetReSTListItem()
{
	int nStListCnt = 0;
	/*for(int nThreadCnt = 0; nThreadCnt < THREAD_MAX ; nThreadCnt++)
	{
	if(m_pThread_SecData[nThreadCnt] != NULL)
	{
	nStListCnt = m_pList_ST_DBSecTag[nThreadCnt]->size();
	if(nStListCnt != 0)
	{
	m_pThread_SecData[nThreadCnt]->NewStListInfo(m_pList_ST_DBSecTag[nThreadCnt],TRUE);
	}
	#ifdef DEBUG
	TRACE("Thread-%d 기동 Item Count %d \n",nThreadCnt,nStListCnt);
	#endif
	}
	}*/
}

void CThread_MinGatherMain::StartSubThread(ST_SUBTHREAD_INFO* stSubTheradInfo, int nTheradCount)
{
	/*	m_pThread_MinGahterSub = new CThread_MinGatherSub*[THREAD_MAX];
	for(int nTemp = 0; nTemp < THREAD_MAX ; nTemp++)
	{
	if(m_pThread_MinGahterSub != NULL)
	m_pThread_MinGahterSub[nTemp] = NULL;
	}*/
	m_pThread_MinGahterSub = new CThread_MinGatherSub * [nTheradCount];
	for (int nTemp = 0; nTemp < nTheradCount; nTemp++)
	{
		//if(m_pThread_MinGahterSub != NULL)
		if (m_pThread_MinGahterSub[nTemp] != NULL)
			m_pThread_MinGahterSub[nTemp] = NULL;
	}


	for (int nSubThreadCount = 0; nSubThreadCount < nTheradCount; nSubThreadCount++)
		//for(int nSubThreadCount = 0; nSubThreadCount < 1; nSubThreadCount++)
	{
		int nSTListCnt = m_pList_ST_TagDivisionList[nSubThreadCount]->size();

		if (nSTListCnt != 0)
		{
			m_pThread_MinGahterSub[nSubThreadCount] = (CThread_MinGatherSub*)AfxBeginThread(RUNTIME_CLASS(CThread_MinGatherSub),
				THREAD_PRIORITY_HIGHEST,
				0,
				CREATE_SUSPENDED);
			m_pThread_MinGahterSub[nSubThreadCount]->SetOwner(m_pOwner);
			m_pThread_MinGahterSub[nSubThreadCount]->SetWnd(m_WindHwnd);
			m_pThread_MinGahterSub[nSubThreadCount]->SetLPVOID(m_pCtrl);

			m_pThread_MinGahterSub[nSubThreadCount]->InitInfo(stSubTheradInfo[nSubThreadCount].nThreadNumber, stSubTheradInfo[nSubThreadCount].szThreadName, stSubTheradInfo[nSubThreadCount].nProduct, stSubTheradInfo[nSubThreadCount].nInterval);

			CString s;
			int nlistCnt = m_pList_ST_TagDivisionList[nSubThreadCount]->size();

			m_pThread_MinGahterSub[nSubThreadCount]->SetSTListData(m_pList_ST_TagDivisionList[nSubThreadCount]);
			m_pThread_MinGahterSub[nSubThreadCount]->SetSubThreadPause(FALSE);

			m_pThread_MinGahterSub[nSubThreadCount]->ResumeThread();

			stSubTheradInfo[nSubThreadCount].bDataChangeCheck = FALSE;
#ifdef DEBUG
			TRACE("%s 기동\n", stSubTheradInfo[nSubThreadCount].szThreadName, nSubThreadCount);
#endif
		}
	}//*/
}

void CThread_MinGatherMain::StopSubThread()
{
	if (m_pThread_MinGahterSub != NULL)
	{
		int nCnt = 0;
		if (m_bButtonStop == TRUE)
		{
			for (nCnt = 0; nCnt < m_nThteadCount; nCnt++)
			{
				if (m_pThread_MinGahterSub[nCnt])
					m_pThread_MinGahterSub[nCnt]->ButtonStop();
			}
		}

		/*for(nCnt = 0; nCnt < THREAD_MAX ; nCnt++)
		{
		if(m_pThread_MinGahterSub[nCnt])
		m_pThread_MinGahterSub[nCnt]->Stop();
		}*/

		for (nCnt = 0; nCnt < m_nThteadCount; nCnt++)
		{
			DWORD dwExitCode;
			if (m_pThread_MinGahterSub[nCnt])
			{
				int nErrorCnt = 0;
				m_pThread_MinGahterSub[nCnt]->Stop();
				while (1)
				{
					if (GetExitCodeThread(m_pThread_MinGahterSub[nCnt]->m_hThread, &dwExitCode))
					{
						if (dwExitCode != STILL_ACTIVE)
							break;
					}
					else
					{
						//m_pThread_MinGahterSub[nCnt] = NULL;
						break;
					}
					Sleep(100);
					if (nErrorCnt++ > TIMEWAIT_FILELOG)
					{
						ASSERT(FALSE);
						return;
					}
				}
			}
		}

		delete[] m_pThread_MinGahterSub;
		//m_pThread_MinGahterSub = NULL;

	}
}

void CThread_MinGatherMain::Release_List_ST_DB_Tag(int nMode)
{
	for (int nI = 0; nI < THREAD_CREADTE_MAX; nI++)
	{
		if (nMode == ST_LIST_INITIAL)
		{
			m_pList_ST_TagDivisionList[nI] = NULL;
			m_pList_ST_TagDivisionList[nI] = new std::list<ST_TagInfoList>;
		}

		if (nMode == ST_LIST_CLEAR)
		{
			if (m_pList_ST_TagDivisionList[nI] != NULL)
			{
				m_pList_ST_TagDivisionList[nI]->clear();
			}
		}

		if (nMode == ST_LIST_RELEASE)
		{
			if (m_pList_ST_TagDivisionList[nI] != NULL)
			{
				int nSize = m_pList_ST_TagDivisionList[nI]->size();
				if (nSize <= 0)
				{
					delete m_pList_ST_TagDivisionList[nI];
					m_pList_ST_TagDivisionList[nI] = NULL;
				}
				else
				{
					m_pList_ST_TagDivisionList[nI]->clear();

					delete m_pList_ST_TagDivisionList[nI];
					m_pList_ST_TagDivisionList[nI] = NULL;
				}
			}
		}
	}
	if (nMode == ST_LIST_RELEASE) // ksw 20210316 이중배열 list 동적할당 해제
	{
		delete[]  m_pList_ST_TagDivisionList;
		m_pList_ST_TagDivisionList = NULL;
	}
}


int CThread_MinGatherMain::GetTagList(const char* szDeviceItem, int nTheradCount, int nDBType, const char* szDBName)
{
	CString strMSGTitle = "GetTagList";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "", strRunlog_E2 = "";
	CString strTagId = "", strTagName = "", strTagType = "", strGroupName = "", strDBName = "";
	int nCountRow = 0;

	if (nDBType == DB_MSSQL)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.HM_TAG_DIC", szDBName);
		else
			strDBName.Format("HM_TAG_DIC");
	}
	//2020-02-12 jsh : postgre 추가
	else if (nDBType == DB_POSTGRE)
	{
		if (strlen(szDBName) > 1) //20210702 ksw 수정
			strDBName.Format("%s.HM_TAG_DIC", szDBName);
		else
			strDBName.Format("easy_hmi.HM_TAG_DIC");
	}
	else
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_TAG_DIC", szDBName);
		else
			strDBName.Format("HM_TAG_DIC");
	}

	//20200220 나정호 수정 tag id -> tag name
	//20200225 나정호 수정 group_name 제거
	//strQuery.Format("SELECT TAG_ID,TAG_NAME,GROUP_NAME,TAG_TYPE FROM %s %s AND GATHER_USE_YN = 0 and TAG_ID = '강원도청사 스마트그리드.스마트그리드DB연동.AI.PCS_AC_ACTIVE_POWER'",strDBName,szDeviceItem);
	//strQuery.Format("SELECT TAG_NAME,GROUP_NAME,TAG_TYPE FROM %s %s GATHER_USE_YN = 0 or GATHER_USE_YN IS NULL ",strDBName,szDeviceItem);


	//strQuery.Format("SELECT TAG_NAME,TAG_TYPE FROM %s %s GATHER_USE_YN = 0 or GATHER_USE_YN IS NULL ",strDBName,szDeviceItem);
	if (nDBType == DB_POSTGRE)  //20210302 ksw POSTGRE 분기
	{//DB_POSTGRE
		strQuery.Format("SELECT TAG_NAME,TAG_TYPE FROM %s %s GATHER_USE_YN = 0 or GATHER_USE_YN IS NULL ", strDBName, szDeviceItem);
		//20200220 나정호 수정 PostgreSQL odbc 부분 select 추가
		try
		{
			Release_List_ST_DB_Tag(ST_LIST_CLEAR);

			//pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);

			SQLRETURN retcode;
			retcode = DB_Connect->SetQueryRun(strQuery);
			if (retcode < 0)	//20210309 ksw Select 문 예외처리
				throw retcode;
			SQLINTEGER sqlRqwCount;

			DB_Connect->codbc->SQLRowCount(&sqlRqwCount);

			nCountRow = sqlRqwCount;

			int nDivideCount = 0;
			//nDivideCount = nCountRow / THREAD_MAX;
			nDivideCount = nCountRow / nTheradCount;

			//if(pRs != NULL)
			if (isSqlOk(retcode))
			{
				int nSelecRowCnt = 0;
				int nTempCount = 0;
				//for(int nSTListCnt = 0; nSTListCnt < THREAD_MAX ; nSTListCnt++)
				for (int nSTListCnt = 0; nSTListCnt < nTheradCount; nSTListCnt++)
				{
					if (nSTListCnt == 0)
						nTempCount = nDivideCount;
					else if (nSTListCnt == 1)
						nTempCount = nDivideCount * (nSTListCnt + 1);
					else if (nSTListCnt > 1)
						nTempCount = nDivideCount * (nSTListCnt + 1);
					while (1)
					{
						int cnt = 0;
						CString t;
						//DB_Connect->codbc->DeleteNonFixedRows();
						retcode = DB_Connect->codbc->COdbc::SQLFetch();
						if (isSqlOk(retcode))
						{
							/*20200219 나정호 수정중*/
							//SQLINTEGER cbTagId=0;
							SQLINTEGER cbTagName = 0;
							SQLINTEGER cbTagTypeTemp = 0;
							//SQLINTEGER cbDbName=0;
							//SQLCHAR szTagId[128] = {0,};
							SQLCHAR szTagName[64] = { 0, };
							//SQLCHAR szTagType[64] = {0,};
							SQLSMALLINT nTagTypeTemp = 0;
							//SQLCHAR szDbName[64] = {0,};
							void* Temp;


							//DB_Connect->codbc->COdbc::SQLGetData( 1,SQL_C_CHAR, szTagId , sizeof(szTagId) ,&cbTagId);
							DB_Connect->codbc->COdbc::SQLGetData(1, SQL_C_CHAR, szTagName, sizeof(szTagName), &cbTagName);
							DB_Connect->codbc->COdbc::SQLGetData(2, SQL_C_SSHORT, &nTagTypeTemp, sizeof(nTagTypeTemp), &cbTagTypeTemp);
							//DB_Connect->codbc->COdbc::SQLGetData( 3,SQL_C_CHAR, szDbName , sizeof(szDbName) ,&cbDbName);


							ST_TagInfoList stNewTagInfo;
							memset(&stNewTagInfo, 0x00, sizeof(ST_TagInfoList));

							Temp = szTagName;
							strcpy_s(stNewTagInfo.szTAG_Name, (const char*)Temp);

							/*
							Temp=szDbName;
							strcpy_s(stNewTagInfo.szGroupName,(const char*)Temp);
							*/

							stNewTagInfo.nTagType = nTagTypeTemp;

							m_pList_ST_TagDivisionList[nSTListCnt]->push_back(stNewTagInfo);

#ifdef _DEBUG
							TRACE("STListCnt = %d,TagCnt= %d, Value = %s,%s,%s,%d\n", nSTListCnt, nSelecRowCnt, strTagId, strTagName, strGroupName, atoi(strTagType));
#endif
							if (nSelecRowCnt + 1 >= nTempCount)
							{
								if ((nSTListCnt + 1) != nTheradCount)
								{
									nSelecRowCnt++;
									break;
								}
							}

							nSelecRowCnt++;
							if ((nSelecRowCnt) >= nCountRow)
								break;

							//dbValue = atoi(szData);

							//DB_Connect->GetFieldValue(pRs, "cnt", dbValue);

						}
						else
						{
							break;
						}
					}
					g_pList_ST_TagDivisionListCnt = nTheradCount;
					g_pList_ST_TagDivisionList = m_pList_ST_TagDivisionList;
				}
			}
			else
			{
				retcode = NULL;
				pRs->Close();
				pRs = NULL;
			}
			return nCountRow;
		}
		catch (_com_error& e)
		{
			CString strErrorCode = Com_Error(strMSGTitle, &e);

			if (strErrorCode == "942")
			{
				strRunlog_E2.Format("%s", strMSGTitle);
				SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
#endif
				return ERROR_DB_NO_TABLE;
			}
			else if (strErrorCode == "3113" || strErrorCode == "80004005")
			{
				int nResult = DB_Connect->DB_ReConnection();
				if (nResult == 0)
				{
					strRunlog_E2.Format("%s - DB 접속 실패!", strMSGTitle);
					SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
#endif
				}
				return ERROR_DB_RECONNECTION;
			}
			else
				return ERROR_DB_COM_ERROR;

		}
		catch (SQLRETURN rt) //20210305 ksw  njh 로직 예외처리
		{
			if (rt != SQL_SUCCESS_WITH_INFO || rt != SQL_SUCCESS) {
				strRunlog_E2.Format("SELECT 실패 Event Error : %s", strMSGTitle);
				SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
			}

#ifdef _DEBUG
			TRACE("GetTagList()/catch - %s\n", strRunlog_E2);
#endif
			return ERROR_DB_QUERY_FAIL1;
		}
		/*catch (...)
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
		}*/
		return nCountRow;
	}
	else if (nDBType == DB_MSSQL) //20210302 ksw MSSQL 분기
	{
		if (m_nProduct == 0) //20210310 ksw SELECT문 분기
			strQuery.Format("SELECT TAG_NAME,TAG_TYPE FROM %s %s GATHER_USE_YN = 0 or GATHER_USE_YN IS NULL ", strDBName, szDeviceItem);
		else
			strQuery.Format("SELECT TAG_ID,TAG_NAME,GROUP_NAME,TAG_TYPE FROM %s %s GATHER_USE_YN = 0 or GATHER_USE_YN IS NULL ", strDBName, szDeviceItem);
		try
		{
			Release_List_ST_DB_Tag(ST_LIST_CLEAR);

			pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
			nCountRow = pRs->RecordCount;

			int nDivideCount = 0;
			//nDivideCount = nCountRow / THREAD_MAX;
			nDivideCount = nCountRow / nTheradCount;

			if (pRs != NULL)
			{
				if (!pRs->GetEndOfFile())
				{
					int nSelecRowCnt = 0;
					int nTempCount = 0;
					//for(int nSTListCnt = 0; nSTListCnt < THREAD_MAX ; nSTListCnt++)
					for (int nSTListCnt = 0; nSTListCnt < nTheradCount; nSTListCnt++)
					{
						if (nSTListCnt == 0)
							nTempCount = nDivideCount;
						else if (nSTListCnt == 1)
							nTempCount = nDivideCount * (nSTListCnt + 1);
						else if (nSTListCnt > 1)
							nTempCount = nDivideCount * (nSTListCnt + 1);

						while (1)
						{
							if (m_nProduct != 0) //20210310 ksw 분기 추가
							{
								DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);
								DB_Connect->GetFieldValue(pRs, "GROUP_NAME", strGroupName);
							}
							DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);
							DB_Connect->GetFieldValue(pRs, "TAG_TYPE", strTagType);

							ST_TagInfoList stNewTagInfo;
							memset(&stNewTagInfo, 0x00, sizeof(ST_TagInfoList));

							if (m_nProduct != 0) //20210310 ksw 분기 추가
							{
								strcpy_s(stNewTagInfo.szTAG_Id, strTagId);
								strcpy_s(stNewTagInfo.szGroupName, strGroupName);
							}
							strcpy_s(stNewTagInfo.szTAG_Name, strTagName);

							stNewTagInfo.nTagType = atoi(strTagType);
							m_pList_ST_TagDivisionList[nSTListCnt]->push_back(stNewTagInfo);
#ifdef _DEBUG
							TRACE("STListCnt = %d,TagCnt= %d, Value = %s,%s,%s,%d\n", nSTListCnt, nSelecRowCnt, strTagId, strTagName, strGroupName, atoi(strTagType));
#endif
							if (nSelecRowCnt + 1 >= nTempCount)
							{
								if ((nSTListCnt + 1) != nTheradCount)
								{
									nSelecRowCnt++;
									pRs->MoveNext();
									break;
								}
							}

							nSelecRowCnt++;
							if ((nSelecRowCnt) >= nCountRow)
								break;

							pRs->MoveNext();
						}


						g_pList_ST_TagDivisionListCnt = nTheradCount;
						g_pList_ST_TagDivisionList = m_pList_ST_TagDivisionList;
					}
				}
				if (pRs != NULL)
				{
					pRs->Close();
					pRs = NULL;
				}
				return nCountRow;
			}
		}
		catch (_com_error& e)
		{
			CString strErrorCode = Com_Error(strMSGTitle, &e);

			if (strErrorCode == "942")
			{
				strRunlog_E2.Format("%s", strMSGTitle);
				SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
#endif
				return ERROR_DB_NO_TABLE;
			}
			else if (strErrorCode == "3113" || strErrorCode == "80004005")
			{
				int nResult = DB_Connect->DB_ReConnection();
				if (nResult == 0)
				{
					strRunlog_E2.Format("%s - DB 접속 실패!", strMSGTitle);
					SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
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
			strRunlog_E2.Format("SELECT 실패 Event Error : %s", strMSGTitle);
			SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);

#ifdef _DEBUG
			TRACE("GetTagList()/catch - %s\n", strRunlog_E2);
#endif
			return ERROR_DB_QUERY_FAIL1;
		}
		return nCountRow;
	}
}

int CThread_MinGatherMain::GetTagTotalCountCheck(const char* szDeviceItem, int nDBType, const char* szDBName)
{
	CString strMSGTitle = "GetTagTotalCountCheck";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "", strRunlog_E2 = "", strDBName = "";
	int nCountRow = 0;

	if (nDBType == DB_MSSQL)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.dbo.HM_TAG_DIC", szDBName);
		else
			strDBName.Format("HM_TAG_DIC");
	}
	//2020-02-12 jsh : postgre 추가
	else if (nDBType == DB_POSTGRE)
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_TAG_DIC", szDBName);
		else
			strDBName.Format("Easy_Hmi.HM_TAG_DIC");
	}
	else
	{
		if (strlen(szDBName) > 1)
			strDBName.Format("%s.HM_TAG_DIC", szDBName);
		else
			strDBName.Format("HM_TAG_DIC");
	}


	//20200219 나정호 PostgreSQL odbc select 부분 추가
	//	strQuery.Format("SELECT COUNT(TAG_NAME) as cnt FROM %s %s AND GATHER_USE_YN = 0 AND sTAG_ID = '강원도청사 스마트그리드.스마트그리드DB연동.AI.PCS_AC_ACTIVE_POWER'",strDBName,szDeviceItem);
	strQuery.Format("SELECT COUNT(TAG_NAME) as cnt FROM %s %s (GATHER_USE_YN = 0 or GATHER_USE_YN  is null)", strDBName, szDeviceItem);
	if (nDBType == DB_POSTGRE)
	{
		try
		{
			SQLRETURN retcode;
			retcode = DB_Connect->SetQueryRun(strQuery);

			//SQLHSTMT  hstmt = NULL;
			/*
			if ((retcode == SQL_INVALID_HANDLE) || (retcode == SQL_SUCCESS_WITH_INFO) || (retcode == SQL_ERROR))
			{
			DB_Connect->codbc->Close();
			DB_Connect->codbc->~COdbc();
			}*/

			if (isSqlOk(retcode))
			{
				int cnt = 0;
				CString t;
				//DB_Connect->codbc->DeleteNonFixedRows();
				retcode = DB_Connect->codbc->COdbc::SQLFetch();
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
				pRs->Close();
				pRs = NULL;
			}
			return nCountRow;
		}
		catch (_com_error& e)
		{
			CString strErrorCode = Com_Error(strMSGTitle, &e);

			if (strErrorCode == "942")
			{
				strRunlog_E2.Format("%s", strMSGTitle);
				SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
#endif
				return ERROR_DB_NO_TABLE;
			}
			else if (strErrorCode == "3113" || strErrorCode == "80004005")
			{
				int nResult = DB_Connect->DB_ReConnection();
				if (nResult == 0)
				{
					strRunlog_E2.Format("%s - DB 접속 실패!", strMSGTitle);
					SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
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

			strRunlog_E2.Format("쿼리 실패 Event Error : %s", strQuery);
			SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
			return ERROR_DB_QUERY_FAIL1;
		}



	}
	else
	{
		try
		{
			pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);

			if (pRs != NULL)
			{
				if (!pRs->GetEndOfFile())
				{
					double dbValue;
					DB_Connect->GetFieldValue(pRs, "cnt", dbValue);
					nCountRow = (int)dbValue;
				}
				// 레코드가 없을경우 DB 쿼리 문제 발생입니다.
				if (pRs != NULL)
				{
					pRs->Close();
					pRs = NULL;
				}

				return nCountRow;
			}
		}
		catch (_com_error& e)
		{
			CString strErrorCode = Com_Error(strMSGTitle, &e);

			if (strErrorCode == "942")
			{
				strRunlog_E2.Format("%s", strMSGTitle);
				SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
#endif
				return ERROR_DB_NO_TABLE;
			}
			else if (strErrorCode == "3113" || strErrorCode == "80004005")
			{
				int nResult = DB_Connect->DB_ReConnection();
				if (nResult == 0)
				{
					strRunlog_E2.Format("%s - DB 접속 실패!", strMSGTitle);
					SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
#ifdef _DEBUG
					TRACE("GetTagList()/catch com error - %s\n", strRunlog_E2);
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

			strRunlog_E2.Format("쿼리 실패 Event Error : %s", strQuery);
			SysLogOutPut(m_strLogTitle, strRunlog_E2, LOG_COLOR_RED);
			return ERROR_DB_QUERY_FAIL1;
		}
	}
	return 0;
}

void CThread_MinGatherMain::SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody)
{
	_addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, crBody, strMsg);
}

void CThread_MinGatherMain::SetWriteLogFile(const char* szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, LOG_FOLDER_NAME_1, szLogMsg);// _szAlarmSettingPath,strLogMsg);
	LeaveCriticalSection(&g_cs);
}

void CThread_MinGatherMain::StartDeleteThread()
{
	if (m_pThread_Delete == NULL)
	{
		m_pThread_Delete = (Thread_Delete*)AfxBeginThread(RUNTIME_CLASS(Thread_Delete),
			THREAD_PRIORITY_NORMAL,
			0,
			CREATE_SUSPENDED);
		m_pThread_Delete->m_WindHwnd = m_WindHwnd;
		m_pThread_Delete->ResumeThread();
	}
}

void CThread_MinGatherMain::StopDeleteThread()
{
	if (m_pThread_Delete != NULL)
	{
		m_pThread_Delete->m_bEndThread = TRUE;
		WaitForSingleObject(m_pThread_Delete->m_hThread, INFINITE);
		//delete m_pThread_Delete;
		m_pThread_Delete = NULL;
	}
}