// EasyView.h : header file
//

#if !defined(AFX_EASYVIEW_H__0991CB50_5EB5_4BA1_9A50_C9C20D26AC70__INCLUDED_)
#define AFX_EASYVIEW_H__0991CB50_5EB5_4BA1_9A50_C9C20D26AC70__INCLUDED_

#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

/////////////////////////////////////////////////////////////////////////////
// CEasyView window

class CEasyView
{
public:

	CEasyView();
	virtual ~CEasyView();

public:

	bool Initialize( LPCTSTR szEvPrjNm );
	bool Uninitialize();

	bool SetTagValue( LPCTSTR szTagNm, CString sVal  );
	bool SetTagValue( LPCTSTR szTagNm, double  dbVal );
	bool SetTagValue( LPCTSTR szTagNm, bool    bFlag );
	
	bool GetTagValue( LPCTSTR szTagNm, CString &sVal  );
	bool GetTagValue( LPCTSTR szTagNm, double  &dbVal );
	bool GetTagValue( LPCTSTR szTagNm, bool    &bFlag );

	bool GetEngMin( LPCTSTR szTagNm, double &dbVal );
	bool GetEngMax( LPCTSTR szTagNm, double &dbVal );
	bool GetAdMin ( LPCTSTR szTagNm, double &dbVal );
	bool GetAdMax ( LPCTSTR szTagNm, double &dbVal );

	bool GetHistoricalTagValues( LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataDi> &vtVals );
	bool GetHistoricalTagValues( LPCTSTR szTagNm, CTime tStart, CTime tEnd, std::vector<CDataAi> &vtVals );


	bool PutBufferValue( LPCTSTR szTagNm, CString sVal);

	int  OutPutValue( LPCTSTR szTag, CString sVal);
	bool OutPutValue( LPCTSTR szTagNm, double  dbVal );
	bool OutPutValue( LPCTSTR szTagNm, bool    bFlag );
	

	int GetDualModeStatus(int *pDualMode);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYVIEW_H__0991CB50_5EB5_4BA1_9A50_C9C20D26AC70__INCLUDED_)
