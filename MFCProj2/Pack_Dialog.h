#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Pack_Dialog ��ȭ �����Դϴ�.

class Pack_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Pack_Dialog)

public:
	Pack_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Pack_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PACK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Wolf_Package_Name;

	afx_msg void OnBnClickedPack();

	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	CButton m_CP_Radio1;
	CButton m_CP_Radio2;

	CButton m_Type_Radio1;
	CButton m_Type_Radio2;
	CButton m_Type_Radio3;
	CButton m_Type_Radio4;
	CButton m_Type_Radio5;
	
	CStatic m_Pack_Print_String;
	CWinThread *Pack_Thread;
	static UINT Pack_Func (LPVOID lParam);
	CProgressCtrl m_Pack_Progress;
};
