// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// DataGathering.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

UINT wm_EVViewStop;
ST_PROJECT_INFO g_stProjectInfo;
int g_nActivateDocCheck;
int g_nCheckTagThreadRun; //20210308 ksw 설정 - 히스토리컬 insert 버튼 플래그
CRITICAL_SECTION g_cs;

int g_pList_ST_TagDivisionListCnt;
std::list<ST_TagInfoList>** g_pList_ST_TagDivisionList;

//공통 폰트 사용
void _CreateFont(CFont *pFont)
{
	/*LOGFONT iFont;
	memset(&iFont, 0, sizeof(iFont));
	iFont.lfHeight = 13;
	iFont.lfCharSet = DEFAULT_CHARSET;
	iFont.lfWeight = FW_MEDIUM;
	iFont.lfPitchAndFamily = VARIABLE_PITCH;
	strcpy_s(iFont.lfFaceName, "맑은 고딕");
	pFont->CreateFontIndirect(&iFont);*/

	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	_tcscpy_s( ncm.lfMessageFont.lfFaceName,12, _T("맑은 고딕"));
	pFont->CreateFontIndirect(&ncm.lfMessageFont);
}

CString _IDCreated()
{
	CString strMillisecond = "",strMilli = "";

	CTime ct(1970,1,1,9,0,0);

	SYSTEMTIME time;
	GetLocalTime(&time);
	CTime ATime(time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond); // 1시간 차이
	CTimeSpan span = ATime - ct;
	/*struct _timeb tstruct;
	_ftime32_s( &tstruct );
	strMillisecond.Format("%d%03d",span,tstruct.millitm);*/
	strMilli.Format("%03d",time.wMilliseconds);
	strMillisecond.Format("%d",span);
	strMillisecond += strMilli;
	return strMillisecond;
}


//////////////////////////////////////////////////////////////////////////
//DataGathering info ini File Check

CString _getDataGatheringInfo(CString strPath, CString strProject)
{
	CFileFind fileFind;
	CString strSettingFile;
	CString strSettingFileCreate = "";
	strSettingFile.Format("%s\\%s.ini",strPath,strProject);

	BOOL bPathCheck = fileFind.FindFile(strSettingFile);
	if(bPathCheck == FALSE)
	{
		CreateDirectory(strPath,NULL);
		strSettingFileCreate.Format(START_SETTING_LIST_1);
		_WriteFile(strSettingFileCreate,strSettingFileCreate.GetLength(),strSettingFile);
	}
	fileFind.Close();

	return "";
}

CString _createFolder(const char *szBasicsPath,const char *szCreateName)
{
	CFileFind fileFind;
	CString strSettingFile;
	CString strSettingFileCreate = "";
	strSettingFile.Format("%s\\%s",szBasicsPath,szCreateName);

	BOOL bPathCheck = fileFind.FindFile(strSettingFile);
	if(bPathCheck == FALSE)
	{
		CreateDirectory(strSettingFile,NULL);
	}
	fileFind.Close();

	return strSettingFile;
}

//DB명 정보 읽어 온다.
ST_DATABASENAME _getDataBesaNameRead(const char *szIniPath)
{
	ST_DATABASENAME stDBName;
	memset(&stDBName ,0x00 ,sizeof(stDBName));

	GetPrivateProfileString("DBName", "HMIDB",		"", stDBName.szHMIDBName, sizeof(stDBName.szHMIDBName), szIniPath);
	GetPrivateProfileString("DBName", "WTDB",		"", stDBName.szWTDBName, sizeof(stDBName.szWTDBName), szIniPath);
	GetPrivateProfileString("DBName", "COMMONDB",	"EASY_COMMON", stDBName.szCommonDBName, sizeof(stDBName.szCommonDBName), szIniPath);
	GetPrivateProfileString("DBName", "CONFIGDB",	"EASY_CONFIG", stDBName.szConfigName, sizeof(stDBName.szConfigName), szIniPath);
	GetPrivateProfileString("DBName", "EMSDB",		"EASY_EMS", stDBName.szEmsName, sizeof(stDBName.szEmsName), szIniPath);
	GetPrivateProfileString("DBName", "FMSDB",		"EASY_FMS", stDBName.szFmsName, sizeof(stDBName.szFmsName), szIniPath);

	return stDBName;
}

