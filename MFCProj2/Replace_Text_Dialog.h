#pragma once
#include "afxwin.h"


// Replace_Text_Dialog ��ȭ �����Դϴ�.

class Replace_Text_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Replace_Text_Dialog)

public:
	Replace_Text_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Replace_Text_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_REPLACE_TEXT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:

	virtual void OnOK();

	CEdit m_Replace_Text_Find;
	CEdit m_Replace_Text_Change;

	CString strtofind;
	CString strtochg;
	unsigned int code;
	bool FCH;
	//*���ڿ�, �ڵ尪, ���� �̸� ���� ���θ� �ޱ� ���� ����

	virtual BOOL OnInitDialog();
	CButton m_CP_Radio1;
	CButton m_CP_Radio2;
	CButton m_CP_Radio3;
	CButton m_RF_check;
};
