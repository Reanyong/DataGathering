#pragma once

// Thread_Delete Ŭ���� ������
class Thread_Delete : public CWinThread
{
	DECLARE_DYNCREATE(Thread_Delete)

public:
	Thread_Delete();
	virtual ~Thread_Delete();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();

	// ������ ���� �Լ���
	void StopThread();														// ������ ���� ���
	void RequestDeleteData(int daysToKeep = 90);							// Ư�� �ϼ� ���� ������ ���� ��û
	bool IsDeleteInProgress() const { return m_bDeleting; }					// ���� ���� �۾� ���� ����
	void SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody);

	// �Ӽ�
	CAdo_Control* DB_Connect;
	HWND m_WindHwnd;

	// ���� �÷���
	BOOL m_bEndThread;         // ������ ���� �÷���
	CString m_strLogTitle;     // �α� Ÿ��Ʋ
	int m_nDBType;             // DB Ÿ��
	bool b_ThreadTry;          // ������ ���� �÷���

private:
	// ���� ���� ����
	bool m_bDeleting;          // ���� ���� �۾� ���� �� ����
	int m_nDaysToKeep;         // ������ �ϼ� (�⺻ 90��)
	bool m_bRequestDelete;     // ���� �۾� ��û �÷���
	DWORD m_lastDBCheckTime;   // ������ DB ���� Ȯ�� �ð�

	// ���� ���� �޼���
	bool EnsureDBConnection();                // DB ���� ���� Ȯ�� �� �翬��
	bool PerformBatchDelete(int batchSize);   // ��ġ ���� ����
	CString GetDeleteQuery(int batchSize);    // ���� ���� ����
	CString GetRecordCountQuery();            // ī��Ʈ ���� ����
	void WriteLog(const CString& message);    // �α� ���

protected:
	DECLARE_MESSAGE_MAP()
};