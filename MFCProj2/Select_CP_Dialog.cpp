// Select_CP_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Select_CP_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Functions.h"

extern TCHAR *CodePageMenu[];

// Select_CP_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Select_CP_Dialog, CDialogEx)

Select_CP_Dialog::Select_CP_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Select_CP_Dialog::IDD, pParent)
{

}

Select_CP_Dialog::~Select_CP_Dialog()
{
}

void Select_CP_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCP_RADIO1, m_CP_Radio1);
	DDX_Control(pDX, IDC_SCP_RADIO2, m_CP_Radio2);
}


BEGIN_MESSAGE_MAP(Select_CP_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Select_CP_Dialog �޽��� ó�����Դϴ�.


BOOL Select_CP_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	SetDlgItemText (IDC_SCP_RADIO1, CodePageMenu[KOR_CODE]);
	SetDlgItemText (IDC_SCP_RADIO2, CodePageMenu[JAP_CODE]);
	//*�ؽ�Ʈ ����

	m_CP_Radio1.SetCheck(BST_UNCHECKED);
	m_CP_Radio2.SetCheck(BST_CHECKED);
	//*���� ����

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Select_CP_Dialog::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialogEx::OnCancel();
	//*ĵ�� ������ �����Ѵ�
}


void Select_CP_Dialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	
	if (m_CP_Radio1.GetCheck() == BST_CHECKED) { CP_Code = KOR_CODE; }
	else if (m_CP_Radio2.GetCheck() == BST_CHECKED) { CP_Code = JAP_CODE; }
	//*������ �����ϰ� ������ �ȴ�

	CDialogEx::OnOK();
}
