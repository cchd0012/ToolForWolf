#pragma once
#include "afxwin.h"
#include "afxcmn.h"

void All_List_Item_Delete();
void All_Text_Data_Reset();
//*����Ʈ �����͸� �����ִ� �Լ�, DIR_TXT_IDX ����ü �ʱ�ȭ �Լ�

void Get_Fidx_Tidx (unsigned int Idx, unsigned int *fidx, unsigned int *tidx);
//*���� ���ڰ�(�ؽ�Ʈ ����Ʈ �ε���)�� �������� ���� �ε����� �ؽ�Ʈ �ε����� ��������ִ� �Լ�
//*��� tidx�� 1 ���� �Ѵ�

int Get_List_Index (unsigned int fidx, unsigned int tidx);
//*���� ���ڰ�(���� �ε���, �ؽ�Ʈ �ε���)�� �������� ����Ʈ �ε����� ��ȯ�ϴ� �Լ�
//*���ǿ� �� �´´� ������ -1 ��ȯ


// Load_Text_Dialog ��ȭ �����Դϴ�.

class Load_Text_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(Load_Text_Dialog)

public:
	Load_Text_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Load_Text_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOAD_TEXT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClicked_SelectFolder();

	CEdit m_FolderPath;
	CString m_FP_tmp;
	
	CWinThread *Load_Text_Thread;
	static UINT Load_Text_Func (LPVOID lParam);
	CProgressCtrl m_Load_Text_Progress;
	CStatic m_Load_Text_Print;
};