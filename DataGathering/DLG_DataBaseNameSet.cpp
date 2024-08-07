// DLG_DataBaseNameSet.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_DataBaseNameSet.h"
#include "afxdialogex.h"


// CDLG_DataBaseNameSet ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDLG_DataBaseNameSet, CDialog)

CDLG_DataBaseNameSet::CDLG_DataBaseNameSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_DataBaseNameSet::IDD, pParent)
{

}

CDLG_DataBaseNameSet::~CDLG_DataBaseNameSet()
{
}

void CDLG_DataBaseNameSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_DataBaseNameSet, CDialog)
	ON_BN_CLICKED(IDOK, &CDLG_DataBaseNameSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDLG_DataBaseNameSet �޽��� ó�����Դϴ�.


BOOL CDLG_DataBaseNameSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	ShowSettingData();
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDLG_DataBaseNameSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CDLG_DataBaseNameSet::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strDataBuffer,strCheck;
	ST_DATABASENAME stDBName;	
	memset(&stDBName,0x00,sizeof(stDBName));
	GetDlgItem(IDC_EDIT_HMI_DBNAME)->GetWindowText(strDataBuffer);
	strcat_s(stDBName.szHMIDBName,strDataBuffer);
	GetDlgItem(IDC_EDIT_WEATHER_DBNAME)->GetWindowText(strDataBuffer);
	strcat_s(stDBName.szWTDBName,strDataBuffer);

	if(MessageBox("�����Ͻðڽ��ϱ�?", "���� Ȯ��", MB_YESNO)== IDNO)
		return;

	_setDataBesaNameWrite(&stDBName, g_stProjectInfo.szProjectIniPath);

	CDialog::OnOK();
}

void CDLG_DataBaseNameSet::ShowSettingData()
{
	ST_DATABASENAME stDBName;	
	stDBName = _getDataBesaNameRead(g_stProjectInfo.szProjectIniPath);
	
	GetDlgItem(IDC_EDIT_HMI_DBNAME)->SetWindowText(stDBName.szHMIDBName);
	GetDlgItem(IDC_EDIT_WEATHER_DBNAME)->SetWindowText(stDBName.szWTDBName);

	Invalidate(FALSE);
}