// Change_FDName_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Change_FDName_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Functions.h"


// Change_FDName_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Change_FDName_Dialog, CDialogEx)

Change_FDName_Dialog::Change_FDName_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Change_FDName_Dialog::IDD, pParent)
{

}

Change_FDName_Dialog::~Change_FDName_Dialog()
{
}

void Change_FDName_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CFD_EDIT1, m_Input_Name);
	DDX_Control(pDX, IDC_CFD_EDIT2, m_Output_Name);
}


BEGIN_MESSAGE_MAP(Change_FDName_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Change_FDName_Dialog �޽��� ó�����Դϴ�.


BOOL Change_FDName_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText (IDC_CFD_EDIT1, Input_WStr);
	SetDlgItemText (IDC_CFD_EDIT2, Output_WStr);
	//*�ؽ�Ʈ ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Change_FDName_Dialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	GetDlgItemText (IDC_CFD_EDIT2, Output_WStr);

	if (Output_WStr.GetLength() == 0) {
		AfxMessageBox (_T("��ȯ�� �������� �Է��ϼ���")); return;
	}

	CString sf; char *tmp; wchar_t *tmp_u;
	if (code == KOR_CODE) {
		tmp = U2K (Output_WStr.GetBuffer()); tmp_u = K2U (tmp); sf = tmp_u;
	}
	else if (code == JAP_CODE) {
		tmp = U2J (Output_WStr.GetBuffer()); tmp_u = J2U (tmp); sf = tmp_u;
	}
	free (tmp); free (tmp_u);

	if (Output_WStr != sf) {
		AfxMessageBox (_T("���ڿ��� ����ڵ�� ȣȯ���� �ʽ��ϴ�")); return;
	}
	
	for (unsigned int i = 0;i < (unsigned int)Output_WStr.GetLength();i++) {
		if (IS_NOFILE_CHAR(Output_WStr[i])) {
			if ((Meta_type == DIR) && ((Output_WStr[i] == '/') || (Output_WStr[i] == '\\'))) { continue; }
			AfxMessageBox (_T("�ش� ���ڿ��� ����/�������� �� �� �����ϴ�.")); return;
		}
	}

	if (Meta_type == DIR && 
		((Output_WStr[Output_WStr.GetLength() - 1] != '/') && (Output_WStr[Output_WStr.GetLength() - 1] != '\\'))) {
			AfxMessageBox (_T("���� ������ ���ڴ� �����ÿ��� �մϴ�.")); return;
	}
	//*���丮�� �� �������� '/'�� '\\'�� �;� �Ѵ�

	CDialogEx::OnOK();
}


void Change_FDName_Dialog::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if (ShouldChange){
		int ret = AfxMessageBox (_T("���ڿ��� �������� �ʰڽ��ϱ�?\n����/���ϸ��� �ٲ��� ������ ����� ������ �� �� �ֽ��ϴ�."), 
			MB_YESNO);
		if (ret == IDNO) { return; }
	}

	CDialogEx::OnCancel();
}
