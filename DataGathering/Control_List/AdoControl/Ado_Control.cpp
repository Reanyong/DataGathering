// Ado_Control.cpp : implementation file
//

#include "stdafx.h"
#include "../../DataGathering.h"
#include "Ado_Control.h"


#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdo_Control

CAdo_Control::CAdo_Control()
{
	pADO_Connect = NULL;
	m_Message = 0;
	m_MsgHwnd = m_hWnd;
	m_strMsgName = "";
	memset(&m_stDBInfo,0x00,sizeof(m_stDBInfo));
	m_strLogPath = "";
	m_nLogFlag = 0;
	//20200225 나정호 추가 ODBC구동에 필요한 변수 초기화
	m_isOdbcConnected = false;
	m_isOdbcConnected_sub = false;
	codbc = NULL;
	pCodbc_Sub = NULL;
}

CAdo_Control::~CAdo_Control()
{
	//20200224 나정호 ODBC 해제 추가
	if(m_stDBInfo.unDBTypeID == DB_POSTGRE)
	{
		if( codbc != NULL )
		{
			delete codbc;
			codbc = NULL;
		}
		if( pCodbc_Sub!= NULL)
		{
			delete pCodbc_Sub;
			pCodbc_Sub = NULL;
		}
	}
}


BEGIN_MESSAGE_MAP(CAdo_Control, CWnd)
	//{{AFX_MSG_MAP(CAdo_Control)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAdo_Control message handlers


