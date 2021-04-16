#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Save_Text_Dialog ��ȭ �����Դϴ�.

class Save_Text_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Save_Text_Dialog)

public:
	Save_Text_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Save_Text_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SAVE_TEXT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CString m_FP_tmp;
	
	CWinThread *Save_Text_Thread;
	static UINT Save_Text_Func (LPVOID lParam);
	CProgressCtrl m_Save_Text_Progress;
};
