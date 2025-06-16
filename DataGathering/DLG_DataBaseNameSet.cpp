// DLG_DataBaseNameSet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_DataBaseNameSet.h"
#include "afxdialogex.h"


// CDLG_DataBaseNameSet 대화 상자입니다.

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


// CDLG_DataBaseNameSet 메시지 처리기입니다.


BOOL CDLG_DataBaseNameSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ShowSettingData();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDLG_DataBaseNameSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CDLG_DataBaseNameSet::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strDataBuffer,strCheck;
	ST_DATABASENAME stDBName;	
	memset(&stDBName,0x00,sizeof(stDBName));
	GetDlgItem(IDC_EDIT_HMI_DBNAME)->GetWindowText(strDataBuffer);
	strcat_s(stDBName.szHMIDBName,strDataBuffer);
	GetDlgItem(IDC_EDIT_WEATHER_DBNAME)->GetWindowText(strDataBuffer);
	strcat_s(stDBName.szWTDBName,strDataBuffer);

	if(MessageBox("저장하시겠습니까?", "저장 확인", MB_YESNO)== IDNO)
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
