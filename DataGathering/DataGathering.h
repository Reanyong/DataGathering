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

// CDataGathering.h : CDataGathering ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CDataGatheringApp:
// �� Ŭ������ ������ ���ؼ��� CDataGathering.cpp�� �����Ͻʽÿ�.
//

class CDataGatheringApp : public CWinAppEx
{
public:
	CDataGatheringApp();


protected:
	BOOL GetEVInfoRead(CString strFileName);

protected:
	BOOL EventActionCheck();
	BOOL SetInitialize(CString strFileName);
// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDataGatheringApp theApp;
