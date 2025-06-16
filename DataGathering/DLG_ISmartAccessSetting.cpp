// DLG_ISmartAccessSetting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DLG_ISmartAccessSetting.h"
#include "afxdialogex.h"


// DLG_ISmartAccessSetting 대화 상자입니다.

IMPLEMENT_DYNAMIC(DLG_ISmartAccessSetting, CDialog)

DLG_ISmartAccessSetting::DLG_ISmartAccessSetting(CWnd* pParent /*=NULL*/)
	: CDialog(DLG_ISmartAccessSetting::IDD, pParent)
{

}

DLG_ISmartAccessSetting::~DLG_ISmartAccessSetting()
{
}

void DLG_ISmartAccessSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DLG_ISmartAccessSetting, CDialog)
END_MESSAGE_MAP()


// DLG_ISmartAccessSetting 메시지 처리기입니다.
