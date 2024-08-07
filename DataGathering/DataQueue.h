#if !defined(AFX_DataQueue_H__44EF4F53_4736_4BB3_A1EC_F52C1FF3052E__INCLUDED_)
#define AFX_DataQueue_H__44EF4F53_4736_4BB3_A1EC_F52C1FF3052E__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <afxmt.h>



class CheckedQueue
{
public:
	int front;
	int rear;
	UINT nCount;
	int size;
	ST_CHECK_DATA* m_st_CheckedData;

	CMutex m_mutex;

	CheckedQueue();
	~CheckedQueue();

	bool empty();
	bool restore();
	bool isFull();
	bool push(int nNewStn, UINT nNewFlag);
	ST_CHECK_DATA GetData();	
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WriteQueue
{
public:
	int front;
	int rear;
	UINT nCount;
	int size;
	ST_FILE_WRITE_DATA* m_st_FileWriteData;

	CMutex m_mutex;

	WriteQueue();
	~WriteQueue();

	bool empty();
	bool isFull();
	bool push(CString sNewData);
	CString GetData();	
};

#endif