#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TAGGather.h"
#include "Thread_Delete.h"

IMPLEMENT_DYNCREATE(Thread_Delete, CWinThread)

Thread_Delete::Thread_Delete()
{
    m_strLogTitle = "Delete Thread";
    m_bEndThread = FALSE;
    DB_Connect = NULL;

    tryCount = 0;
}

Thread_Delete::~Thread_Delete()
{
}

BOOL Thread_Delete::InitInstance()
{
    CoInitialize(NULL);
    return TRUE;
}

int Thread_Delete::ExitInstance()
{
    if (DB_Connect != NULL)
    {
        if (DB_Connect->GetDB_ConnectionStatus() == 1)
            DB_Connect->DB_Close();

        delete DB_Connect;
        DB_Connect = NULL;
    }
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(Thread_Delete, CWinThread)
END_MESSAGE_MAP()

int Thread_Delete::Run()
{
    //Sleep(10);

    CString strSysLogMsg = "";

    while (!m_bEndThread)
    {
        Sleep(1000);

        time_t now = time(0);
        tm* ltm = localtime(&now);

        if (ltm->tm_mday == 1) // 기준 날짜 매월 1일
        {
            if (tryCount == 0 || tryCount == 3600) // 다중 실행 제어
            {
                ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
                ST_DATABASENAME stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

                m_nDBType = stDBInfo.unDBType;
                DB_Connect = new CAdo_Control();
                DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
                DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

                if (DB_Connect->DB_Connection() == TRUE)
                {
                    try
                    {
                        COleDateTime today(ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, 0, 0, 0);
                        COleDateTime twoMonthsAgo = today - COleDateTimeSpan(60, 0, 0, 0);

                        CString DeleteSQL, CountSQL;
                        if (m_nDBType == DB_MSSQL)
                        {
                            CountSQL.Format(_T("SELECT COUNT(*) FROM HM_MINUTE_TREND_HISTORY WHERE RECORD_DATE < '%04d%02d01'"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());

                            DeleteSQL.Format(_T("DELETE FROM HM_MINUTE_TREND_HISTORY WHERE Record_date < '%04d%02d01'"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());
                        }
                        else if (m_nDBType == DB_POSTGRE)
                        {
                            CountSQL.Format(_T("SELECT COUNT(*) FROM easy_hmi.HM_MINUTE_TREND_HISTORY WHERE RECORD_DATE < '%04d%02d01'"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());

                            DeleteSQL.Format(_T("DELETE FROM easy_hmi.HM_MINUTE_TREND_HISTORY WHERE Record_date < '%04d%02d01'"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());
                        }

                        TRACE("SQL Count 작업 완료: %s\n", (LPCTSTR)CountSQL);
                        long recordCount = 0;
                        DB_Connect->GetRecordCount(CountSQL, &recordCount);
                        TRACE("Record Count: %ld\n", recordCount);

                        if (recordCount == 0)
                        {
                            SQLRETURN retcode;
                            TRACE("SQL Delete 작업 시작: %s\n", (LPCTSTR)DeleteSQL);
                            DB_Connect->codbc->COdbc::SQLAllocStmtHandle();
                            retcode = DB_Connect->SetQueryRun(DeleteSQL);

                            if (isSqlOk(retcode))
                            {
                                CString logMessage;
                                logMessage.Format("DELETED FROM HM_MINUTE_TREND_HISTORY before %04d-%02d-01.", twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());
                                TRACE("%s\n", (LPCSTR)logMessage);
                                _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);
                                _systemLog(logMessage, g_stProjectInfo.szDTGatheringIniPath);

                                strSysLogMsg.Format("Delete Record : [%d]", recordCount);
                                SysLogOutPut(m_strLogTitle, strSysLogMsg, LOG_COLOR_RED);
                            }
                        }
                    }
                    catch (CDBException* e)
                    {
                        CString strError = e->m_strError;
                        e->ReportError();
                        e->Delete();

                        CString logMessage;
                        logMessage.Format(_T("Database exception occurred: %s"), (LPCTSTR)strError);
                        TRACE("%s\n", (LPCTSTR)logMessage);
                        _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog_Fail", logMessage);
                        _systemLog(logMessage, g_stProjectInfo.szDTGatheringLogPath);
                    }

                    DB_Connect->DB_Close();
                    delete DB_Connect;
                    DB_Connect = NULL;
                }

            }

            else if (tryCount > 3600)
            {
                tryCount = 0;
            }

            tryCount++;
        }

        if (m_bEndThread)
            break;
    }
    PostThreadMessage(WM_QUIT, 0, 0);
    return CWinThread::Run();
}

void Thread_Delete::StopThread()
{
    m_bEndThread = TRUE;
}

void Thread_Delete::SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody)
{
    _addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, crBody, strMsg);
}