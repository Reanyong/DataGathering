// DLG_TagMappingSet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_TagMappingSet.h"
#include "afxdialogex.h"
#include "DLG_TagMapping.h"


// CDLG_TagMappingSet 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_TagMappingSet, CDialog)

CDLG_TagMappingSet::CDLG_TagMappingSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_TagMappingSet::IDD, pParent)
{
	memset(&m_stSelectTagType,0x00,sizeof(m_stSelectTagType));
}

CDLG_TagMappingSet::~CDLG_TagMappingSet()
{
}

void CDLG_TagMappingSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboGrp);
	DDX_Control(pDX, IDC_COMBO2, m_comboGrpTagType);
	DDX_Control(pDX, IDC_COMBO3, m_comboMtal);
	DDX_Control(pDX, IDC_COMBO4, m_comboUnit);
	DDX_Control(pDX, IDC_COMBO5, m_comboFlr);
	DDX_Control(pDX, IDC_COMBO6, m_comboClsf_Lv1);
	DDX_Control(pDX, IDC_COMBO7, m_comboClsf_Lv2);
	DDX_Control(pDX, IDC_COMBO8, m_comboClsf_Lv3);
	DDX_Control(pDX, IDC_COMBO9, m_comboClsf_Loc);
	DDX_Control(pDX, IDC_COMBO10,m_comboVirt);
	DDX_Control(pDX, IDC_COMBO11,m_comboCycle);
	DDX_Control(pDX, IDC_COMBO12,m_comboCycle_Unit);
	DDX_Control(pDX, IDC_COMBO13,m_comboBld_List);

	
}


BEGIN_MESSAGE_MAP(CDLG_TagMappingSet, CDialog)
	ON_BN_CLICKED(IDOK, &CDLG_TagMappingSet::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDLG_TagMappingSet::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


CString CDLG_TagMappingSet::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "",strErrorID = "",strErrorCode;
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("Position : [%s],Description : [%s], DB Error Code : [%08lx], Code meaning : [%s], Source : [%s]",
		szLogName,(LPCTSTR)bstrDescription,e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource);

	if(m_nDBType == DB_ORACLE)
	{
		strErrorID.Format("%s",(LPCTSTR)bstrDescription);
		strErrorCode = strErrorID.Mid(4,5);//Left(nPos);
	}
	else if(m_nDBType == DB_MSSQL)
	{
		strErrorCode.Format("%08lx",e->Error());
	}

#ifdef _DEBUG
	TRACE("ProcessorName : [TagMapping-Dlg],Position : [%s][%s]\r\n",szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("[TagMapping-Dlg] Position : [DB Com Error..], LogName: [%s], %s",szLogName, strRunlog_E2);

	//	SetWriteLogFile(strRunlog_E2Log);
	//	SysLogOutPut(m_strLogTitle,strRunlog_E2,USER_COLOR_RED);
	Sleep(500);

	return strErrorCode;
}


// CDLG_TagMappingSet 메시지 처리기입니다.
BOOL CDLG_TagMappingSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ComposeCompose();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDLG_TagMappingSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::PreTranslateMessage(pMsg);
}


void CDLG_TagMappingSet::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DataAdd();
	
	CDialog::OnOK();
}

