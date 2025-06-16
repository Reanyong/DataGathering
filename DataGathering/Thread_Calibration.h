#pragma once

// CThread_Calibration

class CDataDi
{
public:
	CTime tLogTime;
	bool bVal;
};

class CDataAi
{
public:
	CTime tLogTime;
	double dbVal;
};

class CThread_Calibration : public CWinThread
{
	DECLARE_DYNCREATE(CThread_Calibration)

public:
	CThread_Calibration();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_Calibration();

	void SetOwner(CWnd* hWnd) {
		m_pOwner = hWnd;
	}
	void Stop() {
		m_bEndThread = TRUE;
	}
	void SetStartTime(CTime ctSelTime) {
		m_ctSelTime = ctSelTime;
	}
	void SetProduct(int Product) //20210308 ksw 제품 분기
	{
		m_nProduct = Product;
	}
	//int GetTagList(const char *szDeviceItem,int nTheradCount,int nDBType,const char *szDBName);  //등록된 TAG 정보
	//CString Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *szLogMsg);

	//void SetSTListDataAll(std::list<ST_TagInfoList> **List_S_TagList, int nListCnt)
	//{
		//m_pList_ST_TagDivisionList = List_S_TagList;
		//m_nListCnt = nListCnt;
	//}

	bool GetHistoricalTagValues(LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataDi>& vtVals);
	bool GetHistoricalTagValues(LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataAi>& vtVals);

	void SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody);

	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CWnd* m_pOwner;
	BOOL m_bEndThread;
	HWND m_WindHwnd;

	std::list<ST_TagInfoList>** m_pList_ST_TagDivisionList;
	int m_nListCnt;

	CAdo_Control *DB_Connect;
	int m_nDBType;
	CString m_strLogTitle;

	CTime m_ctSelTime;
	CString m_strThreadName;
	int m_nProduct; //20210308 ksw 제품 분기 변수
protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


