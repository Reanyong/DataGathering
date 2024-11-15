// DlgTagDic.cpp: 구현 파일
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DlgTagDic.h"
#include "afxdialogex.h"
#include "FormView_TAGGather.h"


// CDlgTagDic 대화 상자

IMPLEMENT_DYNAMIC(CDlgTagDic, CDialog)

CDlgTagDic::CDlgTagDic(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_TAGDIC, pParent)
{
	m_pParent = pParent;
    m_nEditItem = -1;
    m_nEditSubItem = -1;
    m_bIsModified = false;
}

CDlgTagDic::~CDlgTagDic()
{
    if (m_DBConnect)
    {
        delete m_DBConnect;
    }
}

void CDlgTagDic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INSERT, m_ListCtrlInsert);
	DDX_Control(pDX, IDC_LIST_EMS, m_ListCtrlEMS);
}


BEGIN_MESSAGE_MAP(CDlgTagDic, CDialog)
	ON_BN_CLICKED(ID_BTN_Insert, &CDlgTagDic::OnBnClickedSave)
    ON_BN_CLICKED(IDC_BTN_MOVE_RIGHT, &CDlgTagDic::OnBnClickedMoveRight)
    ON_BN_CLICKED(IDC_BTN_MOVE_LEFT, &CDlgTagDic::OnBnClickedMoveLeft)
    ON_BN_CLICKED(IDC_BTN_SEARCH_TAGGROUP, &CDlgTagDic::OnBnClickedSearchTagGroup)
    ON_EN_CHANGE(IDC_EDIT_SEARCH_TAGGROUP, &CDlgTagDic::OnEnChangeEditSearchTagGroup)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_INSERT, &CDlgTagDic::OnLvnEndLabelEdit)
	ON_WM_CLOSE()
    ON_WM_KEYDOWN()
    ON_BN_CLICKED(IDC_BTN_SEARCH_DBTAGGROUP, &CDlgTagDic::OnBnClickedBtnSearchDbtaggroup)
END_MESSAGE_MAP()


BOOL CDlgTagDic::OnInitDialog()
{
    CDialog::OnInitDialog();

    if (m_pParent != nullptr)
    {
        m_pParent->EnableWindow(FALSE);
    }

    SetWindowText(_T("HM_TAG_DIC 수정"));

    ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
    ST_DATABASENAME stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);

    m_strLogTitle = "TAG_DIC";

    m_nDBType = stDBInfo.unDBType;
    m_DBConnect = new CAdo_Control();
    m_DBConnect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE, m_WindHwnd, m_strLogTitle, g_stProjectInfo.szDTGatheringLogPath);
    m_DBConnect->DB_ConnectionInfo(stDBInfo.szServer, stDBInfo.szDB, stDBInfo.szID, stDBInfo.szPW, stDBInfo.unDBType);

    if (!m_DBConnect->DB_Connection())
    {
        AfxMessageBox(_T("DB 연결 실패"));
    }

    m_ListCtrlInsert.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS);
    m_ListCtrlInsert.InsertColumn(0, _T("태그 이름"), LVCFMT_LEFT, 165);
    m_ListCtrlInsert.InsertColumn(1, _T("태그 설명"), LVCFMT_LEFT, 180);
    m_ListCtrlInsert.InsertColumn(2, _T("데이터 타입"), LVCFMT_LEFT, 70);

    m_ListCtrlEMS.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_ListCtrlEMS.InsertColumn(0, _T("태그 이름"), LVCFMT_LEFT, 150);
    m_ListCtrlEMS.InsertColumn(1, _T("태그 설명"), LVCFMT_LEFT, 150);

    m_EditSearch.SubclassDlgItem(IDC_EDIT_SEARCH, this);
    m_BtnSearch.SubclassDlgItem(IDC_BTN_SEARCH, this);

    comboItems.Add(_T("순시"));
    comboItems.Add(_T("가동제어"));
    comboItems.Add(_T("센서"));
    comboItems.Add(_T("적산"));

    LoadTagDic();

    LoadEVTag();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgTagDic::OnCancel()
{
    if (m_bIsModified)
    {
        int result = AfxMessageBox(_T("작업 한 내용은 저장되지 않습니다. 계속 하시겠습니까?"), MB_YESNO | MB_ICONQUESTION);
        if (result == IDNO)
        {
            return;
        }
    }

    if (m_pParent != nullptr)
    {
        m_pParent->EnableWindow(TRUE);
    }

    CFormView_TAGGather* pParentFormView = dynamic_cast<CFormView_TAGGather*>(m_pParent);
    if (pParentFormView)
    {
        pParentFormView->LoadTagDic();
    }

    std::thread dbCloseThread([this]()
        {
            if (m_DBConnect)
            {
                m_DBConnect->DB_Close();
            }
        });
    dbCloseThread.detach();

    CDialog::OnCancel();
}

