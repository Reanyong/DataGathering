// Thread_TagAlarmMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_TagAlarmMain.h"
#include "FormView_AlarmTag.h"

// CThread_TagAlarmMain

IMPLEMENT_DYNCREATE(CThread_TagAlarmMain, CWinThread)

CThread_TagAlarmMain::CThread_TagAlarmMain()
{
	m_bEndThread = FALSE;
	m_bStartCheck = FALSE;
	m_bStratSubThread = TRUE;
	DB_Connect = NULL;

	memset(m_stDeviceList,0x00,sizeof(m_stDeviceList));

	m_pThread_TagAlarmSub = NULL;

	m_nDeviceCountTemp = 0;

	m_nThteadCount = 0;
}

CThread_TagAlarmMain::~CThread_TagAlarmMain()
{
}

BOOL CThread_TagAlarmMain::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_TagAlarmMain::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.

	StopSubThread();

	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	//Release_DeviceList_ST_Tag(ST_LIST_DELETE);



	CoUninitialize();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_TagAlarmMain, CWinThread)
END_MESSAGE_MAP()


// CThread_TagAlarmMain 메시지 처리기입니다.
void CThread_TagAlarmMain::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "";
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("DB Error Code : [%08lx], Code meaning : [%s], Source : [%s], Description : [%s]",
		e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);

#ifdef _DEBUG
	TRACE("ProcessorName : [%s],Position : [%s][%s]\r\n",m_strThreadName,szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("Position : [%s], LogName: [%s], %s",m_strThreadName,szLogName, strRunlog_E2);
	SetWriteLogFile("Processor-log : [DB Com Error..],",strRunlog_E2Log);
	//_addCurrentstateMsg(0,0, m_strThreadName, "DB 처리 오류 상세로그 확인");
	Sleep(500);
}

void CThread_TagAlarmMain::SetWriteLogFile(const char *sTitle,const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, sTitle, USER_COLOR_PINK, szLogMsg);
	_WriteLogFile(g_stProjectInfo.szProjectLogPath, m_strThreadName, szLogMsg);
	LeaveCriticalSection(&g_cs);
}

int CThread_TagAlarmMain::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//m_strThreadName = "TAG Alarm Main";

	CTime currentTime;// CTime::GetCurrentTime();
	int nMinuteCheck = -1;
	CString strLogMsg = "";
	BOOL bTimeCheck = FALSE;
	BOOL bStartCheck = FALSE;

	ST_DBINFO stDBInfo;
	stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,m_strThreadName,g_stProjectInfo.szProjectLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);

	((CFormView_AlarmTag *)(m_pCtrl))->SetDeviceListRemove();

	ST_SUBTHREAD_INFO stSubTherdInfo[THREAD_MAX];
	memset(&stSubTherdInfo,0x00,sizeof(stSubTherdInfo));
	for(int nI = 0; nI < THREAD_MAX ; nI++)
	{
		stSubTherdInfo[nI].init();
	}


	strLogMsg.Format("주기:[1]분 단위");
	//_addCurrentstateMsg(0,0, m_strThreadName, strLogMsg);
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

		int nDeviceCount = GetSiteCheck(currentTime);

		// <-- 20191230-Doo 예외처리
		if (nDeviceCount < 1)
			nDeviceCount = 0;
		// -->

		//if(nDeviceCount > 0)
		if(m_nDeviceCountTemp != nDeviceCount)
		{
			m_nDeviceCountTemp = nDeviceCount;

			if(m_bStratSubThread == FALSE)
			{
				m_nDeviceCountTemp = nDeviceCount;
				//if(m_nDeviceCountTemp != nDeviceCount)
				{
					StopSubThread();
					m_bStratSubThread = TRUE;
				}
			}

			if(m_bStratSubThread == TRUE)
			{
				for(int nI = 0; nI < nDeviceCount ; nI++)
				{
					 sprintf_s(stSubTherdInfo[nI].szThreadName,"%s",m_stDeviceList[nI].szDevice_Name);
					 stSubTherdInfo[nI].nThreadNumber = nI + 1;
					 memcpy(&stSubTherdInfo[nI].stDeviceInfo,&m_stDeviceList[nI],sizeof(stSubTherdInfo[nI].stDeviceInfo));
				}

				StartSubThread(stSubTherdInfo,nDeviceCount);
			}

			m_bStratSubThread = FALSE;
		}

		if(m_bEndThread == TRUE)
			break;

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);

	return CWinThread::Run();
}



