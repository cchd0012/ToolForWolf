#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Edit_Text_Dialog ��ȭ �����Դϴ�.

class Edit_Text_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Edit_Text_Dialog)

public:
	Edit_Text_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Edit_Text_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_TEXT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:

	unsigned int m_Input_CP, m_Output_CP;
	CString m_Input_tmp, m_Output_tmp;
	//*�ʱ�ȭ�� ���� ���� �޾ƿ� �� ����

	CComboBox m_Input_ComboBox;
	CComboBox m_Output_ComboBox;
	//*����ڵ� �����ϴ� �޺��ڽ�

	CEdit m_Input_Text;
	CEdit m_Output_Text;
	//*���� �Է¹��� �ؽ�Ʈ, ������ �ؽ�Ʈ(������ �ؽ�Ʈ�� ��� �� �ȴ�)
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
