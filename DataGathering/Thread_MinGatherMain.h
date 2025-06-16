
#pragma once

// Thread_MinGatherMain.h : header file
//

#include "Thread_MinGatherSub.h"
#include "Thread_Delete.h"

/////////////////////////////////////////////////////////////////////////////
// CThread_MinGatherMain thread

class CThread_MinGatherMain : public CWinThread
{
	DECLARE_DYNCREATE(CThread_MinGatherMain)
protected:
	CThread_MinGatherMain();           // protected constructor used by dynamic creation

// Attributes
public:
	void SetOwner(CWnd* hWnd) {
		m_pOwner = hWnd;
	}
	void SetWnd(HWND hWnd) {
		m_WindHwnd = hWnd;
	}
	void ButtonStop(){
		m_bButtonStop = TRUE;
	}

	void Stop() {
		m_bEndThread = TRUE;
	}
	void SetLPVOID(LPVOID pCtrl)	{
		m_pCtrl = pCtrl;
	}
	BOOL GetStop() { return m_bEndThread;	}
	void SetProgramName(const char *szName)	{
		m_strThreadName = szName;
	}
	void SetProduct(int nProduct)	{ //20210305 ksw 제품선택 추가
		m_nProduct = nProduct;
	}
	void SetInterval(int nInterval)	{ //20210305 ksw 수집주기 추가
		m_nInterval = nInterval;
	}
protected:
	int GetTagTotalCountCheck(const char *szDeviceItem,int nDBType,const char *szDBName); //전체 TAG Count 정보 확인
	void Release_List_ST_DB_Tag(int nMode);
	int GetTagList(const char *szDeviceItem,int nTheradCount,int nDBType,const char *szDBName);  //등록된 TAG 정보
	CString Com_Error(const char *szLogName,_com_error *e);
	void SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody);

	void StartSubThread(ST_SUBTHREAD_INFO *stSubTheradInfo,int nTheradCount);
	void StopSubThread();

	void SetReSTListItem();

	void SetWriteLogFile(const char *szLogMsg);

	Thread_Delete* m_pThread_Delete;
	int DeleteThreadCount;

	void StartDeleteThread();
	void StopDeleteThread();
protected:
	CWnd* m_pOwner;
	LPVOID m_pCtrl;
	BOOL m_bButtonStop;
	BOOL m_bEndThread;
	HWND m_WindHwnd;
	CAdo_Control *DB_Connect;
	int m_nTagListCount;
	int m_nDBType;
	int m_nThteadCount;
	int m_nProduct; //20210305 ksw 제품선택 변수 추가 0: BEMS, 1:EMS(구 BEMS 포함)
	int m_nInterval; //20210831 ksw 수집 주기 설정
	CString m_strLogTitle;
	CString m_strThreadName;


	ST_SUBTHREAD_INFO *m_pstSubTherdInfo;
	std::list<ST_TagInfoList>** m_pList_ST_TagDivisionList;
	CThread_MinGatherSub** m_pThread_MinGahterSub;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThread_MinGatherMain)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CThread_MinGatherMain();

	// Generated message map functions
	//{{AFX_MSG(CThread_MinGatherMain)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