//DB 수정 정보를 쓴다.
void _setDataBesaNameWrite(ST_DATABASENAME *stDBName, const char * szIniPath)
{
	WritePrivateProfileString("DBName",	"HMIDB",	stDBName->szHMIDBName,szIniPath);
	WritePrivateProfileString("DBName",	"WTDB",		stDBName->szWTDBName,szIniPath);
	WritePrivateProfileString("DBName", "COMMONDB",	stDBName->szCommonDBName,  szIniPath);
	WritePrivateProfileString("DBName", "CONFIGDB", stDBName->szConfigName, szIniPath);
	WritePrivateProfileString("DBName", "EMSDB",	stDBName->szEmsName, szIniPath);
	WritePrivateProfileString("DBName", "FMSDB",	stDBName->szFmsName, szIniPath);
}


//DB 정보 읽어 온다.
ST_DBINFO _getInfoDBRead(const char *szIniPath)
{
	ST_DBINFO stDbInfo;

	memset(&stDbInfo ,0x00 ,sizeof(stDbInfo));
	char szDBType[16];
	memset(&szDBType, 0x00, sizeof(szDBType));

	GetPrivateProfileString("DBINFO", "DB_TYPE", "", szDBType, sizeof(szDBType), szIniPath);
	if(strcmp(szDBType,"MSSQL") == 0)
		stDbInfo.unDBType = DB_MSSQL;
	else if (strcmp(szDBType,"ORACLE") == 0)
		stDbInfo.unDBType = DB_ORACLE;
	//20200210 jsh : Postgre 추가
	else if (strcmp(szDBType, "POSTGRE") == 0)
		stDbInfo.unDBType = DB_POSTGRE;
	else
		stDbInfo.unDBType = DB_MSSQL;

	GetPrivateProfileString("DBINFO", "Server", "", stDbInfo.szServer, sizeof(stDbInfo.szServer), szIniPath);
	GetPrivateProfileString("DBINFO", "DB", "", stDbInfo.szDB, sizeof(stDbInfo.szDB), szIniPath);
	GetPrivateProfileString("DBINFO", "ID", "", stDbInfo.szID, sizeof(stDbInfo.szID), szIniPath);
	GetPrivateProfileString("DBINFO", "PW", "", stDbInfo.szPW, sizeof(stDbInfo.szPW), szIniPath);
	//GetPrivateProfileString("DBINFO", "HMIDB", "", stDbInfo.szHMIDBName, sizeof(stDbInfo.szHMIDBName), szIniPath);
	//GetPrivateProfileString("DBINFO", "WTDB", "", stDbInfo.szWTDBName, sizeof(stDbInfo.szWTDBName), szIniPath);

	return stDbInfo;
}

//DB 수정 정보를 쓴다.
void _setInfoDBWrite(ST_DBINFO *stDbInfo, const char * szIniPath)
{

	char szBuffer[16];
	memset(&szBuffer, 0x00 ,sizeof(szBuffer));
	switch(stDbInfo->unDBType)
	{
	case DB_MSSQL:
		strcat_s(szBuffer,"MSSQL");
		break;
	case DB_ORACLE:
		strcat_s(szBuffer,"ORACLE");
		break;
	case DB_POSTGRE: // 20200211 jsh : postgre 추가
		strcat_s(szBuffer,"POSTGRE");
		break;
	case DB_MYSQL:
		strcat_s(szBuffer,"MYSQL");
		break;
	}

	WritePrivateProfileString("DBINFO","DB_TYPE",szBuffer,szIniPath);
	WritePrivateProfileString("DBINFO","Server",stDbInfo->szServer,szIniPath);
	WritePrivateProfileString("DBINFO","DB"		,stDbInfo->szDB,szIniPath);
	WritePrivateProfileString("DBINFO","ID"		,stDbInfo->szID,szIniPath);
	WritePrivateProfileString("DBINFO","PW"		,stDbInfo->szPW,szIniPath);
	//WritePrivateProfileString("DBINFO","HMIDB"		,stDbInfo->szHMIDBName,szIniPath);
	//WritePrivateProfileString("DBINFO","WTDB"		,stDbInfo->szWTDBName,szIniPath);
}

