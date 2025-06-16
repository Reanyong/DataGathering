// DLG_ISmarAccesstSetting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_ISmarAccesstSetting.h"
#include "afxdialogex.h"


// CDLG_ISmarAccesstSetting 대화 상자입니다.

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


// CDLG_ISmarAccesstSetting 메시지 처리기입니다.


BOOL CDLG_ISmarAccesstSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_stISmartAccess = _getInfoFileReadISmartAccess(g_stProjectInfo.szProjectIniPath);
	
	GetDlgItem(IDC_EDIT_SITE_NAME)->SetWindowText(m_stISmartAccess.szSiteName);
	GetDlgItem(IDC_EDIT_ID)->SetWindowText(m_stISmartAccess.szID);
	GetDlgItem(IDC_EDIT_PW)->SetWindowText(m_stISmartAccess.szPW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDLG_ISmarAccesstSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CDLG_ISmarAccesstSetting::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		if(MessageBox(" 변경된 정보가 있습니다?\n 저장 하시겠습니까?", "저장 확인", MB_YESNO)== IDNO)
			return;
	}

	_setInfoFileReadISmartAccess(&stInfo,g_stProjectInfo.szProjectIniPath);

	CDialog::OnOK();
}
