#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Import_List_Dialog ��ȭ �����Դϴ�.

class Import_List_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Import_List_Dialog)

public:
	Import_List_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Import_List_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_IMPORT_LIST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked_ImportFromDir();
	afx_msg void OnBnClicked_ImportFromFile();
};