void CDlgTagDic::OnClose()
{
    if (m_bIsModified)
    {
        int result = AfxMessageBox(_T("작업 한 내용은 저장되지 않습니다. 계속 하시겠습니까?"), MB_YESNO | MB_ICONQUESTION);
        if (result == IDNO)
        {
            return;
        }

        CFormView_TAGGather* pParentFormView = dynamic_cast<CFormView_TAGGather*>(m_pParent);
        if (pParentFormView)
        {
            pParentFormView->LoadTagDic();
        }
    }

    if (m_pParent != nullptr)
    {
        m_pParent->EnableWindow(TRUE);
    }

    std::thread dbCloseThread([this]()
        {
            if (m_DBConnect)
            {
                m_DBConnect->DB_Close();
            }
        });
    dbCloseThread.detach();

    CDialog::OnClose();
}

void CDlgTagDic::OnBnClickedSave()
{
    CString strTruncateQuery;

    if (m_nDBType == DB_MSSQL)
    {
        strTruncateQuery = _T("TRUNCATE TABLE HM_TAG_DIC");
        m_DBConnect->SetQueryRun(strTruncateQuery);
    }

    else if (m_nDBType == DB_POSTGRE)
    {
        strTruncateQuery = _T("TRUNCATE TABLE easy_hmi.HM_TAG_DIC");
        SQLRETURN retcode;

        m_DBConnect->codbc->SQLAllocStmtHandle();

        retcode = m_DBConnect->SetQueryRun(strTruncateQuery);

        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
            AfxMessageBox(_T("테이블 초기화 실패"));
            return;
        }
    }

    CString strAlreadyExists;
    CString strNewInserts;

    bool hasErrorOccurred = false;

    for (int i = 0; i < m_ListCtrlInsert.GetItemCount(); ++i)
    {
        CString strTagID = m_ListCtrlInsert.GetItemText(i, 0);
        CString strTagDesc = m_ListCtrlInsert.GetItemText(i, 1);
        CString strDataType = m_ListCtrlInsert.GetItemText(i, 2);

        int nDataType = 0;

        if (strDataType == _T("순시")) { nDataType = 0; }
        else if (strDataType == _T("가동제어")) { nDataType = 1; }
        else if (strDataType == _T("센서")) { nDataType = 3; }
        else if (strDataType == _T("적산")) { nDataType = 5; }
        else {
            AfxMessageBox(_T("Unknown Data Type"));
            hasErrorOccurred = true;
            continue;
        }

        CString strDataTypeAsString;
        strDataTypeAsString.Format(_T("%d"), nDataType);

        if (m_nDBType == DB_MSSQL)
        {
            CString strInsertQuery;
            strInsertQuery.Format(_T("INSERT INTO HM_TAG_DIC (TAG_ID, TAG_NAME, TAG_DESC, TAG_TYPE, TAG_IOMODE, INTERFACE_TYPE, DATA_TYPE, ALARM_YN, GATHER_USE_YN) VALUES ('%s', '%s', '%s', '%d', '%d', '%d', '%s', '%d', '%d')"),
                strTagID, strTagID, strTagDesc, 3, 0, 0, strDataTypeAsString, 0, 0);

            if (m_DBConnect->SetQueryRun(strInsertQuery) > 0)
            {
                strNewInserts += strTagID + _T("\n");
            }
            else
            {
                strAlreadyExists += strTagID + _T("\n");
                hasErrorOccurred = true;
            }
        }
        else if (m_nDBType == DB_POSTGRE)
        {
            CString strInsertQuery;
            strInsertQuery.Format(_T("INSERT INTO easy_hmi.HM_TAG_DIC (tag_name, tag_desc, data_type, gather_use_yn) VALUES ('%s', '%s', %d, 0)"),
                strTagID, strTagDesc, nDataType);

            SQLRETURN retcode = m_DBConnect->codbc->SQLExecDirect(strInsertQuery);
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
            {
                strNewInserts += strTagID + _T("\n");
            }
            else
            {
                strAlreadyExists += strTagID + _T("\n");
                hasErrorOccurred = true;
            }
        }
    }
    if (hasErrorOccurred) { AfxMessageBox(_T("일부 항목이 저장되지 않았습니다. 이미 존재하거나 알 수 없는 데이터 유형이 있습니다.")); }
    else
    {
        AfxMessageBox(_T("모든 항목이 HM_TAG_DIC 테이블에 성공적으로 저장되었습니다."));
        m_bIsModified = false;          // 저장 성공 후 m_bIsModified를 false로 설정
        m_NewTags.clear();              // 추가된 항목 목록 초기화
    }

    LoadTagDic();
}


