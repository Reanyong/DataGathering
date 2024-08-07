// DlgEMSChart.cpp: 구현 파일
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DlgEMSChart.h"
#include "afxdialogex.h"
#include "FormView_TAGGather.h"
#include <fstream>
#include <vector>

IMPLEMENT_DYNAMIC(CDlgEMSChart, CDialog)

CDlgEMSChart::CDlgEMSChart(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_EMSCHART, pParent)
{
	m_pParent = pParent;
}

CDlgEMSChart::~CDlgEMSChart()
{
	if (m_DBConnect)
	{
		delete m_DBConnect;
	}
}

void CDlgEMSChart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHART_SECTION, m_ListChartSection);
	DDX_Control(pDX, IDC_LIST_CHART_TAGDIC, m_ListTAGDIC);
	DDX_Control(pDX, IDC_CB_CHARTLIST, m_ComboSection);
}


BEGIN_MESSAGE_MAP(CDlgEMSChart, CDialog)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgEMSChart::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BTN_CHART_MB_LEFT, &CDlgEMSChart::OnBnClickedMoveLeft)
	ON_BN_CLICKED(IDC_BTN_CHART_MB_RIGHT, &CDlgEMSChart::OnBnClickedMoveRight)
	ON_CBN_SELCHANGE(IDC_CB_CHARTLIST, &CDlgEMSChart::OnComboBoxSelectionChange)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CDlgEMSChart::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pParent != nullptr)
	{
		m_pParent->EnableWindow(FALSE);
	}

	m_strLogTitle = "EMS Chart Config";

	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	ST_DATABASENAME stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

	m_nDBType = stDBInfo.unDBType;
	m_DBConnect = new CAdo_Control();
	m_DBConnect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
	m_DBConnect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

	if (!m_DBConnect->DB_Connection())
	{
		AfxMessageBox(_T("DB 연결 실패"));
	}

	m_ListTAGDIC.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListTAGDIC.InsertColumn(0, _T("태그 이름"), LVCFMT_LEFT, 150);
	m_ListTAGDIC.InsertColumn(1, _T("태그 설명"), LVCFMT_LEFT, 150);

	m_ListChartSection.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListChartSection.InsertColumn(0, _T("태그 이름"), LVCFMT_LEFT, 130);
	m_ListChartSection.InsertColumn(1, _T("차트 설명"), LVCFMT_LEFT, 80);

	LoadTagDic();
	LoadComboBoxSections();

	return TRUE;
}

void CDlgEMSChart::OnCancel()
{
	if (m_pParent != nullptr)
	{
		m_pParent->EnableWindow(TRUE);
	}

	CFormView_TAGGather* pParentFormView = dynamic_cast<CFormView_TAGGather*>(m_pParent);
	if (pParentFormView)
	{
		pParentFormView->LoadTagDic();
	}

	m_DBConnect->DB_Close();

	CDialog::OnCancel();
}

void CDlgEMSChart::OnClose()
{
	if (m_pParent != nullptr)
	{
		m_pParent->EnableWindow(TRUE);
	}

	m_DBConnect->DB_Close();

	CDialog::OnClose();
}

void CDlgEMSChart::OnBnClickedSave()
{

}

void CDlgEMSChart::LoadTagDic()
{
	//AfxMessageBox("This is LoadTagDic Fuctions");
	   // TAG_DIC 테이블에서 데이터 읽어오기
	CString strQuery;
	if (m_nDBType == DB_MSSQL)
	{
		strQuery = _T("SELECT TAG_ID, TAG_DESC FROM HM_TAG_DIC");

		_RecordsetPtr pRecordset = m_DBConnect->DB_OpenRecordSet(strQuery);
		if (!pRecordset)
		{
			AfxMessageBox(_T("HM_TAG_DIC Table 데이터 로드 실패"));
			return;
		}
		m_ListTAGDIC.DeleteAllItems();

		while (!pRecordset->EndOfFile)
		{
			CString strTagID = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_ID"]->Value;
			CString strTagDesc = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_DESC"]->Value;

			int nItem = m_ListTAGDIC.InsertItem(m_ListTAGDIC.GetItemCount(), strTagID);
			m_ListTAGDIC.SetItemText(nItem, 1, strTagDesc);

			pRecordset->MoveNext();
		}
		pRecordset->Close();
	}

	else if (m_nDBType == DB_POSTGRE)
	{
		SQLRETURN retcode;

		strQuery = _T("SELECT tag_name, tag_desc FROM easy_hmi.hm_tag_dic");

		m_DBConnect->codbc->SQLAllocStmtHandle();
		retcode = m_DBConnect->SetQueryRun(strQuery);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			AfxMessageBox(_T("HM_TAG_DIC Table 데이터 로드 실패"));
			return;
		}

		m_ListTAGDIC.DeleteAllItems();

		while (true)
		{
			retcode = m_DBConnect->codbc->SQLFetch();
			if (retcode == SQL_NO_DATA) {
				break; // 더 이상 데이터가 없으면 루프 종료
			}
			else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				AfxMessageBox(_T("SQLFetch 에러"));
				break;
			}

			SQLCHAR tagID[256], tagDesc[256];
			SQLLEN tagIDLen, tagDescLen;

			m_DBConnect->codbc->SQLGetData(1, SQL_C_CHAR, tagID, sizeof(tagID), &tagIDLen);
			m_DBConnect->codbc->SQLGetData(2, SQL_C_CHAR, tagDesc, sizeof(tagDesc), &tagDescLen);

			CString strTagID(tagID), strTagDesc(tagDesc);

			int nItem = m_ListTAGDIC.InsertItem(m_ListTAGDIC.GetItemCount(), strTagID);
			m_ListTAGDIC.SetItemText(nItem, 1, strTagDesc);
		}
		//m_DBConnect->codbc->Close();
	}
}

