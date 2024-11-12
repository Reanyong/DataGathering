#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TAGGather.h"
#include "Thread_Delete.h"

CEvent g_DeleteInProgressEvent(FALSE, TRUE);
IMPLEMENT_DYNCREATE(Thread_Delete, CWinThread)

Thread_Delete::Thread_Delete()
{
    m_strLogTitle = "Delete Thread";
    m_bEndThread = FALSE;
    DB_Connect = NULL;

    b_ThreadTry = true;
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
    bool bDeleteCompleted = false; // 삭제 완료 플래그

    g_DeleteInProgressEvent.SetEvent();

    while (!m_bEndThread)
    {
        Sleep(1000);

        time_t now = time(0);
        tm* ltm = localtime(&now);

        if (ltm->tm_mday == 1) // 기준 날짜 매월 1일
        {
            if (b_ThreadTry) // 다중 실행 제어
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
                        COleDateTime twoMonthsAgo = today - COleDateTimeSpan(90, 0, 0, 0);

                        int batchSize = 10000;       // 한 번에 삭제할 레코드 수
                        long totalRecordCount = 0;
                        CString DeleteSQL, CountSQL;


                        if (m_nDBType == DB_MSSQL)
                        {
                            CountSQL.Format(_T("SELECT COUNT(*) FROM HM_MINUTE_TREND_HISTORY WHERE RECORD_DATE < '%04d%02d01'"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());

                            BOOL result = DB_Connect->GetRecordCount(CountSQL, &totalRecordCount);

                            DeleteSQL.Format(_T("DELETE TOP(%d) FROM HM_MINUTE_TREND_HISTORY WHERE Record_date < '%04d%02d01'"),
                                batchSize, twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());
                        }
                        else if (m_nDBType == DB_POSTGRE)
                        {
                            CountSQL.Format(_T("SELECT COUNT(*) FROM easy_hmi.HM_MINUTE_TREND_HISTORY WHERE RECORD_DATE < '%04d%02d01'"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());

                            totalRecordCount = DB_Connect->GetRecordCount(CountSQL, &totalRecordCount);

                            DeleteSQL.Format(_T("DELETE FROM easy_hmi.HM_MINUTE_TREND_HISTORY WHERE Record_date < '%04d%02d01' LIMIT %d"),
                                twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth(), batchSize);
                        }

                        TRACE("Total Record Count to Delete: %ld\n", totalRecordCount);

                        SQLRETURN retcode;

                        if (totalRecordCount > 0)
                        {
                            while (totalRecordCount > 0 && !m_bEndThread)
                            {
                                DeleteSQL.Format(_T("DELETE TOP(%d) FROM HM_MINUTE_TREND_HISTORY WHERE Record_date < '%04d%02d01'"),
                                    batchSize, twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());

                                retcode = DB_Connect->SetQueryRun(DeleteSQL);

                                TRACE("SQL Delete 작업 시작: %s\n", (LPCTSTR)DeleteSQL);
                                //DB_Connect->codbc->COdbc::SQLAllocStmtHandle();
                                retcode = DB_Connect->SetQueryRun(DeleteSQL);

                                if (isSqlOk(retcode))
                                {
                                    CString logMessage;
                                    logMessage.Format("Batch Deleted %d records from HM_MINUTE_TREND_HISTORY before %04d-%02d-01.",
                                        batchSize, twoMonthsAgo.GetYear(), twoMonthsAgo.GetMonth());
                                    TRACE("%s\n", (LPCSTR)logMessage);
                                    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);
                                    _systemLog(logMessage, g_stProjectInfo.szDTGatheringIniPath);

                                    // 남은 레코드 수 업데이트
                                    totalRecordCount -= batchSize;
                                    if (totalRecordCount < 0) totalRecordCount = 0; // 음수 방지

                                    strSysLogMsg.Format("Remaining Records to Delete: [%ld]", totalRecordCount);
                                    SysLogOutPut(m_strLogTitle, strSysLogMsg, LOG_COLOR_RED);
                                }

                                else
                                {
                                    CString strError = _T("Error during batch deletion.");
                                    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog_Fail", strError);
                                    _systemLog(strError, g_stProjectInfo.szDTGatheringLogPath);
                                    break;
                                }

                                Sleep(500);
                            }
                        }

                        if (totalRecordCount <= 0 && !bDeleteCompleted)
                        {
                            SysLogOutPut(m_strLogTitle, _T("RAW 테이블 삭제 완료. 데이터 수집 재개..."), LOG_COLOR_BLUE);
                            bDeleteCompleted = true; // 플래그 설정하여 중복 출력 방지
                            b_ThreadTry = false;
                            g_DeleteInProgressEvent.ResetEvent();
                        }
                        else b_ThreadTry = true;
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

            else
            {
                b_ThreadTry = false;
            }
        }

        if (m_bEndThread)   break;
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