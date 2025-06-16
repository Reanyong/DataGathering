#pragma once

// Thread_Delete 클래스 개선안
class Thread_Delete : public CWinThread
{
	DECLARE_DYNCREATE(Thread_Delete)

public:
	Thread_Delete();
	virtual ~Thread_Delete();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();

	// 스레드 제어 함수들
	void StopThread();														// 스레드 중지 명령
	void RequestDeleteData(int daysToKeep = 90);							// 특정 일수 이전 데이터 삭제 요청
	void SysLogOutPut(CString strLogName, CString strMsg, COLORREF crBody);

	// 속성
	CAdo_Control* DB_Connect;
	HWND m_WindHwnd;

	// 상태 플래그
	BOOL m_bEndThread;         // 스레드 종료 플래그
	CString m_strLogTitle;     // 로그 타이틀
	int m_nDBType;             // DB 타입
	bool b_ThreadTry;          // 스레드 동작 플래그

private:
	// 내부 상태 관리
	bool m_bDeleting;          // 현재 삭제 작업 수행 중 여부
	int m_nDaysToKeep;         // 보존할 일수 (기본 90일)
	bool m_bRequestDelete;     // 삭제 작업 요청 플래그
	DWORD m_lastDBCheckTime;   // 마지막 DB 연결 확인 시간

	// 내부 헬퍼 메서드
	bool EnsureDBConnection();													// DB 연결 확인 및 재연결
	bool PerformBatchDelete(int batchSize, const COleDateTime& cutoffDate);		// 배치 삭제 수행
	CString GetDeleteQuery(int batchSize, const COleDateTime& cutoffDate);		// 삭제 쿼리 생성
	CString GetRecordCountQuery(const COleDateTime& cutoffDate);				// 레코드 카운트 쿼리 생성
	void WriteLog(const CString& message);										// 로그 파일 기록

protected:
	DECLARE_MESSAGE_MAP()
};
