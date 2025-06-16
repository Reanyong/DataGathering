#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TAGGather.h"
#include "Thread_Delete.h"

// 전역 이벤트 객체 - 삭제 작업 진행 중 알림용
CEvent g_DeleteInProgressEvent(FALSE, TRUE);

IMPLEMENT_DYNCREATE(Thread_Delete, CWinThread)

bool EnsureEventSignaledState()
{
    DWORD eventStatus = WaitForSingleObject(g_DeleteInProgressEvent.m_hObject, 0);

    // 이벤트가 신호 없음 상태인 경우 로그 기록 및 신호 상태로 변경
    if (eventStatus == WAIT_TIMEOUT) {
        g_DeleteInProgressEvent.SetEvent();

        // 로그 기록
        CString logMsg = _T("이벤트가 신호 없음 상태 발견 - 신호 상태로 교정됨");
        _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "EventLog", logMsg);
        return false;  // 이벤트가 비정상 상태였음
    }

    return true;  // 이벤트가 정상 상태였음
}

void ForceResetEvent()
{
    g_DeleteInProgressEvent.ResetEvent();

    // 로그 기록
    CString logMsg = _T("디버깅: 이벤트가 강제로 신호 없음 상태로 설정됨");
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
    m_nDaysToKeep = 90;       // 기본값 90일
    m_lastDBCheckTime = 0;
}

Thread_Delete::~Thread_Delete()
{
    // 소멸자에서는 명시적 리소스 정리가 필요하지 않음
    // ExitInstance에서 정리됨
}

BOOL Thread_Delete::InitInstance()
{
    CoInitialize(NULL);
    // 로그 초기화
    CString logMessage = _T("Delete Thread 초기화됨");
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

    // 로그 남기기
    CString logMessage = _T("Delete Thread 종료됨");
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);

    CoUninitialize();
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(Thread_Delete, CWinThread)
END_MESSAGE_MAP()

