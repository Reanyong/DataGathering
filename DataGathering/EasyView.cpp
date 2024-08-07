// EasyView.cpp : implementation file
//

#include "stdafx.h"
#include "EasyView.h"

#include "Include/EvStruct.h"
#include "Include/EVILib.h"
#include "Include/EvErrMsg.h"

#pragma comment(lib, "Lib/EVILib.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEasyView

CEasyView::CEasyView()
{
}

CEasyView::~CEasyView()
{
}

bool CEasyView::Initialize( LPCTSTR szEvPrjNm )
{
	try
	{
		strcpy(g_szProjectPath, szEvPrjNm);
		TCHAR szEvEndMsg[512] = {0};
		sprintf( szEvEndMsg, "GM_EVVIEW_END_%s", g_szProjectPath );
		g_uiEvStop = RegisterWindowMessage( szEvEndMsg );

		// 현재 Open 된 프로젝트의 이름으로 EasyView 공유 메모리를 연다.
		if( EV_OpenMem( g_szProjectPath ) < 0 )
		{
			MessageBox(NULL, EV_ERR_MSGE_0001002, EV_ERR_CODE_0001002, EV_ERR_TYPE_0001002 | MB_SYSTEMMODAL);

			TRACE("공유 메모리 Open 실패 \r\n");
			return false;
		}

		TRACE("공유 메모리 Open 성공 \r\n");
	}
	catch( CException* e )
	{
		e->ReportError();
		e->Delete();
	}

	return true;
}

bool CEasyView::Uninitialize()
{
	try
	{
		// 현재 Open 된 프로젝트의 이름으로 공유된 공유 메모리를 해제한다.
		if( EV_FreeMem( g_szProjectPath ) == -1 )
		{
			TRACE("공유 메모리 해제 실패 \r\n");
			return false;
		}

		TRACE("공유 메모리 해제 성공 \r\n");
	}
	catch( CException* e )
	{
		e->ReportError();
		e->Delete();
	}

	return true;
}