//수집 방식 설정 정보 쓴다.
void _setGatherInfoType(ST_GATHERINFO_TYPE *stInfoType, const char * szIniPath)
{
	CString strItem = "";
	char szBuffer[16],szCombineBuffer[128];
	memset(&szBuffer, 0x00 ,sizeof(szBuffer));
	sprintf_s(szBuffer,"%d",stInfoType->nGatherType);
	WritePrivateProfileString("GatherType","Type",szBuffer,szIniPath);
	sprintf_s(szBuffer,"%d",stInfoType->nCount);
	WritePrivateProfileString("GatherType","Count",szBuffer,szIniPath);

	for(int nIndex = 0; nIndex < stInfoType->nCount ; nIndex++)
	{
		strItem.Format("item_%d",nIndex + 1);
		if(stInfoType->stGatherUseList[nIndex].nUse_YN == 0)
		{
			WritePrivateProfileString("GatherType",strItem, NULL,szIniPath);
			stInfoType->nCount--;
		}
		else
		{
			sprintf_s(szCombineBuffer,"%s,%s,%s,%s",stInfoType->stGatherUseList[nIndex].szSiteId,stInfoType->stGatherUseList[nIndex].szSiteName,stInfoType->stGatherUseList[nIndex].szDeviceId,stInfoType->stGatherUseList[nIndex].szDeviceName);
			WritePrivateProfileString("GatherType",strItem,szCombineBuffer,szIniPath);
		}
	}


}
ST_GATHERINFO_TYPE _getGatherInfoType(const char * szIniPath)
{
	ST_GATHERINFO_TYPE stInfoType;
	memset(&stInfoType,0x00,sizeof(stInfoType));

	CString strItemMain = "",strTotalBuffer = "",strBuffer = "";
	char szBuffer[128];
	memset(&szBuffer, 0x00 ,sizeof(szBuffer));

	GetPrivateProfileString("GatherType", "Type", "0", szBuffer, sizeof(szBuffer), szIniPath);
	stInfoType.nGatherType = atoi(szBuffer);

	GetPrivateProfileString("GatherType", "Count", "0", szBuffer, sizeof(szBuffer), szIniPath);
	stInfoType.nCount = atoi(szBuffer);

	for (int nIndex = 0; nIndex < stInfoType.nCount; nIndex++)
	{
		strItemMain.Format("item_%d",nIndex + 1);
		GetPrivateProfileString("GatherType", strItemMain, "0", szBuffer, sizeof(szBuffer), szIniPath);

		strTotalBuffer.Format("%s",szBuffer);
		for(int nFindIndex = 0; nFindIndex < 4;nFindIndex++)
		{
			int nPos = strTotalBuffer.Find(",");

			strBuffer = strTotalBuffer.Left(nPos);
			if (nFindIndex < 3)
				strTotalBuffer.Delete(0,nPos + 1);

			if(nFindIndex == 0)
				strcat_s(stInfoType.stGatherUseList[nIndex].szSiteId,strBuffer);
			else if (nFindIndex == 1)
				strcat_s(stInfoType.stGatherUseList[nIndex].szSiteName,strBuffer);
			else if (nFindIndex == 2)
				strcat_s(stInfoType.stGatherUseList[nIndex].szDeviceId,strBuffer);
			else if (nFindIndex == 3)
				strcat_s(stInfoType.stGatherUseList[nIndex].szDeviceName,strTotalBuffer);


		}
		stInfoType.stGatherUseList[nIndex].nUse_YN = 1;
	}

	return stInfoType;
}

