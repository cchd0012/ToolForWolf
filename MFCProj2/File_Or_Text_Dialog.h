#pragma once
#include "afxwin.h"


// File_Or_Text_Dialog ��ȭ �����Դϴ�.

class File_Or_Text_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(File_Or_Text_Dialog)

public:
	File_Or_Text_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~File_Or_Text_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_F_T_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_FN_Radio;
	CButton m_TE_Radio;

	unsigned int Type;
	virtual void OnOK();
};
