#pragma once

#include "Control_List/XMLCtrl/tinyxml.h"
#include "Control_List/XMLCtrl/XmlNodeWrapper.h"

// CThread_XmlTagValue

class CThread_XmlTagValue : public CWinThread
{
	DECLARE_DYNCREATE(CThread_XmlTagValue)

protected:
	CThread_XmlTagValue();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThread_XmlTagValue();


public:
	void SetOwner(CWnd* hWnd) {
		m_pOwner = hWnd;
	}
	void SetWnd(HWND hWnd) {
		m_WindHwnd = hWnd;
	}
	void Stop() {
		m_bEndThread = TRUE;
	}
	void SetLPVOID(LPVOID pCtrl)	{
		m_pCtrl = pCtrl;
	}
	BOOL GetStop() { return m_bEndThread;	}

	int GetTagInfoListState()	{	return m_nTagInfoListCheck;	}
	void SetTagInfoListOutput()	{ m_nTagInfoListCheck = 10;	}


	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	CString Com_Error(const char *szLogName,_com_error *e);
	void SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody);
	int GetTagTotalCountCheck();
	int GetRegisterTagList();
	int GetRegisterTagInfoList();
	int SetCreateXMLValue(CTime currentTime,int nTagCount,const char *szXmlSavePthe);
	CString GetTagValue(CString strTagName,int nTagTyp);

	int SetCreateXMLTagInfo(CTime currentTime,const char *szXmlSavePthe);
protected:
	CWnd* m_pOwner;
	LPVOID m_pCtrl;
	BOOL m_bEndThread;
	HWND m_WindHwnd;
	int m_nTagListCount;
	int m_nDBType;
	CString m_strLogTitle;

	CAdo_Control *DB_Connect;
	ST_ALLTAGValue *m_stTagValueList;
	int m_nTagInfoListCheck;

	ST_ALLTAGLIST *m_pstAllTagInfoList;

protected:
	virtual int Run();
	DECLARE_MESSAGE_MAP()
};


