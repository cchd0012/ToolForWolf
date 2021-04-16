#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Select_CP_Dialog ��ȭ �����Դϴ�.

class Select_CP_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Select_CP_Dialog)

public:
	Select_CP_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Select_CP_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CP_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CP_Radio1;
	CButton m_CP_Radio2;
	unsigned int CP_Code;

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
};
