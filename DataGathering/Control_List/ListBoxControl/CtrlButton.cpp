// CtrlButton.cpp : implementation file
//

#include "stdafx.h"
#include "CtrlButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlButton

CCtrlButton::CCtrlButton(CWnd* pParent, CRect rect, UINT nID, int nRow, int nColumn)
{
	m_nRow  = nRow;
	m_nCol = nColumn;
	//return pWnd->Create(_T("BUTTON"), lpszCaption, dwStyle, rect, pParentWnd, nID);

	CRect rc = rect;
	
	rect.DeflateRect(1, 1);
	BOOL b = Create(" ", WS_CHILD|BS_PUSHBUTTON, rect, pParent, nID);
	SetFont(pParent->GetFont());
	ShowWindow(SW_SHOW);
}

CCtrlButton::~CCtrlButton()
{
}


BEGIN_MESSAGE_MAP(CCtrlButton, CButton)
	//{{AFX_MSG_MAP(CCtrlButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrlButton message handlers

void CCtrlButton::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CButton::PostNcDestroy();

	delete this;
}

void CCtrlButton::OnClicked() 
{
	// TODO: Add your control notification handler code here
	/*
	CWnd *pWnd = GetParent();
	if (pWnd)
	{
		pWnd = pWnd->GetParent();
		if (pWnd)
			pWnd->PostMessage(WM_FIRE_MANUAL, m_nRow, m_nCol);
	}
	*/
}
