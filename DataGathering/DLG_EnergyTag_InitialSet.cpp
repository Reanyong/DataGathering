// DLG_EnergyTag_InitialSet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_EnergyTag_InitialSet.h"
#include "afxdialogex.h"


// CDLG_EnergyTag_InitialSet 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_EnergyTag_InitialSet, CDialog)

CDLG_EnergyTag_InitialSet::CDLG_EnergyTag_InitialSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_EnergyTag_InitialSet::IDD, pParent)
{

}

CDLG_EnergyTag_InitialSet::~CDLG_EnergyTag_InitialSet()
{
}

void CDLG_EnergyTag_InitialSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_EnergyTag_InitialSet, CDialog)
	ON_BN_CLICKED(IDOK, &CDLG_EnergyTag_InitialSet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_PATH, &CDLG_EnergyTag_InitialSet::OnBnClickedButtonPath)
END_MESSAGE_MAP()


// CDLG_EnergyTag_InitialSet 메시지 처리기입니다.


BOOL CDLG_EnergyTag_InitialSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString strPath = _getXmlPath(g_stProjectInfo.szProjectIniPath);
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(strPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDLG_EnergyTag_InitialSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::PreTranslateMessage(pMsg);
}

void CDLG_EnergyTag_InitialSet::OnBnClickedButtonPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*CString strPath = "";
	CFileDialog fDlg(TRUE,"xml",NULL,OFN_HIDEREADONLY,"Xml Files (*.xml)|*.xml||");
	if (fDlg.DoModal() == IDOK)
	{
		strPath = fDlg.GetPathName();
	}*/
	ITEMIDLIST *pidlBrowse;
	CString strPath;
	char buffer[MAX_PATH];
	BROWSEINFO BrInfo;

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset( &BrInfo, 0, sizeof(BrInfo) );
	BrInfo.pszDisplayName = buffer;
	BrInfo.lpszTitle = "저장할 폴더를 선택해 주십시오.";
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 다이얼로그 띄우기
	pidlBrowse = SHBrowseForFolder(&BrInfo);

	if( pidlBrowse != NULL )
	{
		BOOL bSuccess = ::SHGetPathFromIDList(pidlBrowse, buffer);

		if(!bSuccess)
		{
			MessageBox( _T("잘못된 폴더명 입니다."), _T(""), MB_OKCANCEL|MB_ICONASTERISK );
			return;
		}
		else
		{
			GetDlgItem(IDC_EDIT_PATH)->SetWindowText(buffer);
		}
	}
}


void CDLG_EnergyTag_InitialSet::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strPath;
	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(strPath);
	if(strPath.IsEmpty() == TRUE)
	{
		AfxMessageBox("경로지정이 필요합니다");
		return;
	}
	else
		_setXmlPath(strPath,g_stProjectInfo.szProjectIniPath);

	CDialog::OnOK();
}
