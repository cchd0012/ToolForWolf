// Replace_Text_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Replace_Text_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Functions.h"

extern TCHAR *CodePageMenu[];


// Replace_Text_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Replace_Text_Dialog, CDialogEx)

Replace_Text_Dialog::Replace_Text_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Replace_Text_Dialog::IDD, pParent)
{

}

Replace_Text_Dialog::~Replace_Text_Dialog()
{
}

void Replace_Text_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RPL_EDIT1, m_Replace_Text_Find);
	DDX_Control(pDX, IDC_RPL_EDIT2, m_Replace_Text_Change);
	DDX_Control(pDX, IDC_RP_CP_RADIO1, m_CP_Radio1);
	DDX_Control(pDX, IDC_RP_CP_RADIO2, m_CP_Radio2);
	DDX_Control(pDX, IDC_RP_CP_RADIO3, m_CP_Radio3);
	DDX_Control(pDX, IDC_CHK_REP_FN, m_RF_check);
}


BEGIN_MESSAGE_MAP(Replace_Text_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Replace_Text_Dialog �޽��� ó�����Դϴ�.


BOOL Replace_Text_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	SetDlgItemText (IDC_RP_CP_RADIO1, _T("None"));
	SetDlgItemText (IDC_RP_CP_RADIO2, CodePageMenu[KOR_CODE]);
	SetDlgItemText (IDC_RP_CP_RADIO3, CodePageMenu[JAP_CODE]);
	//*�ؽ�Ʈ ����
	
	m_CP_Radio1.SetCheck(BST_CHECKED);
	m_CP_Radio2.SetCheck(BST_UNCHECKED);
	m_CP_Radio3.SetCheck(BST_UNCHECKED);
	m_RF_check.SetCheck(BST_UNCHECKED);
	//*���� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Replace_Text_Dialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	GetDlgItemText (IDC_RPL_EDIT1, strtofind);
	GetDlgItemText (IDC_RPL_EDIT2, strtochg);

	if (strtochg.Compare(_T("__ENTER__")) == 0) { strtochg = _T("\n"); }
	//*�������� �ٲ� ���� �߰�

	if (m_CP_Radio1.GetCheck() == BST_CHECKED) { code = NON_CODE; }
	else if (m_CP_Radio2.GetCheck() == BST_CHECKED) { code = KOR_CODE; }
	else if (m_CP_Radio3.GetCheck() == BST_CHECKED) { code = JAP_CODE; }

	if (m_RF_check.GetCheck() == BST_CHECKED) { FCH = true; } else { FCH = false; }

	if (strtofind.GetLength() == 0) {
		AfxMessageBox (_T("ã�� ���ڿ��� ���̰� 1 �̻��̾�� �մϴ�."));
		return;
	}

	CDialogEx::OnOK();
}