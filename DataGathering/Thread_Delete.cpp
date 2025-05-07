#include "stdafx.h"
#include "DataGathering.h"
#include "FormView_TAGGather.h"
#include "Thread_Delete.h"

// ���� �̺�Ʈ ��ü - ���� �۾� ���� �� �˸���
CEvent g_DeleteInProgressEvent(FALSE, TRUE);

IMPLEMENT_DYNCREATE(Thread_Delete, CWinThread)

Thread_Delete::Thread_Delete()
{
    m_strLogTitle = "Delete Thread";
    m_bEndThread = FALSE;
    DB_Connect = NULL;

    b_ThreadTry = true;
    m_bDeleting = false;
    m_bRequestDelete = false;
    m_nDaysToKeep = 90;       // �⺻�� 90��
    m_lastDBCheckTime = 0;
}

Thread_Delete::~Thread_Delete()
{
    // �Ҹ��ڿ����� ����� ���ҽ� ������ �ʿ����� ����
    // ExitInstance���� ������
}

BOOL Thread_Delete::InitInstance()
{
    CoInitialize(NULL);
    // �α� �ʱ�ȭ
    CString logMessage = _T("Delete Thread �ʱ�ȭ��");
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);
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

    // �α� �����
    CString logMessage = _T("Delete Thread �����");
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", logMessage);

    CoUninitialize();
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(Thread_Delete, CWinThread)
END_MESSAGE_MAP()

