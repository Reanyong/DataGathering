#if !defined(AFX_ADO_CONTROL_H__78A93A77_1DB3_457F_99AD_E13038BF8189__INCLUDED_)
#define AFX_ADO_CONTROL_H__78A93A77_1DB3_457F_99AD_E13038BF8189__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Ado_Control.h : header file
//

#define _XTP_EXCLUDE_CALENDAR
#define _XTP_EXCLUDE_REPORTCONTROL

//ADO 사용

#pragma warning (disable: 4146)
#import <msado60_Backcompat.tlb> no_namespace rename("EOF", "EndOfFile")
#import <MSJRO.DLL> no_namespace rename("ReplicaTypeEnum", "_ReplicaTypeEnum")
#pragma warning (default: 4146)


#include "../OdbcControl/Odbc.h"

/*#pragma warning(push)
#pragma warning(disable:4146)
#import "c:\program files\common files\system\ado\msadox.dll"
#import "C:\Program files\Common files\system\ado\msado15.dll" no_namespace  rename("EOF","adoEOF") rename("BOF","adoBOF")
#pragma warning(pop)
*/
//WIN32,NDEBUG,_WINDOWS,_AFXDLL,_MBCS
/////////////////////////////////////////////////////////////////////////////
// CAdo_Control window
#define DB_MSSQL						(WM_USER + 100)
#define DB_ORACLE						(WM_USER + 101)
#define DB_MYSQL						(WM_USER + 102)
#define DB_POSTGRE						(WM_USER + 103) // 20200210 jsh -> Postgre 추가

#define DB_SUCCESS						0
#define DB_ERROR						1

// Query 실패시 2회 재 시도후 Return
#define	EXECUTE_QUERY_RETRY_COUNT	5

//DB 접속 정보
typedef struct _stDBAccessInfo_ {

	char szSite[64];
	char szServer[64];
	char szDB[64];
	char szID[64];
	char szPW[64];
	UINT  unDBTypeID;

	BOOL bSubUse;
	char szServer_sub[64];
	char szDB_sub[64];
	char szID_sub[64];
	char szPW_sub[64];
	UINT  unDBTypeID_sub;

	char szSITE_ID[64];
	char szProjectName[64];
	char szBldNam[64];
	char szProduct_code[256];
	int nRandom_Mod;

	BOOL bFileLog;
	BOOL bDisplay;
}ST_DBACCESS_INFO;

void _WriteLog(CString strLogPath,CString sMsg, int nLogFlag);

CString IntToStr(int nVal);
CString LongToStr(long lVal);
CString ULongToStr(unsigned long ulVal);
CString DblToStr(double dblVal, int ndigits = 20);
CString DblToStr(float fltVal);

class CAdo_Control : public CWnd
{
// Construction
public:
	CAdo_Control();
// Attributes
public:
	int m_nLogFlag;
	_ConnectionPtr pADO_Connect;
	_ConnectionPtr pADO_Connect_Sub;

	COdbc* codbc;			//20200218 나정호 추가 codbc 부분 추가
	COdbc* pCodbc_Sub;

protected:
	ST_DBACCESS_INFO m_stDBInfo;
	HWND m_MsgHwnd;
	UINT m_Message;
	CString m_strMsgName;
	CString m_strLogPath;

	bool m_isOdbcConnected;	//20200218 나정호 추가 codbc 연결 확인 부분 추가
	bool m_isOdbcConnected_sub;

// Operations
public:
	void DB_ConnectionInfo(const char* szSerVer,const char* szDBName,const char* szID,const char* szPW,UINT unDBTypeID);
	void DB_ConnectionInfo_Sub(const char* szSerVer_Sub,const char* szDBName_Sub,const char* szID_Sub,const char* szPW_Sub,UINT unDBTypeID_Sub);
	void DB_SetReturnMsg(UINT MsgID,HWND Hwnd, const char* szMegName, const char* szLogPath);
	BOOL DB_Connection(); //DB 접속
	BOOL DB_Close(); //DB 접속 해제

	BOOL DB_Connection_Sub(); //DB 접속 서브
	BOOL DB_Close_Sub(); //DB 접속 서브 해제
	int GetDB_ConnectionStatus(); //접속 상태

	int DB_ReConnection();
	int SetQueryRun(CString strQuery); //insert / Update;

	BOOL GetFieldValue(_RecordsetPtr pRecordSet,LPCTSTR lpFieldName, double& dbValue);
	BOOL GetFieldValue(_RecordsetPtr pRecordSet,LPCTSTR lpFieldName, CString& strValue, CString strDateFormat = _T(""));

	BOOL BeginTrans();
	BOOL CommitTrans();
	BOOL RollbackTrans();

	int Truncate(CString tableName);

	BOOL GetRecordCount(const CString& sql, long* count);
	_RecordsetPtr DB_OpenRecordSet(CString strQuery);

protected:

	int Com_Error(const char* szLogName,const _com_error &e);
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdo_Control)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAdo_Control();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAdo_Control)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADO_CONTROL_H__78A93A77_1DB3_457F_99AD_E13038BF8189__INCLUDED_)
