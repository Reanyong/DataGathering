#pragma once

#include "Thread_XmlTagValue.h"
#include "Control_List/ListBoxControl/XListCtrl.h"

// CFormView_TagMapping �� ���Դϴ�.

class CFormView_TagMapping : public CFormView
{
	DECLARE_DYNCREATE(CFormView_TagMapping)

protected:
	CFormView_TagMapping();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormView_TagMapping();

public:
	enum { IDD = IDD_FORMVIEW_TAGMAPPING };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CString m_strTitle;
	CXListCtrl m_xListHistory;
	ST_GATHERINFO m_stGatherInfo;

	BOOL m_bThreadStart;
	CThread_XmlTagValue *m_pThreadXmlTagValue;
public:
	void OutputHistory(const char *szData1,const char *szData2);
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	void StartThread();
	void StopThread();
	void GetTagInfoVo();
	int GetAllTagInfoList();
	void SetTagInfoList();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonMappingSet();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonTaglistSet();
	afx_msg void OnBnClickedButtonStart();
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


