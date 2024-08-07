// DLG_EnergyTag_InitialSet.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_EnergyTag_InitialSet.h"
#include "afxdialogex.h"


// CDLG_EnergyTag_InitialSet ��ȭ �����Դϴ�.

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


// CDLG_EnergyTag_InitialSet �޽��� ó�����Դϴ�.


BOOL CDLG_EnergyTag_InitialSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CString strPath = _getXmlPath(g_stProjectInfo.szProjectIniPath);
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(strPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDLG_EnergyTag_InitialSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CDialog::PreTranslateMessage(pMsg);
}

void CDLG_EnergyTag_InitialSet::OnBnClickedButtonPath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	BrInfo.lpszTitle = "������ ������ ������ �ֽʽÿ�.";
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// ���̾�α� ����
	pidlBrowse = SHBrowseForFolder(&BrInfo);

	if( pidlBrowse != NULL )
	{
		BOOL bSuccess = ::SHGetPathFromIDList(pidlBrowse, buffer);

		if(!bSuccess)
		{
			MessageBox( _T("�߸��� ������ �Դϴ�."), _T(""), MB_OKCANCEL|MB_ICONASTERISK );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strPath;
	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(strPath);
	if(strPath.IsEmpty() == TRUE)
	{
		AfxMessageBox("��������� �ʿ��մϴ�");
		return;
	}
	else
		_setXmlPath(strPath,g_stProjectInfo.szProjectIniPath);

	CDialog::OnOK();
}
