// DLG_ISmarAccesstSetting.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_ISmarAccesstSetting.h"
#include "afxdialogex.h"


// CDLG_ISmarAccesstSetting ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDLG_ISmarAccesstSetting, CDialog)

CDLG_ISmarAccesstSetting::CDLG_ISmarAccesstSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_ISmarAccesstSetting::IDD, pParent)
{

}

CDLG_ISmarAccesstSetting::~CDLG_ISmarAccesstSetting()
{
}

void CDLG_ISmarAccesstSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_ISmarAccesstSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CDLG_ISmarAccesstSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CDLG_ISmarAccesstSetting �޽��� ó�����Դϴ�.


BOOL CDLG_ISmarAccesstSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_stISmartAccess = _getInfoFileReadISmartAccess(g_stProjectInfo.szProjectIniPath);
	
	GetDlgItem(IDC_EDIT_SITE_NAME)->SetWindowText(m_stISmartAccess.szSiteName);
	GetDlgItem(IDC_EDIT_ID)->SetWindowText(m_stISmartAccess.szID);
	GetDlgItem(IDC_EDIT_PW)->SetWindowText(m_stISmartAccess.szPW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDLG_ISmarAccesstSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CDLG_ISmarAccesstSetting::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strBuffer,strBufferCheck;
	BOOL bTextCheck = FALSE;
	ST_FILE_ISMARTACCESS stInfo;
	memset(&stInfo,0x00,sizeof(stInfo));

	GetDlgItem(IDC_EDIT_SITE_NAME)->GetWindowText(strBuffer);
	strBufferCheck = m_stISmartAccess.szSiteName;
	if(strBufferCheck != strBuffer)
		bTextCheck = TRUE;
	strcat_s(stInfo.szSiteName,strBuffer);

	GetDlgItem(IDC_EDIT_ID)->GetWindowText(strBuffer);
	strBufferCheck = m_stISmartAccess.szID;
	if(strBufferCheck != strBuffer)
		bTextCheck = TRUE;
	strcat_s(stInfo.szID,strBuffer);

	GetDlgItem(IDC_EDIT_PW)->GetWindowText(strBuffer);
	strBufferCheck = m_stISmartAccess.szPW;
	if(strBufferCheck != strBuffer)
		bTextCheck = TRUE;
	strcat_s(stInfo.szPW,strBuffer);

	if(bTextCheck == TRUE)
	{
		if(MessageBox(" ����� ������ �ֽ��ϴ�?\n ���� �Ͻðڽ��ϱ�?", "���� Ȯ��", MB_YESNO)== IDNO)
			return;
	}

	_setInfoFileReadISmartAccess(&stInfo,g_stProjectInfo.szProjectIniPath);

	CDialog::OnOK();
}
