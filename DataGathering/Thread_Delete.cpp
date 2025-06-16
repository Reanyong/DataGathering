#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TAGGather.h"
#include "Thread_Delete.h"

// ?꾩뿭 ?대깽??媛앹껜 - ??젣 ?묒뾽 吏꾪뻾 以??뚮┝??
CEvent g_DeleteInProgressEvent(FALSE, TRUE);

IMPLEMENT_DYNCREATE(Thread_Delete, CWinThread)

bool EnsureEventSignaledState()
{
    DWORD eventStatus = WaitForSingleObject(g_DeleteInProgressEvent.m_hObject, 0);

    // ?대깽?멸? ?좏샇 ?놁쓬 ?곹깭??寃쎌슦 濡쒓렇 湲곕줉 諛??좏샇 ?곹깭濡?蹂寃?
    if (eventStatus == WAIT_TIMEOUT) {
        g_DeleteInProgressEvent.SetEvent();

        // 濡쒓렇 湲곕줉
        CString logMsg = _T("?대깽?멸? ?좏샇 ?놁쓬 ?곹깭 諛쒓껄 - ?좏샇 ?곹깭濡?援먯젙??);
        _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "EventLog", logMsg);
        return false;  // ?대깽?멸? 鍮꾩젙???곹깭???
    }

    return true;  // ?대깽?멸? ?뺤긽 ?곹깭???
}

void ForceResetEvent()
{
    g_DeleteInProgressEvent.ResetEvent();

    // 濡쒓렇 湲곕줉
    CString logMsg = _T("?붾쾭源? ?대깽?멸? 媛뺤젣濡??좏샇 ?놁쓬 ?곹깭濡??ㅼ젙??);
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "EventLog", logMsg);
}

Thread_Delete::Thread_Delete()
{
    m_strLogTitle = "Delete Thread";
    m_bEndThread = FALSE;
    DB_Connect = NULL;

    b_ThreadTry = true;
    m_bDeleting = false;
    m_bRequestDelete = false;
    m_nDaysToKeep = 90;       // 湲곕낯媛?90??
    m_lastDBCheckTime = 0;
}

Thread_Delete::~Thread_Delete()
{
    // ?뚮㈇?먯뿉?쒕뒗 紐낆떆??由ъ냼???뺣━媛 ?꾩슂?섏? ?딆쓬
    // ExitInstance?먯꽌 ?뺣━??
}

BOOL Thread_Delete::InitInstance()
{
    CoInitialize(NULL);

    g_DeleteInProgressEvent.SetEvent();

    // 濡쒓렇 珥덇린??
    CString logMessage = _T("Delete Thread 珥덇린?붾맖");
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);

    g_DeleteInProgressEvent.SetEvent();

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

    // 濡쒓렇 ?④린湲?
    CString logMessage = _T("Delete Thread 醫낅즺??);
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);

    CoUninitialize();
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(Thread_Delete, CWinThread)
END_MESSAGE_MAP()

// Thread_Delete.cpp??Run() 硫붿꽌?쒖뿉???대깽??愿由?遺遺??섏젙

