// DLG_IsSmartAccessSetting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_IsSmartAccessSetting.h"
#include "afxdialogex.h"


// CDLG_IsSmartAccessSetting 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLG_IsSmartAccessSetting, CDialog)

CDLG_IsSmartAccessSetting::CDLG_IsSmartAccessSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_IsSmartAccessSetting::IDD, pParent)
{

}

CDLG_IsSmartAccessSetting::~CDLG_IsSmartAccessSetting()
{
}

void CDLG_IsSmartAccessSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_IsSmartAccessSetting, CDialog)
END_MESSAGE_MAP()


// CDLG_IsSmartAccessSetting 메시지 처리기입니다.
