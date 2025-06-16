// Odbc.cpp: implementation of the COdbc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../../DataGathering.h"
#include "Odbc.h"

#define DEFINE_ODBC_SIZE		100

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COdbc::COdbc(const char *szDsn, const char *szUid, const char *szPwd)
{
	m_hEnv = SQL_NULL_HENV;
	m_hDbc = SQL_NULL_HDBC;
	m_hStmt = SQL_NULL_HSTMT;

	m_szDsn = szDsn;
	m_szUid = szUid;
	m_szPwd = szPwd;

	m_fConnection = false;
}

COdbc::~COdbc()
{
	SQLFreeStmtHandle();
	if(IsConnected() == true)
	{
		Close();
	}

}
int COdbc::ResetHandle()
{
	m_hEnv = SQL_NULL_HENV;
	m_hDbc = SQL_NULL_HDBC;
	m_hStmt = SQL_NULL_HSTMT;

	return 0;
}
int COdbc::Connect()
{
	SQLRETURN sr;

	if (m_fConnection)
		return 1;

	// Memory allocation for environment handle
	sr = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if ((sr == SQL_INVALID_HANDLE) || (sr == SQL_ERROR))
	{
		GetError(SQL_HANDLE_ENV, m_hEnv);
		return -1;
	}

	sr = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);
	if ((sr == SQL_INVALID_HANDLE) || (sr == SQL_ERROR))
	{
		GetError(SQL_HANDLE_ENV, m_hEnv);
		Close();
		return -1;
	}

	// Memory allocation for connection handle
	sr = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
	if ((sr == SQL_INVALID_HANDLE) || (sr == SQL_ERROR))
	{
		GetError(SQL_HANDLE_ENV, m_hEnv);
		Close();
		return -1;
	}

	// Test SQL_LOGIN_TIMEOUT
	UDWORD pvParam = 1;
	sr = SQLSetConnectAttr(m_hDbc, SQL_LOGIN_TIMEOUT, (void*)&pvParam, sizeof(pvParam));
	if (sr != SQL_SUCCESS && sr != SQL_SUCCESS_WITH_INFO)
	{
		GetError(SQL_HANDLE_DBC, m_hDbc);
		Close();
		return -1;
	}

	sr = SQLConnect(m_hDbc,
		(SQLCHAR*)(const char*)m_szDsn,
		SQL_NTS,
		(SQLCHAR*)(const char*)m_szUid,
		SQL_NTS,
		(SQLCHAR*)(const char*)m_szPwd,
		SQL_NTS);
	if ((sr == SQL_INVALID_HANDLE) || (sr == SQL_ERROR))
	{
		GetError(SQL_HANDLE_DBC, m_hDbc);
		Close();
		return -1;
	}

	m_fConnection = TRUE;

	return 1;
}

SQLRETURN COdbc::SQLAllocStmtHandle()
{
	SQLFreeStmtHandle();

	return SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
}


SQLRETURN COdbc::SQLExecDirect(const char *szSql)
{
	SQLRETURN sr;

	sr = ::SQLExecDirect( m_hStmt,
		(SQLCHAR *)szSql,
		SQL_NTS );
	if (sr != SQL_SUCCESS)
	{
		GetError( SQL_HANDLE_STMT, m_hStmt );
	}

	return sr;
}

SQLRETURN COdbc::SQLFreeStmtHandle()
{
	SQLRETURN sr;

	if ( m_hStmt != SQL_NULL_HSTMT )
	{
		sr = SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
		m_hStmt = SQL_NULL_HSTMT;

		return sr;
	}

	return SQL_SUCCESS;
}

SQLRETURN COdbc::SQLPrepare(SQLCHAR *StatementText, SQLINTEGER TextLength)
{
	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	return ::SQLPrepare(m_hStmt, StatementText, SQL_NTS);
}

SQLRETURN COdbc::SQLBindParameter(SQLUSMALLINT ParameterNumber,
	SQLSMALLINT InputOutputType,
	SQLSMALLINT ValueType,
	SQLSMALLINT ParameterType,
	SQLUINTEGER ColumnSize,
	SQLSMALLINT DecimalDigits,
	SQLPOINTER ParameterValuePtr,
	SQLINTEGER BufferLength,
	SQLINTEGER *StrLen_or_IndPtr)
{
	SQLRETURN sr;

	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	sr = ::SQLBindParameter(m_hStmt, ParameterNumber, InputOutputType, ValueType, ParameterType, ColumnSize, DecimalDigits, ParameterValuePtr, BufferLength, StrLen_or_IndPtr);

	if( sr != SQL_SUCCESS )
	{
		GetError(SQL_HANDLE_STMT, m_hStmt);
	}

	return sr;
}