//게더링 정보 읽어 온다
ST_GATHERINFO _getInfoGatherRead(const char *szIniPath)
{
	ST_GATHERINFO stGatherInfo;
	memset(&stGatherInfo ,0x00 ,sizeof(stGatherInfo));
	char szBuffer[6];
	int nInterval = 0;
	memset(szBuffer,0x00,sizeof(szBuffer));


	//stGatherInfo.nAutoRun_Check = GetPrivateProfileInt("DataGathering","AutoRun_UseCheck",0, szIniPath);
	GetPrivateProfileString("DataGathering", "AutoRun_UseCheck", "0", szBuffer, sizeof(szBuffer), szIniPath);
	stGatherInfo.nAutoRun_Check = atoi(szBuffer);

	GetPrivateProfileString("DataGathering", "Product_Check", "0", szBuffer, sizeof(szBuffer), szIniPath);//20210305 ksw 제품설정 추가
	stGatherInfo.nProduct_Check = atoi(szBuffer);//20210305 ksw 제품설정 추가

	GetPrivateProfileString("DataGathering", "Gathe_interval", "1", szBuffer, sizeof(szBuffer), szIniPath);
	stGatherInfo.nGathe_Interval = atoi(szBuffer);
	nInterval = atoi(szBuffer);
	stGatherInfo.nGathe_IntervalPos = nInterval;

	stGatherInfo.nGathe_Interval = nInterval;

	/*
	if(nInterval == 0)
		stGatherInfo.nGathe_Interval = 2;
	else if(nInterval == 1)
		stGatherInfo.nGathe_Interval = 5;
	else if(nInterval == 2)
		stGatherInfo.nGathe_Interval = 30;
	else if(nInterval == 3)
		stGatherInfo.nGathe_Interval = 60;
	*/

	GetPrivateProfileString("DataGathering", "Delete_interval", "0", szBuffer, sizeof(szBuffer), szIniPath);
	nInterval = atoi(szBuffer);
	stGatherInfo.nDelete_IntervalPos = nInterval;
	if(nInterval == 0)
		stGatherInfo.nDelete_Interval = 2;
	else if(nInterval == 1)
		stGatherInfo.nDelete_Interval = 5;
	else if(nInterval == 2)
		stGatherInfo.nDelete_Interval = 30;
	else if(nInterval == 3)
		stGatherInfo.nDelete_Interval = 60;

	GetPrivateProfileString("DataGathering", "Demp_UseCheck", "0", szBuffer, sizeof(szBuffer), szIniPath); //DB 통신시 발생시 발생시간 저장
	stGatherInfo.nDemp_Check = atoi(szBuffer);
	GetPrivateProfileString("DataGathering", "Demp_AutoManual", "0", szBuffer, sizeof(szBuffer), szIniPath); //발생시간 데이터 자동 생성
	stGatherInfo.nDemp_AutoCheck = atoi(szBuffer);
	GetPrivateProfileString("DataGathering", "Weather_Gather", "2", szBuffer, sizeof(szBuffer), szIniPath); //날씨 수집 연동 타입
	stGatherInfo.nWeather_GatherType = atoi(szBuffer);
	GetPrivateProfileString("DataGathering", "ISmart_Gather", "2", szBuffer, sizeof(szBuffer), szIniPath); //한전데이터 연동 타입
	stGatherInfo.nISmart_GatherType = atoi(szBuffer);
	GetPrivateProfileString("DataGathering", "EngMng_Gather", "2", szBuffer, sizeof(szBuffer), szIniPath); //에너지관리공단 연동 타입
	stGatherInfo.nEngMng_GatherType = atoi(szBuffer);


	//GetPrivateProfileString("Weather", "UrlLive", "http://", stGatherInfo.szUrlLive, sizeof(stGatherInfo.szUrlLive), szIniPath); //라이브 날씨 URL(인증키,위도,경도 파람은제외)
	//GetPrivateProfileString("Weather", "UrlForecast", "http://", stGatherInfo.szUrlForecast, sizeof(stGatherInfo.szUrlForecast), szIniPath); //예보 날씨 URL(위도,경도 파람은제외)

	return stGatherInfo;
}

