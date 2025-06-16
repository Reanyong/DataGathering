#pragma once

#include "Control_List/ListBoxControl/XListCtrl.h"
#include "Thread_TAGSearch.h"
// CDLG_TagMapping 대화 상자입니다.

class CDLG_TagMapping : public CDialog
{
	DECLARE_DYNAMIC(CDLG_TagMapping)

public:
	CDLG_TagMapping(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_TagMapping();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ENERGYTAG_MAPPING };
protected:
	CXListCtrl m_xListTagItems;
	CThread_TAGSearch *m_pThread_TagSearch;
	ST_ALLTAGLIST *m_STAllTagList;
	CAdo_Control *DB_Connect;
	
	int m_nPageItemCount;
	int m_nItemRowStart;
	int m_nListPageCheck;
	int m_nDBType;

	ST_CODEITEMs m_stGrp;
	ST_CODEITEMs m_stMtal;
	ST_CODEITEMs m_stUnit;
	ST_CODEITEMs m_stFlr;
	ST_CODEITEMs m_stClsf_Lv1;
	ST_CODEITEMs m_stClsf_Lv2;
	ST_CODEITEMs m_stClsf_Lv3;
	ST_CODEITEMs m_stClsf_Loc;
	ST_CODEITEMs m_stVirt;
	ST_CODEITEMs m_stCycle;
	ST_CODEITEMs m_stCycle_Unit;
	ST_CODEITEMs m_stBld_List;

	ST_SITE_INFO m_stSiteInfo;
protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	int ADO_Connection();
	void ADO_Close();
	CString Com_Error(const char *szLogName,_com_error *e);
	void StartThread();
	void StopThread();
	void GetType();
	int SelectCode(const char *szTableName,ST_CODE *stCodeList, const char *szOrder);
	int SelectCode_Count(const char *szTableName);
	int SelectSite();
	void SetListRowTagItems(const char *szNum,ST_TAG_LIST *pstTagInfo);
	void ControlEnable(BOOL bEnable);
	void SetTagMapping_PopUp(int nPos);
	void DeleteRegisterTagItem(int nPos);
	void ListRowAdd_Test(const char *szData,const char *szData1);
public:
	
	int GetSearchType();
	void DataShowTemp();
	void SetListUpdate(int nRowPos,ST_TAG_LIST *pstTagInfo);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedRadioUnregisteredTag();
	afx_msg void OnBnClickedRadioRegisterTag();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonBackItem();
	afx_msg void OnBnClickedButtonNextItem();
	afx_msg void OnBLClickedList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnBnClickedButtonDelete();
};
