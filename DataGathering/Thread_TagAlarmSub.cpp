// Thread_TagAlarmSub.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_TagAlarmSub.h"
#include "FormView_AlarmTag.h"


// CThread_TagAlarmSub

IMPLEMENT_DYNCREATE(CThread_TagAlarmSub, CWinThread)

CThread_TagAlarmSub::CThread_TagAlarmSub()
{
	m_bEndThread = FALSE;
	DB_Connect = NULL;

	m_pstTagAlarmList = NULL;
	m_pstTagAlarmList = new std::list<ST_TagAlarmList>;
	m_pstUMSSend_UserList = NULL;
	m_pstUMSSend_UserList = new std::list<ST_UMSSend_UserList>;
	m_strProcessorTitle = "";
	m_bButtonStop = FALSE;
}

CThread_TagAlarmSub::~CThread_TagAlarmSub()
{
}

BOOL CThread_TagAlarmSub::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_TagAlarmSub::ExitInstance()
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

BEGIN_MESSAGE_MAP(CThread_TagAlarmSub, CWinThread)
END_MESSAGE_MAP()



// CThread_TagAlarmSub 메시지 처리기입니다.

void CThread_TagAlarmSub::Release_ST_List(int nMode)
{
	if(m_pstTagAlarmList != NULL)
	{
		m_pstTagAlarmList->clear();

		if(nMode == ST_LIST_DELETE)
		{
			delete m_pstTagAlarmList;
			m_pstTagAlarmList =NULL;
		}
	}
}

void CThread_TagAlarmSub::Release_ST_UMS_List(int nMode)
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

void CThread_TagAlarmSub::Com_Error(const char *szLogName,_com_error *e)
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
	SetWriteLogFile(": [DB Com Error..],",strRunlog_E2Log);
	Sleep(20);
}

void CThread_TagAlarmSub::SetWriteLogFile(const char *sTitle,const char *szLogMsg)
{
	CString strTitle = "";
	EnterCriticalSection(&g_cs);
	strTitle.Format("%s %s",m_strProcessorTitle,sTitle);
	_addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, strTitle, USER_COLOR_PINK, szLogMsg);
	_WriteLogFile(g_stProjectInfo.szProjectLogPath,m_strThreadName,szLogMsg);		
	LeaveCriticalSection(&g_cs);
}

void CThread_TagAlarmSub::ShowAlarmOccursMsg(const char *szData1,const char *szData2,const char *szData3,const char *szData4,const char *szData5)
{
	EnterCriticalSection(&g_cs);
	((CFormView_AlarmTag *)(m_pCtrl))->SetAlarmOccursMsg(szData1,szData2,szData3,szData4,szData5);
	LeaveCriticalSection(&g_cs);
}

int CThread_TagAlarmSub::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CTime currentTime;// CTime::GetCurrentTime();
	int nMinuteCheck = -1;
	CString strLogMsg = "";
	BOOL bTimeCheck = FALSE;
	BOOL bStartCheck = FALSE;

	ST_DBINFO stDBInfo;
	stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);

	m_strProcessorTitle.Format("%s-Sub log",m_strThreadName);

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,m_strThreadName,g_stProjectInfo.szProjectLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);
	
	strLogMsg.Format("ProcessorName : [%s], Tag Count : [%d],상태 : [처리 시작..]",m_stSubInfo.szThreadName,m_stSubInfo.stDeviceInfo.nTagTotalNumber);
	_addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, m_strProcessorTitle, USER_COLOR_PINK, "시작");

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
		Sleep(20);

		int nRet = 0;
		nRet = GetAlarmRegisterListAI_DB(m_stSubInfo.stDeviceInfo.szDevice_Id);
		if(nRet == THREAD_END)
			break;

		if(nRet > 0)
		{
			GetAlarmCheck_AI(m_stSubInfo.stDeviceInfo.szDevice_Id,currentTime);
		}
		if(m_bEndThread == TRUE)
			break;

		/*nRet = GetAlarmRegisterListDI_DB(m_stSubInfo.stDeviceInfo.szDevice_Id);
		if(nRet == THREAD_END)
			break;
		if(nRet > 0)
		{
			GetAlarmCheck_DI(m_stSubInfo.stDeviceInfo.szDevice_Id,currentTime);
		}*/

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);

	return CWinThread::Run();
}