int Thread_Delete::Run()
{
    // �ʱ� �̺�Ʈ ���´� ��ȣ ���·� ���� (�ٸ� �����尡 ������� �ʵ���)
    g_DeleteInProgressEvent.SetEvent();

    // DB ���� ���� �̸� �غ�
    ST_DBINFO stDBInfo;
    ST_DATABASENAME stDBName;
    bool dbInfoInitialized = false;

    DWORD checkInterval = 3600000; // 1�ð����� ���� �˻�
    DWORD lastPeriodicCheck = GetTickCount();

    while (!m_bEndThread)
    {
        Sleep(1000); // 1�� ���

        DWORD currentTime = GetTickCount();

        // �ֱ����� DB ���� Ȯ�� (1�ð�����)
        if (currentTime - lastPeriodicCheck > checkInterval) {
            lastPeriodicCheck = currentTime;

            // DB ���� ���� �ʱ�ȭ�� �ʿ��ϸ� ����
            if (!dbInfoInitialized) {
                stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
                stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
                m_nDBType = stDBInfo.unDBType;
                dbInfoInitialized = true;
            }

            /*
            // ���� ��¥ Ȯ��
            time_t now = time(0);
            tm* ltm = localtime(&now);

            // �ſ� 1���̸� �ڵ� ���� �÷��� ����
            if (ltm->tm_mday == 7 && !m_bDeleting && b_ThreadTry) {
                m_bRequestDelete = true;
                SysLogOutPut(m_strLogTitle, _T("�ſ� ���� ������ ���� �����"), LOG_COLOR_BLUE);
            }
            */
        }

        // ���� �۾� ��û�� �ְų� �ڵ� ���� ������ �����Ǹ� ����
        if (m_bRequestDelete && !m_bDeleting) {
            m_bRequestDelete = false;  // ��û �÷��� �ʱ�ȭ
            m_bDeleting = true;        // ���� �۾� ���� �� ����

            // ���� �۾� ������ �ٸ� �����忡�� �˸�
            g_DeleteInProgressEvent.ResetEvent();

            // DB ���� ���� �ε� (�ʿ��)
            if (!dbInfoInitialized) {
                stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
                stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
                m_nDBType = stDBInfo.unDBType;
                dbInfoInitialized = true;
            }

            // DB ���� ��ü ���� (������)
            if (DB_Connect == NULL) {
                DB_Connect = new CAdo_Control();
                DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
                DB_Connect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);
            }

            // DB ����
            if (EnsureDBConnection()) {
                try {
                    // ���� ��¥ ���
                    COleDateTime today = COleDateTime::GetCurrentTime();
                    COleDateTime cutoffDate = today - COleDateTimeSpan(m_nDaysToKeep, 0, 0, 0);

                    // �� ���ڵ� �� Ȯ��
                    CString countSQL = GetRecordCountQuery();

                    long totalRecordCount = 0;
                    BOOL result = DB_Connect->GetRecordCount(countSQL, &totalRecordCount);

                    if (result && totalRecordCount > 0) {
                        CString logMessage;
                        logMessage.Format(_T("���� ��� ���ڵ� ��: %ld"), totalRecordCount);
                        WriteLog(logMessage);
                        SysLogOutPut(m_strLogTitle, logMessage, LOG_COLOR_BLUE);

                        // ��ġ ũ�� 10000�Ǿ� ���� ����
                        int batchSize = 10000;
                        int successfulBatches = 0;
                        bool continueDelete = true;

                        while (totalRecordCount > 0 && !m_bEndThread && continueDelete) {
                            // �ֱ������� DB ���� Ȯ��
                            if (!EnsureDBConnection()) {
                                WriteLog(_T("DB ���� ���з� ���� �۾� �ߴ�"));
                                break;
                            }

                            // ��ġ ���� ����
                            if (PerformBatchDelete(batchSize)) {
                                successfulBatches++;
                                totalRecordCount -= batchSize;
                                if (totalRecordCount < 0) totalRecordCount = 0;

                                // ���� ��Ȳ �α�
                                CString progressMsg;
                                progressMsg.Format(_T("���� ���� ��: ���� ���ڵ� [%ld]"), totalRecordCount);
                                SysLogOutPut(m_strLogTitle, progressMsg, LOG_COLOR_BLUE);
                            }
                            else {
                                WriteLog(_T("��ġ ���� ����, ���� ��ġ �õ�"));
                                // ���� �߻� �� ���� �� ��õ�
                                Sleep(2000);
                            }

                            // ������ ���� ������ ���� ����
                            Sleep(500);
                        }

                        // ���� �۾� �Ϸ� �޽���
                        CString completionMsg;
                        if (totalRecordCount <= 0) {
                            completionMsg.Format(_T("������ ���� �Ϸ�. �� %d ��ġ ���������� ó����"), successfulBatches);
                            SysLogOutPut(m_strLogTitle, completionMsg, LOG_COLOR_BLUE);
                        }
                        else {
                            completionMsg.Format(_T("������ ���� �ߴܵ�. %ld ���ڵ� ����"), totalRecordCount);
                            SysLogOutPut(m_strLogTitle, completionMsg, LOG_COLOR_RED);
                        }
                        WriteLog(completionMsg);
                    }
                    else {
                        SysLogOutPut(m_strLogTitle, _T("������ �����Ͱ� ���ų� ī��Ʈ ���� ����"), LOG_COLOR_BLUE);
                    }
                }
                catch (CDBException* e) {
                    CString strError = e->m_strError;
                    e->ReportError();
                    e->Delete();

                    CString errorMsg;
                    errorMsg.Format(_T("DB ���� �߻�: %s"), strError);
                    WriteLog(errorMsg);
                    SysLogOutPut(m_strLogTitle, errorMsg, LOG_COLOR_RED);
                }
                catch (...) {
                    WriteLog(_T("�� �� ���� ���� �߻�"));
                    SysLogOutPut(m_strLogTitle, _T("�� �� ���� ���� �߻�"), LOG_COLOR_RED);
                }

                // DB ���� �ݱ� (���ҽ� ����)
                if (DB_Connect != NULL) {
                    DB_Connect->DB_Close();
                }
            }

            // ���� �۾� �Ϸ� ���� ����
            m_bDeleting = false;
            b_ThreadTry = false;  // ���� 1�ϱ��� �ڵ� ���� ����

            // �ٸ� �����忡�� ���� �۾� �Ϸ� �˸�
            g_DeleteInProgressEvent.SetEvent();

            SysLogOutPut(m_strLogTitle, _T("Delete Thread �۾� �Ϸ�"), LOG_COLOR_BLUE);
        }

        // ������ ���� Ȯ��
        if (m_bEndThread) {
            break;
        }
    }

    // �޽��� ť ����
    PostThreadMessage(WM_QUIT, 0, 0);
    return CWinThread::Run();
}

