#pragma once

#include "Thread_TagAlarmSub.h"


// CThread_TagAlarmMain

class CThread_TagAlarmMain : public CWinThread
{
	DECLARE_DYNCREATE(CThread_TagAlarmMain)

protected:
	CThread_TagAlarmMain();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_TagAlarmMain();

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
	BOOL m_bStartCheck;
	BOOL m_bStratSubThread;
	BOOL m_bButtonStop;
	int m_nDeviceTotal;
	int m_nDeviceCountTemp;
	int m_nThteadCount;

	ST_DEVICE_INFO m_stDeviceList[THREAD_MAX];
	CThread_TagAlarmSub **m_pThread_TagAlarmSub;

protected:
	void Release_DeviceList_ST_Tag(int nMode);
	void Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *sTitle,const char *szLogMsg);

	int GetSiteCheck(CTime currentTime);
	void StartSubThread(ST_SUBTHREAD_INFO *stSubThreadInfo,int nTotalDevice);
	void StopSubThread();
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()	
};