int CThread_TagAlarmSub::GetAlarmRegisterListAI_DB(const char *szDeviceId)
{
	CString strMsgTitle = "AlarmRegisterList AI";
	CString strQuery = "" ,strTempBuffer = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strAlarmId,strTagId,strAlarmLevelid,strAlarmMsg,strTagName,strTagType;
	int nAlarmKind,nAlarmType,nTagType;
	float fCurrentValue,fHihi,fHigh,fLow,fLolo;
	BOOL bHihiCheck = FALSE,bHighCheck = FALSE,bLowCheck = FALSE,bLoloCheck = FALSE;

	//20200220 나정호 수정 tag id -> tag name
	strQuery.Format("SELECT alarmList.ALARM_ID "
		",alarmList.TAG_ID "
		",alarmList.ALARM_KIND "
		",alarmList.ALARM_TYPE "
		",alarmList.ALARM_LEVEL_ID "
		",curValue.CURRENT_TAG_VALUE "
		",alarmList.HIHI "
		",CASE  "
		"	WHEN alarmList.HIHI < curValue.CURRENT_TAG_VALUE THEN '1' "
		"	WHEN alarmList.HIHI > curValue.CURRENT_TAG_VALUE THEN '0' "
		"END  as Hihi_Result "
		",alarmList.HIGH "
		",CASE  "
		"	WHEN (alarmList.HIHI > curValue.CURRENT_TAG_VALUE) and (curValue.CURRENT_TAG_VALUE > alarmList.HIGH) THEN '1' "
		"	WHEN alarmList.HIGH > curValue.CURRENT_TAG_VALUE THEN '0' "
		"END as High_Result "
		",alarmList.LOW "
		",CASE  "
		"	WHEN (alarmList.LOLO < curValue.CURRENT_TAG_VALUE) and (curValue.CURRENT_TAG_VALUE < alarmList.LOW) THEN '1' "
		"	WHEN alarmList.LOW < curValue.CURRENT_TAG_VALUE THEN '0' "
		"END as Low_Result "
		",alarmList.LOLO "
		",CASE  "
		"	WHEN alarmList.LOLO > curValue.CURRENT_TAG_VALUE THEN '1' "
		"	WHEN alarmList.LOLO < curValue.CURRENT_TAG_VALUE THEN '0' "
		"END as Lolo_Result "
		",alarmList.ALARM_MSG "
		",alarmList.REGISTER_DATE "
		",alarmList.REGISTER_ID "
		",alarmList.MODIFY_DATE "
		",alarmList.MODIFY_ID "
		",tagDic.TAG_NAME "
		",tagDic.TAG_TYPE "
		"	FROM HM_ALARM_LIST alarmList ,HM_TAG_DIC tagDic, HM_VALUE_CHANGE_HISTORY curValue "
		"where alarmList.TAG_ID = tagDic.TAG_NAME "
		"AND curValue.TAG_ID = tagDic.TAG_NAME "
		"AND tagDic.TAG_TYPE = 3 "
		"AND tagDic.ALARM_TAG_YN = 1 "
		//"AND alarmList.ALARM_ID = 'ALARM_cb9bf838-6e09-4579-8170-47105be0d5a7'");
		"AND tagDic.DEVICE_ID = '%s' ",szDeviceId);
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		Release_ST_List(ST_LIST_CLEAR);

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				for(int nI = 0; nI < nCountRow ; nI++)
				{
					//20200220 나정호 수정 tag id -> tag name
					DB_Connect->GetFieldValue(pRs, "ALARM_ID", strAlarmId);
					DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);
					DB_Connect->GetFieldValue(pRs, "ALARM_KIND", dbValue);
					nAlarmKind  = (int)dbValue;
					DB_Connect->GetFieldValue(pRs, "ALARM_TYPE", dbValue);
					nAlarmType  = (int)dbValue;
					DB_Connect->GetFieldValue(pRs, "ALARM_LEVEL_ID", strAlarmLevelid);

					DB_Connect->GetFieldValue(pRs, "CURRENT_TAG_VALUE", strTempBuffer);
					fCurrentValue = (float)atof(strTempBuffer);

					DB_Connect->GetFieldValue(pRs, "HIHI", dbValue);
					fHihi = (float)dbValue;
					DB_Connect->GetFieldValue(pRs, "HIGH", dbValue);
					fHigh = (float)dbValue;
					DB_Connect->GetFieldValue(pRs, "LOW", dbValue); 
					fLow = (float)dbValue;
					DB_Connect->GetFieldValue(pRs, "LOW", dbValue);
					fLolo = (float)dbValue;

					DB_Connect->GetFieldValue(pRs, "Hihi_Result", strTempBuffer);
					bHihiCheck = (BOOL)atoi(strTempBuffer);
					DB_Connect->GetFieldValue(pRs, "High_Result", strTempBuffer);
					bHighCheck = (BOOL)atoi(strTempBuffer);
					DB_Connect->GetFieldValue(pRs, "Low_Result", strTempBuffer);
					bLowCheck = (BOOL)atoi(strTempBuffer);
					DB_Connect->GetFieldValue(pRs, "Lolo_Result", strTempBuffer);
					bLoloCheck = (BOOL)atoi(strTempBuffer);

					DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);
					DB_Connect->GetFieldValue(pRs, "TAG_TYPE", dbValue);
					nTagType = (int)dbValue;

					if(m_bEndThread == TRUE)
						return THREAD_END;

					ST_TagAlarmList stTagAlarm;
					memset(&stTagAlarm ,0x00, sizeof(stTagAlarm));
					strcpy_s(stTagAlarm.szAlarmId,strAlarmId);
					strcpy_s(stTagAlarm.szTagId,strTagId);
					stTagAlarm.nAlarmKind = nAlarmKind;
					stTagAlarm.nAlarmType = nAlarmType;

					stTagAlarm.fCurrentValue = fCurrentValue;
					stTagAlarm.fHIHIVal = fHihi;
					stTagAlarm.fHIGHVal = fHigh;
					stTagAlarm.fLOWVal = fLow;
					stTagAlarm.fLOLOVal = fLolo;
					stTagAlarm.bHihi_Result = bHihiCheck;
					stTagAlarm.bHigh_Result = bHighCheck;
					stTagAlarm.bLow_Result = bLowCheck;
					stTagAlarm.bLolo_Result = bLoloCheck;

					strcpy_s(stTagAlarm.szTagName,strTagName);
					stTagAlarm.nTagType = nTagType;

					ST_ALARMCHECK stAlarmCheck;
					memset(&stAlarmCheck,0x00,sizeof(stAlarmCheck));
					BOOL bAlarmCheck = GetAlarmOccursAI_File(szDeviceId,strAlarmId,&stAlarmCheck);

					if(bAlarmCheck == TRUE)
					{
						strcpy_s(stTagAlarm.szStartTime,stAlarmCheck.szAlarmStartTime);
						stTagAlarm.fAckValue = stAlarmCheck.fAckValue;
						stTagAlarm.nOldAlarmAckType = stAlarmCheck.nOldAlarmAckType;
						stTagAlarm.nAlarmSt = 1;
					}
					else
						stTagAlarm.nAlarmSt = 0;

					m_pstTagAlarmList->push_back(stTagAlarm);

					pRs->MoveNext();			
					Sleep(20);
					if(m_bEndThread == TRUE)
						return THREAD_END;
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
		Com_Error(strMsgTitle,&e);

		if(0x80004005 == e.Error())
		{
			strRunlog_E2.Format("Position : [%s], log : [DB 접속 접속 시도..]",strMsgTitle);
			SetWriteLogFile(": [_com_error..],",strRunlog_E2);

			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB 접속 실패!]",strMsgTitle);
				SetWriteLogFile(": [_com_error..],",strRunlog_E2);

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
		SetWriteLogFile(": [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}
	return 0;
}

