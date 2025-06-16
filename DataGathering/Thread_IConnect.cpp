// Thread_IConnect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_IConnect.h"
#include "FormView_IConnect.h"


// CThread_IConnect

IMPLEMENT_DYNCREATE(CThread_IConnect, CWinThread)

CThread_IConnect::CThread_IConnect()
{
	m_bEndThread = FALSE;
	DB_Connect = NULL;
	m_bThreadRunState = FALSE;
	memset(m_stSiteInterlock,0x00,sizeof(m_stSiteInterlock));
	m_nSiteCount = 0;
}

CThread_IConnect::~CThread_IConnect()
{
}

BOOL CThread_IConnect::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	::CoInitialize(NULL); //DB-ADO 컨트롤 사용시
	return TRUE;
}

int CThread_IConnect::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}

	::CoUninitialize();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_IConnect, CWinThread)
END_MESSAGE_MAP()


// CThread_IConnect 메시지 처리기입니다.
//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 처리 과정중 오류 발생시 오류 메시지 발생 정보
-void Com_Error(const char *szLogName,_com_error *e)
*/
//////////////////////////////////////////////////////////////////////////
void CThread_IConnect::Com_Error(const char *szLogName,_com_error *e)
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
	//_addCurrentstateMsg(1,0, m_strThreadName, "DB 처리 오류 상세로그 확인");
	Sleep(500);
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 발생이력 정보 파일로 저장
-void SetWriteLogFile(const char *sTitle,const char *szLogMsg)
*/
//////////////////////////////////////////////////////////////////////////
void CThread_IConnect::SetWriteLogFile(const char *sTitle,const char *szLogMsg)
{
	EnterCriticalSection(&g_cs);
	_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, sTitle, USER_COLOR_PINK, szLogMsg);
	_WriteLogFile(g_stProjectInfo.szProjectLogPath,m_strThreadName,szLogMsg);	
	LeaveCriticalSection(&g_cs);
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 이벤트
- 쓰레드 시작
-int Run()
*/
//////////////////////////////////////////////////////////////////////////
int CThread_IConnect::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//m_strThreadName = "IConnect Main";

	CTime currentTime;// CTime::GetCurrentTime();
	int nMinuteCheck = -1;
	CString strLogMsg = "";
	BOOL bTimeCheck = FALSE;
	BOOL bStartCheck = FALSE;

	ST_DBINFO stDBInfo;
	stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	m_stDataBaseName =  _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	ST_GATHERINFO stGatherInfo = _getInfoGatherRead(g_stProjectInfo.szProjectIniPath);
	ST_FILE_ISMARTACCESS stISmartAccess = _getInfoFileReadISmartAccess(g_stProjectInfo.szProjectIniPath);

	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,m_strThreadName,g_stProjectInfo.szProjectLogPath);
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);

	m_bThreadRunState = TRUE;
	int nSiteCount = 0;

	int nTestMode = 0; //0 : 정상 운영,1 : 테스트 
	int nInterval = 15;
	DWORD dw = GetTickCount() - ((nInterval * 60) * 1000);

	strLogMsg.Format("수집 주기:[%d]분 주기",nInterval);
	//_addCurrentstateMsg(0,0, m_strThreadName, strLogMsg);

	

	do 
	{
		if(dw  > GetTickCount())
			dw = GetTickCount();

		if(m_bEndThread == TRUE)
			break;

		if(DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if(bConnectCheck != TRUE)
			{
				Sleep(500);
				continue;
			}		
		}

		if(dw + ((nInterval*60) * 1000) < GetTickCount())
		{
			if(m_bEndThread == TRUE)
				break;
		
			currentTime = CTime::GetCurrentTime();

			if(nTestMode != 1)
			{
				if(stGatherInfo.nISmart_GatherType == 0) //사용자 직접 접속 정보 입력
				{
					if(bStartCheck == FALSE)
					{
						memset(m_stSiteInterlock,0x00,sizeof(m_stSiteInterlock));
						nSiteCount = 1;
						strcpy_s(m_stSiteInterlock[m_nSiteCount].szID,stISmartAccess.szID);	
						strcpy_s(m_stSiteInterlock[m_nSiteCount].szPW,stISmartAccess.szPW);	
						strcpy_s(m_stSiteInterlock[m_nSiteCount].szSiteId,stISmartAccess.szSiteName);
						strcpy_s(m_stSiteInterlock[m_nSiteCount].szSiteName,stISmartAccess.szSiteName);
						m_nSiteCount = 1;
						bStartCheck = TRUE;
						((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Site(stISmartAccess.szSiteName,stISmartAccess.szID,"사용자 직접 설정 연동");
					}

				}
				else if(stGatherInfo.nISmart_GatherType == 1)//Web 접속 정보 확인
				{
					if(bStartCheck == FALSE)
					{
						nSiteCount = GetSiteSearch(stDBInfo.unDBType);
						if(nSiteCount > 0)
							bStartCheck = TRUE;
					}
				}
				else
				{
					((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Site("정보 없음","강제 중지","환경설정 설정 수집 설정 필요!");
					m_bEndThread = TRUE;
					break;
				}
				
				int nRet = 0;
				if(m_nSiteCount > 0)
				{
					if(currentTime.GetHour() == 0)
					{
						if(currentTime.GetHour() == 15)
						{
							CTimeSpan timeSpan(1,0,0,0); //or timeSpan = CTimeSpan(일,시,분,초);
							currentTime = currentTime - timeSpan;
						}
					}

					nRet = ISmartConnect(m_nSiteCount,currentTime,stDBInfo.unDBType);
				}
				else
				{
					strLogMsg.Format("%s : 수집할 사이트가 설정이 필요합니다.",currentTime.Format("%Y-%m-%d %H:%M"));
					((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item("수집 FAIL",strLogMsg,TRUE);
					//_addCurrentstateMsg(0,0, m_strThreadName, strLogMsg);
				}
				if(nRet == THREAD_END)
					break;
			}
			else
			{
				nSiteCount = GetSiteSearch(stDBInfo.unDBType);
				if(m_nSiteCount != 0)
					TestModData(m_nSiteCount,currentTime,stDBInfo.unDBType);
			}
			dw = GetTickCount();

			//_addCurrentstateMsg(0,0, m_strThreadName, "Processor 정상 처리중..");
		}
		else
			Sleep(500);
		
	} while (!m_bEndThread);

	//_addCurrentstateMsg(0,0, m_strThreadName, "Processor End");
	PostThreadMessage(WM_QUIT, 0, 0);

	return CWinThread::Run();
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 임시 파일로 읽어서 처리
-void TestModData(int nCount, CTime currentDay)
*/
//////////////////////////////////////////////////////////////////////////
void CThread_IConnect::TestModData(int nCount, CTime currentDay,int nDBType)
{
	CFile Rfile;
	CString strTestPath = "";
	if(CFile::hFileNull != Rfile.m_hFile)
	{
		Rfile.Close();
	}
	strTestPath.Format("%s\\htmlTest.htm",g_stProjectInfo.szProjectPath);
	if(!Rfile.Open(strTestPath, CFile::modeRead|CFile::shareDenyWrite))

	{
		AfxMessageBox( " 지정 된 파일을 열수 없습 니다." );
		Rfile.Close();
		return;
	}
	
	UINT FileLength = (UINT)Rfile.GetLength();

	char* szReadData = new char[FileLength];
	Rfile.Read(szReadData,FileLength);
	Rfile.Close();
	
	for(int nII = 0; nII < nCount ; nII++)
		GetDataParsing(szReadData,m_stSiteInterlock[nII].szSiteName, m_stSiteInterlock[nII].szSiteId, currentDay,nDBType);
	//CClientDC dc(this);
	//dc.TextOut(0,0,ps,lstrlen(ps));
	delete szReadData;
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 수집할 사이트 정보 조회
-int GetSiteSearch()
*/
//////////////////////////////////////////////////////////////////////////
int CThread_IConnect::GetSiteSearch(int nDBType)
{
	CString strMsgTitle = "GetSiteList Search";
	CString strQuery = "";
	CString strRunlog_E2 ="",strMsg = "",strDBName;
	_RecordsetPtr pRs = NULL;
	int nCountRow = 0;
	CString strRetSiteid,strRetSiteName,strUseYNMsg,strAccessId,strAccessPw;

	if(nDBType == DB_MSSQL)
	{
		if(strlen(m_stDataBaseName.szCommonDBName) > 1)
			strDBName.Format("%s.dbo.CM_KEPCO_CFG",m_stDataBaseName.szCommonDBName);
		else 
			strDBName.Format("CM_KEPCO_CFG");
	}
	else
	{
		if(strlen(m_stDataBaseName.szCommonDBName) > 1)
			strDBName.Format("%s.CM_KEPCO_CFG",m_stDataBaseName.szCommonDBName);
		else 
			strDBName.Format("CM_KEPCO_CFG");
	}
	
	strQuery.Format("SELECT kepcocfg.SITE_ID"
					",siteinfo.SITE_NAME"
					",kepcocfg.ACCESS_ID"
					",kepcocfg.ACCESS_PW"
					"FROM %s kepcocfg,CM_SITE_INFO siteinfo"
				"where siteinfo.SITE_ID = kepcocfg.SITE_ID"
				"and kepcocfg.USE_YN = 1",strDBName);
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;
		
		memset(m_stSiteInterlock,0x00,sizeof(m_stSiteInterlock));
		m_nSiteCount = 0;
		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nI = 0; nI < nCountRow ; nI++)
				{
					DB_Connect->GetFieldValue(pRs, "SITE_ID", strRetSiteid);
					DB_Connect->GetFieldValue(pRs, "SITE_NAME", strRetSiteName);
					DB_Connect->GetFieldValue(pRs, "ACCESS_ID", strAccessId);
					DB_Connect->GetFieldValue(pRs, "ACCESS_PW", strAccessPw);

					strcpy_s(m_stSiteInterlock[m_nSiteCount].szID,strAccessId);
					strcpy_s(m_stSiteInterlock[m_nSiteCount].szPW,strAccessPw);
					strcpy_s(m_stSiteInterlock[m_nSiteCount].szSiteId,strRetSiteid);
					strcpy_s(m_stSiteInterlock[m_nSiteCount].szSiteName,strRetSiteName);
					((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Site(strRetSiteName,m_stSiteInterlock[m_nSiteCount].szID,"Web 설정 정보 사용");
					m_nSiteCount++;
				
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

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 사이트별 한전망 접속 정보 읽어오기
-CString GetSiteSettingInfo(const char *szSiteID)
*/
//////////////////////////////////////////////////////////////////////////
CString CThread_IConnect::GetSiteSettingInfo(const char *szSiteID)
{
	char szBuffer[64];
	memset(szBuffer,0x00,sizeof(szBuffer));
	CString strSettingPath = "";
	strSettingPath.Format("%s\\SiteList.ini",g_stProjectInfo.szProjectPath);
	GetPrivateProfileString("SiteList", szSiteID, "-", szBuffer, sizeof(szBuffer), strSettingPath);

	return szBuffer;
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- ismart 웹페이지 접근 Url 접속 및 웹페이지 데이터 읽어오기
-int ISmartConnect(int nSiteCount,CTime currentDay)
*/
//////////////////////////////////////////////////////////////////////////
int CThread_IConnect::ISmartConnect(int nSiteCount,CTime currentDay,int nDBType)
{
	CString lpUrl = "https://pccs.kepco.co.kr/iSmart/cm/login.do?method=execute";
	CString lpUrl2 = "https://pccs.kepco.co.kr/iSmart/cm/logout.do";
	CString lpUrl3 = "https://pccs.kepco.co.kr/iSmart/pccs/usageStats.do?method=getGlobalUsageStats";

	CString strSendMsg;
	CString strOutMsg;

	DWORD dwSearviceType,dwSearviceType2,dwSearviceType3;
	CString strServer,strServer2,strServer3, strObject,strObject2,strObject3;
	INTERNET_PORT nPort,nPort2,nPort3;
	if(!AfxParseURL(lpUrl, dwSearviceType, strServer, strObject, nPort))
	{
		strOutMsg.Format("CONNECT FAIL 1:[%s]",lpUrl);
		((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item("URL",strOutMsg,TRUE);
		ASSERT(0);
		return URL_CONNECT_FAIL_1;
	}
	if(!AfxParseURL(lpUrl2, dwSearviceType2, strServer2, strObject2, nPort2))
	{
		strOutMsg.Format("CONNECT FAIL 2:[%s]",lpUrl2);
		((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item("URL",strOutMsg,TRUE);
		ASSERT(0);
		return URL_CONNECT_FAIL_2;
	}

	if(!AfxParseURL(lpUrl3, dwSearviceType3, strServer3, strObject3, nPort3))
	{
		strOutMsg.Format("CONNECT FAIL 3:[%s]",lpUrl3);
		((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item("URL",strOutMsg,TRUE);
		ASSERT(0);
		return URL_CONNECT_FAIL_3;
	}

	CString strReciveMessage("");
	
	int aFlags;
	if( nPort != 80)
		aFlags = INTERNET_FLAG_SECURE;// | INTERNET_FLAG_MAKE_PERSISTENT;
	else
		aFlags = INTERNET_FLAG_EXISTING_CONNECT ;//| INTERNET_FLAG_MAKE_PERSISTENT;
	//aFlags = INTERNET_FLAG_DONT_CACHE ;
//	pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL,aFlags);

	CString strContent;
	CString strHeader;
	CHttpConnection* pServer = NULL;
	CHttpFile *pFile = NULL;
	CInternetFile *pFile1 = NULL;
	CString strSite_Name = "",strSite_Id = "",strUser_Id = "",strUser_Pw = "";

	//for(int nSitePos = 0; nSitePos < 1; nSitePos++)
	for(int nSitePos = 0; nSitePos < nSiteCount; nSitePos++)
	{
		strSite_Name = m_stSiteInterlock[nSitePos].szSiteName;
		strSite_Id = m_stSiteInterlock[nSitePos].szSiteId;
		strUser_Id = m_stSiteInterlock[nSitePos].szID;
		strUser_Pw = m_stSiteInterlock[nSitePos].szPW;
		strOutMsg.Format("%s 한전망 데이터 수집 중...",strSite_Name);
		//_addCurrentstateMsg(1,0, m_strThreadName, strOutMsg);

		Sleep(1000);
		try
		{
			CInternetSession session(NULL, 1, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
			
			pServer = session.GetHttpConnection(strServer2, nPort);//,"0633658352","kdn002053");
			
			if(m_bEndThread == TRUE)
				break;

			Sleep(1);
			
			if(pFile)
			{
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}

			pFile = NULL;
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL,aFlags);

			strSendMsg.Format("userId=%s&password=%s",strUser_Id,strUser_Pw);
			strHeader.Format("Accept: text/html, application/xhtml+xml, */*\r\n"
			"Referer: http://pccs.kepco.co.kr/iSmart/jsp/cm/login/login.jsp\r\n"
			"Accept-Language: ko-KR\r\n"
			"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Accept-Encoding: gzip, deflate\r\n"
			"Connection: Keep-Alive\r\n"
			"Content-Length: %d\r\n"
			"DNT: 1\r\n"
			"Host: pccs.kepco.co.kr\r\n"
			"Pragma: no-cache\r\n",strSendMsg.GetLength());
		//	"Cookie:%s%s\r\n",strSendMsg.GetLength(),_users[nIndex].szusername,_users[nIndex].szpassword);

			BOOL bRet = pFile->SendRequest(strHeader, (LPVOID)strSendMsg.GetBuffer(strSendMsg.GetLength()), strSendMsg.GetLength());

			strSendMsg.ReleaseBuffer();
			if(bRet ==FALSE)
			{
				strOutMsg.Format("[Object1],Site:[%s][%s],ID:[%s],PWD:[%s] I-Smart Account Failed.",strSite_Name,strSite_Id,strUser_Id,strUser_Pw);
				SetWriteLogFile("IConnect Account Failed",strOutMsg);
				((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item(strSite_Name,strOutMsg,TRUE);
				return URL_ACCOUNT_FAIL_1;
			}

			int nBuffSize = 1024*1024;
			int nRxSize = 4096;
			int nLength = 0;
			int nOpMode = 0;
			char *buff = new char[nBuffSize];
			char *RxBuff = new char[nRxSize+1];

			memset(buff, 0, nBuffSize);
			while(1)
			{
				Sleep(0);
				memset(RxBuff, 0, nRxSize);
				pFile->ReadString(RxBuff, nRxSize);
				int nRecvLen = strlen(RxBuff);

				if(nLength+nRecvLen > nBuffSize)
					break;

				memcpy(&buff[nLength], RxBuff, nRecvLen);
				nLength += nRecvLen;

				if(nRecvLen < 1)
					break;
			}
			strContent.Format(_T("%s"),buff);
					
			pServer = session.GetHttpConnection(strServer3, nPort3);//,"0633658352","kdn002053");

			if(pFile)
			{
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}

			pFile = NULL;

			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject3, NULL, 1, NULL, NULL,aFlags);
			//year=%d&month=%02d&day=%02d&diodGubun=0&searchType_min=15
			strSendMsg.Format(_T("diodval=15&year=%d&month=%02d&day=%02d&diodGubun=0&searchType_min=15"),
				currentDay.GetYear(), currentDay.GetMonth(), currentDay.GetDay());
			strHeader.Format("Accept: text/html, application/xhtml+xml, */*\r\n"
							"Referer: http://pccs.kepco.co.kr/iSmart/pccs/usageStats.do?method=getGlobalUsageStats\r\n"
							"Accept-Language: ko-KR\r\n"
							"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko\r\n"
							"Content-Type: application/x-www-form-urlencoded\r\n"
							"Accept-Encoding: gzip, deflate\r\n"
							"Connection: Keep-Alive\r\n"
							"Content-Length: %d\r\n"
							"DNT: 1\r\n"
							"Host: pccs.kepco.co.kr\r\n"
							"Pragma: no-cache\r\n",strSendMsg.GetLength());
							//"Cookie:%s%s\r\n",strSendMsg.GetLength(),_users[nIndex].szusername,_users[nIndex].szpassword);
			BOOL bRet2 = pFile->SendRequest(strHeader, (LPVOID)strSendMsg.GetBuffer(strSendMsg.GetLength()), strSendMsg.GetLength());

			if(bRet2 ==FALSE)
			{
				strOutMsg.Format("[Object3],Site:[%s][%s],ID:[%s] I-Smart search data Failed.[%s]",strSite_Name,strSite_Id,strUser_Id,currentDay.Format("%Y%m%d"));
				SetWriteLogFile("IConnect Search Failed",strOutMsg);
				((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item(strSite_Name,strOutMsg,TRUE);
				return URL_ACCOUNT_FAIL_1;
			}

			nBuffSize = 1024*1024;
			nRxSize = 4096;
			nLength = 0;
			nOpMode = 0;
			memset(buff, 0, nBuffSize);
			while(1)
			{
				Sleep(0);
				memset(RxBuff, 0, nRxSize);
				pFile->ReadString(RxBuff, nRxSize);
				int nRecvLen = strlen(RxBuff);

				if(nLength+nRecvLen > nBuffSize)
				{
					break;
				}

				memcpy(&buff[nLength], RxBuff, nRecvLen);
				nLength += nRecvLen;

				if(nRecvLen < 1)
					break;
			}
			
			strContent = "";
			strContent.Format(_T("%s"),buff);
			if(strContent.Find("로그인") > 0)
			{
				strOutMsg.Format("[Object3],Site:[%s][%s],ID:[%s],PW:[%s] I-Smart LogIn Failed.",strSite_Name,strSite_Id,strUser_Id,strUser_Pw);
				SetWriteLogFile("IConnect LogIn Failed",strOutMsg);

				((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item(strSite_Name,strOutMsg,TRUE);
				return URL_ACCOUNT_FAIL_1;
			}
			
			if(m_bEndThread != TRUE)
			{
				BOOL nRet = GetDataParsing(strContent,strSite_Name,strSite_Id,currentDay,nDBType);
				if(nRet == TRUE)
					break;
			}

			
			if(pFile)
			{
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}

			pFile = NULL;

			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject2, NULL, 1, NULL, NULL,aFlags);

			nLength = 0;
			memset(buff, 0, nBuffSize);
			while(1)
			{
				if(m_bEndThread != TRUE)
					break;
				Sleep(1);
				memset(RxBuff, 0, nRxSize);
//				pFile->SetReadBufferSize(nRxSize);
				pFile->ReadString(RxBuff, nRxSize);
				int nRecvLen = strlen(RxBuff);

				if(nLength+nRecvLen > nBuffSize)
					break;

				memcpy(&buff[nLength], RxBuff, nRecvLen);
				nLength += nRecvLen;

				if(nRecvLen < 1)
					break;
			}
			strContent.Format("%s",buff);
			session.Close();
			
			if(buff)
			{
				delete[] buff;
				buff=NULL;
			}
			if(RxBuff)
			{
				delete[] RxBuff;
				RxBuff=NULL;
			}
			if(pFile)
			{
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}
			if(pFile1)
			{
				pFile1->Close();
				delete pFile1;
				pFile1 = NULL;
			}
			if(pServer)
			{
				pServer->Close();
				delete pServer;
				pServer = NULL;
			}

			if(m_bEndThread != TRUE)
				return THREAD_END;
		}
		catch (CInternetException* e)
		{
			char szError[255];
			e->GetErrorMessage(szError,255);
			e->Delete();

			strOutMsg.Format("[CInternetException],Site:[%s][%s],ID:[%s],PW:[%s] catch error.[%s]",strSite_Name,strSite_Id,strUser_Id,strUser_Pw,szError);
			SetWriteLogFile("IConnect catch error",strOutMsg);

			if(pFile)
			{
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}
			return -1;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- 웹페이지에서 읽어온 데이터 파싱 및 한전 데잍 관리 테이블에 저장
-BOOL GetDataParsing(CString strContent,const char *szSiteName,const char *szSiteId,CTime currentDay)
*/
//////////////////////////////////////////////////////////////////////////
BOOL CThread_IConnect::GetDataParsing(CString strContent,const char *szSiteName,const char *szSiteId,CTime currentDay,int nDBType)
{
	CString szTime, szCurrentTime, szCalTime,szCurTime, szCsvTime,szCsvTime2,szCsvTime3,strDBName;
	int nCurTime=0;
	CString strContentBuffer = strContent;
	strContentBuffer.TrimLeft(_T("\t"));

	//CTime cTimeTemp(2016, 4, 12, 0, 0, 0);
	CString strDay = currentDay.Format("%Y%m%d");
	CString strInsertQuery = "";

	char* bmin[] = {_T("15"), _T("30"), _T("45"),_T("00")};
	int nSucc = 0;

	for(int nHour = 0; nHour < 24; nHour++)
	{
		ST_ICONNECT_DATA stIConnectdata;
		memset(&stIConnectdata,0x00,sizeof(stIConnectdata));
		strcpy_s(stIConnectdata.szSiteId, szSiteId);

		for(int min = 0; min < _countof(bmin); min++)
		{
			if(m_bEndThread == TRUE)
				return FALSE;

			Sleep(1);

			switch(min)
			{
			case 0:
				stIConnectdata.nMin[min] = 0;
				break;
			case 1:
				stIConnectdata.nMin[min] = 15;
				break;
			case 2:
				stIConnectdata.nMin[min] = 30;
				break;
			case 3:
				stIConnectdata.nMin[min] = 45;
				break;
			}

			if(min == 3)
				szTime.Format(_T("<td width=\"34\" class=\"bg_white_c\">%02d%s</td>"), nHour + 1, bmin[min]);
			else
				szTime.Format(_T("<td width=\"34\" class=\"bg_white_c\">%02d%s</td>"), nHour, bmin[min]);

			int nIndexTime = strContentBuffer.Find(szTime);

			if(nIndexTime > -1)
			{
				CString szFind = strContentBuffer.Right(strContentBuffer.GetLength() - nIndexTime - szTime.GetLength());

				//CString szFinds[] = 

				char *szFinds[] = 
				{
					"<td width=\"109\" class=\"bg_white_c\">",
					"<td width=\"128\" class=\"bg_white_c\">",
					"<td width=\"118\"class=\"bg_white_c\">",					
					"<td width=\"118\"class=\"bg_white_c\">",
					"<td width=\"100\"class=\"bg_white_c\">",
					"<td width=\"100\"class=\"bg_white_c\">",
					"<td width=\"100\"class=\"bg_white_c\">",
					"</td>"
				};
				float fIValue;
				for(int nDataPos = 0; nDataPos < 7;nDataPos++)
				{
					int iFind_1 = szFind.Find(szFinds[nDataPos]);
					int nLength = szFind.GetLength() - iFind_1 - strlen(szFinds[nDataPos]);

					if(iFind_1 > -1)
					{
						CString strFindData ="";
						CString strData ="";

						strFindData = szFind.Right(nLength);
						strData= strFindData.Mid(0, strFindData.Find(szFinds[7]));

						szFind.Delete(0,iFind_1+ strlen(szFinds[nDataPos]) + strFindData.Find(szFinds[7])+strData.GetLength()+ 1);

						if(strData == _T("-"))
							fIValue = 0;
						else
						{
							strData.Replace(",","");

							fIValue = (float)_ttof(strData);
						}

						stIConnectdata.fParsingValue[nDataPos][min] = fIValue; 
						stIConnectdata.nDataType[nDataPos][min] = nDataPos;
						strData.ReleaseBuffer();
					}
				}
			}
			else
			{
				for(int nDataPos = 0; nDataPos < 7;nDataPos++)
				{
					stIConnectdata.fParsingValue[nDataPos][min] = 0; 
					stIConnectdata.nDataType[nDataPos][min] = nDataPos;
				}
			}
		}

		if(nDBType == DB_MSSQL)
		{
			if(strlen(m_stDataBaseName.szEmsName) > 1)
				strDBName.Format("%s.dbo.EM_MINUTE_TREND_HISTORY",m_stDataBaseName.szEmsName);
			else 
				strDBName.Format("EM_MINUTE_TREND_HISTORY");
		}
		else
		{
			if(strlen(m_stDataBaseName.szEmsName) > 1)
				strDBName.Format("%s.EM_MINUTE_TREND_HISTORY",m_stDataBaseName.szEmsName);
			else 
				strDBName.Format("EM_MINUTE_TREND_HISTORY");
		}

		for(int nPos = 0; nPos < 7 ; nPos ++)
		{
			strInsertQuery.Format("IF EXISTS( "
										" SELECT RECORD_TIME FROM %s " //테이블 명
										" WHERE SITE_ID = '%s' "
											" AND DATA_COLUMN_TYPE = %d AND RECORD_DATE = '%s' AND RECORD_TIME = %d "
								" ) "
								" BEGIN "  //--SELECT문의 결과값이 존재할  경우 실행
										" UPDATE %s "//테이블 명
											" SET T00= %f,T15 = %f,T30=%f,T45=%f "
										" WHERE SITE_ID = '%s'  "
											" AND DATA_COLUMN_TYPE = %d AND RECORD_DATE = '%s' AND RECORD_TIME = %d  "
									" END "
								" ELSE "
									" BEGIN " // --SELECT문의 결과값이 없을 경우 실행
										" INSERT INTO %s(SITE_ID,DATA_COLUMN_TYPE,RECORD_DATE,RECORD_TIME,T00,T15,T30,T45) "
										" VALUES('%s',%d,'%s',%d,%f,%f,%f,%f) "
								" END",
								strDBName,
								stIConnectdata.szSiteId,
								stIConnectdata.nDataType[nPos][0],strDay,nHour,
								strDBName,
								stIConnectdata.fParsingValue[nPos][0],stIConnectdata.fParsingValue[nPos][1],stIConnectdata.fParsingValue[nPos][2],stIConnectdata.fParsingValue[nPos][3],
								stIConnectdata.szSiteId,stIConnectdata.nDataType[nPos][0],strDay,nHour,
								strDBName,
								stIConnectdata.szSiteId,stIConnectdata.nDataType[nPos][0],strDay,nHour,
								stIConnectdata.fParsingValue[nPos][0],stIConnectdata.fParsingValue[nPos][1],stIConnectdata.fParsingValue[nPos][2],stIConnectdata.fParsingValue[nPos][3]);

			CString strOutMsg = "";
			strOutMsg.Format("%s%d",strDay,nHour);
			
			SetQueryValue(strInsertQuery,stIConnectdata.szSiteId,strOutMsg);

			strOutMsg.Format("정상-%s,%s %d,[T0=%f,T15=%f,T30=%f,T45=%f]",stIConnectdata.szSiteId,strDay,nHour,stIConnectdata.fParsingValue[nPos][0],stIConnectdata.fParsingValue[nPos][1],stIConnectdata.fParsingValue[nPos][2],stIConnectdata.fParsingValue[nPos][3]);
			((CFormView_IConnect *)(m_pCtrl))->ListInsertItem_Item(szSiteName,strOutMsg,FALSE);
		}
		Sleep(20);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/*
- 호출 방법 : 호출
- DB 쿼리 실행(Insert/Update)
-int SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos);
*/
//////////////////////////////////////////////////////////////////////////
int CThread_IConnect::SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos)
{
	CString strRunlog_E2 = "";

	int nResult = DB_Connect->SetQueryRun(strQuery);

	if(nResult < 1)
	{
		strRunlog_E2.Format("Position : [%s][%s], log : [Query Fail],[%s]",szLogTitle,szLogPos,strQuery);
		SetWriteLogFile("Set Query Error..",strRunlog_E2);

#ifdef _DEBUG
		TRACE("Query Error-ProcessorName : [%s][%s][%s]\r\n",m_strThreadName,szLogTitle,strRunlog_E2);
#endif
		return -1;
	}	

	if (m_bEndThread == TRUE)
		return THREAD_END;

	return 0;
}
