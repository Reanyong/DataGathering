#pragma once


// CDLG_TagMappingSet 대화 상자입니다.

class CDLG_TagMappingSet : public CDialog
{
	DECLARE_DYNAMIC(CDLG_TagMappingSet)

public:
	CDLG_TagMappingSet(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLG_TagMappingSet();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ENERGYTAG_MAPPING_SETTING };
protected:
	CComboBox m_comboGrp;
	CComboBox m_comboGrpTagType;
	CComboBox m_comboMtal;
	CComboBox m_comboUnit;
	CComboBox m_comboFlr;
	CComboBox m_comboClsf_Lv1;
	CComboBox m_comboClsf_Lv2;
	CComboBox m_comboClsf_Lv3;
	CComboBox m_comboClsf_Loc;
	CComboBox m_comboVirt;
	CComboBox m_comboCycle;
	CComboBox m_comboCycle_Unit;
	CComboBox m_comboBld_List;

public:
	int m_nDBType;
	CAdo_Control *DB_Connect;
	ST_TAG_LIST m_stTagInfo;

	ST_CODEITEMs m_stGrp;
	ST_CODEITEMs m_stSelectTagType;
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
	int m_nRowPos;
	LPVOID m_pCtrl;

protected:
	CString Com_Error(const char *szLogName,_com_error *e);
	void ComposeCompose();
	void TagGrpTypeSelect(const char *szkeyCode);	
	void DataAdd();
	int SelectCode(const char *szTableName);
	void InsertData(ST_TAG_LIST *stTagInfo);
public:
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCombo1();
	DECLARE_MESSAGE_MAP()
};

