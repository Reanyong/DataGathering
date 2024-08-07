// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")��
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե�
// Microsoft Foundation Classes Reference �� ���� ���� ������ ����
// �߰������� �����Ǵ� �����Դϴ�.
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������
// http://msdn.microsoft.com/officeui�� �����Ͻʽÿ�.
//
// Copyright (C) Microsoft Corporation
// ��� �Ǹ� ����.

// DataGatheringView.h : CDataGatheringView Ŭ������ �������̽�
//

#pragma once


class CDataGatheringView : public CView
{
protected: // serialization������ ��������ϴ�.
	CDataGatheringView();
	DECLARE_DYNCREATE(CDataGatheringView)

// Ư���Դϴ�.
public:
	CDataGatheringDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CDataGatheringView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // DataGatheringView.cpp�� ����� ����
inline CDataGatheringDoc* CDataGatheringView::GetDocument() const
   { return reinterpret_cast<CDataGatheringDoc*>(m_pDocument); }
#endif

