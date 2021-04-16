#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define NON_DIRECTION	0x0
#define UP_DIRECTION	0x1
#define DOWN_DIRECTION	0x2
//*���� ���� �ڵ�

// Find_Text_Dialog ��ȭ �����Դϴ�.

class Find_Text_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Find_Text_Dialog)

public:
	Find_Text_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Find_Text_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FIND_TEXT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CEdit m_Find_String;
	CButton m_FT_Up_Radio;
	CButton m_FT_Down_Radio;

	unsigned int Direction;
	CString Find_Str;
	//*����� ���ڿ��� �ޱ� ���� ����

};