void CDLG_TagMappingSet::ComposeCompose()
{
	GetDlgItem(IDC_EDIT_TAG_NAME)->SetWindowText(m_stTagInfo.szTAG_NAME);
	GetDlgItem(IDC_EDIT_TAG_DESC)->SetWindowText(m_stTagInfo.szTAG_DESC);
	GetDlgItem(IDC_EDIT_SITE_NAME)->SetWindowText(m_stSiteInfo.szSiteName);

	int nRow = 0;
	int nCount = m_stGrp.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stGrp.GetData(nRow);
		m_comboGrp.AddString(stCode.szCodeName);
	}
	m_comboGrp.SetCurSel(0);

	TagGrpTypeSelect("00");

	nCount = m_stMtal.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stMtal.GetData(nRow);
		m_comboMtal.AddString(stCode.szCodeName);
	}
	m_comboMtal.SetCurSel(0);

	nCount = m_stUnit.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stUnit.GetData(nRow);
		m_comboUnit.AddString(stCode.szCodeName);
	}
	m_comboUnit.SetCurSel(0);

	nCount = m_stFlr.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stFlr.GetData(nRow);
		m_comboFlr.AddString(stCode.szCodeName);
	}
	m_comboFlr.SetCurSel(0);

	nCount = m_stClsf_Lv1.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stClsf_Lv1.GetData(nRow);
		m_comboClsf_Lv1.AddString(stCode.szCodeName);
	}
	m_comboClsf_Lv1.SetCurSel(0);
	
	nCount = m_stClsf_Lv2.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stClsf_Lv2.GetData(nRow);
		m_comboClsf_Lv2.AddString(stCode.szCodeName);
	}
	m_comboClsf_Lv2.SetCurSel(0);

	nCount = m_stClsf_Lv3.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stClsf_Lv3.GetData(nRow);
		m_comboClsf_Lv3.AddString(stCode.szCodeName);
	}
	m_comboClsf_Lv3.SetCurSel(0);

	nCount = m_stClsf_Loc.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stClsf_Loc.GetData(nRow);
		m_comboClsf_Loc.AddString(stCode.szCodeName);
	}
	m_comboClsf_Loc.SetCurSel(0);

	nCount = m_stVirt.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stVirt.GetData(nRow);
		m_comboVirt.AddString(stCode.szCodeName);
	}
	m_comboVirt.SetCurSel(2);

	nCount = m_stCycle.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stCycle.GetData(nRow);
		m_comboCycle.AddString(stCode.szCodeName);
	}
	m_comboCycle.SetCurSel(0);

	nCount = m_stCycle_Unit.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stCycle_Unit.GetData(nRow);
		m_comboCycle_Unit.AddString(stCode.szCodeName);
	}
	m_comboCycle_Unit.SetCurSel(0);

	nCount = m_stBld_List.GetSize();
	for( nRow = 0; nRow < nCount; nRow++)
	{
		ST_CODE stCode;
		memset(&stCode,0x00,sizeof(ST_CODE));
		stCode = m_stBld_List.GetData(nRow);
		m_comboBld_List.AddString(stCode.szCodeName);
	}
	m_comboBld_List.SetCurSel(0);
}


void CDLG_TagMappingSet::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nPos = m_comboGrp.GetCurSel();
	ST_CODE stCode;
	memset(&stCode,0x00,sizeof(ST_CODE));
	stCode = m_stGrp.GetData(nPos);
	TagGrpTypeSelect(stCode.szCode);

}

void CDLG_TagMappingSet::TagGrpTypeSelect(const char *szkeyCode)
{
	SelectCode(szkeyCode);
	m_comboGrpTagType.Invalidate(FALSE);
	
}

int CDLG_TagMappingSet::SelectCode(const char *szkeyCode)
{
	CString strMSGTitle = "SelectCode";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="";
	CString strCode,strCodeName;
	int nTotalRow = 0;

	//오라클 타입 쿼리 필요

	//MSSQL 타입 쿼리
	strQuery.Format("SELECT CODE,CODE_NAME	FROM [ENERGY_MANAGE].[dbo].[02_NEW_TAG_TP] WHERE GRP_CODE = '%s'",szkeyCode);

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nTotalRow  = pRs->RecordCount;

		m_stSelectTagType.DeleteItem();
		m_stSelectTagType.init(nTotalRow);
		m_comboGrpTagType.ResetContent();

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				for(int nRow = 0; nRow < nTotalRow ; nRow++)
				{
					DB_Connect->GetFieldValue(pRs, "CODE", strCode);				//TAG id
					DB_Connect->GetFieldValue(pRs, "CODE_NAME", strCodeName);			//TAG 명
					m_comboGrpTagType.AddString(strCodeName);

					ST_CODE stCode;
					memset(&stCode,0x00,sizeof(stCode));
					strcpy_s(stCode.szCode,strCode);
					strcpy_s(stCode.szCodeName,strCodeName);
					m_stSelectTagType.SetDataAdd_1(stCode);
#ifdef _DEBUG
					TRACE("ItemNum= %d, Value = %s,%s\n",nRow,strCode,strCodeName);
#endif
					pRs->MoveNext();
				}
				m_comboGrpTagType.SetCurSel(0);
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}	
			return nTotalRow;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			//			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB 접속 실패!",strMSGTitle);
				//				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}
		strRunlog_E2.Format("SELECT 실패 Event Error : %s",strMSGTitle);
		//		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

