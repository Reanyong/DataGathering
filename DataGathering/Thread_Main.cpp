// Thread_Main.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_Main.h"
#include "FormView_Main.h"


// CThread_Main

IMPLEMENT_DYNCREATE(CThread_Main, CWinThread)

CThread_Main::CThread_Main()
{
	m_bEndThread = FALSE;
}

CThread_Main::~CThread_Main()
{
}

BOOL CThread_Main::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	return TRUE;
}

int CThread_Main::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_Main, CWinThread)
END_MESSAGE_MAP()

void CThread_Main::SetOutPutMsg(CString strTitle,CString strMsg,COLORREF msgColor)
{
	_addSystemMsg(FORM_VIEW_ID_SYSTEM, USER_COLOR_BLUE, strTitle, msgColor, strMsg);

	SetWriteLogFile(strMsg);
}

void CThread_Main::SetWriteLogFile(CString strLogMsg)
{
	EnterCriticalSection(&g_cs);
	//_WriteLogFile(g_stProjectInfo.szDTGatheringLogPath,"system",strLogMsg);
	_systemLog(strLogMsg,g_stProjectInfo.szProjectLogPath);
	LeaveCriticalSection(&g_cs);
}

// CThread_Main 메시지 처리기입니다.
int CThread_Main::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CTime currentTime =  CTime::GetCurrentTime();
	BOOL bStartCheck  = FALSE;
	do
	{
		if(m_bEndThread == TRUE)
			break;

		Sleep(1000); //매 5초 마다 확인

		BOOL bFileCheck = GetFileAttributeCheck(currentTime, g_stProjectInfo.szProjectIniPath);
		if(bFileCheck == TRUE)
		{
			//GetSettingRead(g_stProjectInfo.szDTGatheringIniPath);
			((CFormView_Main *)(m_pCtrl))->ShowSettingInfo(g_stProjectInfo.szProjectIniPath);
			if(bStartCheck == TRUE)
				SetOutPutMsg("DataGathering - Main", "Main : [설정 정보 Refresh], 상태 : [수정 정보 Read]", USER_COLOR_PINK);
			else
				SetOutPutMsg("DataGathering - Main", "Main : [설정 정보 Read...], 상태 : [정상]", USER_COLOR_PINK);
		}

		GetCurrentStateMsg();

		bStartCheck = TRUE;

	} while (!m_bEndThread);

	PostThreadMessage(WM_QUIT, 0, 0);
	return CWinThread::Run();
}

void CThread_Main::GetCurrentStateMsg()
{
	CCurrentstate msg;

	if (_getCurrentstateMsg(&msg) < 1)
		return;

	if(msg.m_nIsPos == 0)
	{
		((CFormView_Main *)(m_pCtrl))->ListInsertItem_Info(msg.m_strThreadState,msg.m_strStateMsg,"");
	}
	else if(msg.m_nIsPos == 1)
	{
		((CFormView_Main *)(m_pCtrl))->ListInsertItem_Msg(msg.m_strThreadState,msg.m_strStateMsg,"");
	}
}

BOOL CThread_Main::GetFileAttributeCheck(CTime currentTime,CString strPathName)
{
	CString strDataTime = "";
	CTime date_Time;
	// 파일의 타입과 아이콘 정보를 얻기위한 구조체를 선언한다.
	SHFILEINFO shfile_info;

	struct _finddata_t shfile_data;
	_findfirst(strPathName, &shfile_data);

	// 선택된 파일의 아이콘과 타입 정보를 얻는다.
	SHGetFileInfo((char*)(LPCTSTR)strPathName, 0, &shfile_info,
		sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_TYPENAME);

	//수정 날짜정보
	date_Time = CTime(shfile_data.time_write);
	//strDataTime.Format("%s", date_Time->Format("%H:%M:%S, %B, %d, %Y"));//Data Type 형식 16:18:31, September, 07, 2015
	strDataTime.Format("%s", date_Time.Format("%Y-%m-%d %H:%M:%S"));

	if(m_DatawriteTimeCheck != date_Time)
	{
		m_DatawriteTimeCheck = date_Time;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