void _setInfoGatherWrite(ST_GATHERINFO *stGatherInfo, const char * szIniPath)
{
	CString strBuffer = "";

 	strBuffer.Format("%d",stGatherInfo->nGathe_IntervalPos);
	WritePrivateProfileString("DataGathering","Gathe_interval",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nDelete_IntervalPos);
	WritePrivateProfileString("DataGathering","Delete_interval",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nAutoRun_Check);
	WritePrivateProfileString("DataGathering","AutoRun_UseCheck",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nDemp_Check);
	WritePrivateProfileString("DataGathering","Demp_UseCheck",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nDemp_AutoCheck);
	WritePrivateProfileString("DataGathering","Demp_AutoManual",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nProduct_Check); //20210305 ksw 제품설정 추가
	WritePrivateProfileString("DataGathering","Product_Check",strBuffer,szIniPath);//20210305 ksw 제품설정 추가

	strBuffer.Format("%d",stGatherInfo->nWeather_GatherType);
	WritePrivateProfileString("DataGathering","Weather_Gather",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nISmart_GatherType);
	WritePrivateProfileString("DataGathering","ISmart_Gather",strBuffer,szIniPath);
	strBuffer.Format("%d",stGatherInfo->nEngMng_GatherType);
	WritePrivateProfileString("DataGathering","EngMng_Gather",strBuffer,szIniPath);

	//WritePrivateProfileString("Weather","UrlLive",stGatherInfo->szUrlLive,szIniPath);
	//WritePrivateProfileString("Weather","UrlForecast",stGatherInfo->szUrlForecast,szIniPath);
}

//ISmart사용자 설정  연동 정보 읽어온다
ST_FILE_ISMARTACCESS _getInfoFileReadISmartAccess(const char *szIniPath)
{
	ST_FILE_ISMARTACCESS stInfo_1;
	memset(&stInfo_1 ,0x00 ,sizeof(stInfo_1));

	GetPrivateProfileString("ISmartInfo", "SiteName", "0", stInfo_1.szSiteName, sizeof(stInfo_1.szSiteName), szIniPath);
	GetPrivateProfileString("ISmartInfo", "AccessID", "0", stInfo_1.szID, sizeof(stInfo_1.szID), szIniPath);
	GetPrivateProfileString("ISmartInfo", "AccessPW", "0", stInfo_1.szPW, sizeof(stInfo_1.szPW), szIniPath);

	return stInfo_1;
}

//ISmart사용자 설정  연동 정보 저장한다
void _setInfoFileReadISmartAccess(ST_FILE_ISMARTACCESS *stInfo, const char * szIniPath)
{
	char szBuffer[16];
	memset(&szBuffer, 0x00 ,sizeof(szBuffer));

	WritePrivateProfileString("ISmartInfo","SiteName",stInfo->szSiteName,szIniPath);
	WritePrivateProfileString("ISmartInfo","AccessID",stInfo->szID,szIniPath);
	WritePrivateProfileString("ISmartInfo","AccessPW",stInfo->szPW,szIniPath);
}


//기상청 연동 설정 읽어온다
ST_WEATHER_INFO _getInfoWeatherRead(const char *szIniPath)
{
	ST_WEATHER_INFO stWeatherInfo;
	memset(&stWeatherInfo ,0x00 ,sizeof(stWeatherInfo));

	GetPrivateProfileString("WEATHER", "LiveUseYN", "", stWeatherInfo.szLiveUSE_YN, sizeof(stWeatherInfo.szLiveUSE_YN), szIniPath);
	GetPrivateProfileString("WEATHER", "Services", "", stWeatherInfo.szServiceKey, sizeof(stWeatherInfo.szServiceKey), szIniPath);
	GetPrivateProfileString("WEATHER", "LiveLat", "0", stWeatherInfo.szLiveLat, sizeof(stWeatherInfo.szLiveLat), szIniPath);
	GetPrivateProfileString("WEATHER", "LiveLon", "0", stWeatherInfo.szLiveLon, sizeof(stWeatherInfo.szLiveLon), szIniPath);

	GetPrivateProfileString("WEATHER", "ForecastUseYN", "", stWeatherInfo.szForecastUSE_YN, sizeof(stWeatherInfo.szForecastUSE_YN), szIniPath);
	//GetPrivateProfileString("WEATHER", "ForecastServices", "", stWeatherInfo.szServiceKey, sizeof(stWeatherInfo.szServiceKey), szIniPath);
	GetPrivateProfileString("WEATHER", "ForecastLat", "0", stWeatherInfo.szForecastLat, sizeof(stWeatherInfo.szForecastLat), szIniPath);
	GetPrivateProfileString("WEATHER", "ForecastLon", "0", stWeatherInfo.szForecastLon, sizeof(stWeatherInfo.szForecastLon), szIniPath);

	return stWeatherInfo;
}