void CDlgEMSChart::LoadEMSChart()
{
	m_ListChartSection.DeleteAllItems();

	CString selectedSection;
	m_ComboSection.GetLBText(m_ComboSection.GetCurSel(), selectedSection);

	std::vector<std::pair<CString, CString>> dataValues = GetDataForSection(selectedSection);

	for (int i = 0; i < dataValues.size(); ++i)
	{
		CString tagNames = dataValues[i].first;
		CString description = dataValues[i].second;

		int itemIndex = m_ListChartSection.InsertItem(i, tagNames);
		m_ListChartSection.SetItemText(itemIndex, 1, description);
	}
}

void CDlgEMSChart::LoadComboBoxSections()
{
	std::vector<CString> sections = GetIniSections();

	for (const auto& section : sections)
	{
		m_ComboSection.AddString(section);
	}

	if (!sections.empty())
	{
		m_ComboSection.SetCurSel(0);
		LoadEMSChart();
	}
}

void CDlgEMSChart::OnComboBoxSelectionChange()
{
	LoadEMSChart();
}

CString CDlgEMSChart::GetAppDirectory()
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString strPath = path;
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	return strPath;
}

CString CDlgEMSChart::GetIniFilePath()
{
	return GetAppDirectory() + _T("\\EMSChart.ini");
}

std::vector<CString> CDlgEMSChart::GetIniSections()
{
	std::vector<CString> sections;
	CString iniFilePath = GetIniFilePath();

	TCHAR buffer[2048];
	GetPrivateProfileSectionNames(buffer, 2048, iniFilePath);

	TCHAR* p = buffer;
	while (*p)
	{
		sections.push_back(CString(p));
		p += _tcslen(p) + 1;
	}

	return sections;
}

std::vector<std::pair<CString, CString>> CDlgEMSChart::GetDataForSection(const CString& section)
{
	std::vector<std::pair<CString, CString>> data;
	CString iniFilePath = GetIniFilePath();

	TCHAR buffer[2048];
	GetPrivateProfileSection(section, buffer, 2048, iniFilePath);

	TCHAR* p = buffer;
	while (*p)
	{
		CString line(p);
		int pos = line.Find('=');
		if (pos > 0)
		{
			CString key = line.Left(pos).Trim();
			CString value = line.Mid(pos + 1).Trim();

			if (key.CompareNoCase(_T("CNT")) != 0)
			{
				int semicolonPos = value.Find(';');
				if (semicolonPos > 0)
				{
					CString tagNames = value.Left(semicolonPos).Trim();
					CString description = value.Mid(semicolonPos + 1).Trim();

					tagNames.Replace(_T("'"), _T(""));

					data.push_back(std::make_pair(tagNames, description));
				}
				else
				{
					CString tagNames = value.Trim();
					tagNames.Replace(_T("'"), _T(""));
					data.push_back(std::make_pair(tagNames, _T("-")));
				}
			}
		}
		p += _tcslen(p) + 1;
	}
	return data;
}

void CDlgEMSChart::OnBnClickedMoveRight()
{
	POSITION pos = m_ListTAGDIC.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nSelected = m_ListTAGDIC.GetNextSelectedItem(pos);
		CString strTagID = m_ListTAGDIC.GetItemText(nSelected, 0);

		int nItem = m_ListChartSection.InsertItem(m_ListChartSection.GetItemCount(), strTagID);

		// bool type 활용 추가 예정
	}
}

void CDlgEMSChart::OnBnClickedMoveLeft()
{
	POSITION pos = m_ListChartSection.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nSelected = m_ListChartSection.GetNextSelectedItem(pos);
		CString strTagID = m_ListChartSection.GetItemText(nSelected, 0);

		m_ListChartSection.DeleteItem(nSelected);

		pos = m_ListChartSection.GetFirstSelectedItemPosition();

		// bool type 활용 추가 예정
	}
}

BOOL CDlgEMSChart::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}