// DB ���� ���� Ȯ�� �� �翬��
bool Thread_Delete::EnsureDBConnection()
{
    if (DB_Connect == NULL) {
        return false;
    }

    // ���� �ð�
    DWORD currentTime = GetTickCount();

    // ���� Ȯ�� ���� (10��)
    if (currentTime - m_lastDBCheckTime < 10000 && DB_Connect->GetDB_ConnectionStatus() == 1) {
        return true;
    }

    m_lastDBCheckTime = currentTime;

    // ������ ������ ������ �翬�� �õ�
    if (DB_Connect->GetDB_ConnectionStatus() != 1) {
        SysLogOutPut(m_strLogTitle, _T("DB ���� Ȯ�� ��..."), LOG_COLOR_BLUE);
        BOOL bConnectCheck = DB_Connect->DB_Connection();
        if (bConnectCheck != TRUE) {
            SysLogOutPut(m_strLogTitle, _T("DB ���� ����"), LOG_COLOR_RED);
            return false;
        }
        SysLogOutPut(m_strLogTitle, _T("DB ���� ����"), LOG_COLOR_BLUE);
    }

    return true;
}

// ��ġ ���� ����
bool Thread_Delete::PerformBatchDelete(int batchSize)
{
    CString deleteSQL = GetDeleteQuery(batchSize);
    TRACE("SQL Delete �۾� ����: %s\n", (LPCTSTR)deleteSQL);

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

// ���� ���� ����
CString Thread_Delete::GetDeleteQuery(int batchSize)
{
    COleDateTime today = COleDateTime::GetCurrentTime();
    COleDateTime cutoffDate = today - COleDateTimeSpan(m_nDaysToKeep, 0, 0, 0);
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

// ���ڵ� ī��Ʈ ���� ����
CString Thread_Delete::GetRecordCountQuery()
{
    COleDateTime today = COleDateTime::GetCurrentTime();
    COleDateTime cutoffDate = today - COleDateTimeSpan(m_nDaysToKeep, 0, 0, 0);
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

// ������ ���� ���
void Thread_Delete::StopThread()
{
    m_bEndThread = TRUE;

    // ���� �۾� ���̾��ٸ� �̺�Ʈ ��ȣ ���·� ����
    if (m_bDeleting) {
        g_DeleteInProgressEvent.SetEvent();
    }
}

// Ư�� �ϼ� ���� ������ ���� ��û
void Thread_Delete::RequestDeleteData(int daysToKeep)
{
    if (daysToKeep > 0) {
        m_nDaysToKeep = daysToKeep;
    }

    m_bRequestDelete = true;

    CString logMessage;
    logMessage.Format(_T("������ ���� ��û��. ���� �Ⱓ: %d��"), m_nDaysToKeep);
    SysLogOutPut(m_strLogTitle, logMessage, LOG_COLOR_BLUE);
}

// �α� ���
void Thread_Delete::SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody)
{
    _addSystemMsg(LOG_MESSAGE_2, USER_COLOR_BLUE, m_strLogTitle, crBody, strMsg);
}

// �α� ���� ���
void Thread_Delete::WriteLog(const CString& message)
{
    _WriteLogFile(g_stProjectInfo.szDTGatheringLogPath, "DeleteLog", message);
    _systemLog(message, g_stProjectInfo.szDTGatheringIniPath);
}