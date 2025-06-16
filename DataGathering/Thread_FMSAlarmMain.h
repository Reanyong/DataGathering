#pragma once



// CThread_FMSAlarmMain

class CThread_FMSAlarmMain : public CWinThread
{
	DECLARE_DYNCREATE(CThread_FMSAlarmMain)

protected:
	CThread_FMSAlarmMain();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_FMSAlarmMain();

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
	void ButtonStop(){
		m_bButtonStop = TRUE;
	}
	void Stop() {
		_addCurrentstateMsg(0,0, m_strThreadName, "Processor 종료");
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
	CWnd* m_pOwner;
	HWND m_WindHwnd;
	BOOL m_bEndThread;
	BOOL m_bEndCheck;
	LPVOID m_pCtrl;

	CAdo_Control *DB_Connect;
	CString m_strThreadName;
	CString m_strLogTitle;
	BOOL m_bStartCheck;
	BOOL m_bStratSubThread;
	BOOL m_bButtonStop;
	int m_nDeviceTotal;

	int m_nDBType;

	std::list<ST_FMSAlarmList> *m_pstFMSAlarmList;
	std::list<ST_UMSSend_UserList> *m_pstUMSSend_UserList;
protected:
	CString Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *szLogMsg);
	void SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody);
	void Release_ST_List(int nMode);
	void Release_ST_UMS_List(int nMode);

	int SetQueryValue(CString strQuery,const char *szLogTitle,const char *szLogPos);
	int GetSite_ChangeFCTList();
	int GetFctAlarmSearch();
	BOOL GetAlarmOccursFMS_File(const char *szAlarmId,ST_ALARMCHECK *stAlarmCheck);
	BOOL SetAlarmOccursFMS_File(const char *szAlarmId, int nAlarmType ,const char *szStartTime, int nAckValue);
	BOOL SetAlarmOccursFMS_ClearFile(const char *szAlarmId); //해제된 알람은 삭제

	int GetAlarmOccursFMS_DB(const char *szAlarmId,const char *szStartTime);
	int GetAlarmUMSCheck(const char *szAlsrmLevel_ID);
	BOOL SetSUMSendOutput(ST_FMSAlarmList &stAlrmInfo,ST_UMSSend_UserList &stUmsSend,const char *szAlarmKind);
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


