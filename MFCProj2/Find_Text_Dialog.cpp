// Find_Text_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Find_Text_Dialog.h"
#include "afxdialogex.h"


// Find_Text_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Find_Text_Dialog, CDialogEx)

Find_Text_Dialog::Find_Text_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Find_Text_Dialog::IDD, pParent)
{

}

Find_Text_Dialog::~Find_Text_Dialog()
{
}

void Find_Text_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FT_EDIT1, m_Find_String);
	DDX_Control(pDX, IDC_FT_RADIO1, m_FT_Up_Radio);
	DDX_Control(pDX, IDC_FT_RADIO2, m_FT_Down_Radio);
}


BEGIN_MESSAGE_MAP(Find_Text_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Find_Text_Dialog �޽��� ó�����Դϴ�.


BOOL Find_Text_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_FT_Up_Radio.SetCheck (BST_UNCHECKED);
	m_FT_Down_Radio.SetCheck (BST_CHECKED);
	//*���� ����

	Direction = DOWN_DIRECTION;
	//*���� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Find_Text_Dialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	GetDlgItemText (IDC_FT_EDIT1, Find_Str);
	if (Find_Str.GetLength() == 0) {
		AfxMessageBox (_T("Ž���� �ؽ�Ʈ�� ���̰� 1 �̻��̾�� �մϴ�."));
		return;
	}
	//*ã�� ���ڿ��� �� ���ڿ��̸� �ȵȴ�

	if (m_FT_Up_Radio.GetCheck() == BST_CHECKED) { Direction = UP_DIRECTION; }
	else if (m_FT_Down_Radio.GetCheck() == BST_CHECKED) { Direction = DOWN_DIRECTION; }
	else { Direction = NON_DIRECTION; }
	//*���� ����

	CDialogEx::OnOK();
}