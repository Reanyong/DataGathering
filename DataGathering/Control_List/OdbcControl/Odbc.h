// Odbc.h: interface for the COdbc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ODBC_H__277D1B98_B6D2_4996_8D15_CF5274FE6C2F__INCLUDED_)
#define AFX_ODBC_H__277D1B98_B6D2_4996_8D15_CF5274FE6C2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sqlext.h>
#include <sql.h>
#include "stdafx.h"

#define	IS_SQLSUCCESS(r)							((r == SQL_SUCCESS) || (r == SQL_SUCCESS_WITH_INFO))

class COdbc
{
public:
	COdbc(const char *szDsn, const char *szUid, const char *szPwd);
	virtual ~COdbc();

	int ResetHandle();
	int Connect();
	SQLRETURN SQLExecDirect(const char *szSql);
	SQLRETURN SQLExecute();
	void Close();
	SQLRETURN SQLAllocStmtHandle();
	SQLRETURN SQLFreeStmtHandle();
	SQLRETURN SQLFetch();
	SQLRETURN SQLGetData(SQLUSMALLINT ColumnNum, SQLSMALLINT Type, SQLPOINTER ValuePtr, SQLINTEGER BufferLen, SQLINTEGER *StrLen_or_IndPtr);
	SQLRETURN SQLNumResultCols(SQLSMALLINT *pColumnCount);
	void GetError(SQLSMALLINT HandleType, SQLHANDLE Handle);

	char* lmsSqlGetError( SQLSMALLINT HandleType, SQLINTEGER * pdwErrCode);
	SQLRETURN SQLRowCount(SQLLEN* RawCount);

	BOOL IsConnected() {
		return m_fConnection;
	}
	SQLHANDLE GetConnectionHandle()
	{
		return m_hDbc;
	}
	SQLHANDLE GetStmtHandle()
	{
		return m_hStmt;
	}
	SQLRETURN SQLPrepare(SQLCHAR *StatementText, SQLINTEGER TextLength);
	SQLRETURN SQLBindParameter(SQLUSMALLINT ParameterNumber, SQLSMALLINT InputOutputType, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLUINTEGER ColumnSize, SQLSMALLINT DecimalDigits, SQLPOINTER ParameterValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_IndPtr);
	SQLRETURN SQLBindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_IndPtr);

	CString GetErrorString() {
		return m_szLastError;
	}

protected:
	SQLHANDLE m_hEnv;
	SQLHANDLE m_hDbc;
	SQLHANDLE m_hStmt;

	CString m_szDsn;
	CString m_szUid;
	CString m_szPwd;

	CString m_szLastError;

	BOOL m_fConnection;
};

#endif // !defined(AFX_ODBC_H__277D1B98_B6D2_4996_8D15_CF5274FE6C2F__INCLUDED_)
