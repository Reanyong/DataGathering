// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS
#ifdef _DEBUG
#include <vld.h>
#endif

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원


#include <comcat.h>
#include <afxinet.h>
#include <XTToolkitPro.h>

#include "./Control_List/InterlockSource/JtFreeStruct.h"
#include "./Control_List/AdoControl/Ado_Control.h"
//#include "./Control_List/OdbcControl/Odbc.h"
#include "./Control_List/XMLCtrl/UTF8Conv.h"
#include "afxdb.h"								//20200218 나정호 추가
#include "odbcinst.h"							//20200218 나정호 추가
#include "./Control_List/OdbcControl/Odbc.h"	//20200218 나정호 추가
#include <exception>							//20210308 ksw 추가

// <-- 20191230-Doo
#ifdef _DEBUG
#define NOUSE_STYLE_VISTA
#endif
// -->

#include "..\..\..\Include\EvStruct.h"
#include "..\..\..\Include\EVILib.h"
#include "..\..\..\Include\EvErrMsg.h"


#define ID_USER_NEW_VIEW_OPEN		(WM_USER + 1000)

extern	int g_nActivateDocCheck;
extern  int g_nCheckTagThreadRun;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#define WM_ADDLIST	WM_USER+1
#define WM_ENABLECONTROL	WM_USER+2

extern UINT wm_EVViewStop;  //EasyView 종료 이벤트ID
extern ST_PROJECT_INFO g_stProjectInfo; //EasyView 정보
extern  CRITICAL_SECTION g_cs;
#include <sys/timeb.h>
#include <time.h>


#define	isSqlOk(r) ((r == SQL_SUCCESS) || (r == SQL_SUCCESS_WITH_INFO))


extern void _CreateFont(CFont *pFont);
extern CString _IDCreated();
//////////////////////////////////////////////////////////////////////////
//DataGathering info ini File Check
extern CString _createFolder(const char *szBasicsPath,const char *szCreateName);
extern CString _getDataGatheringInfo(CString strPath, CString strProject);

extern ST_DATABASENAME _getDataBesaNameRead(const char *szIniPath);
extern void _setDataBesaNameWrite(ST_DATABASENAME *stDBName, const char * szIniPath);

extern ST_DBINFO _getInfoDBRead(const char *szIniPath);
extern void _setInfoDBWrite(ST_DBINFO *stDbInfo, const char * szIniPath);

extern ST_GATHERINFO _getInfoGatherRead(const char *szIniPath);
extern void _setInfoGatherWrite(ST_GATHERINFO *stGatherInfo, const char * szIniPath);

extern ST_GATHERINFO_TYPE _getGatherInfoType(const char * szIniPath);
extern void _setGatherInfoType(ST_GATHERINFO_TYPE *stInfoType, const char * szIniPath);

extern ST_FILE_ISMARTACCESS _getInfoFileReadISmartAccess(const char *szIniPath);
extern void _setInfoFileReadISmartAccess(ST_FILE_ISMARTACCESS *stInfo, const char * szIniPath);

extern ST_WEATHER_INFO _getInfoWeatherRead(const char *szIniPath);
extern void _setInfoWeatherWrite(ST_WEATHER_INFO *stWeatherInfo, const char * szIniPath);

extern CString _getXmlPath(const char *szIniPath);
extern void _setXmlPath(const char *szXmlPath, const char * szIniPath);

#include <io.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern void _systemLog(const char * data, const char * szFilePath);
extern void _WriteFile(const char * data, const int len, const char * szFileName);
extern void _WriteLogFile(CString strLogPath,CString strFaileName,CString data);
extern void _WriteBackUpFile(CString strLogPath,CString data,int nMin);

extern CEvent g_DeleteInProgressEvent; // SubThread 동기화 이벤트 변수
//////////////////////////////////////////////////////////////////////////
//Output Msg
//2015-08-26
class CSysMsg {
public:
	CSysMsg() {
		m_nIsOutput = 0;
		m_sTitle = "";
		m_sBody = "";
		m_crTitle = RGB(0, 0 , 255);
		m_crBody = RGB(255, 0 , 0);
	}
	virtual ~CSysMsg() {
		;
	}
	void operator = (CSysMsg *p) {
		this->m_nIsOutput = p->m_nIsOutput;
		this->m_sTitle = p->m_sTitle;
		this->m_sBody = p->m_sBody;
		this->m_crBody = p->m_crBody;
		this->m_crTitle = p->m_crTitle;
	}
public:

	int m_nIsOutput;
	CString m_sTitle;
	CString m_sBody;
	COLORREF m_crTitle;
	COLORREF m_crBody;
};

extern	CPtrArray _arrMsg;

extern int _addSystemMsg(int nIsOutput, COLORREF crTitle, const char *sTitle, COLORREF crBody, const char *sBody);
extern int _getSystemMsg(CSysMsg *p);

//////////////////////////////////////////////////////////////////////////
//Currentstate Msg
class CCurrentstate {
public:
	CCurrentstate() {
		m_nIsPos = 0;
		m_nCount = 0;
		m_strThreadState = "";
		m_strStateMsg = "";
	}
	virtual ~CCurrentstate() {
		;
	}
	void operator = (CCurrentstate *p) {
		this->m_nIsPos = p->m_nIsPos;
		this->m_nCount = p->m_nCount;
		this->m_strThreadState = p->m_strThreadState;
		this->m_strStateMsg = p->m_strStateMsg;
	}
public:
	int m_nIsPos;
	int m_nCount;
	CString m_strThreadState;
	CString m_strStateMsg;
};

extern	CPtrArray _arrStateMsg;

extern int _addCurrentstateMsg(int nIsPos,int nCount, const char *szThreadState, const char *szStateMsg);
extern int _getCurrentstateMsg(CCurrentstate *p);

extern int g_pList_ST_TagDivisionListCnt;
extern std::list<ST_TagInfoList>** g_pList_ST_TagDivisionList;

struct ST_CHECK_DATA
{
	int StnPos;
	UINT Flag;//DI 첫째자리 비트/ AI 두번째 자리 비트 / SI 3번째 자리 비트
};