SQLRETURN COdbc::SQLExecute()
{
	SQLRETURN sr;

	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	sr =  ::SQLExecute(m_hStmt);

	if( sr != SQL_SUCCESS )
	{
		GetError(SQL_HANDLE_STMT, m_hStmt);
	}

	return sr;
}

SQLRETURN COdbc::SQLFetch()
{
	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	return ::SQLFetch(m_hStmt);
}

SQLRETURN COdbc::SQLGetData(SQLUSMALLINT ColumnNum, SQLSMALLINT Type, SQLPOINTER ValuePtr, SQLINTEGER BufferLen, SQLINTEGER *StrLen_or_IndPtr)
{
	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	return ::SQLGetData(m_hStmt, ColumnNum, Type, ValuePtr, BufferLen, StrLen_or_IndPtr);
}

SQLRETURN COdbc::SQLNumResultCols(SQLSMALLINT *pColumnCount)
{
	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	return ::SQLNumResultCols(m_hStmt, pColumnCount);
}

void COdbc::Close()
{
	SQLRETURN sr;

	SQLFreeStmtHandle();

	if (m_fConnection)
	{
		SQLEndTran(SQL_HANDLE_DBC, m_hDbc, SQL_COMMIT);
		SQLDisconnect(m_hDbc);
		m_fConnection = FALSE;
	}

	if (m_hDbc != SQL_NULL_HDBC)
	{
		sr = SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
		m_hDbc = SQL_NULL_HDBC;
	}

	if (m_hEnv != SQL_NULL_HENV)
	{
		sr = SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = SQL_NULL_HENV;
	}
}

SQLRETURN COdbc::SQLBindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_IndPtr)
{
	if (SQL_NULL_HSTMT == m_hStmt)
		return SQL_ERROR;

	return ::SQLBindCol(m_hStmt, ColumnNumber, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPtr);
}

void COdbc::GetError(SQLSMALLINT HandleType, SQLHANDLE Handle)
{
	SWORD sMsgNum = 1;
	SWORD wErrMsgLen; // Error message length
	SQLINTEGER dwErrCode; // Native Error code
	UCHAR szErrState[SQL_SQLSTATE_SIZE+1] = { 0, }; // SQL Error State string
	UCHAR szErrText[SQL_MAX_MESSAGE_LENGTH+1] = { 0, }; // SQL Error Text string

	SQLRETURN retcode = SQLGetDiagRec(HandleType,
		Handle,
		sMsgNum++,
		szErrState,
		&dwErrCode,
		szErrText,
		SQL_MAX_MESSAGE_LENGTH-1,
		&wErrMsgLen);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		CString s;
		s.Format("Error :: %s, %s", szErrState, szErrText);
		TRACE("%s\n", s);

		s.Remove('\r');
		s.Remove('\n');

		m_szLastError = s;
		//_dump(s, s.GetLength());
	}
}

char* COdbc::lmsSqlGetError( SQLSMALLINT HandleType, SQLINTEGER * pdwErrCode)
{
	SQLHANDLE Handle;
	Handle = m_hDbc;

	SWORD sMsgNum= 1;
	SWORD wErrMsgLen; // Error message length
	//SQLINTEGER dwErrCode; // Native Error code
	UCHAR szErrState[SQL_SQLSTATE_SIZE+1]= { 0, }; // SQL Error State string
	UCHAR *szErrText=NULL;
	szErrText = new UCHAR[SQL_MAX_MESSAGE_LENGTH]; // SQL Error Text string
	SQLRETURN retcode= SQLGetDiagRec(	HandleType,
		Handle,
		sMsgNum++,
		szErrState,
		pdwErrCode,
		szErrText,
		SQL_MAX_MESSAGE_LENGTH-1,
		&wErrMsgLen);
	if( retcode== SQL_SUCCESS || retcode== SQL_SUCCESS_WITH_INFO){
		//리턴된_에러코드 = dwErrCode;
		//에러상태_문자 = szErrState;
		//에러_메시지 = szErrText;
		//TRACE( "Odbc Error :: %s, %s\n", szErrState, szErrText );
	}

	//lmsDataFlowLog("ODBC Error Message", strlen((char *)szErrText), (char *)szErrText, 1);

	return (char *)szErrText;
}

SQLRETURN COdbc::SQLRowCount(SQLLEN * RawCount)
{
	return ::SQLRowCount(m_hStmt,RawCount);
}
