// DlgEMSChart.cpp: 구현 파일
//

#include "stdafx.h"
#include "DataGathering.h"
#include "DlgEMSChart.h"
#include "afxdialogex.h"
#include "FormView_TAGGather.h"

// CDlgEMSChart 대화 상자

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
}


BEGIN_MESSAGE_MAP(CDlgEMSChart, CDialog)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgEMSChart::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BTN_CHART_MB_LEFT, &CDlgEMSChart::OnBnClickedMoveLeft)
	ON_BN_CLICKED(IDC_BTN_CHART_MB_RIGHT, &CDlgEMSChart::OnBnClickedMoveRight)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CDlgEMSChart::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
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
		pParentFormView->LoadTagDic(); // 부모 폼 뷰의 LoadTagDic 함수 호출
	}

	// DB 연결 해제
	m_DBConnect->DB_Close();

	CDialog::OnCancel();
}

void CDlgEMSChart::OnClose()
{
	// 부모 다이얼로그 활성화
	if (m_pParent != nullptr)
	{
		m_pParent->EnableWindow(TRUE);
	}

	// DB 연결 해제
	m_DBConnect->DB_Close();

	CDialog::OnClose();
}

void CDlgEMSChart::OnBnClickedSave()
{

}

void CDlgEMSChart::LoadTagDic()
{

}

void CDlgEMSChart::OnBnClickedMoveRight()
{

}

void CDlgEMSChart::OnBnClickedMoveLeft()
{

}

BOOL CDlgEMSChart::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}