void CDlgTagDic::LoadTagDic()
{
    CString strQuery;
    if (m_nDBType == DB_MSSQL)
    {
        strQuery = _T("SELECT TAG_ID, TAG_DESC, DATA_TYPE FROM HM_TAG_DIC");

        _RecordsetPtr pRecordset = m_DBConnect->DB_OpenRecordSet(strQuery);
        if (!pRecordset)
        {
            AfxMessageBox(_T("HM_TAG_DIC Table 데이터 로드 실패"));
            return;
        }
        m_ListCtrlInsert.DeleteAllItems();

        while (!pRecordset->EndOfFile)
        {
            CString strTagID = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_ID"]->Value;
            CString strTagDesc = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_DESC"]->Value;
            int nDataType = pRecordset->Fields->Item["DATA_TYPE"]->Value;

            CString strDataType;
            int comboIndex = 0;
            switch (nDataType)
            {
            case 0:
                strDataType = _T("순시");
                comboIndex = 0;
                break;
            case 1:
                strDataType = _T("가동제어");
                comboIndex = 1;
                break;
            case 3:
                strDataType = _T("센서");
                comboIndex = 2;
                break;
            case 5:
                strDataType = _T("적산");
                comboIndex = 3;
                break;
            default:
                strDataType = _T("-");
                comboIndex = 4;
                break;
            }

            int nItem = m_ListCtrlInsert.InsertItem(m_ListCtrlInsert.GetItemCount(), strTagID);
            m_ListCtrlInsert.SetItemText(nItem, 1, strTagDesc);
            m_ListCtrlInsert.SetItemText(nItem, 2, strDataType);
            m_ListCtrlInsert.SetComboBox(nItem, 2, TRUE, &comboItems, 4, comboIndex); // 3번째 열을 콤보박스로 설정

            pRecordset->MoveNext();
        }

        pRecordset->Close();
    }

    else if (m_nDBType == DB_POSTGRE)
    {
        SQLRETURN retcode;

        strQuery = _T("SELECT tag_name, tag_desc, data_type FROM easy_hmi.hm_tag_dic");

        m_DBConnect->codbc->SQLAllocStmtHandle();
        retcode = m_DBConnect->SetQueryRun(strQuery);

        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        {
            AfxMessageBox(_T("HM_TAG_DIC Table 데이터 로드 실패"));
            return;
        }

        m_ListCtrlInsert.DeleteAllItems();

        while (true)
        {
            retcode = m_DBConnect->codbc->SQLFetch();
            if (retcode == SQL_NO_DATA) {
                break;
            }
            else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
                AfxMessageBox(_T("SQLFetch 에러"));
                break;
            }

            SQLCHAR tagID[256], tagDesc[256], dataType[256];
            SQLLEN tagIDLen, tagDescLen, dataTypeLen;

            m_DBConnect->codbc->SQLGetData(1, SQL_C_CHAR, tagID, sizeof(tagID), &tagIDLen);
            m_DBConnect->codbc->SQLGetData(2, SQL_C_CHAR, tagDesc, sizeof(tagDesc), &tagDescLen);
            m_DBConnect->codbc->SQLGetData(3, SQL_C_CHAR, dataType, sizeof(dataType), &dataTypeLen);

            CString strTagID(tagID), strTagDesc(tagDesc), strDataType(dataType);
            int nDataType = _ttoi(strDataType);

            int comboIndex = 0;
            switch (nDataType)
            {
            case 0:
                strDataType = _T("순시");
                comboIndex = 0;
                break;
            case 1:
                strDataType = _T("가동제어");
                comboIndex = 1;
                break;
            case 3:
                strDataType = _T("센서");
                comboIndex = 2;
                break;
            case 5:
                strDataType = _T("적산");
                comboIndex = 3;
                break;
            default:
                strDataType = _T("-");
                comboIndex = 4;
                break;
            }

            int nItem = m_ListCtrlInsert.InsertItem(m_ListCtrlInsert.GetItemCount(), strTagID);
            m_ListCtrlInsert.SetItemText(nItem, 1, strTagDesc);
            m_ListCtrlInsert.SetItemText(nItem, 2, strDataType);
            m_ListCtrlInsert.SetComboBox(nItem, 2, TRUE, &comboItems, 4, comboIndex); // 3번째 열을 콤보박스로 설정
        }
        //m_DBConnect->codbc->Close();
    }
}

