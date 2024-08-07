// XListBox.cpp : implementation file
//

#include "stdafx.h"
#include "../../DataGathering.h"
#include "XListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXListBox

CXListBox::CXListBox()
{
	m_nStringCnt = 1024;
}

CXListBox::~CXListBox()
{
}


BEGIN_MESSAGE_MAP(CXListBox, CListBox)
	//{{AFX_MSG_MAP(CXListBox)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXListBox message handlers

void CXListBox::AddEntry(LPCTSTR lpszFrom, LPCTSTR lpszItem, COLORREF color)
{
	SYSTEMTIME Time;
	CString szMsg;

	GetLocalTime(&Time);
	szMsg.Format("[%04d/%02d/%02d %02d:%02d:%02d.%03d] %s->%s",
					Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds,
					lpszFrom, 
					lpszItem);

	if(GetCount() > m_nStringCnt) DeleteString(0);
	int index = AddString(szMsg);
	SetItemData(index, color);
}

void CXListBox::AddEntry_2(LPCTSTR lpszItem, COLORREF color)
{
	SYSTEMTIME Time;
	CString szMsg;
	
	GetLocalTime(&Time);
	szMsg.Format("[%04d/%02d/%02d %02d:%02d:%02d.%03d] %s",
		Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds,
		lpszItem);
	
	if(GetCount() > m_nStringCnt) DeleteString(0);
	int index = AddString(szMsg);
	SetItemData(index, color);
}

void CXListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	int nItem = lpMIS->itemID;
	if(nItem < 0 || nItem >= GetCount()) return;
	CPaintDC dc(this);
	CString sLabel;
	CRect rcLabel;

	GetText(nItem, sLabel);
	GetItemRect(nItem, rcLabel);

	// Using the flags below, calculate the required rectangle for the text and set the item height for this specific item based
	// on the return value (new height).
	int itemHeight = dc.DrawText(sLabel, -1, rcLabel, DT_WORDBREAK|DT_CALCRECT);
	lpMIS->itemHeight = itemHeight;
}

//*************************************************************
//
//  CXListBox :: DrawItem()
//
//  Purpose: 고유의 DC를 이용하여 Item들을 Drawing한다.
//          
//  Parameters: LPDRAWITEMSTRUCT lpDIS
//
//  Return: None
//*************************************************************
void CXListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if(lpDIS->itemID < 0 || lpDIS->itemID >= (UINT)GetCount()) return;
	CDC *pDC = CDC::FromHandle(lpDIS->hDC);

	// draw label text
	COLORREF colorTextSave;
	COLORREF colorBkSave;
	COLORREF rColor = (COLORREF)lpDIS->itemData; // RGB in item data

	CString sLabel;
	GetText(lpDIS->itemID, sLabel);
	CRect Rect = lpDIS->rcItem;

	// item selected
	if(lpDIS->itemState & ODS_SELECTED)
	{
		// draw color box
		CRect colorRect = lpDIS->rcItem;

		// draw label background
		//CBrush labelBrush(::GetSysColor(COLOR_HIGHLIGHT));
		COLORREF crHighlightBk = RGB(182, 189, 210);
		CBrush labelBrush(crHighlightBk);

		CRect labelRect = lpDIS->rcItem;
		//pDC->FillRect(&labelRect, &labelBrush);

		CPen pen(PS_SOLID, 1, RGB(10, 36, 106)), *pPen;
		CBrush *pBr;
		pPen = pDC->SelectObject(&pen);
		pBr = pDC->SelectObject(&labelBrush);
		pDC->Rectangle(labelRect);
		if(pBr) pDC->SelectObject(pBr);
		if(pPen) pDC->SelectObject(pPen);

		colorTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		//colorBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		colorBkSave = pDC->SetBkColor(crHighlightBk);
		pDC->DrawText(sLabel, -1, &lpDIS->rcItem, DT_VCENTER|DT_SINGLELINE);
		pDC->SetTextColor(colorTextSave);
		pDC->SetBkColor(colorBkSave);
		labelBrush.DeleteObject();
	}
	else
	{
		pDC->FillSolidRect(&Rect, ::GetSysColor(COLOR_WINDOW));
		colorTextSave = pDC->SetTextColor(rColor);
		int nOldbkMode = pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(sLabel, -1, &lpDIS->rcItem, DT_VCENTER|DT_SINGLELINE);
		pDC->SetTextColor(colorTextSave);
		pDC->SetBkMode(nOldbkMode);
	}
}

void CXListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}
