// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")��
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե�
// Microsoft Foundation Classes Reference �� ���� ���� ������ ����
// �߰������� �����Ǵ� �����Դϴ�.
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������
// http://msdn.microsoft.com/officeui�� �����Ͻʽÿ�.
//
// Copyright (C) Microsoft Corporation
// ��� �Ǹ� ����.

// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.

#pragma once


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS
#ifdef _DEBUG
#include <vld.h>
#endif

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����


#include <comcat.h>
#include <afxinet.h>
#include <XTToolkitPro.h>

#include "./Control_List/InterlockSource/JtFreeStruct.h"
#include "./Control_List/AdoControl/Ado_Control.h"
//#include "./Control_List/OdbcControl/Odbc.h"
#include "./Control_List/XMLCtrl/UTF8Conv.h"
#include "afxdb.h"								//20200218 ����ȣ �߰�
#include "odbcinst.h"							//20200218 ����ȣ �߰�
#include "./Control_List/OdbcControl/Odbc.h"	//20200218 ����ȣ �߰�
#include <exception>							//20210308 ksw �߰�

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

extern UINT wm_EVViewStop;  //EasyView ���� �̺�ƮID
extern ST_PROJECT_INFO g_stProjectInfo; //EasyView ����
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

extern CEvent g_DeleteInProgressEvent; // SubThread ����ȭ �̺�Ʈ ����
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
	UINT Flag;//DI ù°�ڸ� ��Ʈ/ AI �ι�° �ڸ� ��Ʈ / SI 3��° �ڸ� ��Ʈ
};