#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Change_FDName_Dialog ��ȭ �����Դϴ�.

class Change_FDName_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Change_FDName_Dialog)

public:
	Change_FDName_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Change_FDName_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHANGE_FD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Input_Name;
	CEdit m_Output_Name;

	bool ShouldChange;

	CString Input_WStr;
	CString Output_WStr;
	unsigned int Meta_type;
	unsigned int code;
	//*�ۿ��� �ްų� ������ �� �� ������

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
};