void CDlgTagDic::OnBnClickedMoveRight()
{
    POSITION pos = m_ListCtrlEMS.GetFirstSelectedItemPosition();
    while (pos)
    {
        int nSelected = m_ListCtrlEMS.GetNextSelectedItem(pos);
        CString strTagID = m_ListCtrlEMS.GetItemText(nSelected, 0);
        CString strTagDesc = m_ListCtrlEMS.GetItemText(nSelected, 1);

        int nItem = m_ListCtrlInsert.InsertItem(m_ListCtrlInsert.GetItemCount(), strTagID);
        m_ListCtrlInsert.SetItemText(nItem, 1, strTagDesc);
        m_ListCtrlInsert.SetItemText(nItem, 2, _T("적산"));

        m_ListCtrlInsert.SetComboBox(nItem, 2, TRUE, &comboItems, 4, 3);

        m_NewTags.insert(strTagID);
        m_bIsModified = true;
    }
}

void CDlgTagDic::OnBnClickedMoveLeft()
{
    POSITION pos = m_ListCtrlInsert.GetFirstSelectedItemPosition();
    while (pos)
    {
        int nSelected = m_ListCtrlInsert.GetNextSelectedItem(pos);
        CString strTagID = m_ListCtrlInsert.GetItemText(nSelected, 0);
        m_ListCtrlInsert.DeleteItem(nSelected);
        m_NewTags.erase(strTagID);
        m_bIsModified = true;
        pos = m_ListCtrlInsert.GetFirstSelectedItemPosition();
    }
}

void CDlgTagDic::LoadEVTag()
{
    int nAiTagCount = EV_GetTotalAiTagCnt();
    int Err = 0;
    for (int i = 0; i < nAiTagCount; ++i)
    {
        ST_EV_TAG_ANALOG_INPUT* pTagInfo = EV_GetAiTagInfo(0, i, &Err);
        if (pTagInfo)
        {
            int nItem = m_ListCtrlEMS.InsertItem(m_ListCtrlEMS.GetItemCount(), pTagInfo->szName);
            m_ListCtrlEMS.SetItemText(nItem, 1, pTagInfo->szDesc);
        }
    }
}

