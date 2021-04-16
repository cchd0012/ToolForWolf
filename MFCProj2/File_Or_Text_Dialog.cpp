// File_Or_Text_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "File_Or_Text_Dialog.h"
#include "afxdialogex.h"


// File_Or_Text_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(File_Or_Text_Dialog, CDialogEx)

File_Or_Text_Dialog::File_Or_Text_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(File_Or_Text_Dialog::IDD, pParent)
{
	m_FN_Radio.SetCheck (BST_CHECKED);
	m_TE_Radio.SetCheck (BST_UNCHECKED);
	Type = 0;
	//*��ư �ʱ�ȭ. �Ϲ������δ� ���� �̸��� �� ���Ƿ� ���Ϸ� ����
}

File_Or_Text_Dialog::~File_Or_Text_Dialog()
{
}

void File_Or_Text_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FN_RADIO, m_FN_Radio);
	DDX_Control(pDX, IDC_TE_RADIO, m_TE_Radio);
}


BEGIN_MESSAGE_MAP(File_Or_Text_Dialog, CDialogEx)
END_MESSAGE_MAP()


// File_Or_Text_Dialog �޽��� ó�����Դϴ�.


void File_Or_Text_Dialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if (m_FN_Radio.GetCheck() == BST_CHECKED) { Type = 1; }
	else if (m_TE_Radio.GetCheck() == BST_CHECKED) { Type = 2; }
	//*���õ� ��ư�� ���� Type�� ����

	CDialogEx::OnOK();
}