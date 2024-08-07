#pragma once
#include "Control_List/ListBoxControl/XListCtrl.h"

// CDLG_GatherInfoSetting ��ȭ �����Դϴ�.

class CDLG_GatherInfoSetting : public CDialog
{
	DECLARE_DYNAMIC(CDLG_GatherInfoSetting)

public:
	CDLG_GatherInfoSetting(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDLG_GatherInfoSetting();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_GATHERINFO_SETTING };

protected:
	// ����Ʈ ���� ����Ʈ
	// ����̽� ����Ʈ
	CComboBox m_ComboDeicevList;
	CXListCtrl m_xListSiteList;
	CFont m_Font;
protected:

	ST_GATHERINFO_TYPE m_stInfoType;
	ST_DBINFO m_stDbInfo;

	CAdo_Control *DB_Connect;

	ST_GATHER_USE_LIST m_stDBGatherList[24];
	int m_nItemTotalCount;

protected:
	BOOL ComposeList(CXListCtrl &listCtrl, UINT nListID, UINT nPosListId,int nColumns,char *szColumn[]);
	BOOL SetDBConnect();
	void GetDeviceInfo();
	void GetGatherSetting();
	void SetGatherSetting();
	int ListInsertItem(const char *szData1,const char *szData2,const char *szData3);
	int Com_Error(const char *szLogName,_com_error *e);
public:
	void SetSettingSave() {SetGatherSetting();}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioGatherType1();
	afx_msg void OnBnClickedRadioGatherType2();
	
	afx_msg void OnBnClickedBtItemAdd();
	afx_msg void OnBnClickedBtItemDelete();
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnClose();
};