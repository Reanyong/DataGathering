#pragma once



// CThread_TAGSearch

class CThread_TAGSearch : public CWinThread
{
	DECLARE_DYNCREATE(CThread_TAGSearch)

protected:
	CThread_TAGSearch();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_TAGSearch();

public:
	void SetOwner(CWnd* hWnd) {
		m_pOwner = hWnd;
	}	
	void SetWnd(HWND hWnd) {
		m_WindHwnd = hWnd;
	}
	void Stop() {
		m_bEndThread = TRUE;
	}
	void SetLPVOID(LPVOID pCtrl)	{
		m_pCtrl = pCtrl;
	}
	BOOL GetStop() { return m_bEndThread;	}

	void GetSTTagListData(ST_ALLTAGLIST *pTagList)
	{
		m_tempTagList = pTagList;
	}

protected:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CString Com_Error(const char *szLogName,_com_error *e);
	void SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody);
	void SetWriteLogFile(const char *szLogMsg);
	int GetUnregisteredTag(int nDBType,const char *szDeviceItem,const char *szDBName);
	int GetRegisterTag(int nDBType,const char *szDeviceItem,const char *szDBName);
protected:
	CWnd* m_pOwner;
	LPVOID m_pCtrl;	
	BOOL m_bEndThread;
	HWND m_WindHwnd;
	int m_nTagListCount;
	
	CString m_strLogTitle;
	ST_ALLTAGLIST *m_tempTagList;

public:
	int m_nSearchType;
	CAdo_Control *DB_Connect;
	int m_nDBType;
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()	
};