bool CEasyView::SetTagValue( LPCTSTR szTagNm, CString sVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_STRING_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "SI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	if ( EV_SetSiValue( stTagInfo.nStnPos, stTagInfo.nTagPos, sVal, sVal.GetLength() ) != TRUE )
	{
		TRACE( "SI 태그값 설정 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

bool CEasyView::SetTagValue( LPCTSTR szTagNm, double dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	if ( EV_SetAiValue( stTagInfo.nStnPos, stTagInfo.nTagPos, dbVal ) != TRUE )
	{
		TRACE( "AI 태그값 설정 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

bool CEasyView::SetTagValue( LPCTSTR szTagNm, bool bFlag )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_DIGITAL_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "DI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	BYTE bVal = (bFlag == true) ? TRUE : FALSE;

	if ( EV_SetDiValue( stTagInfo.nStnPos, stTagInfo.nTagPos, bVal ) != TRUE )
	{
		TRACE( "DI 태그값 설정 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

bool CEasyView::GetTagValue( LPCTSTR szTagNm, CString &sVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_STRING_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "SI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	double dbVal            = 0.0;
	TCHAR  szTemp[MAX_PATH] = {0};

	if ( EV_GetSiData( stTagInfo.nStnPos, stTagInfo.nTagPos, szTemp, sizeof(szTemp), &dbVal ) != TRUE )
	{
		TRACE( "SI 태그값 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	sVal = szTemp;

	return true;
}

bool CEasyView::GetTagValue( LPCTSTR szTagNm, double &dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	if ( EV_GetAiData( stTagInfo.nStnPos, stTagInfo.nTagPos, &dbVal) != TRUE )
	{
		TRACE( "AI 태그값 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

bool CEasyView::GetTagValue( LPCTSTR szTagNm, bool &bFlag )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_DIGITAL_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "DI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	BYTE bVal;

	if ( EV_GetDiData( stTagInfo.nStnPos, stTagInfo.nTagPos, &bVal ) != TRUE )
	{
		TRACE( "DI 태그값 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	bFlag = (bVal == FALSE) ? false : true;

	return true;
}


bool CEasyView::GetEngMin( LPCTSTR szTagNm, double &dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	int nErrorCode = 0;

	LPST_EV_TAG_ANALOG_INPUT pAI = EV_GetAiTagInfo( stTagInfo.nStnPos, stTagInfo.nTagPos, &nErrorCode );

	if ( pAI == NULL )
	{
		TRACE( "AI 태그 정보 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	dbVal = pAI->dbEngBase;

	return true;
}

bool CEasyView::GetEngMax( LPCTSTR szTagNm, double &dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	int nErrorCode = 0;

	LPST_EV_TAG_ANALOG_INPUT pAI = EV_GetAiTagInfo( stTagInfo.nStnPos, stTagInfo.nTagPos, &nErrorCode );

	if ( pAI == NULL )
	{
		TRACE( "AI 태그 정보 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	dbVal = pAI->dbEngFull;

	return true;
}

bool CEasyView::GetAdMin( LPCTSTR szTagNm, double &dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	int nErrorCode = 0;

	LPST_EV_TAG_ANALOG_INPUT pAI = EV_GetAiTagInfo( stTagInfo.nStnPos, stTagInfo.nTagPos, &nErrorCode );

	if ( pAI == NULL )
	{
		TRACE( "AI 태그 정보 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	dbVal = pAI->dbADBase;

	return true;
}

bool CEasyView::GetAdMax( LPCTSTR szTagNm, double &dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	int nErrorCode = 0;

	LPST_EV_TAG_ANALOG_INPUT pAI = EV_GetAiTagInfo( stTagInfo.nStnPos, stTagInfo.nTagPos, &nErrorCode );

	if ( pAI == NULL )
	{
		TRACE( "AI 태그 정보 획득 실패 [%s] \r\n", szTagNm );
		return false;
	}

	dbVal = pAI->dbADFull;

	return true;
}

bool CEasyView::GetHistoricalTagValues( LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataDi> &vtVals )
{
	HANDLE hAccess = EV_QueryLog_MI( tStart.GetTime(), tEnd.GetTime(), szTagNm );

	if ( hAccess == NULL )
		return false;

	vtVals.clear();

	int nDataCount = 0;

	double *pData = EV_AccessLogData_MI(hAccess, 0, &nDataCount);

	if ( nDataCount <= 0 )
		return true;

	if ( pData == NULL )
		return false;

	try
	{
		for ( size_t i=0 ; i<nDataCount ; i++ )
		{
			CDataDi di;
			di.tLogTime = tStart + CTimeSpan( 0, 0, i, 0 );
			di.bVal = pData[i] == 0.0f ? false : true;
			vtVals.push_back( di );
		}

		EV_CloseLog_MI(hAccess);

		hAccess = NULL;
	}
	catch(...)
	{
		hAccess = NULL;
		return false;
	}

	return true;
}

bool CEasyView::GetHistoricalTagValues( LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataAi> &vtVals )
{
	HANDLE hAccess = EV_QueryLog_MI( tStart.GetTime(), tEnd.GetTime(), szTagNm );

	if ( hAccess == NULL )
		return false;

	vtVals.clear();

	int nDataCount = 0;

	double *pData = EV_AccessLogData_MI(hAccess, 0, &nDataCount);

	if ( nDataCount <= 0 )
		return true;

	if ( pData == NULL )
		return false;

	try
	{
		for ( size_t i=0 ; i<nDataCount ; i++ )
		{
			CDataAi ai;
			ai.tLogTime = tStart + CTimeSpan( 0, 0, i, 0 );
			ai.dbVal = pData[i];
			vtVals.push_back( ai );
		}

		EV_CloseLog_MI(hAccess);

		hAccess = NULL;
	}
	catch(...)
	{
		hAccess = NULL;
		return false;
	}

	return true;
}

bool CEasyView::PutBufferValue( LPCTSTR szTagNm, CString sVal)
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_STRING_INPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "SI 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	if ( EV_PutTagString( stTagInfo.nStnPos, stTagInfo.nTagPos, sVal, sVal.GetLength() ) != TRUE )
	{
		TRACE( "EV_PutTagString 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

bool CEasyView::OutPutValue( LPCTSTR szTagNm, double dbVal )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_ANALOG_OUTPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "AO 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	if ( EV_SetAoValue( stTagInfo.nStnPos, stTagInfo.nTagPos, dbVal ) != TRUE )
	{
		TRACE( "AO 태그값 설정 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

bool CEasyView::OutPutValue( LPCTSTR szTagNm, bool bFlag )
{
	ST_EV_TAG_INFO stTagInfo;

	if ( EV_GetTagInfo2( EV_TYPE_DIGITAL_OUTPUT, szTagNm, &stTagInfo ) != TRUE )
	{
		TRACE( "DO 태그 검색 실패 [%s] \r\n", szTagNm );
		return false;
	}

	BYTE bVal = (bFlag == true) ? TRUE : FALSE;

	if ( EV_SetDoValue( stTagInfo.nStnPos, stTagInfo.nTagPos, bVal ) != TRUE )
	{
		TRACE( "DO 태그값 설정 실패 [%s] \r\n", szTagNm );
		return false;
	}

	return true;
}

int CEasyView::OutPutValue(LPCTSTR szTag, CString sVal)
{
	ST_EV_TAG_INFO stTagInfo;
	int nErrorCode = 0;

	if(EV_GetTagInfo(szTag, &stTagInfo) < 0)
	{
		TRACE( "SI 태그 검색 실패 [%s] \r\n", szTag );
		return -1;
	}

	ST_CONTROL2 stCtrl;
	ST_CTRL_COMMON *pCtrlCommon = &stCtrl.CtrlCommon;
	ST_CTRL_STRING *pCtrlString = &stCtrl.CtrlString;

	memset(&stCtrl, 0, sizeof(stCtrl));
	pCtrlCommon->nTagType = TYPE_SI;
	pCtrlCommon->nProcessingMode = 0;			// 여기서 발송하는 정보를 이용할 것...
	pCtrlCommon->nStnPos = stTagInfo.nStnPos;
	pCtrlCommon->nTagPos = stTagInfo.nTagPos;
	time(&pCtrlCommon->tStart);
	ST_EV_TAG_STRING_INPUT *pSi = EV_GetSiTagInfo(stTagInfo.nStnPos, stTagInfo.nTagPos, &nErrorCode);
	if (NULL == pSi)
	{
		return -2;
	}

	double db = 0;
	int nStnPos = pSi->nStnPos;
	SYSTEMTIME st;
	GetLocalTime(&st);

	if(pSi->nIOMode)
	{
		//sprintf(pCtrlString->CtrlBuffer, szVal);
		sprintf(pCtrlString->CtrlBuffer, sVal);
		int nLen = strlen(pCtrlString->CtrlBuffer);
	}
	else if (nStnPos > -1 && nStnPos < g_nStation)
	{
		// 스트링 태그가 참조하는 스캔버퍼와 스트링 태그의 현재값을 바꿔준다
		//_PutStringTag2(nStnPos, pSi->nTagPos, szVal, strlen(szVal));
		return -3;
	}

	EV_GetOutputDeviceName(pSi->nStnPos, pCtrlCommon->szDevice);

	if(pSi->nIOMode)												// 만약 출력 겸용이고
	{
		if(strlen(pSi->szIOAddr) > 0)								// 직접 주소가 등록되어 있으면
			strcpy(pCtrlCommon->szAddr, pSi->szIOAddr);				// 직접 주소를 사용하고
		else
			strcpy(pCtrlCommon->szAddr, pSi->szAddr);				// 그렇지 않으면 출력 주소를 사용한다
	}
	else
		strcpy(pCtrlCommon->szAddr, pSi->szAddr);

	strcpy(pCtrlCommon->szTag, szTag);
	pCtrlCommon->nInvokeProcessId = _nProcessId;
	pCtrlCommon->bFrom = FROM_DM;
	
	if(pSi->nType != 1 && pSi->nIsMemTag == FALSE) // real tag and not ASCII type
	{
		if (pSi->nByteCount > sizeof(pCtrlString->CtrlBuffer))
		{
			return -4;
		}

		pCtrlString->nMode = 1; // hex
		pCtrlString->nByteCount = pSi->nByteCount;
	}
	else
	{
		int ncLength = strlen(pSi->cBuffer);
		if (ncLength > sizeof(pCtrlString->CtrlBuffer))
			ncLength = sizeof(pCtrlString->CtrlBuffer);
		pCtrlString->nMode = 2; // ascii
		pCtrlString->nByteCount = ncLength;
	}

	return EV_InvokeControl(&stCtrl);
}



int CEasyView::GetDualModeStatus(int *pDualMode)
{
	int nResult;
	int nDualMode;

	nResult = EV_GetDualModeStatus(&nDualMode);

	*pDualMode = nDualMode;

	return nResult;
}

