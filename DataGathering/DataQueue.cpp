#include "stdafx.h"
#include "DataQueue.h"
#include <afxmt.h>


CheckedQueue::CheckedQueue()
{
	size = EV_STATION_MAX;
	m_st_CheckedData = new ST_CHECK_DATA[size];
	memset(m_st_CheckedData, 0, sizeof(ST_CHECK_DATA)*size);
	nCount = 0;
	front = 0;
	rear = 0;
}


CheckedQueue::~CheckedQueue()
{
	delete[] m_st_CheckedData;
}

bool CheckedQueue::empty()
{
	if(rear == front)
		return true;
	else 
		return false;
}

bool CheckedQueue::restore()
{
	memset(m_st_CheckedData, 0, sizeof(ST_CHECK_DATA)*size);
	nCount = 0;
	front = 0;
	rear = 0;

	return true;
}

bool CheckedQueue::isFull()
{
	if((rear + 1) % size == front) 
		return true;
	else 
		return false;
}


bool CheckedQueue::push(int nNewStn, UINT nNewFlag)
{/*
	while(m_mutex.Lock() == false)
	{
		Sleep(2);
	}
	*/

	m_mutex.Lock();

	if(isFull() != true)
	{
		m_st_CheckedData[rear].StnPos = nNewStn;
		
		m_st_CheckedData[rear].Flag = nNewFlag;
		
		rear = (rear + 1) % size;

		++nCount;

		m_mutex.Unlock();
		return true;
	}
	else
	{
		m_mutex.Unlock();
		return false;
	}

	m_mutex.Unlock();
}

ST_CHECK_DATA CheckedQueue::GetData()
{
	/*
	while(m_mutex.Lock() == false)
	{
		Sleep(2);
	}*/
	m_mutex.Lock();

	ST_CHECK_DATA retVal = {-1,-1};
	if(empty() != true)
	{
		retVal.StnPos = m_st_CheckedData[front].StnPos;
		
		retVal.Flag = m_st_CheckedData[front].Flag;
		
		front = (front + 1) % size;

		--nCount;

		m_mutex.Unlock();
		
		return retVal;
	}
	else
	{
		m_mutex.Unlock();

		return retVal;
	}

	m_mutex.Unlock();

	return retVal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WriteQueue::WriteQueue()
{
	size = 100000;
	m_st_FileWriteData = new ST_FILE_WRITE_DATA[size];
	memset(m_st_FileWriteData, 0, sizeof(ST_FILE_WRITE_DATA)*size);
	nCount = 0;
	front = 0;
	rear = 0;
}


WriteQueue::~WriteQueue()
{
	delete[] m_st_FileWriteData;
}

bool WriteQueue::empty()
{
	if(rear == front)
		return true;
	else 
		return false;
}

bool WriteQueue::isFull()
{
	if((rear + 1) % size == front) 
		return true;
	else 
		return false;
}


bool WriteQueue::push(CString sNewData)
{
	m_mutex.Lock();

	if(isFull() != true)
	{
		memcpy(m_st_FileWriteData[rear].temp,sNewData, sNewData.GetLength() );
		
		rear = (rear + 1) % size;

		++nCount;
		
		m_mutex.Unlock();

		return true;
	}
	else
	{
		m_mutex.Unlock();

		return false;
	}

	m_mutex.Unlock();
}

CString WriteQueue::GetData()
{
	m_mutex.Lock();

	CString retStr;

	retStr.Empty();
	
	if(empty() != true)
	{
		retStr.Format("%s",m_st_FileWriteData[front].temp);
		
		front = (front + 1) % size;

		--nCount;
		m_mutex.Unlock();

		return retStr;
	}
	else
	{
		m_mutex.Unlock();

		return retStr;
	}

	m_mutex.Unlock();
	
	return retStr;
}