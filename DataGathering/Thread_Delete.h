#pragma once

class Thread_Delete : public CWinThread
{
	DECLARE_DYNCREATE(Thread_Delete)

public:
	Thread_Delete();
	virtual ~Thread_Delete();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();

	void StopThread();
	void SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody);

	CAdo_Control* DB_Connect;
	HWND m_WindHwnd;

	BOOL m_bEndThread;
	CString m_strLogTitle;
	int m_nDBType;
	int tryCount;

protected:
	DECLARE_MESSAGE_MAP()
};

