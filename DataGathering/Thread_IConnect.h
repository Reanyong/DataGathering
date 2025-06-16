#pragma once



// CThread_IConnect

class CThread_IConnect : public CWinThread
{
	DECLARE_DYNCREATE(CThread_IConnect)

protected:
	CThread_IConnect();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_IConnect();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetOwner(CWnd* hWnd) {
		m_pOwner = hWnd;
	}
	void SetWnd(HWND hWnd) {
		m_WindHwnd = hWnd;
	}
	void Stop() {
		m_bEndThread = TRUE;
	}
	void EndCheck() {
		m_bEndCheck = TRUE;
	}
	void SetProgramName(const char *szName)	{
		m_strThreadName = szName;
	}
	BOOL GetStop() { return m_bEndThread;	}
	BOOL GetRunState(){return m_bThreadRunState; }

	void SetLPVOID(LPVOID pCtrl)
	{
		m_pCtrl = pCtrl;
	}
protected:
	CWnd* m_pOwner;
	HWND m_WindHwnd;
	BOOL m_bEndThread;
	BOOL m_bEndCheck;
	LPVOID m_pCtrl;
	BOOL m_bThreadRunState;

	CAdo_Control *DB_Connect;
	CString m_strThreadName;
	BOOL m_bStartCheck;
	BOOL m_bStratSubThread;
	int m_nSiteCount;

	ST_SITE_INTERLOCK m_stSiteInterlock[24];
	ST_DATABASENAME m_stDataBaseName;

protected:
	void Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *sTitle,const char *szLogMsg);

	int GetSiteSearch(int nDBType);
	CString GetSiteSettingInfo(const char *szSiteID);
	//void Search(int nSiteCount);

	int ISmartConnect(int nSiteCount,CTime currentDay,int nDBType);
	BOOL GetDataParsing(CString strContent,const char *szSiteName,const char *szSiteId,CTime currentDay,int nDBType);
	int SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos);
	void TestModData(int nCount,CTime currentDay,int nDBType);
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


