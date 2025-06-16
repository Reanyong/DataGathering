#pragma once



// CThread_TagAlarmSub

class CThread_TagAlarmSub : public CWinThread
{
	DECLARE_DYNCREATE(CThread_TagAlarmSub)

protected:
	CThread_TagAlarmSub();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_TagAlarmSub();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetOwner(CWnd* hWnd) {		m_pOwner = hWnd;	}
	void SetWnd(HWND hWnd) {		m_WindHwnd = hWnd;	}
	void ButtonStop(){
		m_bButtonStop = TRUE;
	}

	void Stop() {		m_bEndThread = TRUE;	}
	void EndCheck() {		m_bEndCheck = TRUE;	}
	BOOL GetStop() { return m_bEndThread;	}
	void SetLPVOID(LPVOID pCtrl)	{		m_pCtrl = pCtrl;	}
	void InitInfo(ST_SUBTHREAD_INFO *pStSubInfo)	{
		m_nThreadNumber = pStSubInfo->nThreadNumber;
		m_strThreadName = pStSubInfo->szThreadName;
		memcpy(&m_stSubInfo,pStSubInfo,sizeof(ST_DEVICE_INFO));
	}
	void SetSubThreadPause(BOOL bPause)	{		m_bSubThreadPause = bPause;	}

protected:
	CWnd* m_pOwner;
	HWND m_WindHwnd;
	BOOL m_bEndThread;
	BOOL m_bEndCheck;
	LPVOID m_pCtrl;
	BOOL m_bButtonStop;

	int m_nThreadNumber;
	BOOL m_bSubThreadPause;

	CAdo_Control *DB_Connect;
	CString m_strThreadName;
	CString m_strProcessorTitle;
	ST_SUBTHREAD_INFO m_stSubInfo;
	std::list<ST_TagAlarmList> *m_pstTagAlarmList;
	std::list<ST_UMSSend_UserList> *m_pstUMSSend_UserList;
protected:
	void Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *sTitle,const char *szLogMsg);

	void Release_ST_List(int nMode);
	void Release_ST_UMS_List(int nMode);
	int GetAlarmRegisterListAI_DB(const char *szDeviceId);
	int GetAlarmRegisterListDI_DB(const char *szDeviceId);
	BOOL GetAlarmOccursAI_File(const char *szDeviceId, const char *szAlarmId, ST_ALARMCHECK *stAlarmCheck);
	BOOL GetAlarmOccursDI_File(const char *szDeviceId, const char *szAlarmId, ST_ALARMCHECK *stAlarmCheck);
	int GetAlarmOccurs_DB(const char *szAlarmId, const char *szStartTime);
	BOOL SetAlarmOccursAI_File(const char *szDeviceId, const char *szAlarmId, int nAlarmType, const char *szStartTime, float fAckValue);
	BOOL SetAlarmOccursDI_File(const char *szDeviceId, const char *szAlarmId, int nAlarmType, const char *szStartTime, float fAckValue);
	BOOL SetAlarmOccursAI_ClearFile(const char *szDeviceId, const char *szAlarmId); //해제된 알람은 삭제
	BOOL SetAlarmOccursDI_ClearFile(const char *szDeviceId, const char *szAlarmId); //해제된 알람은 삭제
	int SetQueryValue(CString strQuery, const char *szLogTitle, const char *szLogPos);

	void ShowAlarmOccursMsg(const char *szData1, const char *szData2, const char *szData3, const char *szData4, const char *szData5);
	int GetAlarmCheck_AI(const char *szDeviceId,CTime currentTime);
	int GetAlarmCheck_DI(const char *szDeviceId,CTime currentTime);

	int GetAlarmUMSCheck(const char *szAlsrmLevel_ID);
	BOOL SetSUMSendOutput(ST_TagAlarmList &stTagAlrmInfo,ST_UMSSend_UserList &stUmsSend,const char *szAlarmKind);
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


