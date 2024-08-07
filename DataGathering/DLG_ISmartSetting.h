#pragma once
#include "Control_List/ListBoxControl/XListCtrl.h"

// CDLG_ISmartSetting 대화 상자입니다.

class CDLG_ISmartSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_ISmartSetting)

public:
	CDLG_ISmartSetting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_ISmartSetting();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ISMART_SETTING };

protected:
	CAdo_Control *DB_Connect;
	CXListCtrl m_xListSiteList;

	ST_SITE_INTERLOCK m_stSiteInterlock[24];
	CFont m_Font;

	BOOL m_bDbClkCheck;
	int m_nSiteCount;
protected:
	void Com_Error(const char *szLogName,_com_error *e);
	void SetWriteLogFile(const char *sTitle,const char *szLogMsg);
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	BOOL GetSiteSearch();
	int GetSiteList();

	CString GetSiteSettingInfo(const char *szSiteID);

	void ListInsertItem(const char *szData1,const char *szData2,const char *szData3);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnClickedList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()		
public:
	afx_msg void OnBnClickedRadioY();
	afx_msg void OnBnClickedRadioN();
	afx_msg void OnBnClickedOk();
};
