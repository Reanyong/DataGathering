#pragma once

#include "Control_List/XMLCtrl/tinyxml.h"
#include "Control_List/XMLCtrl/XmlNodeWrapper.h"

#define WEATHER_LIVE	0
#define WEATHER_FORECAST	1

// CThread_WeatherGather

class CThread_WeatherGather : public CWinThread
{
	DECLARE_DYNCREATE(CThread_WeatherGather)

protected:
	CThread_WeatherGather();           // 동적 만들기에 사용되는 protected 생성자입니다.
public:
	virtual ~CThread_WeatherGather();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetOwner(CWnd* hWnd) {
		m_pOwner = hWnd;
	}
	void SetWnd(HWND hWnd) {
		m_WindHwnd = hWnd;
	}
	void SetProgramName(const char *szName)	{
		m_strThreadName = szName;
	}
	void Stop() {
		//_addCurrentstateMsg(0,0, m_strThreadName, "Processor 종료");
		m_bEndThread = TRUE;
	}
	void EndCheck() {
		m_bEndCheck = TRUE;
	}
	BOOL GetStop() { return m_bEndThread;	}

	void SetLPVOID(LPVOID pCtrl)
	{
		m_pCtrl = pCtrl;
	}
protected:
	void Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *sTitle,const char *szLogMsg);

	int GetWeatherList(int nQueryType);
	int GetWeatherLiveGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck);
	int GetWeatherLiveGather_2(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,const char *szDBName,ST_WEATHER_INFO *stWeatherInfo,BOOL bStartCheck);
	BOOL GetWeatherLive_URL(CTime time,const char *szUrl,ST_WEATHER_LIST stWeatherInfoList,const char *szSavePath);
	BOOL GetWeatherLive_URL_2(CTime time,const char *szUrl, ST_WEATHER_INFO *stWeatherInfo,const char *szSavePath);
	int GetWeatherForecastGather(CTime currentTime,const char *szSavePath,ST_DBINFO *stDBInfo,ST_GATHERINFO *stGatherInfo,BOOL bStartCheck);
	BOOL GetWeatherForecast_URL(CTime time,const char *szUrl,ST_WEATHER_LIST stWeatherInfoList,const char *szSavePath);
	CString GetElementText(TiXmlNode* pNode, const char* szChildName,const char *szLogTitle,const char *szLogPos);
	int GetWeatherInfo(ST_DBINFO *stDBInfo,ST_WEATHER_INFO *stWeatherInfo,const char *szDBName);

	void Release_List_ST(int nMode);
	void SetWeatherForecastSpaceData(CTime time, int nIndex,const char *szSiteId,ST_DBINFO *stDBInfo);
	int GetWeatherForecast_UseCheck(const char *szSiteid,const char *szMsrTime);
	void SetForecastSpaceData(CTime time, int nIndex,const char *szSiteId);
	int SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos);
protected:
	CWnd* m_pOwner;
	HWND m_WindHwnd;
	BOOL m_bEndThread;
	BOOL m_bEndCheck;
	LPVOID m_pCtrl;

	CAdo_Control *DB_Connect;

	CString m_strThreadName;
	CString m_strLogTitle;
	CString m_strXmlOldFileName;

	int m_nSiteCount;
	int m_nWeatherLiveMinuteCheck;
	BOOL m_bWeatherForecastTimeCheck;


	ST_WEATHER_LIST m_stWeatherList[24];
	std::list<ST_DB_WeatherColumn> *m_List_ST_Weather;
	ST_WEATHER_FROECAST m_stWeatherForecast;
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