int Thread_Delete::Run()
{
    // 초기 이벤트 상태는 신호 상태로 설정 (다른 스레드가 대기하지 않도록)
    SysLogOutPut(m_strLogTitle, _T("정기 데이터 삭제 스레드 시작됨"), LOG_COLOR_BLUE);

    // DB 연결 정보 미리 준비
    ST_DBINFO stDBInfo;
    ST_DATABASENAME stDBName;
    bool dbInfoInitialized = false;

    int lastCheckedDay = -1;  // 마지막으로 확인한 날짜 (초기값 -1)
    int targetDeleteDay = 1;  // 매월 20일에 삭제 실행

    DWORD checkInterval = 60000; // 1분마다 정기 검사
    DWORD lastPeriodicCheck = GetTickCount();

    while (!m_bEndThread)
    {
        Sleep(1000); // 1초 대기

        DWORD currentTime = GetTickCount();

        // 주기적인 DB 상태 확인 (1분마다)
        if (currentTime - lastPeriodicCheck > checkInterval) {
            lastPeriodicCheck = currentTime;

            // 현재 날짜 확인
            time_t now = time(0);
            tm* ltm = localtime(&now);

            // 로그 남기기 - 매일 한 번만 기록
            if (lastCheckedDay != ltm->tm_mday) {
                CString dayCheckMsg;
                dayCheckMsg.Format(_T("현재 날짜: %d일, 삭제 예정일: %d일, 이벤트 상태: %s"),
                    ltm->tm_mday, targetDeleteDay,
                    (WaitForSingleObject(g_DeleteInProgressEvent.m_hObject, 0) == WAIT_OBJECT_0) ? _T("신호 상태") : _T("신호 없음 상태"));
                _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", dayCheckMsg);
                lastCheckedDay = ltm->tm_mday;
            }

            // 매월 지정된 날짜이면 자동 삭제 플래그 설정
            if (ltm->tm_mday == targetDeleteDay && !m_bDeleting && b_ThreadTry) {
                m_bRequestDelete = true;
                SysLogOutPut(m_strLogTitle, _T("매월 정기 데이터 정리 예약됨"), LOG_COLOR_BLUE);

                CString logMsg;
                logMsg.Format(_T("삭제 작업 요청 - 현재일: %d, 목표일: %d, 삭제 진행중: %s, 실행 가능: %s"),
                    ltm->tm_mday, targetDeleteDay,
                    m_bDeleting ? _T("예") : _T("아니오"),
                    b_ThreadTry ? _T("예") : _T("아니오"));
                WriteLog(logMsg);
            }
        }

        // 삭제 작업 요청이 있거나 자동 실행 조건이 충족되면 실행
        if (m_bRequestDelete && !m_bDeleting) {
            m_bRequestDelete = false;  // 요청 플래그 초기화
            m_bDeleting = true;        // 삭제 작업 진행 중 설정

            // 삭제 작업 시작을 다른 스레드에게 알림
            CString statusMsg = _T("RAW 테이블 데이터 정리 시작 - 다른 스레드 일시 정지");
            WriteLog(statusMsg);
            g_DeleteInProgressEvent.ResetEvent();

            // DB 연결 정보 로드 (필요시)
            if (!dbInfoInitialized) {
                stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
                stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
                m_nDBType = stDBInfo.unDBType;
                dbInfoInitialized = true;
            }

            // DB 연결 객체 생성 (없으면)
            if (DB_Connect == NULL) {
                DB_Connect = new CAdo_Control();
                DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
                DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);
            }

            // DB 연결
            if (EnsureDBConnection()) {
                try {
                    // 기준 날짜 계산
                    COleDateTime today = COleDateTime::GetCurrentTime();
                    COleDateTime cutoffDate = today - COleDateTimeSpan(m_nDaysToKeep, 0, 0, 0);

                    // 총 레코드 수 확인
                    CString countSQL = GetRecordCountQuery(cutoffDate);

                    long totalRecordCount = 0;
                    DB_Connect->GetRecordCount(countSQL, &totalRecordCount);

                    if (totalRecordCount > 0) {
                        CString logMessage;
                        logMessage.Format(_T("삭제 대상 레코드 수: %ld"), totalRecordCount);
                        WriteLog(logMessage);
                        SysLogOutPut(m_strLogTitle, logMessage, LOG_COLOR_BLUE);

                        // 배치 크기 10000개씩 삭제 진행
                        int batchSize = 10000;
                        int successfulBatches = 0;
                        bool continueDelete = true;

                        while (totalRecordCount > 0 && !m_bEndThread && continueDelete) {
                            // 주기적으로 DB 연결 확인
                            if (!EnsureDBConnection()) {
                                WriteLog(_T("DB 연결 실패로 삭제 작업 중단"));
                                break;
                            }

                            // 배치 삭제 실행
                            if (PerformBatchDelete(batchSize, cutoffDate)) {
                                successfulBatches++;
                                totalRecordCount -= batchSize;
                                if (totalRecordCount < 0) totalRecordCount = 0;

                                // 진행 상황 로그
                                CString progressMsg;
                                progressMsg.Format(_T("삭제 진행 중: 남은 레코드 [%ld]"), totalRecordCount);
                                SysLogOutPut(m_strLogTitle, progressMsg, LOG_COLOR_BLUE);
                            }
                            else {
                                WriteLog(_T("배치 삭제 실패, 다음 배치 시도"));
                                // 오류 발생 시 지연 후 재시도
                                Sleep(2000);
                            }

                            // 과도한 부하 방지를 위한 지연
                            Sleep(500);
                        }

                        // 삭제 작업 완료 메시지
                        CString completionMsg;
                        if (totalRecordCount <= 0) {
                            completionMsg.Format(_T("데이터 삭제 완료. 총 %d 배치 성공적으로 처리됨"), successfulBatches);
                            SysLogOutPut(m_strLogTitle, completionMsg, LOG_COLOR_BLUE);
                        }
                        else {
                            completionMsg.Format(_T("데이터 삭제 중단됨. %ld 레코드 남음"), totalRecordCount);
                            SysLogOutPut(m_strLogTitle, completionMsg, LOG_COLOR_RED);
                        }
                        WriteLog(completionMsg);
                    }
                    else {
                        SysLogOutPut(m_strLogTitle, _T("삭제할 데이터가 없거나 카운트 쿼리 실패"), LOG_COLOR_BLUE);
                    }
                }
                catch (CDBException* e) {
                    CString strError = e->m_strError;
                    e->ReportError();
                    e->Delete();

                    CString errorMsg;
                    errorMsg.Format(_T("DB 예외 발생: %s"), strError);
                    WriteLog(errorMsg);
                    SysLogOutPut(m_strLogTitle, errorMsg, LOG_COLOR_RED);
                }
                catch (...) {
                    WriteLog(_T("알 수 없는 예외 발생"));
                    SysLogOutPut(m_strLogTitle, _T("알 수 없는 예외 발생"), LOG_COLOR_RED);
                }

                // DB 연결 닫기 (리소스 절약)
                if (DB_Connect != NULL) {
                    DB_Connect->DB_Close();
                    delete DB_Connect;
                    DB_Connect = NULL;
                }
            }

            // 삭제 작업 완료 상태 설정
            m_bDeleting = false;
            b_ThreadTry = false;  // 다음 날에 다시 자동 실행 방지

            // 다른 스레드에게 삭제 작업 완료 알림
            CString resumeMsg = _T("데이터 정리 완료 - 분 수집 스레드 재개");
            WriteLog(resumeMsg);
            g_DeleteInProgressEvent.SetEvent();

            SysLogOutPut(m_strLogTitle, _T("Delete Thread 작업 완료"), LOG_COLOR_BLUE);
        }

        if (m_bEndThread) {
            break;
        }
    }

    // 스레드 종료 시 이벤트 신호 상태로 설정하여 다른 스레드가 계속 작동하도록 함
    if (!g_DeleteInProgressEvent.Lock(0)) {
        g_DeleteInProgressEvent.SetEvent();
        WriteLog(_T("스레드 종료 시 이벤트 신호 상태로 설정"));
    }

    // 메시지 큐 종료
    PostThreadMessage(WM_QUIT, 0, 0);
    return CWinThread::Run();
}