void CDlgTagDic::OnBnClickedSearchTagGroup()
{
    CString strSearch;
    m_EditSearch.GetWindowText(strSearch);
    strSearch.MakeUpper();

    m_ListCtrlEMS.SetRedraw(FALSE);
    m_ListCtrlEMS.DeleteAllItems();

    int nAiTagCount = EV_GetTotalAiTagCnt();
    int Err = 0;
    for (int i = 0; i < nAiTagCount; ++i)
    {
        ST_EV_TAG_ANALOG_INPUT* pTagInfo = EV_GetAiTagInfo(0, i, &Err);
        if (pTagInfo)
        {
            CString strTagName(pTagInfo->szName);
            CString strTagDesc(pTagInfo->szDesc);

            strTagName.MakeUpper();
            strTagDesc.MakeUpper();

            if (strTagName.Find(strSearch) != -1 || strTagDesc.Find(strSearch) != -1)
            {
                int nItem = m_ListCtrlEMS.InsertItem(m_ListCtrlEMS.GetItemCount(), pTagInfo->szName);
                m_ListCtrlEMS.SetItemText(nItem, 1, pTagInfo->szDesc);
            }
        }
    }

    m_ListCtrlEMS.SetRedraw(TRUE);
    m_ListCtrlEMS.Invalidate();
    m_ListCtrlEMS.UpdateWindow();
}

void CDlgTagDic::OnEnChangeEditSearchTagGroup()
{
    // 검색 Edit Control의 내용이 변경될 때 호출됩니다.
    // 이 함수는 필요에 따라 구현할 수 있습니다.
}

BOOL CDlgTagDic::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // IDC_EDIT_SEARCH_TAGGROUP에 포커스가 있는 경우 왼쪽 리스트 검색 함수 호출
        if (GetFocus()->GetDlgCtrlID() == IDC_EDIT_SEARCH_TAGGROUP)
        {
            OnBnClickedSearchTagGroup();
            return TRUE;
        }
        // IDC_EDIT_SEARCH_DBTAGGROUP에 포커스가 있는 경우 오른쪽 리스트 검색 함수 호출
        else if (GetFocus()->GetDlgCtrlID() == IDC_EDIT_SEARCH_DBTAGGROUP)
        {
            OnBnClickedBtnSearchDbtaggroup();
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CDlgTagDic::OnLvnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    if (pDispInfo->item.pszText != nullptr)
    {
        int nItem = pDispInfo->item.iItem;
        int nSubItem = pDispInfo->item.iSubItem;

        m_ListCtrlInsert.SetItemText(nItem, nSubItem, pDispInfo->item.pszText);

        m_bIsModified = true;
    }

    *pResult = 0;
}