//기상청 연동 설정 저장
void _setInfoWeatherWrite(ST_WEATHER_INFO *stWeatherInfo, const char * szIniPath)
{
	char szBuffer[16];
	memset(&szBuffer, 0x00 ,sizeof(szBuffer));

	WritePrivateProfileString("WEATHER","LiveUseYN"	,stWeatherInfo->szLiveUSE_YN,szIniPath);
	WritePrivateProfileString("WEATHER","Services"	,stWeatherInfo->szServiceKey,szIniPath);
	WritePrivateProfileString("WEATHER","LiveLat"		,stWeatherInfo->szLiveLat,szIniPath);
	WritePrivateProfileString("WEATHER","LiveLon"		,stWeatherInfo->szLiveLon,szIniPath);

	WritePrivateProfileString("WEATHER","ForecastUseYN"	,stWeatherInfo->szForecastUSE_YN,szIniPath);
	//WritePrivateProfileString("WEATHER","Services"	,stWeatherInfo->szServiceKey,szIniPath);
	WritePrivateProfileString("WEATHER","ForecastLat"		,stWeatherInfo->szForecastLat,szIniPath);
	WritePrivateProfileString("WEATHER","ForecastLon"		,stWeatherInfo->szForecastLon,szIniPath);
}


CString _getXmlPath(const char *szIniPath)
{
	char szPath[128];
	memset(szPath,0x00,sizeof(szPath));
	GetPrivateProfileString("EngMng", "XMLPath", "", szPath, sizeof(szPath), szIniPath);

	return szPath;
}
void _setXmlPath(const char *szXmlPath, const char * szIniPath)
{
	WritePrivateProfileString("EngMng","XMLPath"		,szXmlPath,szIniPath);
}

//////////////////////////////////////////////////////////////////////////
//LOG 파일 쓰기
//시작/종료 및 시스템 로그
void _systemLog(const char * data, const char * szFilePath)
{
	CTime cTimeCheck;
	cTimeCheck = CTime::GetCurrentTime();

	CString strFileName = "", strMsgData = "";
	strFileName.Format("%s\\DataGathering.log",szFilePath);
	strMsgData.Format("%s :: DataGathering Program  %s ",cTimeCheck.Format("%Y-%m-%d %H:%M"),data);
	_WriteFile(strMsgData,strMsgData.GetLength(),strFileName);
}

void _WriteFile(const char * data, const int len, const char * szFileName)
{
	int		fh = 0;

	//_sopen_s(&fh,szFileName, _O_WRONLY|_O_CREAT|_O_APPEND, 0,_S_IREAD|_S_IWRITE );
	fh = _open(szFileName, _O_WRONLY|_O_CREAT|_O_APPEND, _S_IREAD|_S_IWRITE );
	if(-1 == fh)
		return;

	_write(fh, data, len);
	_write(fh, "\r\n", 2);
	_close(fh);
}

void _WriteLogFile(CString strLogPath,CString strFaileName,CString data)
{
	CFileFind fileFind;
	CString szLogFile="",szLogtext="",strLogPathCheck = "";
	CTime tm = CTime::GetCurrentTime();

	strLogPathCheck.Format("%s",strLogPath);
	BOOL bPathCheck = fileFind.FindFile(strLogPathCheck);
	if(bPathCheck == FALSE)
	{
		CreateDirectory(strLogPathCheck,NULL);
	}
	/*if(strFaileName.IsEmpty() != TRUE)
	{
		strLogPathCheck.Format("%s\\LOG\\%s",strLogPath,strFaileName);
		bPathCheck = fileFind.FindFile(strLogPathCheck);
		if(bPathCheck == FALSE)
		{
			CreateDirectory(strLogPathCheck,NULL);
		}
	}*/

	fileFind.Close();

	szLogFile.Format("%s\\%s_%04d%02d%02d.log",
		strLogPathCheck,strFaileName,tm.GetYear(), tm.GetMonth(), tm.GetDay());

	szLogtext.Format("[%04d/%02d/%02d %02d:%02d:%02d] %s",tm.GetYear(), tm.GetMonth(), tm.GetDay(),tm.GetHour(), tm.GetMinute(), tm.GetSecond(),data);

	BOOL bFile = FALSE;
	CFile f;
	bFile = f.Open(szLogFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareExclusive);
	if (bFile == FALSE)
	{
		return ;
	}

	f.SeekToEnd();
	f.Write((char*)(LPCSTR)szLogtext, (unsigned int)szLogtext.GetLength());
	f.Write("\r\n", 2);
	f.Close();

	return ;
}

