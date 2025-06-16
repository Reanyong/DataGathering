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
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CHART_SECTION, &CDlgEMSChart::OnNMDblclkListChartSection)
	ON_EN_KILLFOCUS(1, &CDlgEMSChart::OnEditKillFocus)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CDlgEMSChart::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pParent != nullptr)
	{
		m_pParent->EnableWindow(FALSE);
	}

	m_EditControl.Create(WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, CRect(0, 0, 0, 0), this, 1);
	m_EditControl.ShowWindow(SW_HIDE);

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
	m_ListTAGDIC.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	m_ListChartSection.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListChartSection.InsertColumn(0, _T("태그 이름"), LVCFMT_LEFT, 130);
	m_ListChartSection.InsertColumn(1, _T("차트 설명"), LVCFMT_LEFT , 80);
	m_ListChartSection.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	LoadTagDic();
	LoadComboBoxSections();

	return TRUE;
}

void CDlgEMSChart::OnNMDblclkListChartSection(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nItem = pNMItemActivate->iItem;
	int nSubItem = pNMItemActivate->iSubItem;

	if (nItem != -1 && nSubItem == 1)
	{
		StartEdit(nItem, nSubItem);
	}

	*pResult = 0;
}

void CDlgEMSChart::StartEdit(int nItem, int nSubItem)
{
	if (nItem < 0 || nSubItem < 0) return;

	m_nEditItem = nItem;
	m_nEditSubItem = nSubItem;

	CString strText = m_ListChartSection.GetItemText(nItem, nSubItem);

	CRect rect;
	m_ListChartSection.GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

	CRect listRect;
	m_ListChartSection.GetClientRect(&listRect);
	m_ListChartSection.ClientToScreen(&rect);
	this->ScreenToClient(&rect);

	m_EditControl.SetWindowText(strText);
	m_EditControl.MoveWindow(&rect);
	m_EditControl.ShowWindow(SW_SHOW);
	m_EditControl.SetFocus();
	m_EditControl.SetSel(0, -1);
}

void CDlgEMSChart::EndEdit()
{
	CString strText;
	m_EditControl.GetWindowText(strText);

	m_ListChartSection.SetItemText(m_nEditItem, m_nEditSubItem, strText);

	m_ListChartSection.SetItemState(m_nEditItem, 0, LVIS_SELECTED | LVIS_FOCUSED);

	m_ListChartSection.SetSelectionMark(-1);
	m_ListChartSection.RedrawItems(m_nEditItem, m_nEditItem);
	m_ListChartSection.UpdateWindow();

	m_EditControl.ShowWindow(SW_HIDE);

	this->SetFocus();
}

void CDlgEMSChart::OnEditKillFocus()
{
	EndEdit();
}

BOOL CDlgEMSChart::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		EndEdit();  // Enter 입력 시 편집 종료
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		m_EditControl.ShowWindow(SW_HIDE);  // Esc 입력 시 편집 취소
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
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
	CString selectedSection;
	m_ComboSection.GetLBText(m_ComboSection.GetCurSel(), selectedSection);

	CString iniFilePath = GetIniFilePath();

	for (int i = 1; ; ++i)
	{
		CString key;
		key.Format(_T("%d"), i);

		TCHAR buffer[256];
		DWORD result = GetPrivateProfileString(selectedSection, key, NULL, buffer, sizeof(buffer), iniFilePath);

		if (result == 0)
		{
			break;
		}

		WritePrivateProfileString(selectedSection, key, NULL, iniFilePath);
	}

	int itemCount = m_ListChartSection.GetItemCount();

	CString cntValue;
	cntValue.Format(_T("%d"), itemCount);
	WritePrivateProfileString(selectedSection, _T("CNT"), cntValue, iniFilePath);

	for (int i = 0; i < itemCount; ++i)
	{
		CString tagName = m_ListChartSection.GetItemText(i, 0);
		CString tagDesc = m_ListChartSection.GetItemText(i, 1);

		//CString value = tagName + _T(";") + tagDesc;
		CString value;
		value.Format(_T("'%s';%s"), tagName, tagDesc);

		CString key;
		key.Format(_T("%d"), i + 1);

		WritePrivateProfileString(selectedSection, key, value, iniFilePath);
	}
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

		strQuery = _T("SELECT tag_name, tag_desc FROM easy_hmi.hm_tag_dic order by 1");

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
		CString strTag = m_ListTAGDIC.GetItemText(nSelected, 0);
		CString strTagDesc = m_ListTAGDIC.GetItemText(nSelected, 1);

		bool isDuplicate = false;
		int itemCount = m_ListChartSection.GetItemCount();

		for (int i = 0; i < itemCount; ++i)
		{
			CString existingTag = m_ListChartSection.GetItemText(i, 0);
			if (strTag.CompareNoCase(existingTag) == 0)
			{
				isDuplicate = true;
				break;
			}
		}

		if (isDuplicate)
		{
			AfxMessageBox(_T("이미 선언되어있습니다."));
		}
		else
		{
			int nItem = m_ListChartSection.InsertItem(m_ListChartSection.GetItemCount(), strTag);	// TagID Add
			m_ListChartSection.SetItemText(nItem, 1, strTagDesc);									// TagDesc Add
		}


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