int CThread_TagAlarmSub::GetAlarmRegisterListDI_DB(const char *szDeviceId)
{
	CString strMsgTitle = "AlarmRegisterList DI";
	CString strQuery = "" ,strTempBuffer = "";
	CString strRunlog_E2 ="",strMsg = "";
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strAlarmId,strTagId,strAlarmLevelid,strAlarmMsg,strTagName,strTagType,strAlarmTypeTxt;
	int nAlarmKind,nAlarmType,nTagType;
	float fCurrentValue;
	BOOL bHihiCheck = FALSE,bHighCheck = FALSE,bLowCheck = FALSE,bLoloCheck = FALSE;
	
	//20200220 나정호 수정 tag id -> tag name
	strQuery.Format("SELECT alarmList.ALARM_ID "
							",alarmList.TAG_ID "
							",alarmList.ALARM_KIND "
							",alarmList.ALARM_TYPE "
							",alarmList.ALARM_LEVEL_ID "
							",curValue.CURRENT_TAG_VALUE "
							",curValue.PREVIOUS_TAG_VALUE "
							",CASE  "
							"	WHEN alarmList.ALARM_TYPE = 1 THEN 'OFF->ON' "
							"	WHEN alarmList.ALARM_TYPE = 2 THEN 'ON->OFF' "
							"	WHEN alarmList.ALARM_TYPE = 3 THEN 'OFF<->ON' "
							"END  as AlarmTypeTxt "
							",alarmList.ALARM_MSG "
							",tagDic.TAG_NAME "
							",tagDic.TAG_TYPE "
				"	FROM HM_ALARM_LIST alarmList ,HM_TAG_DIC tagDic, HM_VALUE_CHANGE_HISTORY curValue "
					"where alarmList.TAG_NAME = tagDic.TAG_NAME "
					"AND curValue.TAG_NAME = tagDic.TAG_NAME "
					"AND tagDic.TAG_TYPE = 1 "
					"AND tagDic.ALARM_TAG_YN = 1 "
					//"AND alarmList.ALARM_ID = 'ALARM_cb9bf838-6e09-4579-8170-47105be0d5a7'");
					"AND tagDic.DEVICE_ID = '%s' ",szDeviceId);
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		Release_ST_List(ST_LIST_CLEAR);

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				for(int nI = 0; nI < nCountRow ; nI++)
				{
					//20200220 나정호 수정 tag id -> tag name
					DB_Connect->GetFieldValue(pRs, "ALARM_ID", strAlarmId);
					DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);
					DB_Connect->GetFieldValue(pRs, "ALARM_KIND", dbValue);
					nAlarmKind  = (int)dbValue;
					DB_Connect->GetFieldValue(pRs, "ALARM_TYPE", dbValue);
					nAlarmType  = (int)dbValue;
					DB_Connect->GetFieldValue(pRs, "ALARM_LEVEL_ID", strAlarmLevelid);

					DB_Connect->GetFieldValue(pRs, "CURRENT_TAG_VALUE", strTempBuffer);
					fCurrentValue = (float)atof(strTempBuffer);

					DB_Connect->GetFieldValue(pRs, "AlarmTypeTxt", strAlarmTypeTxt);
					 					
					DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);
					DB_Connect->GetFieldValue(pRs, "TAG_TYPE", dbValue);
					nTagType = (int)dbValue;

					if(m_bEndThread == TRUE)
						return THREAD_END;

					ST_TagAlarmList stTagAlarm;
					memset(&stTagAlarm ,0x00, sizeof(stTagAlarm));
					strcpy_s(stTagAlarm.szAlarmId,strAlarmId);
					strcpy_s(stTagAlarm.szTagId,strTagId);
					stTagAlarm.nAlarmKind = nAlarmKind;
					stTagAlarm.nAlarmType = nAlarmType;
					strcpy_s(stTagAlarm.szAlarmLevel,strAlarmLevelid);

					stTagAlarm.fCurrentValue = fCurrentValue;
					strcpy_s(stTagAlarm.szAlarmTypeTxt,strAlarmTypeTxt);
					//stTagAlarm.fLastValue = fLastValue;

					strcpy_s(stTagAlarm.szTagName,strTagName);
					stTagAlarm.nTagType = nTagType;
	
					ST_ALARMCHECK stAlarmCheck;
					memset(&stAlarmCheck,0x00,sizeof(stAlarmCheck));
					BOOL bAlarmCheck = GetAlarmOccursDI_File(szDeviceId,strAlarmId,&stAlarmCheck);
										
					if(bAlarmCheck == TRUE)
					{
						strcpy_s(stTagAlarm.szStartTime,stAlarmCheck.szAlarmStartTime);
						stTagAlarm.fAckValue = stAlarmCheck.fAckValue;
						stTagAlarm.nOldAlarmAckType = stAlarmCheck.nOldAlarmAckType;
						stTagAlarm.nAlarmSt = 1;
					}
					else
						stTagAlarm.nAlarmSt = 0;

					m_pstTagAlarmList->push_back(stTagAlarm);

					pRs->MoveNext();			
					Sleep(20);
					if(m_bEndThread == TRUE)
						return THREAD_END;
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
		Com_Error(strMsgTitle,&e);

		if(0x80004005 == e.Error())
		{
			strRunlog_E2.Format("Position : [%s], log : [DB 접속 접속 시도..]",strMsgTitle);
			SetWriteLogFile(": [_com_error..],",strRunlog_E2);

			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB 접속 실패!]",strMsgTitle);
				SetWriteLogFile(": [_com_error..],",strRunlog_E2);

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
		SetWriteLogFile(": [catch error..],",strRunlog_E2);

		return ERROR_DB_COM_ERROR;
	}
	return 0;
}

 BOOL CThread_TagAlarmSub::GetAlarmOccursAI_File(const char *szDeviceId,const char *szAlarmId,ST_ALARMCHECK *stAlarmCheck)
 {
	 char szBuff[64];
	 memset(szBuff,0x00,sizeof(szBuff));
	 CString strAlarmSettingPath = "";
	 CString strTotalBuff = "",strTempBuff = "";
	 strAlarmSettingPath.Format("%s\\Alarm\\Tag\\%s_AI.ini",g_stProjectInfo.szDTGatheringPath,szDeviceId);
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

 BOOL CThread_TagAlarmSub::GetAlarmOccursDI_File(const char *szDeviceId,const char *szAlarmId,ST_ALARMCHECK *stAlarmCheck)
 {
	 char szBuff[64];
	 memset(szBuff,0x00,sizeof(szBuff));
	 CString strAlarmSettingPath = "";
	 CString strTotalBuff = "",strTempBuff = "";
	 strAlarmSettingPath.Format("%s\\Alarm\\Tag\\%s_DI.ini",g_stProjectInfo.szDTGatheringPath,szDeviceId);
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

 BOOL CThread_TagAlarmSub::SetAlarmOccursAI_File(const char *szDeviceId,const char *szAlarmId, int nAlarmType ,const char *szStartTime, float fAckValue)
 {
	 CString strAlarmSettingPath = "";
	 CString strTempBuff = "";
	 strAlarmSettingPath.Format("%s\\Alarm\\Tag\\%s_AI.ini",g_stProjectInfo.szDTGatheringPath,szDeviceId);

	 strTempBuff.Format("%s,%d,%f",szStartTime,nAlarmType,fAckValue);
	 WritePrivateProfileString("AckValue", szAlarmId, strTempBuff,strAlarmSettingPath);
	 return TRUE;
 }

 BOOL CThread_TagAlarmSub::SetAlarmOccursDI_File(const char *szDeviceId,const char *szAlarmId, int nAlarmType ,const char *szStartTime, float fAckValue)
 {
	 CString strAlarmSettingPath = "";
	 CString strTempBuff = "";
	 strAlarmSettingPath.Format("%s\\Alarm\\Tag\\%s_DI.ini",g_stProjectInfo.szDTGatheringPath,szDeviceId);

	 strTempBuff.Format("%s,%d,%f",szStartTime,nAlarmType,fAckValue);
	 WritePrivateProfileString("AckValue", szAlarmId, strTempBuff,strAlarmSettingPath);
	 return TRUE;
 }


 BOOL CThread_TagAlarmSub::SetAlarmOccursAI_ClearFile(const char *szDeviceId,const char *szAlarmId)
 {
	 char szBuff[26];
	 memset(szBuff,0x00,sizeof(szBuff));
	 CString strTotalBuff = "",strTempBuff = "";
	 CString strAlarmSettingPath = "";
	 strAlarmSettingPath.Format("%s\\Alarm\\Tag\\%s_AI.ini",g_stProjectInfo.szDTGatheringPath,szDeviceId);

	 WritePrivateProfileString("AckValue", szAlarmId, NULL,strAlarmSettingPath);
	 return TRUE;
 }

 BOOL CThread_TagAlarmSub::SetAlarmOccursDI_ClearFile(const char *szDeviceId,const char *szAlarmId)
 {
	 char szBuff[26];
	 memset(szBuff,0x00,sizeof(szBuff));
	 CString strTotalBuff = "",strTempBuff = "";
	 CString strAlarmSettingPath = "";
	 strAlarmSettingPath.Format("%s\\Alarm\\Tag\\%s_DI.ini",g_stProjectInfo.szDTGatheringPath,szDeviceId);

	 WritePrivateProfileString("AckValue", szAlarmId, NULL,strAlarmSettingPath);
	 return TRUE;
 }

 int CThread_TagAlarmSub::GetAlarmCheck_AI(const char *szDeviceId,CTime currentTime)
 {
	 CString strQuery = "";
	 CString strAlarmKind = "";
	 CString strAlarmMsg = "";
	 CString strAlarmKindType = "";
	 CString strLogMsg = "";
	 int nAlarmType = 0;
	 float fAlarmScale = 0;
	 
	 std::list<ST_TagAlarmList>::iterator iter = m_pstTagAlarmList->begin();
	 while(iter != m_pstTagAlarmList->end())
	 {	
		 if(m_bEndThread == TRUE)
			 return THREAD_END;

		 BOOL bAlarmConditionCheck = FALSE;
		 if(iter->bHihi_Result == TRUE)
		 {
			 nAlarmType = 4;
			 strAlarmKindType = "HIHI";
			 fAlarmScale = iter->fHIHIVal;
			 bAlarmConditionCheck = TRUE;
		 }
		 if(iter->bHigh_Result == TRUE) 
		 {
			 nAlarmType = 5;
			 strAlarmKindType = "HIGH";
			 fAlarmScale = iter->fHIGHVal;
			 bAlarmConditionCheck = TRUE;
		 }
		 if(iter->bLow_Result == TRUE) 
		 {
			 nAlarmType = 6;
			 strAlarmKindType = "LOW";
			 fAlarmScale = iter->fLOWVal;
			 bAlarmConditionCheck = TRUE;
		 }
		 if(iter->bLolo_Result == TRUE) 
		 {
			 nAlarmType = 7;
			 strAlarmKindType = "LOLO";
			 fAlarmScale = iter->fLOLOVal;
			 bAlarmConditionCheck = TRUE;
		 }

		 if(bAlarmConditionCheck == TRUE)
		 {
			 switch(iter->nAlarmKind)
			 {
			 case 0:
				 strAlarmKind = "경보";
				 break;
			 case 1:
				 strAlarmKind = "장애";
				 break;
			 default:
				 strAlarmKind = "시설물 교체 시기";
				 break;
			 }
			 
			 if(m_bEndThread == TRUE)
				 return THREAD_END;

			 if(iter->nAlarmSt == 0)
			 {
				 strAlarmMsg.Format("알람 종류 : [%s][%s(%0.2f)], 발생값 : [%f]",strAlarmKind,strAlarmKindType,fAlarmScale,iter->fCurrentValue);
				 strQuery.Format("INSERT INTO EASY_COMMON.dbo.CM_ALARM_HISTORY "
					 "(ALARM_ID,START_TIME,ALARM_OCCURRENCE_INFO,STATION_NAME,ALARM_CHECK_USE_YN,ALARM_KIND,ALARM_TYPE,ALARM_VALUE,ALARM_MESSAGE)"
					 " VALUES "
					 " ('%s','%s',1,'%s',0,%d,%d,%f,'%s')",iter->szAlarmId,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,iter->nAlarmKind,nAlarmType,iter->fCurrentValue,strAlarmMsg);

					 SetAlarmOccursAI_File(szDeviceId,iter->szAlarmId,nAlarmType,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->fCurrentValue);
					 SetQueryValue(strQuery,"TagAlarm Insert",iter->szTagName);
					 
					 strAlarmMsg.Format("알람:[발생],태그명:[%s],알람 종류:[%s][%s(%0.2f)],발생값:[%f]",iter->szTagName,strAlarmKind,strAlarmKindType,fAlarmScale,iter->fCurrentValue);
					 _addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, m_strProcessorTitle, USER_COLOR_PINK, strAlarmMsg);

					 ShowAlarmOccursMsg("알람발생",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);

					 int nRetLen = strlen(iter->szAlarmLevel);
					 if(nRetLen != 0)
					 {
						 GetAlarmUMSCheck(iter->szAlarmLevel);
						 std::list<ST_UMSSend_UserList>::iterator iter_UMS = m_pstUMSSend_UserList->begin();
						 while(iter_UMS != m_pstUMSSend_UserList->end())
						 {
							 if(m_bEndThread == TRUE)
								 return THREAD_END;

							 SetSUMSendOutput(*iter,*iter_UMS,strAlarmKindType);
							 iter_UMS++;
							 Sleep(100);
						 }
					 }

					 
					// _addCurrentstateMsg(1,0, m_strThreadName, strAlarmMsg);
			 }
			 else
			 {
				 if(m_bEndThread == TRUE)
					 return THREAD_END;

				 int nRet = GetAlarmOccurs_DB(iter->szAlarmId,iter->szStartTime); //1:현재 있음,0:없음

				 if(iter->nOldAlarmAckType != nAlarmType)
				 {
					 if(nRet == 1)
					 {
						 strQuery.Format(" UPDATE EASY_COMMON.dbo.CM_ALARM_HISTORY SET ALARM_OCCURRENCE_INFO = 2, END_TIME = '%s' "
							 " WHERE ALARM_ID = '%s' AND START_TIME='%s'",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szAlarmId,iter->szStartTime);

						 SetAlarmOccursAI_ClearFile(szDeviceId,iter->szAlarmId);

						 SetQueryValue(strQuery,"TagAlarm Update",iter->szTagName);

						 strAlarmMsg.Format("알람:[자동 해제],태그명:[%s]",iter->szTagName);
						 _addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, m_strProcessorTitle, USER_COLOR_PINK, strAlarmMsg);
						 ShowAlarmOccursMsg("자동해제",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);

						 strAlarmMsg.Format("알람 종류 : [%s][%s(%0.2f)], 발생값 : [%f]",strAlarmKind,strAlarmKindType,fAlarmScale,iter->fCurrentValue);
						 strQuery.Format("INSERT INTO EASY_COMMON.dbo.CM_ALARM_HISTORY "
							 "(ALARM_ID,START_TIME,ALARM_OCCURRENCE_INFO,STATION_NAME,ALARM_CHECK_USE_YN,ALARM_KIND,ALARM_TYPE,ALARM_VALUE,ALARM_MESSAGE)"
							 " VALUES "
							 " ('%s','%s',1,'%s',0,%d,%d,%f,'%s')",iter->szAlarmId,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,iter->nAlarmKind,nAlarmType,iter->fCurrentValue,strAlarmMsg);

						 SetAlarmOccursAI_File(szDeviceId,iter->szAlarmId,nAlarmType,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->fCurrentValue);
						 strAlarmMsg.Format("알람:[발생],태그명:[%s],알람 종류:[%s][%s(%0.2f)],발생값:[%f]",iter->szTagName,strAlarmKind,strAlarmKindType,fAlarmScale,iter->fCurrentValue);
						 _addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, m_strProcessorTitle, USER_COLOR_PINK, strAlarmMsg);

						 SetQueryValue(strQuery,"TagAlarm Insert",iter->szTagName);
						 ShowAlarmOccursMsg("알람발생",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);

						 int nRetLen = strlen(iter->szAlarmLevel);
						 if(nRetLen != 0)
						 {
							 GetAlarmUMSCheck(iter->szAlarmLevel);
							 std::list<ST_UMSSend_UserList>::iterator iter_UMS = m_pstUMSSend_UserList->begin();
							 while(iter_UMS != m_pstUMSSend_UserList->end())
							 {
								 if(m_bEndThread == TRUE)
									 return THREAD_END;

								 SetSUMSendOutput(*iter,*iter_UMS,strAlarmKindType);
								 iter_UMS++;
								 Sleep(100);
							 }
						 }

						 //_addCurrentstateMsg(1,0, m_strThreadName, strAlarmMsg);
					 }
				 }
				 /*else
				 {
					// if(nRet == 0)
					// {
					//	 SetAlarmOccurs_Clear(iter->szAlarmId);
					//	 ShowAlarmOccursMsg("자동해제",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);
					// }
					// _addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, "Sub Processor-log : [Sub..]", USER_COLOR_PINK, "여기 들어간다 ");
				 }*/
			 }
			 if(m_bEndThread == TRUE)
				 return THREAD_END;
		 }
		 else
		 {
			 if(iter->nAlarmSt == 1)
			 {
				 int nRet = GetAlarmOccurs_DB(iter->szAlarmId,iter->szStartTime); //1:현재 있음,0:없음

				 if(nRet == 1)
				 {
					 if(iter->nAlarmSt == 1) //알람 해제
					 {
						 strQuery.Format(" UPDATE EASY_COMMON.dbo.CM_ALARM_HISTORY SET ALARM_OCCURRENCE_INFO = 2, END_TIME = '%s' "
							 " WHERE ALARM_ID = '%s' AND START_TIME='%s'",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szAlarmId,iter->szStartTime);

						 SetAlarmOccursAI_ClearFile(szDeviceId,iter->szAlarmId);
						 
						 SetQueryValue(strQuery,"TagAlarm Update",iter->szTagName);
						 strAlarmMsg.Format("알람:[자동 해제],태그명:[%s]",iter->szTagName);
						 ShowAlarmOccursMsg("자동해제",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);

						 //_addCurrentstateMsg(1,0, m_strThreadName, strAlarmMsg);
					 }
				 }			
			 }
		 }
		 Sleep(20);
		

		iter++;
	 }

	 return 0;
 }

 int CThread_TagAlarmSub::GetAlarmCheck_DI(const char *szDeviceId,CTime currentTime)
 {
	 CString strQuery = "";
	 CString strAlarmKind = "";
	 CString strAlarmMsg = "";
	 CString strAlarmKindType = "";
	 CString strValueText = "";
	 int nAlarmType = 0;
	 float fAlarmScale = 0;
	 int nMakeType = 0;
	 
	 std::list<ST_TagAlarmList>::iterator iter = m_pstTagAlarmList->begin();
	 while(iter != m_pstTagAlarmList->end())
	 {	
		 if(m_bEndThread == TRUE)
			 return THREAD_END;

		 BOOL bAlarmConditionCheck = FALSE;

		 if (iter->nAlarmType == 1)
		 {
			 strAlarmKindType = "OFF->ON";
			 if(iter->fAckValue != iter->fCurrentValue)
			 {
				 if(iter->nAlarmSt == 0)
				 {
					 if(iter->fCurrentValue == 1)
					 {
						 nMakeType = QUERY_TYPE_INSERT;
						 bAlarmConditionCheck = TRUE;
					 }
				 }
				 else
				 {
					 if(iter->fCurrentValue == 0)
					 {
						 nMakeType = QUERY_TYPE_UPDATE;
						 bAlarmConditionCheck = TRUE;
					 }
				 }
			 }
		 }
		 else if (iter->nAlarmType == 2)
		 {
			 if(iter->fAckValue != iter->fCurrentValue)
			 {
				 if(iter->nAlarmSt == 0)
				 {
					 if(iter->fCurrentValue == 0)
					 {
						 nMakeType = QUERY_TYPE_INSERT;
						 bAlarmConditionCheck = TRUE;
					 }
				 }
				 else
				 {
					 if(iter->fCurrentValue == 1)
					 {
						 nMakeType = QUERY_TYPE_UPDATE;
						 bAlarmConditionCheck = TRUE;
					 }
				 }
			 }
		 }
		 else if (iter->nAlarmType == 3)
		 {
			 strAlarmKindType = "OFF<->ON";
			 if(iter->fAckValue != iter->fCurrentValue)
			 {
				 nMakeType = QUERY_TYPE_INSERT;
				 bAlarmConditionCheck = TRUE;
			 }
		 }

		 if(bAlarmConditionCheck == TRUE)
		 {
			 switch(iter->nAlarmKind)
			 {
			 case 0:
				 strAlarmKind = "경보";
				 break;
			 case 1:
				 strAlarmKind = "장애";
				 break;
			 default:
				 strAlarmKind = "시설물 교체 시기";
				 break;
			 }

			 if(iter->fCurrentValue == 1)
				 strValueText = "ON";
			 else 
				strValueText = "OFF";

			 if(m_bEndThread == TRUE)
				 return THREAD_END;

			
			 if(nMakeType == QUERY_TYPE_INSERT)
			 {
				 strAlarmMsg.Format("알람 종류 : [%s][%s], 발생값 : [%s]",strAlarmKind,strAlarmKindType,strValueText);
				 strQuery.Format("INSERT INTO EASY_COMMON.dbo.CM_ALARM_HISTORY "
					 "(ALARM_ID,START_TIME,ALARM_OCCURRENCE_INFO,STATION_NAME,ALARM_CHECK_USE_YN,ALARM_KIND,ALARM_TYPE,ALARM_VALUE,ALARM_MESSAGE)"
					 " VALUES "
					 " ('%s','%s',1,'%s',0,%d,%d,%f,'%s')",iter->szAlarmId,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,iter->nAlarmKind,nAlarmType,iter->fCurrentValue,strAlarmMsg);

				 SetAlarmOccursDI_File(szDeviceId,iter->szAlarmId,nAlarmType,currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->fCurrentValue);
				 SetQueryValue(strQuery,"TagAlarm Insert",iter->szTagName);
				 ShowAlarmOccursMsg("알람발생",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);

				 int nRetLen = strlen(iter->szAlarmLevel);
				 if(nRetLen != 0)
				 {
					 GetAlarmUMSCheck(iter->szAlarmLevel);
					 std::list<ST_UMSSend_UserList>::iterator iter_UMS = m_pstUMSSend_UserList->begin();
					 while(iter_UMS != m_pstUMSSend_UserList->end())
					 {
						 SetSUMSendOutput(*iter,*iter_UMS,strAlarmKindType);
						 iter_UMS++;
						 if(m_bEndThread == TRUE)
							 return THREAD_END;
						 Sleep(100);
					 }
				 }
				 //_addCurrentstateMsg(1,0, m_strThreadName, strAlarmMsg);
			 }
			 else if(nMakeType == QUERY_TYPE_UPDATE)
			 {
				 int nRet = GetAlarmOccurs_DB(iter->szAlarmId,iter->szStartTime); //1:현재 있음,0:없음

				 if(nRet == 0) 
				 {
					 SetAlarmOccursDI_ClearFile(szDeviceId,iter->szAlarmId);
					 ShowAlarmOccursMsg("사용자 해제",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태","알람 상태:[사용자 해제]");
				 }
				 else
				 {
					 strQuery.Format(" UPDATE EASY_COMMON.dbo.CM_ALARM_HISTORY SET ALARM_OCCURRENCE_INFO = 2, END_TIME = '%s' "
						 " WHERE ALARM_ID = '%s' AND START_TIME='%s'",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szAlarmId,iter->szStartTime);

					 SetAlarmOccursDI_ClearFile(szDeviceId,iter->szAlarmId);
					 SetQueryValue(strQuery,"TagAlarm Update",iter->szTagName);

					 strAlarmMsg.Format("알람:[자동 해제],태그명:[%s]",iter->szTagName);
					 _addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, m_strProcessorTitle, USER_COLOR_PINK, strAlarmMsg);
					 ShowAlarmOccursMsg("자동해제",currentTime.Format("%Y-%m-%d %H:%M:%S"),iter->szTagName,"UMS 처리 상태",strAlarmMsg);
				 }
			 }
		 }

		 Sleep(20);
		 iter++;
	 }

	 return 0;
 }

 int CThread_TagAlarmSub::GetAlarmOccurs_DB(const char *szAlarmId,const char *szStartTime)
 {
	 CString strMsgTitle = "GetAlarmOccurs_DB";
	 CString strQuery = "";
	 CString strRunlog_E2 ="",strMsg = "";
	 _RecordsetPtr pRs = NULL;
	 int nCountRow = 0;
	 int nSelectCount;

	 strQuery.Format("SELECT COUNT(*) as Occurs FROM EASY_COMMON.dbo.CM_ALARM_HISTORY "
					" WHERE ALARM_ID = '%s' AND START_TIME = '%s' "
					" AND END_TIME is null",szAlarmId,szStartTime);
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
		 Com_Error(strMsgTitle,&e);

		 if(0x80004005 == e.Error())
		 {
			 strRunlog_E2.Format("Position : [%s], log : [DB 접속 접속 시도..]",strMsgTitle);
			 SetWriteLogFile(": [_com_error..],",strRunlog_E2);

			 int nResult = DB_Connect->DB_ReConnection();
			 if(nResult == 0)
			 {
				 strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB 접속 실패!]",strMsgTitle);
				 SetWriteLogFile(": [_com_error..],",strRunlog_E2);

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
		 SetWriteLogFile(": [catch error..],",strRunlog_E2);

		 return ERROR_DB_COM_ERROR;
	 }

	 return 0;
 }

 int CThread_TagAlarmSub::SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos)
 {
	 CString strRunlog_E2 = "";

	 int nResult = DB_Connect->SetQueryRun(strQuery);

	 if(nResult < 1)
	 {
		 strRunlog_E2.Format("Position : [%s][%s], log : [Query Fail],[%s]",szLogTitle,szLogPos,strQuery);
		 SetWriteLogFile(": [Set Query Error..],",strRunlog_E2);

#ifdef _DEBUG
		 TRACE("Set Query Error-ProcessorName : [%s][%s]\r\n",m_strThreadName,szLogTitle,strRunlog_E2);
#endif
		 return -1;
	 }	

	 if (m_bEndThread == TRUE)
		 return THREAD_END;

	 return 0;
 }

 int CThread_TagAlarmSub::GetAlarmUMSCheck(const char *szAlsrmLevel_ID)
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
		 "FROM EASY_COMMON.dbo.CM_ALARM_LEVEL_INFO levelinfo,EASY_COMMON.dbo.CM_USER_INFO userInfo "
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
		 Com_Error(strMsgTitle,&e);

		 if(0x80004005 == e.Error())
		 {
			 strRunlog_E2.Format("Position : [%s], log : [DB 접속 접속 시도..]",strMsgTitle);
			 SetWriteLogFile(": [_com_error..],",strRunlog_E2);

			 int nResult = DB_Connect->DB_ReConnection();
			 if(nResult == 0)
			 {
				 strRunlog_E2.Format("Position : [%s], log :[ReConnection][DB 접속 실패!]",strMsgTitle);
				 SetWriteLogFile(": [_com_error..],",strRunlog_E2);

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
		 SetWriteLogFile(": [catch error..],",strRunlog_E2);

		 return ERROR_DB_COM_ERROR;
	 }
	 return 0;
 }

 BOOL CThread_TagAlarmSub::SetSUMSendOutput(ST_TagAlarmList &stTagAlrmInfo,ST_UMSSend_UserList &stUmsSend,const char *szAlarmKind)
 {
	 CString strAlarmMsg = "",strSMSSendUseYN = "",strEmailSendUseYN = "";
	 CString strSMSMsg = "",strEmailMsg = "",strId = "";
	 CString strMsg = "",strSendValue = "";
	 CString strQuery = "";
	 
	 EnterCriticalSection(&g_cs);
	 strId = _IDCreated();
	 LeaveCriticalSection(&g_cs);
	
	 if(stTagAlrmInfo.nTagType == 1)
	 {
		 if(stTagAlrmInfo.fCurrentValue == 1)
			strSendValue = "ON";
		 else
			 strSendValue = "OFF";
	 }
	 else
		 strSendValue.Format("%0.4f",stTagAlrmInfo.fCurrentValue);

	  strMsg.Format("알람발생:[%s/%s],[%s],조건:[%s],값:[%s]",stTagAlrmInfo.szAlarmLevel,stUmsSend.szAlarmleveldesc,stTagAlrmInfo.szTagName,szAlarmKind,strSendValue);

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

	 int nRet = SetQueryValue(strQuery,"TAG Alarm UMS Send",stTagAlrmInfo.szTagName);
	 if(nRet == 0)
		 strAlarmMsg.Format("UMS 전송:[성공],[%s],태그명:[%s],알람타입:[%s],사용자:[%s],SMS/Email:[%s/%s],"
		 ,stTagAlrmInfo.szAlarmLevel,stTagAlrmInfo.szTagName,szAlarmKind,stUmsSend.szUserName,strSMSSendUseYN,strEmailSendUseYN);
	 else	
		 strAlarmMsg.Format("UMS 전송:[실패][%s],태그명:[%s],알람타입:[%s],사용자:[%s],SMS/Email:[%s/%s],"
		,stTagAlrmInfo.szAlarmLevel,stTagAlrmInfo.szTagName,szAlarmKind,stUmsSend.szUserName,strSMSSendUseYN,strEmailSendUseYN);

	_addSystemMsg(LOG_MESSAGE_7, USER_COLOR_BLUE, m_strProcessorTitle, USER_COLOR_PINK, strAlarmMsg);

	 return FALSE;
 }

 