// DB 연결 상태 확인 및 재연결
bool Thread_Delete::EnsureDBConnection()
{
    if (DB_Connect == NULL) {
        return false;
    }

    // 현재 시간
    DWORD currentTime = GetTickCount();

    // 연결 확인 간격 (10초)
    if (currentTime - m_lastDBCheckTime < 10000 && DB_Connect->GetDB_ConnectionStatus() == 1) {
        return true;
    }

    m_lastDBCheckTime = currentTime;

    // 연결이 끊어져 있으면 재연결 시도
    if (DB_Connect->GetDB_ConnectionStatus() != 1) {
        SysLogOutPut(m_strLogTitle, _T("DB 연결 확인 중..."), LOG_COLOR_BLUE);
        BOOL bConnectCheck = DB_Connect->DB_Connection();
        if (bConnectCheck != TRUE) {
            SysLogOutPut(m_strLogTitle, _T("DB 연결 실패"), LOG_COLOR_RED);
            return false;
        }
        SysLogOutPut(m_strLogTitle, _T("DB 연결 성공"), LOG_COLOR_BLUE);
    }

    return true;
}

// 배치 삭제 수행
bool Thread_Delete::PerformBatchDelete(int batchSize, const COleDateTime& cutoffDate)
{
    CString deleteSQL = GetDeleteQuery(batchSize, cutoffDate);
    TRACE("SQL Delete 작업 시작: %s\n", (LPCTSTR)deleteSQL);

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

// 삭제 쿼리 생성
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

// 레코드 카운트 쿼리 생성
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

// 특정 일수 이전 데이터 삭제 요청
void Thread_Delete::RequestDeleteData(int daysToKeep)
{
    if (daysToKeep > 0) {
        m_nDaysToKeep = daysToKeep;
    }

    m_bRequestDelete = true;

    CString logMessage;
    logMessage.Format(_T("데이터 삭제 요청됨. 보존 기간: %d일"), m_nDaysToKeep);
    SysLogOutPut(m_strLogTitle, logMessage, LOG_COLOR_BLUE);
}

// 스레드 중지 명령
void Thread_Delete::StopThread()
{
    m_bEndThread = TRUE;

    // 삭제 작업 중이었다면 이벤트 신호 상태로 설정
    if (m_bDeleting) {
        g_DeleteInProgressEvent.SetEvent();
    }
}

// 로그 출력
void Thread_Delete::SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody)
{
    _addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, crBody, strMsg);
}

// 로그 파일 기록
void Thread_Delete::WriteLog(const CString& message)
{
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", message);
    _systemLog(message, g_stProjectInfo.szDTGatheringIniPath);
}