int Thread_Delete::Run()
{
    // 珥덇린 ?대깽???곹깭 ?뺤씤 諛??ㅼ젙
    DWORD eventStatus = WaitForSingleObject(g_DeleteInProgressEvent.m_hObject, 0);
    if (eventStatus == WAIT_TIMEOUT) {
        g_DeleteInProgressEvent.SetEvent();
        WriteLog(_T("?쒖옉 ???대깽???좏샇 ?놁쓬 ?곹깭 諛쒓껄 - ?좏샇 ?곹깭濡?蹂듦뎄"));
    }

    SysLogOutPut(m_strLogTitle, _T("?뺢린 ?곗씠????젣 ?ㅻ젅???쒖옉??), LOG_COLOR_BLUE);

    ST_DBINFO stDBInfo;
    ST_DATABASENAME stDBName;
    bool dbInfoInitialized = false;

    int lastCheckedDay = -1;
    int lastExecutedMonth = -1;  // 留덉?留됱쑝濡???젣 ?묒뾽???ㅽ뻾????異붽?
    int targetDeleteDay = 1;     // 留ㅼ썡 1?쇱뿉 ??젣 ?ㅽ뻾

    DWORD checkInterval = 60000; // 1遺꾨쭏???뺢린 寃??
    DWORD lastPeriodicCheck = GetTickCount();

    while (!m_bEndThread)
    {
        Sleep(1000);

        DWORD currentTime = GetTickCount();

        // 二쇨린?곸씤 DB ?곹깭 ?뺤씤 (1遺꾨쭏??
        if (currentTime - lastPeriodicCheck > checkInterval) {
            lastPeriodicCheck = currentTime;

            // ?꾩옱 ?좎쭨 ?뺤씤
            time_t now = time(0);
            tm* ltm = localtime(&now);

            // 留ㅼ썡 ?먮룞 ?ㅽ뻾???꾪븳 ?뚮옒洹?愿由?
            // 1. ?덈줈???ъ씠 ?섏뿀怨? ?꾩쭅 ?대쾲 ?ъ뿉 ??젣 ?묒뾽???섏? ?딆븯?ㅻ㈃ b_ThreadTry = true
            if (lastExecutedMonth != ltm->tm_mon) {
                if (!b_ThreadTry) {
                    b_ThreadTry = true;
                    CString resetMsg;
                    resetMsg.Format(_T("?덈줈?????쒖옉 - ??젣 ?ㅽ뻾 ?뚮옒洹??쒖꽦??(?꾩옱?? %d, 留덉?留됱떎?됱썡: %d)"),
                        ltm->tm_mon + 1, lastExecutedMonth + 1);
                    WriteLog(resetMsg);
                    SysLogOutPut(m_strLogTitle, resetMsg, LOG_COLOR_BLUE);
                }
            }

            // 濡쒓렇 ?④린湲?- 留ㅼ씪 ??踰덈쭔 湲곕줉
            if (lastCheckedDay != ltm->tm_mday) {
                CString dayCheckMsg;
                dayCheckMsg.Format(_T("?꾩옱: %d??%d??%d?? ??젣?덉젙?? %d?? ThreadTry: %s, Deleting: %s, 留덉?留됱떎?됱썡: %d??),
                    ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, targetDeleteDay,
                    b_ThreadTry ? _T("True") : _T("False"),
                    m_bDeleting ? _T("True") : _T("False"),
                    lastExecutedMonth + 1);
                _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", dayCheckMsg);
                lastCheckedDay = ltm->tm_mday;
            }

            // 留ㅼ썡 吏?뺣맂 ?좎쭨?대㈃???꾩쭅 ?대쾲 ?ъ뿉 ?ㅽ뻾?섏? ?딆븯?ㅻ㈃ ?먮룞 ??젣 ?뚮옒洹??ㅼ젙
            if (ltm->tm_mday == targetDeleteDay && !m_bDeleting && b_ThreadTry && lastExecutedMonth != ltm->tm_mon) {
                m_bRequestDelete = true;
                SysLogOutPut(m_strLogTitle, _T("留ㅼ썡 ?뺢린 ?곗씠???뺣━ ?덉빟??), LOG_COLOR_BLUE);

                CString logMsg;
                logMsg.Format(_T("??젣 ?묒뾽 ?붿껌 - ?꾩옱: %d??%d??%d?? 紐⑺몴?? %d?? ??젣吏꾪뻾以? %s, ?ㅽ뻾媛?? %s"),
                    ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, targetDeleteDay,
                    m_bDeleting ? _T("??) : _T("?꾨땲??),
                    b_ThreadTry ? _T("??) : _T("?꾨땲??));
                WriteLog(logMsg);
            }
        }

        // ??젣 ?묒뾽 ?붿껌???덇굅???먮룞 ?ㅽ뻾 議곌굔??異⑹”?섎㈃ ?ㅽ뻾
        if (m_bRequestDelete && !m_bDeleting) {
            m_bRequestDelete = false;  // ?붿껌 ?뚮옒洹?珥덇린??
            m_bDeleting = true;        // ??젣 ?묒뾽 吏꾪뻾 以??ㅼ젙

            bool deleteSuccess = false;

            try {
                // ??젣 ?묒뾽 ?쒖옉???ㅻⅨ ?ㅻ젅?쒖뿉寃??뚮┝
                CString statusMsg = _T("RAW ?뚯씠釉??곗씠???뺣━ ?쒖옉 - ?ㅻⅨ ?ㅻ젅???쇱떆 ?뺤?");
                WriteLog(statusMsg);
                g_DeleteInProgressEvent.ResetEvent();

                // DB ?곌껐 ?뺣낫 濡쒕뱶 (?꾩슂??
                if (!dbInfoInitialized) {
                    stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
                    stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
                    m_nDBType = stDBInfo.unDBType;
                    dbInfoInitialized = true;
                }

                // DB ?곌껐 媛앹껜 ?앹꽦 (?놁쑝硫?
                if (DB_Connect == NULL) {
                    DB_Connect = new CAdo_Control();
                    DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
                    DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);
                }

                // DB ?곌껐
                if (EnsureDBConnection()) {
                    // 湲곗? ?좎쭨 怨꾩궛
                    COleDateTime today = COleDateTime::GetCurrentTime();
                    COleDateTime cutoffDate = today - COleDateTimeSpan(m_nDaysToKeep, 0, 0, 0);

                    // 珥??덉퐫?????뺤씤
                    CString countSQL = GetRecordCountQuery(cutoffDate);

                    long totalRecordCount = 0;
                    DB_Connect->GetRecordCount(countSQL, &totalRecordCount);

                    if (totalRecordCount > 0) {
                        CString logMessage;
                        logMessage.Format(_T("??젣 ????덉퐫???? %ld"), totalRecordCount);
                        WriteLog(logMessage);
                        SysLogOutPut(m_strLogTitle, logMessage, LOG_COLOR_BLUE);

                        // 諛곗튂 ?ш린 10000媛쒖뵫 ??젣 吏꾪뻾
                        int batchSize = 10000;
                        int successfulBatches = 0;
                        bool continueDelete = true;

                        while (totalRecordCount > 0 && !m_bEndThread && continueDelete) {
                            // 二쇨린?곸쑝濡?DB ?곌껐 ?뺤씤
                            if (!EnsureDBConnection()) {
                                WriteLog(_T("DB ?곌껐 ?ㅽ뙣濡???젣 ?묒뾽 以묐떒"));
                                break;
                            }

                            // 諛곗튂 ??젣 ?ㅽ뻾
                            if (PerformBatchDelete(batchSize, cutoffDate)) {
                                successfulBatches++;
                                totalRecordCount -= batchSize;
                                if (totalRecordCount < 0) totalRecordCount = 0;

                                // 吏꾪뻾 ?곹솴 濡쒓렇
                                CString progressMsg;
                                progressMsg.Format(_T("??젣 吏꾪뻾 以? ?⑥? ?덉퐫??[%ld]"), totalRecordCount);
                                SysLogOutPut(m_strLogTitle, progressMsg, LOG_COLOR_BLUE);
                            }
                            else {
                                WriteLog(_T("諛곗튂 ??젣 ?ㅽ뙣, ?ㅼ쓬 諛곗튂 ?쒕룄"));
                                Sleep(2000);
                            }

                            Sleep(500);
                        }

                        // ??젣 ?묒뾽 ?꾨즺 硫붿떆吏
                        CString completionMsg;
                        if (totalRecordCount <= 0) {
                            completionMsg.Format(_T("?곗씠????젣 ?꾨즺. 珥?%d 諛곗튂 ?깃났?곸쑝濡?泥섎━??), successfulBatches);
                            SysLogOutPut(m_strLogTitle, completionMsg, LOG_COLOR_BLUE);
                            deleteSuccess = true;
                        }
                        else {
                            completionMsg.Format(_T("?곗씠????젣 以묐떒?? %ld ?덉퐫???⑥쓬"), totalRecordCount);
                            SysLogOutPut(m_strLogTitle, completionMsg, LOG_COLOR_RED);
                        }
                        WriteLog(completionMsg);
                    }
                    else {
                        SysLogOutPut(m_strLogTitle, _T("??젣???곗씠?곌? ?놁쓬"), LOG_COLOR_BLUE);
                        deleteSuccess = true;  // ??젣???곗씠?곌? ?녿뒗 寃껊룄 ?깃났?쇰줈 媛꾩＜
                    }
                }
                else {
                    WriteLog(_T("DB ?곌껐 ?ㅽ뙣濡???젣 ?묒뾽 痍⑥냼"));
                    SysLogOutPut(m_strLogTitle, _T("DB ?곌껐 ?ㅽ뙣"), LOG_COLOR_RED);
                }

                // DB ?곌껐 ?リ린
                if (DB_Connect != NULL) {
                    if (DB_Connect->GetDB_ConnectionStatus() == 1)
                        DB_Connect->DB_Close();
                    delete DB_Connect;
                    DB_Connect = NULL;
                }

            }
            catch (CDBException* e) {
                CString strError = e->m_strError;
                e->ReportError();
                e->Delete();

                CString errorMsg;
                errorMsg.Format(_T("DB ?덉쇅 諛쒖깮: %s"), strError);
                WriteLog(errorMsg);
                SysLogOutPut(m_strLogTitle, errorMsg, LOG_COLOR_RED);

            }
            catch (...) {
                WriteLog(_T("?????녿뒗 ?덉쇅 諛쒖깮"));
                SysLogOutPut(m_strLogTitle, _T("?????녿뒗 ?덉쇅 諛쒖깮"), LOG_COLOR_RED);
            }

            // ??젣 ?묒뾽 ?꾨즺 ?곹깭 ?ㅼ젙
            m_bDeleting = false;

            if (deleteSuccess) {
                // ?꾩옱 ?붿쓣 湲곕줉?섏뿬 ?대쾲 ?ъ뿉?????댁긽 ?ㅽ뻾?섏? ?딅룄濡???
                time_t now = time(0);
                tm* ltm = localtime(&now);
                lastExecutedMonth = ltm->tm_mon;
                b_ThreadTry = false;  // ?대쾲 ???ㅽ뻾 ?꾨즺??

                CString successMsg;
                successMsg.Format(_T("?대쾲 ??%d?? ??젣 ?묒뾽 ?꾨즺 - ?ㅼ쓬 ?ш퉴吏 ?湲?), ltm->tm_mon + 1);
                WriteLog(successMsg);
            }
            else {
                WriteLog(_T("??젣 ?묒뾽 ?ㅽ뙣 - ThreadTry ?뚮옒洹??좎??섏뿬 ?ъ떆??媛??));
                // ?ㅽ뙣??寃쎌슦 lastExecutedMonth瑜??낅뜲?댄듃?섏? ?딆븘???ъ떆??媛??
            }

            // ?ㅻⅨ ?ㅻ젅?쒖뿉寃???젣 ?묒뾽 ?꾨즺 ?뚮┝ - 諛섎뱶???몄텧
            g_DeleteInProgressEvent.SetEvent();
            CString resumeMsg = _T("?곗씠???뺣━ ?꾨즺 - 遺??섏쭛 ?ㅻ젅???ш컻");
            WriteLog(resumeMsg);

            SysLogOutPut(m_strLogTitle, _T("Delete Thread ?묒뾽 ?꾨즺"), LOG_COLOR_BLUE);
        }

        if (m_bEndThread) {
            break;
        }
    }

    // ?ㅻ젅??醫낅즺 ???대깽???좏샇 ?곹깭濡??ㅼ젙?섏뿬 ?ㅻⅨ ?ㅻ젅?쒓? 怨꾩냽 ?묐룞?섎룄濡???
    g_DeleteInProgressEvent.SetEvent();
    WriteLog(_T("?ㅻ젅??醫낅즺 ???대깽???좏샇 ?곹깭濡??ㅼ젙"));

    // 硫붿떆吏 ??醫낅즺
    PostThreadMessage(WM_QUIT, 0, 0);
    return CWinThread::Run();
}

// DB ?곌껐 ?곹깭 ?뺤씤 諛??ъ뿰寃?
bool Thread_Delete::EnsureDBConnection()
{
    if (DB_Connect == NULL) {
        return false;
    }

    // ?꾩옱 ?쒓컙
    DWORD currentTime = GetTickCount();

    // ?곌껐 ?뺤씤 媛꾧꺽 (10珥?
    if (currentTime - m_lastDBCheckTime < 10000 && DB_Connect->GetDB_ConnectionStatus() == 1) {
        return true;
    }

    m_lastDBCheckTime = currentTime;

    // ?곌껐???딆뼱???덉쑝硫??ъ뿰寃??쒕룄
    if (DB_Connect->GetDB_ConnectionStatus() != 1) {
        SysLogOutPut(m_strLogTitle, _T("DB ?곌껐 ?뺤씤 以?.."), LOG_COLOR_BLUE);
        BOOL bConnectCheck = DB_Connect->DB_Connection();
        if (bConnectCheck != TRUE) {
            SysLogOutPut(m_strLogTitle, _T("DB ?곌껐 ?ㅽ뙣"), LOG_COLOR_RED);
            return false;
        }
        SysLogOutPut(m_strLogTitle, _T("DB ?곌껐 ?깃났"), LOG_COLOR_BLUE);
    }

    return true;
}

// 諛곗튂 ??젣 ?섑뻾
bool Thread_Delete::PerformBatchDelete(int batchSize, const COleDateTime& cutoffDate)
{
    CString deleteSQL = GetDeleteQuery(batchSize, cutoffDate);
    TRACE("SQL Delete ?묒뾽 ?쒖옉: %s\n", (LPCTSTR)deleteSQL);

    SQLRETURN retcode = DB_Connect->SetQueryRun(deleteSQL);

    if (isSqlOk(retcode)) {
        CString logMessage;
        logMessage.Format("Batch Deleted %d records from HM_MINUTE_TREND_HISTORY.", batchSize);
        WriteLog(logMessage);
        return true;
    }
    else {
        CString strError = _T("Error during batch deletion.");
        WriteLog(strError);
        return false;
    }
}

// ??젣 荑쇰━ ?앹꽦
CString Thread_Delete::GetDeleteQuery(int batchSize, const COleDateTime& cutoffDate)
{
    CString deleteSQL;

    if (m_nDBType == DB_MSSQL) {
        deleteSQL.Format(_T("DELETE TOP(%d) FROM HM_MINUTE_TREND_HISTORY WHERE Record_date < '%04d%02d01'"),
            batchSize, cutoffDate.GetYear(), cutoffDate.GetMonth());
    }
    else if (m_nDBType == DB_POSTGRE) {
        ST_DATABASENAME stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
        CString strDBName;

        if (strlen(stDBName.szHMIDBName) > 1)
            strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", stDBName.szHMIDBName);
        else
            strDBName.Format("easy_hmi.HM_MINUTE_TREND_HISTORY");

        deleteSQL.Format(_T("DELETE FROM %s WHERE Record_date < '%04d%02d01' LIMIT %d"),
            strDBName, cutoffDate.GetYear(), cutoffDate.GetMonth(), batchSize);
    }

    return deleteSQL;
}

// ?덉퐫??移댁슫??荑쇰━ ?앹꽦
CString Thread_Delete::GetRecordCountQuery(const COleDateTime& cutoffDate)
{
    CString countSQL;

    if (m_nDBType == DB_MSSQL) {
        countSQL.Format(_T("SELECT COUNT(*) FROM HM_MINUTE_TREND_HISTORY WHERE RECORD_DATE < '%04d%02d01'"),
            cutoffDate.GetYear(), cutoffDate.GetMonth());
    }
    else if (m_nDBType == DB_POSTGRE) {
        ST_DATABASENAME stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
        CString strDBName;

        if (strlen(stDBName.szHMIDBName) > 1)
            strDBName.Format("%s.HM_MINUTE_TREND_HISTORY", stDBName.szHMIDBName);
        else
            strDBName.Format("easy_hmi.HM_MINUTE_TREND_HISTORY");

        countSQL.Format(_T("SELECT COUNT(*) FROM %s WHERE RECORD_DATE < '%04d%02d01'"),
            strDBName, cutoffDate.GetYear(), cutoffDate.GetMonth());
    }

    return countSQL;
}

// ?뱀젙 ?쇱닔 ?댁쟾 ?곗씠????젣 ?붿껌
void Thread_Delete::RequestDeleteData(int daysToKeep)
{
    if (daysToKeep > 0) {
        m_nDaysToKeep = daysToKeep;
    }

    m_bRequestDelete = true;

    CString logMessage;
    logMessage.Format(_T("?곗씠????젣 ?붿껌?? 蹂댁〈 湲곌컙: %d??), m_nDaysToKeep);
    SysLogOutPut(m_strLogTitle, logMessage, LOG_COLOR_BLUE);
}

// ?ㅻ젅??以묒? 紐낅졊
void Thread_Delete::StopThread()
{
    m_bEndThread = TRUE;

    // ??젣 ?묒뾽 以묒씠?덈떎硫??대깽???좏샇 ?곹깭濡??ㅼ젙
    if (m_bDeleting) {
        g_DeleteInProgressEvent.SetEvent();
    }
}

// 濡쒓렇 異쒕젰
void Thread_Delete::SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody)
{
    _addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, crBody, strMsg);
}

// 濡쒓렇 ?뚯씪 湲곕줉
void Thread_Delete::WriteLog(const CString& message)
{
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", message);
    _systemLog(message, g_stProjectInfo.szDTGatheringIniPath);
}
