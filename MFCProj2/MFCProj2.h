
// MFCProj2.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once


#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMFCProj2App:
// �� Ŭ������ ������ ���ؼ��� MFCProj2.cpp�� �����Ͻʽÿ�.
//


class CMFCProj2App : public CWinApp
{
public:
	CMFCProj2App();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMFCProj2App theApp;