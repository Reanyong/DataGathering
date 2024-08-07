#pragma once



// CThread_Main

class CThread_Main : public CWinThread
{
	DECLARE_DYNCREATE(CThread_Main)

protected:
	CThread_Main();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_Main();

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
	void SetLPVOID(LPVOID pCtrl)
	{
		m_pCtrl = pCtrl;
	}
protected:
	BOOL GetFileAttributeCheck(CTime currentTime,CString strPathName);
	void SetOutPutMsg(CString strTitle,CString strMsg,COLORREF msgColor);
	void SetWriteLogFile(CString strLogMsg);
	void GetCurrentStateMsg();
public:
protected:
	CWnd* m_pOwner;
	HWND m_WindHwnd;
	BOOL m_bEndThread;
	LPVOID m_pCtrl;

	CString m_strLogTitle;

	CTime m_DatawriteTimeCheck;

protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