int CThread_TagAlarmMain::GetSiteCheck(CTime currentTime)
{
	CString strMsgTitle = "SiteDevice Check";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strRetSiteid,strRetSiteName,strRetDeviceID,strRetDeviceName,strRetTagTotalNumber;

	strQuery.Format("SELECT cm_Site.SITE_ID "
							",cm_Site.SITE_NAME"
							",hm_Device.DEVICE_ID"
							",hm_Device.DEVICE_NAME"
							", (SELECT count(TAG_NAME) AS cnt FROM HM_TAG_DIC WHERE DEVICE_ID = hm_Device.DEVICE_ID AND ALARM_TAG_YN = 1) as tagcnt "
				" FROM EASY_COMMON.dbo.CM_SITE_INFO cm_Site,HM_DEVICE_CFG hm_Device "
					" WHERE hm_Device.USE_YN = 1 "
						" AND cm_Site.SITE_ID = hm_Device.SITE_ID "
						" AND 0 < (SELECT count(TAG_NAME) AS cnt FROM HM_TAG_DIC WHERE DEVICE_ID = hm_Device.DEVICE_ID AND ALARM_TAG_YN = 1) ");

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				if(m_nDeviceTotal == 0)
				{
					//Release_DeviceList_ST_Tag(ST_LIST_CLEAR);
				}
				for(int nI = 0; nI < nCountRow ; nI++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strRetSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strRetSiteName);
					DB_Connect->GetFieldValue(pRs, "DEVICE_ID", strRetDeviceID);
					DB_Connect->GetFieldValue(pRs, "DEVICE_NAME", strRetDeviceName);
					DB_Connect->GetFieldValue(pRs, "tagcnt", dbValue);

					if(m_bStartCheck == FALSE)
					{
						//처음 시작시 리스트컨트롤에 등록

						strRetTagTotalNumber.Format("%d",(int)dbValue);
						strMsg.Format("[%s][알람 처리 중..]",currentTime.Format("%Y-%m-%d %H:%M:%S"));
						((CFormView_AlarmTag *)(m_pCtrl))->SetStartDeviceList(strRetSiteName,strRetDeviceName,strRetTagTotalNumber,"알람 처리 중..");
						strcpy_s(m_stDeviceList[nI].szSite_Id,strRetSiteid);
						strcpy_s(m_stDeviceList[nI].szSite_Name,strRetSiteName);
						strcpy_s(m_stDeviceList[nI].szDevice_Id,strRetDeviceID);
						strcpy_s(m_stDeviceList[nI].szDevice_Name,strRetDeviceName);
						m_stDeviceList[nI].nTagTotalNumber = (int)dbValue;

					}
					else
					{
						if(m_nDeviceTotal == nCountRow)
						{
							for(int nY = 0; nY < m_nDeviceTotal ; nY++)
							{
								if(!_strcmpi(m_stDeviceList[nY].szDevice_Id,strRetDeviceID))
								{
									if(m_stDeviceList[nY].nTagTotalNumber != (int)dbValue)
									{
										strRetTagTotalNumber.Format("%d",(int)dbValue);
										strMsg.Format("[%s][등록 알람 변경]",currentTime.Format("%Y-%m-%d %H:%M:%S"));
										((CFormView_AlarmTag *)(m_pCtrl))->SetStartDeviceList(strRetSiteName,strRetDeviceName,strRetTagTotalNumber,"등록 알람 변경");
									}
									break;
								}
							}
						}
					}

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

void CThread_TagAlarmMain::StartSubThread(ST_SUBTHREAD_INFO *stSubThreadInfo,int nTotalDevice)
{
	CString strText = "";
/*	m_pThread_TagAlarmSub= new CThread_TagAlarmSub*[THREAD_MAX];
	for(int nTemp = 0; nTemp < THREAD_MAX ; nTemp++)
	{
		if(m_pThread_TagAlarmSub != NULL)
			m_pThread_TagAlarmSub[nTemp] = NULL;
	}*/
	m_pThread_TagAlarmSub= new CThread_TagAlarmSub*[nTotalDevice];
	for(int nTemp = 0; nTemp < nTotalDevice ; nTemp++)
	{
		if(m_pThread_TagAlarmSub != NULL)
			m_pThread_TagAlarmSub[nTemp] = NULL;
	}

	if(m_bEndThread == TRUE)
		return;

	for(int nSubIndex = 0; nSubIndex < nTotalDevice; nSubIndex++)
	{
		if(nTotalDevice != 0)
		{
			m_pThread_TagAlarmSub[nSubIndex] = (CThread_TagAlarmSub *)AfxBeginThread(RUNTIME_CLASS(CThread_TagAlarmSub),
				THREAD_PRIORITY_HIGHEST,
				0,
				CREATE_SUSPENDED);
			m_pThread_TagAlarmSub[nSubIndex]->SetOwner(m_pOwner);
			m_pThread_TagAlarmSub[nSubIndex]->SetWnd(m_WindHwnd);
			m_pThread_TagAlarmSub[nSubIndex]->SetLPVOID(m_pCtrl);

			m_pThread_TagAlarmSub[nSubIndex]->InitInfo(stSubThreadInfo);
			//m_pThread_TagAlarmSub[nSubThreadCount]->SetSTListData(m_pList_ST_TagDivisionList[nSubThreadCount]);
			m_pThread_TagAlarmSub[nSubIndex]->SetSubThreadPause(FALSE);
			m_pThread_TagAlarmSub[nSubIndex]->ResumeThread();

			stSubThreadInfo[nSubIndex].bDataChangeCheck = FALSE;

			strText.Format("ProcessorName : [%s], Tag Count : [%d],상태 : [처리 시작..]",stSubThreadInfo[nSubIndex].szThreadName, stSubThreadInfo[nSubIndex].stDeviceInfo.nTagTotalNumber);
			_addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, "Main Processor-log : [Sub Creat..]", USER_COLOR_PINK, strText);
#ifdef _DEBUG
			TRACE("TagAlarm - %s\n",strText);
#endif
		}
	}//*/
}