void CDlgTagDic::OnBnClickedBtnSearchDbtaggroup()
{
    CString strSearch;
    GetDlgItem(IDC_EDIT_SEARCH_DBTAGGROUP)->GetWindowText(strSearch);
    strSearch.MakeUpper();

    m_ListCtrlInsert.SetRedraw(FALSE);
    m_ListCtrlInsert.DeleteAllItems();

    CString strQuery;
    if (m_nDBType == DB_MSSQL)
    {
        strQuery.Format(_T("SELECT TAG_ID, TAG_DESC, DATA_TYPE FROM HM_TAG_DIC WHERE UPPER(TAG_ID) LIKE '%%%s%%' OR UPPER(TAG_DESC) LIKE '%%%s%%'"), strSearch, strSearch);

        _RecordsetPtr pRecordset = m_DBConnect->DB_OpenRecordSet(strQuery);
        if (!pRecordset)
        {
            AfxMessageBox(_T("HM_TAG_DIC Table 데이터 로드 실패"));
            return;
        }

        while (!pRecordset->EndOfFile)
        {
            CString strTagID = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_ID"]->Value;
            CString strTagDesc = (LPCTSTR)(_bstr_t)pRecordset->Fields->Item["TAG_DESC"]->Value;
            int nDataType = pRecordset->Fields->Item["DATA_TYPE"]->Value;

            CString strDataType;
            int comboIndex = 0;
            switch (nDataType)
            {
            case 0: strDataType = _T("순시"); comboIndex = 0; break;
            case 1: strDataType = _T("가동제어"); comboIndex = 1; break;
            case 3: strDataType = _T("센서"); comboIndex = 2; break;
            case 5: strDataType = _T("적산"); comboIndex = 3; break;
            default: strDataType = _T("-"); comboIndex = 4; break;
            }

            int nItem = m_ListCtrlInsert.InsertItem(m_ListCtrlInsert.GetItemCount(), strTagID);
            m_ListCtrlInsert.SetItemText(nItem, 1, strTagDesc);
            m_ListCtrlInsert.SetItemText(nItem, 2, strDataType);
            m_ListCtrlInsert.SetComboBox(nItem, 2, TRUE, &comboItems, 4, comboIndex);

            pRecordset->MoveNext();
        }
        pRecordset->Close();
    }

    else if (m_nDBType == DB_POSTGRE)
    {
        SQLRETURN retcode;
        strQuery.Format(_T("SELECT tag_name, tag_desc, data_type FROM easy_hmi.hm_tag_dic WHERE UPPER(tag_name) LIKE '%%%s%%' OR UPPER(tag_desc) LIKE '%%%s%%'"), strSearch, strSearch);

        m_DBConnect->codbc->SQLAllocStmtHandle();
        retcode = m_DBConnect->SetQueryRun(strQuery);

        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        {
            AfxMessageBox(_T("HM_TAG_DIC Table 데이터 로드 실패"));
            return;
        }

        while (true)
        {
            retcode = m_DBConnect->codbc->SQLFetch();
            if (retcode == SQL_NO_DATA) {
                break;
            }
            else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
                AfxMessageBox(_T("SQLFetch 에러"));
                break;
            }

            SQLCHAR tagID[256], tagDesc[256], dataType[256];
            SQLLEN tagIDLen, tagDescLen, dataTypeLen;

            m_DBConnect->codbc->SQLGetData(1, SQL_C_CHAR, tagID, sizeof(tagID), &tagIDLen);
            m_DBConnect->codbc->SQLGetData(2, SQL_C_CHAR, tagDesc, sizeof(tagDesc), &tagDescLen);
            m_DBConnect->codbc->SQLGetData(3, SQL_C_CHAR, dataType, sizeof(dataType), &dataTypeLen);

            CString strTagID(tagID), strTagDesc(tagDesc), strDataType(dataType);
            int nDataType = _ttoi(strDataType);

            int comboIndex = 0;
            switch (nDataType)
            {
            case 0: strDataType = _T("순시"); comboIndex = 0; break;
            case 1: strDataType = _T("가동제어"); comboIndex = 1; break;
            case 3: strDataType = _T("센서"); comboIndex = 2; break;
            case 5: strDataType = _T("적산"); comboIndex = 3; break;
            default: strDataType = _T("-"); comboIndex = 4; break;
            }

            int nItem = m_ListCtrlInsert.InsertItem(m_ListCtrlInsert.GetItemCount(), strTagID);
            m_ListCtrlInsert.SetItemText(nItem, 1, strTagDesc);
            m_ListCtrlInsert.SetItemText(nItem, 2, strDataType);
            m_ListCtrlInsert.SetComboBox(nItem, 2, TRUE, &comboItems, 4, comboIndex);
        }
    }

    m_ListCtrlInsert.SetRedraw(TRUE);
    m_ListCtrlInsert.Invalidate();
    m_ListCtrlInsert.UpdateWindow();
}