void CDLG_TagMappingSet::DataAdd()
{
	int nPos = m_comboGrp.GetCurSel();
	
	ST_CODE stCode = m_stGrp.GetData(nPos);

	strcpy_s(m_stTagInfo.szNEW_TAG_TP_GRP_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_TAG_TP_GRP_NM,stCode.szCodeName);

	nPos = m_comboGrpTagType.GetCurSel();
	stCode = m_stSelectTagType.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_TAG_TP_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_TAG_TP_NM,stCode.szCodeName);

	nPos = m_comboMtal.GetCurSel();
	stCode = m_stMtal.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_MTAL_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_MTAL_NM,stCode.szCodeName);

	nPos = m_comboUnit.GetCurSel();
	stCode = m_stUnit.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_UNIT_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_UNIT_NM,stCode.szCodeName);

	nPos = m_comboFlr.GetCurSel();
	stCode = m_stFlr.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_LOC_FLR_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_LOC_FLR_NM,stCode.szCodeName);

	nPos = m_comboClsf_Lv1.GetCurSel();
	stCode = m_stClsf_Lv1.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_01_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_01_NM,stCode.szCodeName);

	nPos = m_comboClsf_Lv2.GetCurSel();
	stCode = m_stClsf_Lv2.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_02_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_02_NM,stCode.szCodeName);

	nPos = m_comboClsf_Lv3.GetCurSel();
	stCode = m_stClsf_Lv3.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_03_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_03_NM,stCode.szCodeName);

	nPos = m_comboClsf_Loc.GetCurSel();
	stCode = m_stClsf_Loc.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_LOC_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_DV_CLSF_LOC_NM,stCode.szCodeName);

	nPos = m_comboVirt.GetCurSel();
	stCode = m_stVirt.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_VIRT_TAG_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_VIRT_TAG_NM,stCode.szCodeName);

	nPos = m_comboCycle.GetCurSel();
	stCode = m_stCycle.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_MEAU_CYCLE_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_MEAU_CYCLE_NM,stCode.szCodeName);

	nPos = m_comboCycle_Unit.GetCurSel();
	stCode = m_stCycle_Unit.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_MEAU_CYCLE_UNIT_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_MEAU_CYCLE_UNIT_NM,stCode.szCodeName);

	nPos = m_comboBld_List.GetCurSel();
	stCode = m_stBld_List.GetData(nPos);
	strcpy_s(m_stTagInfo.szNEW_BLD_BUND_code,stCode.szCode);
	strcpy_s(m_stTagInfo.szNEW_BLD_BUND_NM,stCode.szCodeName);

	InsertData(&m_stTagInfo);

	((CDLG_TagMapping *)(m_pCtrl))->SetListUpdate(m_nRowPos,&m_stTagInfo);
}

void CDLG_TagMappingSet::InsertData(ST_TAG_LIST *stTagInfo)
{
	CString strRunlog_E2;
	CString strQuery = "";
	CString strID = _IDCreated();
	strQuery.Format("INSERT INTO [ENERGY_MANAGE].[dbo].[TAG_MAPPING_LIST] "
					" (HMI_TAG_ID,DV_ID,NEW_TAG_TP_GRP,NEW_TAG_TP"
					",NEW_MTAL,NEW_UNIT,NEW_LOC_FLR,NEW_DV_CLSF_01,NEW_DV_CLSF_02,NEW_DV_CLSF_03 "
					" ,NEW_DV_CLSF_LOC,NEW_VIRT_TAG,NEW_MEAU_CYCLE,NEW_MEAU_CYCLE_UNIT,NEW_BLD_BUND,TAG_NAME,TAG_DESC,HIGH_LIMIT_VALUE,LOW_LIMIT_VALUE,TAG_TYPE) "
					" VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s'"
					",'%s','%s','%s','%s','%s','%s','%s',0,0,%d)"
					,m_stTagInfo.szHmiTag_ID,strID.Right(10),m_stTagInfo.szNEW_TAG_TP_GRP_code,m_stTagInfo.szNEW_TAG_TP_code,
					m_stTagInfo.szNEW_MTAL_code,m_stTagInfo.szNEW_UNIT_code,m_stTagInfo.szNEW_LOC_FLR_code,m_stTagInfo.szNEW_DV_CLSF_01_code,m_stTagInfo.szNEW_DV_CLSF_02_code,m_stTagInfo.szNEW_DV_CLSF_03_code,
					m_stTagInfo.szNEW_DV_CLSF_LOC_code,m_stTagInfo.szNEW_VIRT_TAG_code,m_stTagInfo.szNEW_MEAU_CYCLE_code,m_stTagInfo.szNEW_MEAU_CYCLE_UNIT_code,m_stTagInfo.szNEW_BLD_BUND_code,
					m_stTagInfo.szTAG_NAME,m_stTagInfo.szTAG_DESC,m_stTagInfo.nTAG_TYPE);

	int nResult = DB_Connect->SetQueryRun(strQuery);

	if(nResult < 1)
	{
		strRunlog_E2.Format("Position : [공휴일][설정], log : [Query Fail],[%s]",strQuery);
		//SetWriteLogFile(": [Set Query Error..],",strRunlog_E2);

#ifdef _DEBUG
		TRACE("Set Query Error-ProcessorName : [%s]\r\n",strRunlog_E2);
#endif
		return;
	}	
}