void CThread_TagAlarmMain::StopSubThread()
{
	if(m_pThread_TagAlarmSub != NULL)
	{
		int nCnt = 0;
		/*if(m_bButtonStop == TRUE)
		{
			for(nCnt = 0; nCnt < m_nThteadCount ; nCnt++)
			{
				if(m_pThread_TagAlarmSub[nCnt])
					m_pThread_TagAlarmSub[nCnt]->Stop();
			}
		}*/
		for(nCnt = 0; nCnt < m_nDeviceCountTemp ; nCnt++)
		{
			DWORD dwExitCode;
			if(m_pThread_TagAlarmSub[nCnt])
			{
				int nErrorCnt = 0;
				m_pThread_TagAlarmSub[nCnt]->Stop();
				while(1)
				{
					if(GetExitCodeThread(m_pThread_TagAlarmSub[nCnt]->m_hThread, &dwExitCode))
					{
						if(dwExitCode != STILL_ACTIVE)
							break;
					}
					else break;
					Sleep(100);
					if(nErrorCnt++ > TIMEWAIT_FILELOG)
					{
						ASSERT(FALSE);
						return;
					}
				}
				m_pThread_TagAlarmSub[nCnt] = NULL;
			}
		}
		delete [] m_pThread_TagAlarmSub;
		m_pThread_TagAlarmSub = NULL;
	}
}