int	CAdo_Control::GetDB_ConnectionStatus()
{
	if(m_stDBInfo.unDBTypeID == DB_POSTGRE )
	{
		if( codbc != NULL)
		{
			if ( m_isOdbcConnected == true )
			{
				if(codbc->IsConnected())
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
	}

	if(pADO_Connect != NULL)
	{
		if(pADO_Connect->State == adStateOpen)
		{
			return 1; //정상 접속중
		}
		else
		{
			return -1; //비정상
		}
	}
	return 0;
}

void CAdo_Control::DB_SetReturnMsg(UINT MsgID, HWND Hwnd, const char* szMegName, const char* szLogPath)
{
	m_Message = MsgID;
	m_MsgHwnd = Hwnd;
	m_strMsgName = szMegName;

	char path[512] = { 0, };
	if (strlen(szLogPath) != 0)
		m_strLogPath.Format("%s\\LOG", szLogPath);
	else
	{
		GetCurrentDirectory(sizeof(path), path);
		m_strLogPath.Format("%s\\DB_Log", path);
	}
}

void CAdo_Control::DB_ConnectionInfo(const char* szSerVer,const char* szDBName,
	const char* szID,const char* szPW,UINT unDBTypeID)
{

	strcpy_s(m_stDBInfo.szServer,szSerVer);
	strcpy_s(m_stDBInfo.szDB, szDBName);
	strcpy_s(m_stDBInfo.szID, szID);
	strcpy_s(m_stDBInfo.szPW, szPW);
	m_stDBInfo.unDBTypeID = unDBTypeID;

}

void CAdo_Control::DB_ConnectionInfo_Sub(const char* szSerVer_Sub,const char* szDBName_Sub,
	const char* szID_Sub,const char* szPW_Sub,UINT unDBTypeID_Sub)
{
	strcpy_s(m_stDBInfo.szServer_sub, szSerVer_Sub);
	strcpy_s(m_stDBInfo.szDB_sub, szDBName_Sub);
	strcpy_s(m_stDBInfo.szID_sub, szID_Sub);
	strcpy_s(m_stDBInfo.szPW_sub, szPW_Sub);
	m_stDBInfo.unDBTypeID_sub = unDBTypeID_Sub;
}

int CAdo_Control::Com_Error(const char* szLogName,const _com_error &e)
{
	CString strErrorCode = "";
	CString strRunlog_E2 = "";
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	strRunlog_E2.Format("%s- Error Code:[%08lx], Code meaning:[%s], Source:[%s], Description:[%s] ",
		szLogName, e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);
#ifdef _DEBUG
	TRACE("Ado-Control(%s) - catch Com Error = %s\n",m_strMsgName, strRunlog_E2);
#endif

	::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);

	strErrorCode.Format("%08lx",e.Error());
	if("80040e14" == strErrorCode || "80004005" == strErrorCode )
	{
		return -100;
	}
	else
		return -1;
}


BOOL CAdo_Control::DB_Connection()
{
	CString strRunlog_E2 = "";

	_bstr_t  soracle(L""), strMissing(L"");
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;
	CString strDBType = "";

	try
	{
		DB_Close();

		HRESULT nResultADO_DB;

		nResultADO_DB = pADO_Connect.CreateInstance("ADODB.Connection");
		if(m_stDBInfo.unDBTypeID == DB_MSSQL)
		{
			soracle ="Provider=SQLOLEDB.1;Persist Security Info=True;Data Source="+(_bstr_t)m_stDBInfo.szServer+";";
			soracle +="Initial Catalog="+(_bstr_t)m_stDBInfo.szDB+";USER ID="+(_bstr_t)m_stDBInfo.szID+";Password="+(_bstr_t)m_stDBInfo.szPW+";";
			strDBType = "MS-SQL";
		}
		else if(m_stDBInfo.unDBTypeID == DB_ORACLE)
		{
			soracle = "Provider=OraOLEDB.Oracle;User Id="+ (_bstr_t)m_stDBInfo.szID +
				";Password=" + (_bstr_t)m_stDBInfo.szPW + ";Data Source="+ (_bstr_t)m_stDBInfo.szDB;
			strDBType = "ORACLE";
		}
		//20200210 jsh -> postgre 추가로 인한 provide 추가
		else if(m_stDBInfo.unDBTypeID == DB_POSTGRE)
		{
			/*수정해야함*/
			//soracle = "Provider=PGNP.1;Password=ver30;Persist Security Info=True;User ID=postgres;Initial Catalog=postgres;Data Source=192.168.123.75;Extended Properties=\"SSL=prefer;\"";
			/*
			soracle = "Provider=PGNP.1;Password="+(_bstr_t)m_stDBInfo.szPW +";Persist Security Info=True;User ID=" + (_bstr_t)m_stDBInfo.szID +
			";Initial Catalog=" + (_bstr_t)m_stDBInfo.szDB+ ";Data Source=" +(_bstr_t)m_stDBInfo.szServer+ ";Extended Properties=\"SSL=prefer;\"";
			*/
 			soracle = "postgres";//; SERVER=192.168.123.75; PORT=5432";//; UID=postgres; PWD=ver30;";
			codbc = new COdbc(soracle,(_bstr_t)m_stDBInfo.szID,(_bstr_t)m_stDBInfo.szPW);
			strDBType = "POSTGRE";
		}
		else
		{
			strDBType = "MY-SQL";
		}

		if(strDBType != "POSTGRE")
		{

			pADO_Connect->ConnectionString = soracle;
			pADO_Connect->CursorLocation = adUseClient;

			pADO_Connect->ConnectionTimeout = 60;
			pADO_Connect->CommandTimeout = 120;


			nResultADO_DB = pADO_Connect->Open(strMissing,strMissing,strMissing,-1);



			if(pADO_Connect->State == adStateOpen)
			{
				strRunlog_E2.Format("Call Class Name : [%s], DataBase Type : [%s], DBName : [%s], 접속 상태 : [정상]",m_strMsgName,strDBType,m_stDBInfo.szDB);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_SUCCESS);
#ifdef _DEBUG
				TRACE("Ado-Control(%s) - DB 접속성공. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif
				//WriteLog(strRunlog_E2);
				_WriteLog(m_strLogPath,strRunlog_E2,m_nLogFlag);




				return TRUE;
			}
			else
			{
				strRunlog_E2.Format("DataBase Type:[%s], DBName:[%s], 접속 상태:[실패], Call Class Name:[%s]",strDBType,m_stDBInfo.szDB,m_strMsgName);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
#ifdef _DEBUG
				TRACE("Ado-Control(%s) - DB 접속실패. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif

				//WriteLog(strRunlog_E2);
				_WriteLog(m_strLogPath,strRunlog_E2,m_nLogFlag);
				return FALSE;
			}

		}
		else
		{
			nResultADO_DB = codbc ->Connect();

			if(nResultADO_DB == 1 )
			{
				strRunlog_E2.Format("Call Class Name : [%s], DataBase Type : [%s], DBName : [%s], 접속 상태 : [정상]",m_strMsgName,strDBType,m_stDBInfo.szDB);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_SUCCESS);
#ifdef _DEBUG
				TRACE("ODBC-Control(%s) - DB 접속성공. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif
				//WriteLog(strRunlog_E2);
				_WriteLog(m_strLogPath,strRunlog_E2,m_nLogFlag);
				m_isOdbcConnected = true;

				return TRUE;

			}
			else
			{
				strRunlog_E2.Format("DataBase Type:[%s], DBName:[%s], 접속 상태:[실패], Call Class Name:[%s]",strDBType,m_stDBInfo.szDB,m_strMsgName);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
#ifdef _DEBUG
				TRACE("ODBC-Control(%s) - DB 접속실패. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif

				//WriteLog(strRunlog_E2);
				_WriteLog(m_strLogPath,strRunlog_E2,m_nLogFlag);
				return FALSE;

			}
		}
	}
	catch (_com_error &e)
	{

		Com_Error("",e);


		if(m_stDBInfo.unDBTypeID == DB_POSTGRE)
		{
			try
			{
				if(codbc != NULL)
				{
					delete codbc;
					codbc = NULL;
					m_isOdbcConnected = false;
				}
			}
			catch(...)
			{
				delete codbc;
				codbc = NULL;
				m_isOdbcConnected = false;
			}

		}
		else
		{
			try
			{
				if(pADO_Connect)
				{
					if(pADO_Connect->State == adStateOpen)
					{
						pADO_Connect->Close();
						pADO_Connect.Release();
						pADO_Connect=NULL;
					}
				}
			}
			catch(...)
			{
				pADO_Connect = NULL;
			}

		}

		return FALSE;
	}
	catch (...)
	{
		strRunlog_E2.Format("Event Sql Db Open Fail!: Call Class Name : " + m_strMsgName);
		::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
		//==================================
		if(m_stDBInfo.unDBTypeID == DB_POSTGRE)
		{
#ifdef _DEBUG
			TRACE("ODBC-Control(%s) - %s \n",m_strMsgName,strRunlog_E2);
#endif
			try
			{
				if(codbc != NULL)
				{
					delete codbc;
					codbc = NULL;
					m_isOdbcConnected  = FALSE;

				}
			}
			catch(...)
			{
				delete codbc;
				codbc = NULL;
				m_isOdbcConnected  = FALSE;
			}

		}
		//================================
		else
		{

#ifdef _DEBUG
			TRACE("Ado-Control(%s) - %s \n",m_strMsgName,strRunlog_E2);
#endif
			try
			{
				if(pADO_Connect)
				{
					if(pADO_Connect->State == adStateOpen)
					{
						pADO_Connect->Close();
						pADO_Connect.Release();
						pADO_Connect=NULL;
					}
				}
			}
			catch(...)
			{
				pADO_Connect = NULL;
			}
			return FALSE;
		}


	}
	return TRUE;
}

BOOL CAdo_Control::DB_Close()
{
	/* ------ */
	if(m_stDBInfo.unDBTypeID == DB_POSTGRE)
	{
		try
		{
			if( codbc != NULL)
			{
				delete codbc;
				codbc = NULL;
				m_isOdbcConnected = false;
			}
		}
		catch(...)
		{
			delete codbc;
			codbc = NULL;
			m_isOdbcConnected = false;
		}

		m_isOdbcConnected = false;

		return TRUE;
	}
	/* ------ */
	else
	{
		try
		{
			/*
			const char* szSvrName;
			const char* szDBName;
			const char* szID;
			const char* szPW;
			UINT unDBTypeID ;

			pADO_Connect->DB_ConnectionInfo(szSvrName,szDBName,szID, szPW, unDBTypeID );

			szSvrName;
			//if( szSvrName ==  )
			*/

			if(pADO_Connect)
			{
				if(pADO_Connect->State == adStateOpen)
				{
					pADO_Connect->Close();
#ifdef _DEBUG
					TRACE("Ado-Control(%s) - ADO 접속 해제 및 소멸 \n",m_strMsgName);
#endif
				}

				pADO_Connect.Release();
				pADO_Connect = NULL;
			}
		}
		catch(...)
		{
			pADO_Connect.Release();
			pADO_Connect = NULL;
		}

	}

	return TRUE;
}

BOOL CAdo_Control::DB_Connection_Sub()
{
	CString strRunlog_E2 = "";

	_bstr_t  soracle(L""), strMissing(L"");
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strDBType = "";

	try
	{
		DB_Close_Sub();

		HRESULT nResultADO_DB;

		nResultADO_DB = pADO_Connect.CreateInstance("ADODB.Connection");
		if(m_stDBInfo.unDBTypeID_sub == DB_MSSQL)
		{
			soracle ="Provider=SQLOLEDB.1;Persist Security Info=True;Data Source="+(_bstr_t)m_stDBInfo.szServer_sub+";";
			soracle +="Initial Catalog="+(_bstr_t)m_stDBInfo.szDB_sub+";USER ID="+(_bstr_t)m_stDBInfo.szID_sub+";Password="+(_bstr_t)m_stDBInfo.szPW_sub+";";
			strDBType = "MS-SQL";
		}
		else if(m_stDBInfo.unDBTypeID_sub == DB_ORACLE)
		{
			soracle = "Provider=OraOLEDB.Oracle;User Id="+ (_bstr_t)m_stDBInfo.szID_sub +
				";Password=" + (_bstr_t)m_stDBInfo.szPW_sub + ";Data Source="+ (_bstr_t)m_stDBInfo.szDB_sub;
			strDBType = "ORACLE";
		}
		else if(m_stDBInfo.unDBTypeID_sub == DB_POSTGRE)
		{
			/*수정해야함*/
			//soracle = "Provider=PGNP.1;Password=ver30;Persist Security Info=True;User ID=postgres;Initial Catalog=postgres;Data Source=192.168.123.75;Extended Properties=\"SSL=prefer;\"";
			//soracle = "Provider=PGNP.1;Password="+(_bstr_t)m_stDBInfo.szPW +";Persist Security Info=True;User ID=" + (_bstr_t)m_stDBInfo.szID +
			//	";Initial Catalog=" + (_bstr_t)m_stDBInfo.szDB+ ";Data Source=" +(_bstr_t)m_stDBInfo.szServer+ ";Extended Properties=\"SSL=prefer;\"";

			soracle = "postgres";
			pCodbc_Sub = new COdbc(soracle,(_bstr_t)m_stDBInfo.szID,(_bstr_t)m_stDBInfo.szPW);

			strDBType = "POSTGRE";
		}
		else
		{
			strDBType = "MY-SQL";
		}

		if(strDBType != "POSTGRE")
		{
			pADO_Connect->ConnectionString = soracle;
			pADO_Connect->CursorLocation = adUseClient;

			pADO_Connect->ConnectionTimeout = 60;
			pADO_Connect->CommandTimeout = 120;

			nResultADO_DB = pADO_Connect->Open(strMissing,strMissing,strMissing,-1);

			if(pADO_Connect->State == adStateOpen)
			{
				strRunlog_E2.Format("DB-Sub 접속성공. Call Class Name : %s",m_strMsgName);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_SUCCESS);
				//WriteLog(strRunlog_E2);
#ifdef _DEBUG
				TRACE("Ado-Control-Sub(%s) - DB 접속성공. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif
				return TRUE;
			}
			else
			{

				strRunlog_E2.Format("DB-Sub 접속실패. Call Class Name : %s",m_strMsgName);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
#ifdef _DEBUG
				TRACE("Ado-Control-Sub(%s) - DB 접속실패. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif
				return FALSE;
			}
		}
		else
		{
			try
			{
				if(nResultADO_DB == 1 )
				{
					strRunlog_E2.Format("DB-Sub 접속성공. Call Class Name : %s",m_strMsgName);
					::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_SUCCESS);
					//WriteLog(strRunlog_E2);
#ifdef _DEBUG
					TRACE("ODBC-Control-Sub(%s) - DB 접속성공. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif
					m_isOdbcConnected_sub = true;
					return TRUE;

				}
				else
				{
					strRunlog_E2.Format("DB-Sub 접속실패. Call Class Name : %s",m_strMsgName);
					::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
#ifdef _DEBUG
					TRACE("ODBC-Control-Sub(%s) - DB 접속실패. Call Class Name : %s \n",m_strMsgName,strRunlog_E2);
#endif
					return FALSE;

				}
			}
			catch (_com_error &e)
			{
				Com_Error("",e);
				try
				{
					if(pCodbc_Sub != NULL)
					{
						if(m_isOdbcConnected_sub != FALSE)
						{
							delete pCodbc_Sub;
							pCodbc_Sub = NULL;


						}
					}
					m_isOdbcConnected_sub = false;
				}
				catch(...)
				{
					if(pCodbc_Sub != NULL )
					{
						delete pCodbc_Sub;
						pCodbc_Sub = NULL;
						m_isOdbcConnected_sub = false;
					}
				}

				return FALSE;
			}
			catch(...)
			{
				strRunlog_E2.Format("Event Sql Db Open Fail!: Call Class Name : " + m_strMsgName);
				::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
				//WriteLog("CDataBaseControl DB_Access() : Call Class Name : " + m_strThreadName  + " " + strRunlog_E2, _LOG_MAIN);

				try
				{
					if(codbc != NULL)
					{
						delete pCodbc_Sub;
						pCodbc_Sub = NULL;
						m_isOdbcConnected_sub = false;
					}
				}
				catch(...)
				{
					delete pCodbc_Sub;
					pCodbc_Sub = NULL;
					m_isOdbcConnected_sub = false;
				}

				return FALSE;
			}
		}
	}
	catch (_com_error &e)
	{
		if( m_stDBInfo.unDBTypeID_sub == DB_POSTGRE )
		{
			Com_Error("",e);
			try
			{
				if(pCodbc_Sub != NULL)
				{
					if(m_isOdbcConnected_sub != FALSE)
					{
						delete pCodbc_Sub;
						pCodbc_Sub = NULL;

					}

					m_isOdbcConnected_sub = false;
				}
			}
			catch(...)
			{
				if(pCodbc_Sub != NULL )
				{
					delete pCodbc_Sub;
					pCodbc_Sub = NULL;
					m_isOdbcConnected_sub = false;
				}
			}

		}
		else
		{
			Com_Error("",e);
			try
			{
				if(pADO_Connect)
				{
					if(pADO_Connect->State == adStateOpen)
					{
						pADO_Connect->Close();
						pADO_Connect.Release();
						pADO_Connect=NULL;
					}
				}
			}
			catch(...)
			{
				pADO_Connect = NULL;
			}
		}

		return FALSE;
	}
	catch (...)
	{
		strRunlog_E2.Format("Event Sql Db Open Fail!: Call Class Name : " + m_strMsgName);
		::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
		//WriteLog("CDataBaseControl DB_Access() : Call Class Name : " + m_strThreadName  + " " + strRunlog_E2, _LOG_MAIN);

		if( m_stDBInfo.unDBTypeID_sub == DB_POSTGRE )
		{
			try
			{
				if(pCodbc_Sub != NULL)
				{
					if(m_isOdbcConnected_sub != FALSE)
					{
						delete pCodbc_Sub;
						pCodbc_Sub = NULL;

						m_isOdbcConnected_sub = false;


					}
				}
			}
			catch(...)
			{
				if(pCodbc_Sub != NULL)
				{
					delete pCodbc_Sub;
					pCodbc_Sub = NULL;
					m_isOdbcConnected_sub = false;
				}
			}
		}
		else
		{
			try
			{
				if(pADO_Connect)
				{
					if(pADO_Connect->State == adStateOpen)
					{
						pADO_Connect->Close();
						pADO_Connect.Release();
						pADO_Connect=NULL;
					}
				}
			}
			catch(...)
			{
				pADO_Connect = NULL;
			}

		}

		return FALSE;
	}
	return TRUE;
}

BOOL CAdo_Control::DB_Close_Sub()
{

	//20200220 나정호 수정
	if(m_stDBInfo.unDBTypeID_sub == DB_POSTGRE)
	{
		try
		{
			if(pCodbc_Sub != NULL )
			{
				delete pCodbc_Sub;
				pCodbc_Sub = NULL;
			}

			m_isOdbcConnected_sub = false;
			return true;
		}
		catch(...)
		{
			if(pCodbc_Sub != NULL )
			{
				delete pCodbc_Sub;
				pCodbc_Sub = NULL;
				m_isOdbcConnected_sub = false;
			}

		}
		return TRUE;
	}
	else
	{
		try
		{
			if(pADO_Connect)
			{
				if(pADO_Connect->State == adStateOpen)
				{
					pADO_Connect->Close();
#ifdef _DEBUG
					TRACE("Ado-Control_Sub(%s) - ADO 접속 해제 및 소멸 \n",m_strMsgName);
#endif
				}

				pADO_Connect.Release();
				pADO_Connect = NULL;
			}
		}
		catch(...)
		{
			pADO_Connect.Release();
			pADO_Connect = NULL;
		}
		return TRUE;
	}



}


int CAdo_Control::DB_ReConnection()
{
	CString strRunlog_E2;
	DB_Close();

	for(int nCount = 0; nCount < EXECUTE_QUERY_RETRY_COUNT ; nCount++)
	{
		strRunlog_E2.Format("ReConnection : [시도중...],처리횟수 : [%d]", nCount + 1);
		::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
		if(DB_Connection())
		{
			return 1;
		}
		Sleep(10);
	}
	return 0;
}

BOOL CAdo_Control::GetFieldValue(_RecordsetPtr pRecordSet,LPCTSTR lpFieldName, double& dbValue)
{
	CString strRunlog_E2 ="";
	double val = (double)NULL;
	_variant_t vtFld;


	try
	{
		//vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		vtFld = pRecordSet->GetCollect(lpFieldName);

		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			//Corrected by Jos?Carlos Mart?ez Gal?
			val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow((double)10, vtFld.decVal.scale);
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_CY:   //Added by John Andy Johnson!!!!
			vtFld.ChangeType(VT_R8);
			val = vtFld.dblVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.dblVal;
		}
		dbValue = val;

		vtFld.Clear();

		return TRUE;
	}
	catch(_com_error &e)
	{
		Com_Error("GetFieldValue",e);
		return FALSE;
	}




}

BOOL CAdo_Control::GetFieldValue(_RecordsetPtr pRecordSet,LPCTSTR lpFieldName, CString& strValue, CString strDateFormat)
{
	CString str = "";
	_variant_t vtFld;
	CString strRunlog_E2 = "";

	try
	{
		//vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		vtFld = pRecordSet->GetCollect(lpFieldName);

		switch(vtFld.vt)
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr(vtFld.dblVal);
			break;
		case VT_BSTR:
			str = vtFld.bstrVal;
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_INT:
			str = IntToStr(vtFld.intVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_UI4:
			str = ULongToStr(vtFld.ulVal);
			break;
		case VT_DECIMAL:
			{
				//Corrected by Jos?Carlos Mart?ez Gal?
				double val = vtFld.decVal.Lo32;
				val *= (vtFld.decVal.sign == 128)? -1 : 1;
				val /= pow((double)10, vtFld.decVal.scale);
				str = DblToStr(val);
			}
			break;
		case VT_DATE:
			{
				COleDateTime dt(vtFld);

				if(strDateFormat.IsEmpty())
					strDateFormat = _T("%Y-%m-%d %H:%M:%S");
				str = dt.Format(strDateFormat);
			}
			break;
		case VT_CY:		//Added by John Andy Johnson!!!
			{
				vtFld.ChangeType(VT_R8);

				CString str;
				str.Format(_T("%f"), vtFld.dblVal);

				_TCHAR pszFormattedNumber[64];

				//	LOCALE_USER_DEFAULT
				if(GetCurrencyFormat(
					LOCALE_USER_DEFAULT,	// locale for which string is to be formatted
					0,						// bit flag that controls the function's operation
					str,					// pointer to input number string
					NULL,					// pointer to a formatting information structure
					//	NULL = machine default locale settings
					pszFormattedNumber,		// pointer to output buffer
					63))					// size of output buffer
				{
					str = pszFormattedNumber;
				}
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			str.Empty();
			break;
		case VT_BOOL:
			str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
			break;
		default:
			str.Empty();

			vtFld.Clear();

			return FALSE;
		}
		strValue = str;

		vtFld.Clear();

		return TRUE;
	}
	catch(_com_error &e)
	{
		Com_Error("",e);
		return FALSE;
	}
}

int CAdo_Control::SetQueryRun(CString strQuery)  //insert / Update 시만 사용
{
	VARIANT RecordsAffected;
	::VariantInit (&RecordsAffected);

	CString strRunlog_E2 = "";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;
	int nResult = 0;

	if(m_stDBInfo.unDBTypeID == DB_POSTGRE )
	{
		try
		{
			TRACE(strQuery+"\r\n");
			SQLRETURN retcode;
			retcode = codbc->SQLAllocStmtHandle();
			retcode = codbc->SQLExecDirect(strQuery);

			if (isSqlOk(retcode))
			{
				return 1;
			}
			else
			{
				return -1;
			}


			return 1;

		}

		catch (_com_error &e)
		{
			int nResult = Com_Error("",e);
			return nResult; //쿼리 생성 실패
		}
		catch(...)
		{
			strRunlog_E2.Format("INSERT / UPDATE Event Error : %s",strQuery);
			::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
			return -1;
		}

	}

	try
	{

		pRs = pADO_Connect->Execute((_bstr_t)strQuery, &RecordsAffected, adExecuteNoRecords);

		if (RecordsAffected.iVal == -1)
		{
			CString verifyQuery;
			verifyQuery.Format("SELECT COUNT(*) AS cnt FROM %s WHERE TAG_ID = 'BEMS_ELEC_SUM_PAY' AND RECORD_DATE = '20240617' and RECORD_TIME = 16", "HM_MINUTE_TREND_HISTORY");

			_RecordsetPtr pVerifyRs = pADO_Connect->Execute((_bstr_t)verifyQuery, NULL, adCmdText);
			if (!pVerifyRs->EndOfFile)
			{
				long count = pVerifyRs->Fields->Item["cnt"]->Value;
				if (count > 0)
				{
					return 1; // 업데이트가 성공적으로 이루어짐
				}
				else
				{
					return -1; // 업데이트가 실패함
				}
			}
		}
		else
		{
			return RecordsAffected.iVal;
		}
	}
	catch (_com_error &e)
	{
		int nResult = Com_Error("",e);
		return nResult; //쿼리 생성 실패
	}
	catch (...)
	{
		strRunlog_E2.Format("INSERT / UPDATE Event Error : %s",strQuery);
		::SendMessage(m_MsgHwnd,m_Message,(long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()),DB_ERROR);
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////
void _WriteLog(CString strLogPath, CString sMsg, int nLogFlag)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	SYSTEMTIME tm;
	CString s_msg;
	CString strTypeFolder;

	DWORD dwBytesWritten;
	BOOL bDelFlag = FALSE;
	WIN32_FIND_DATA findFileData;
	char logpath_c2[512];

	GetLocalTime(&tm);

	if(nLogFlag != 0)
		return;

	memset(logpath_c2,0x00,sizeof(logpath_c2));
	strcpy_s(logpath_c2, strLogPath);
	hFile = FindFirstFile(logpath_c2, &findFileData);
	if(hFile == INVALID_HANDLE_VALUE)
		CreateDirectory(logpath_c2, NULL);
	::FindClose(hFile);

	memset(logpath_c2,0x00,sizeof(logpath_c2));
	sprintf_s(logpath_c2, "%s\\%04d", strLogPath, tm.wYear);
	hFile = FindFirstFile(logpath_c2, &findFileData);
	if(hFile == INVALID_HANDLE_VALUE)
		CreateDirectory(logpath_c2, NULL);
	::FindClose(hFile);

	memset(logpath_c2,0x00,sizeof(logpath_c2));
	sprintf_s(logpath_c2, "%s\\%04d\\%02d", strLogPath, tm.wYear, tm.wMonth);
	hFile = FindFirstFile(logpath_c2, &findFileData);
	if(hFile == INVALID_HANDLE_VALUE)
		CreateDirectory(logpath_c2, NULL);
	::FindClose(hFile);

	memset(logpath_c2,0x00,sizeof(logpath_c2));
	sprintf_s(logpath_c2, "%s\\%04d\\%02d\\%s", strLogPath, tm.wYear, tm.wMonth, strTypeFolder);
	hFile = FindFirstFile(logpath_c2, &findFileData);
	if(hFile == INVALID_HANDLE_VALUE)
		CreateDirectory(logpath_c2, NULL);
	::FindClose(hFile);


	memset(logpath_c2,0x00,sizeof(logpath_c2));
	//sprintf(logpath_c2, "%s\\%04d\\%02d\\%04d%02d%02d%02d.log", g_cPath_log, tm.wYear, tm.wMonth, tm.wYear, tm.wMonth, tm.wDay, tm.wHour);

	sprintf_s(logpath_c2, "%s\\%04d\\%02d\\%s\\%04d%02d%02d%02d.log",
		strLogPath,
		tm.wYear,
		tm.wMonth,
		strTypeFolder,
		tm.wYear,
		tm.wMonth,
		tm.wDay,
		tm.wHour);

	hFile = CreateFile(logpath_c2, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}

	s_msg.Format("[%04d/%02d/%02d %02d:%02d:%02d:%03d] %s\r\n",
		tm.wYear,
		tm.wMonth,
		tm.wDay,
		tm.wHour,
		tm.wMinute,
		tm.wSecond,
		tm.wMilliseconds,
		sMsg);

	SetFilePointer(hFile, 0L, NULL, FILE_END);
	WriteFile(hFile, s_msg, s_msg.GetLength(), &dwBytesWritten, NULL);

	if (GetFileSize(hFile, NULL) > 1024*1024*50)  // 50 MByte 제한.
		bDelFlag = TRUE;
	else
		bDelFlag = FALSE;

	CloseHandle(hFile);
	if (bDelFlag)
		DeleteFile(logpath_c2);
}

BOOL CAdo_Control::GetRecordCount(const CString& sql, long* count)
{
	try
	{
		_RecordsetPtr pRs = pADO_Connect->Execute((_bstr_t)sql, NULL, adCmdText);
		if (!pRs->EndOfFile)
		{
			_variant_t varCount = pRs->Fields->Item[(long)0]->Value;
			varCount.ChangeType(VT_I4); // 값을 `long`으로 변환 시도
			*count = varCount.lVal;
		}
		pRs->Close();
	}
	catch (_com_error& e)
	{
		CString errorMsg;
		errorMsg.Format(_T("Error in GetRecordCount: %s"), (LPCTSTR)e.Description());
		Com_Error("GetRecordCount", e);
		return FALSE;
	}
	return TRUE;
}


BOOL CAdo_Control::BeginTrans()
{
	try
	{
		if (pADO_Connect != NULL && pADO_Connect->State == adStateOpen)
		{
			pADO_Connect->BeginTrans(); // 트랜잭션 시작
#ifdef _DEBUG
			TRACE("Ado-Control(%s) - 트랜잭션 시작\n", m_strMsgName);
#endif
			return TRUE;
		}
	}
	catch (_com_error& e)
	{
		Com_Error("BeginTrans", e);
	}
	return FALSE;
}

BOOL CAdo_Control::CommitTrans()
{
	try
	{
		if (pADO_Connect != NULL && pADO_Connect->State == adStateOpen)
		{
			pADO_Connect->CommitTrans(); // 트랜잭션 커밋
#ifdef _DEBUG
			TRACE("Ado-Control(%s) - 트랜잭션 커밋\n", m_strMsgName);
#endif
			return TRUE;
		}
	}
	catch (_com_error& e)
	{
		Com_Error("CommitTrans", e);
	}
	return FALSE;
}

BOOL CAdo_Control::RollbackTrans()
{
	try
	{
		if (pADO_Connect != NULL && pADO_Connect->State == adStateOpen)
		{
			pADO_Connect->RollbackTrans(); // 트랜잭션 롤백
#ifdef _DEBUG
			TRACE("Ado-Control(%s) - 트랜잭션 롤백\n", m_strMsgName);
#endif
			return TRUE;
		}
	}
	catch (_com_error& e)
	{
		Com_Error("RollbackTrans", e);
	}
	return FALSE;
}


_RecordsetPtr CAdo_Control::DB_OpenRecordSet(CString strQuery)
{
	try
	{
		_RecordsetPtr pRecordset = pADO_Connect->Execute((_bstr_t)strQuery, NULL, adCmdText);
		return pRecordset;
	}
	catch (_com_error& e)
	{
		CString strError;
		strError.Format(_T("Error: %s\nCode: %08lx\nDescription: %s\nSource: %s"),
			e.ErrorMessage(), e.Error(), (LPCTSTR)e.Description(), (LPCTSTR)e.Source());
		AfxMessageBox(strError);
		Com_Error("DB_OpenRecordSet", e);
		return NULL;
	}
}

int CAdo_Control::Truncate(CString tableName)
{
	_CommandPtr pCommand = NULL;
	CString strRunlog_E2 = "";

	if (pADO_Connect == NULL || pADO_Connect->State != adStateOpen)
	{
		if (!DB_Connection())  // 연결 시도
		{
			AfxMessageBox("데이터베이스 연결 실패");
			return -1;
		}
	}


	try
	{
		// Command 객체 생성
		HRESULT hr = pCommand.CreateInstance(__uuidof(Command));
		if (FAILED(hr))
		{
			AfxMessageBox("Command 객체 생성 실패");
			return -1;
		}

		// Connection 설정
		pCommand->ActiveConnection = pADO_Connect;

		// 트랜잭션 시작
		if (!BeginTrans())
		{
			AfxMessageBox("트랜잭션 시작 실패");
			return -1;
		}

		// TRUNCATE TABLE 쿼리 설정
		CString truncateQuery;
		truncateQuery.Format("TRUNCATE TABLE %s", tableName);
		pCommand->CommandText = (_bstr_t)truncateQuery;
		pCommand->CommandType = adCmdText;

		// 쿼리 실행
		pCommand->Execute(NULL, NULL, adExecuteNoRecords);

		// 트랜잭션 커밋
		if (!CommitTrans())
		{
			AfxMessageBox("트랜잭션 커밋 실패");
			return -1;
		}

		return 1;  // 성공 시 1 반환
	}
	catch (_com_error& e)
	{
		RollbackTrans();  // 오류 시 트랜잭션 롤백
		return Com_Error("TruncateTable", e);
	}
	catch (...)
	{
		RollbackTrans();  // 오류 시 트랜잭션 롤백
		strRunlog_E2.Format("TRUNCATE TABLE Event Error : %s", tableName);
		::SendMessage(m_MsgHwnd, m_Message, (long)strRunlog_E2.GetBuffer(strRunlog_E2.GetLength()), DB_ERROR);
		return -1;
	}
}


CString IntToStr(int nVal)
{
	CString strRet;
	char buff[10];
	//VS 6.0
	//itoa(ulVal, buff, 10);
	_itoa_s(nVal, buff, 10);
	strRet = buff;
	return strRet;
}

CString LongToStr(long lVal)
{
	CString strRet;
	char buff[20];

	//VS 6.0
	//itoa(ulVal, buff, 10);
	_itoa_s(lVal, buff, 10);
	strRet = buff;
	return strRet;
}

CString ULongToStr(unsigned long ulVal)
{
	CString strRet;
	char buff[20];

	//VS 6.0
	//ultoa(ulVal, buff, 10);
	_ultoa_s(ulVal, buff, 10);
	strRet = buff;
	return strRet;
}

CString DblToStr(double dblVal, int ndigits)
{
	CString strRet;
	char buff[50];

	//VS 6.0
	//_gcvt(dblVal, ndigits, buff);
	_gcvt_s(buff,sizeof(buff),dblVal,ndigits);
	strRet = buff;
	return strRet;
}

CString DblToStr(float fltVal)
{
	CString strRet;
	char buff[50];
	//VS 6.0
	//_gcvt(fltVal, 10, buff);
	_gcvt_s(buff,sizeof(buff),fltVal, 10);
	strRet = buff;
	return strRet;
}