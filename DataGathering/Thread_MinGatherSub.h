#if !defined(AFX_THREAD_MINDATA_H__FE94CDE3_80D8_4ED9_9DB7_1E979D5DE20E__INCLUDED_)
#define AFX_THREAD_MINDATA_H__FE94CDE3_80D8_4ED9_9DB7_1E979D5DE20E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Thread_MinData.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CThread_MinGatherSub thread

class CThread_MinGatherSub : public CWinThread
{
	DECLARE_DYNCREATE(CThread_MinGatherSub)
protected:
	CThread_MinGatherSub();           // protected constructor used by dynamic creation
public:
	virtual ~CThread_MinGatherSub();
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
	void InitInfo(int nThreadNumber,CString strThreadID,int nProduct, int nInterval)	{ //20210308 ksw 제품번호 20210831 ksw 수집주기
		m_nThreadNumber = nThreadNumber;
		m_strThreadName = strThreadID;
		m_nProduct = nProduct;
		m_nInterval = nInterval;
	}
	void SetSubThreadPause(BOOL bPause)
	{
		m_bSubThreadPause = bPause;
	}
	void SetSTListData(std::list<ST_TagInfoList> *List_S_TagList)
	{
		m_List_ST_Tag = List_S_TagList;
		//CString s;
		//int nCnt = m_List_ST_Tag->size();
		//s.Format("Sub Thread m_List_ST_Tag Cnt : %d",nCnt);
		//SetWriteLogFile(s);
	}
protected:

	CString Com_Error(const char* szLogName,_com_error *e);
	void Release_List_ST_Tag(int nMode);

	int GetMinRegCheck(CString strTagId,CTime currentTime,const char *szDBName,int nDBType);
	int SetTagValue(int nQueryType,int nDBType,CString strTagId,CString strTagName,CString strGroupName,int nTagTyp, CTime currentTime,CString strValue,const char *szDBName);

	int SetTagValue(int nQueryType,int nDBType,ST_TagInfoList stList,CTime currentTime,CString strValue,const char *szDBName); //향후 변경

	CString GetTagValue(CString strTagName,int nTagTyp);

	void LogHistory(CString strLogName,CString strMsg,int nColorType); //로창 데이터 표시
	void SetWriteLogFile(const char *szLogMsg); //로그 파일 생성
	void ShowGridDataOutPut(CString strStatus,CString strMsg); //그리드 상태 표시

protected:
	CWnd* m_pOwner;
	LPVOID m_pCtrl;
	BOOL m_bButtonStop;
	BOOL m_bEndThread;
	BOOL m_bSubThreadPause;
	HWND m_WindHwnd;
	CAdo_Control *DB_Connect;
	int m_nDBType;
	int m_nProduct; //20210308 ksw 제품버전
	int m_nInterval;

	int m_nThreadNumber;
	CString m_strLogTitle;
	CString m_strThreadName;
	std::list<ST_TagInfoList> *m_List_ST_Tag;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThread_MinGatherSub)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:


	// Generated message map functions
	//{{AFX_MSG(CThread_MinGatherSub)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THREAD_MINDATA_H__FE94CDE3_80D8_4ED9_9DB7_1E979D5DE20E__INCLUDED_)