void _WriteBackUpFile(CString strLogPath,CString data,int nMin)
{
	CFileFind fileFind;
	CString szLogFile="",szLogtext="",strLogPathCheck = "";
	CTime tm = CTime::GetCurrentTime();

	strLogPathCheck.Format("%s\\Backup",strLogPath);
	BOOL bPathCheck = fileFind.FindFile(strLogPathCheck);
	if(bPathCheck == FALSE)
	{
		CreateDirectory(strLogPathCheck,NULL);
	}
	strLogPathCheck.Format("%s\\BackUp\\BackUpData",strLogPath);
	bPathCheck = fileFind.FindFile(strLogPathCheck);
	if(bPathCheck   == FALSE)
	{
		CreateDirectory(strLogPathCheck,NULL);
	}

	fileFind.Close();

	szLogFile.Format("%s\\%04d%02d%02d_%02d%02d.sql",
		strLogPathCheck,tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(),nMin);

	szLogtext.Format("%s",data);

	BOOL bFile = FALSE;
	CFile f;
	bFile = f.Open(szLogFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareExclusive);
	if (bFile == FALSE)
	{
		return ;
	}

	f.SeekToEnd();
	f.Write((char*)(LPCSTR)szLogtext, (unsigned int)szLogtext.GetLength());
	f.Write("\r\n", 2);
	f.Close();

	return ;
}


//////////////////////////////////////////////////////////////////////////
//Output Msg
CCriticalSection _csMsg;
CPtrArray _arrMsg;

int _addSystemMsg(int nIsOutput, COLORREF crTitle, const char *sTitle, COLORREF crBody, const char *sBody)
{
	int nSize;

	CSysMsg *p = new CSysMsg;
	p->m_nIsOutput = nIsOutput;
	p->m_crTitle = crTitle;
	p->m_crBody = crBody;
	p->m_sTitle = sTitle;
	p->m_sBody = sBody;

	_csMsg.Lock();
	if (_arrMsg.GetSize() > 512)
	{
		_csMsg.Unlock();
		delete p;
		return -1;
	}
	nSize = _arrMsg.Add(p);
	_csMsg.Unlock();

	return nSize;
}

int _getSystemMsg(CSysMsg *p)
{
	int nSize;
	CSysMsg *src;

	_csMsg.Lock();
	nSize = _arrMsg.GetSize();
	if (nSize < 1)
	{
		_csMsg.Unlock();
		return 0;
	}
	src = (CSysMsg*)_arrMsg.GetAt(0);
	_arrMsg.RemoveAt(0);
	_csMsg.Unlock();

	*p = src;
	delete src;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//Currentstate Msg
CCriticalSection _csStateMsg;
CPtrArray _arrStateMsg;

int _addCurrentstateMsg(int nIsPos,int nCount, const char *szThreadState, const char *szStateMsg)
{
	int nSize;

	CCurrentstate *p = new CCurrentstate;
	p->m_nIsPos = nIsPos;
	p->m_nCount = nCount;
	p->m_strThreadState = szThreadState;
	p->m_strStateMsg = szStateMsg;

	_csStateMsg.Lock();
	if (_arrStateMsg.GetSize() > 512)
	{
		_csStateMsg.Unlock();
		delete p;
		return -1;
	}
	nSize = _arrStateMsg.Add(p);
	_csStateMsg.Unlock();

	return nSize;
}

int _getCurrentstateMsg(CCurrentstate *p)
{
	int nSize;
	CCurrentstate *src;

	_csStateMsg.Lock();
	nSize = _arrStateMsg.GetSize();
	if (nSize < 1)
	{
		_csStateMsg.Unlock();
		return 0;
	}
	src = (CCurrentstate*)_arrStateMsg.GetAt(0);
	_arrStateMsg.RemoveAt(0);
	_csStateMsg.Unlock();

	*p = src;
	delete src;
	return 1;
}