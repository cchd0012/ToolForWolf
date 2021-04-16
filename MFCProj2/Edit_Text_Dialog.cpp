// Edit_Text_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Edit_Text_Dialog.h"
#include "afxdialogex.h"


// Edit_Text_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Edit_Text_Dialog, CDialogEx)

Edit_Text_Dialog::Edit_Text_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Edit_Text_Dialog::IDD, pParent)
{

}

Edit_Text_Dialog::~Edit_Text_Dialog()
{
}

void Edit_Text_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ET_COMBO_BFR, m_Input_ComboBox);
	DDX_Control(pDX, IDC_ET_COMBO_AFT, m_Output_ComboBox);
	DDX_Control(pDX, IDC_ET_EDIT_BFR, m_Input_Text);
	DDX_Control(pDX, IDC_ET_EDIT_AFT, m_Output_Text);
}


BEGIN_MESSAGE_MAP(Edit_Text_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Edit_Text_Dialog �޽��� ó�����Դϴ�.


BOOL Edit_Text_Dialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) {

		if (GetFocus() == GetDlgItem(IDC_ET_EDIT_AFT)) { m_Output_Text.ReplaceSel (_T("\r\n")); }
		return true; 
		//*�ؽ�Ʈ �Է�â�� ��Ŀ���� ������ ���� ���� ������ �־��ش�

	}
	//*enter�� ������ ������ �ʰ� �״�� ������ �� �ְ� �Ѵ�

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Edit_Text_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_Input_ComboBox.AddString(_T("Japanese"));
	m_Input_ComboBox.AddString(_T("Korean"));
	m_Output_ComboBox.AddString(_T("Japanese"));
	m_Output_ComboBox.AddString(_T("Korean"));
	SetDlgItemText (IDC_ET_EDIT_BFR, m_Input_tmp);
	SetDlgItemText (IDC_ET_EDIT_AFT, m_Output_tmp);
	//*�ؽ�Ʈ �߰� �� ����

	m_Input_ComboBox.SetCurSel (m_Input_CP);
	m_Output_ComboBox.SetCurSel (m_Output_CP);
	GetDlgItem(IDC_ET_COMBO_BFR)->EnableWindow(FALSE);
	//*m_Input_ComboBox�� �����϶�� ���� �� �ƴϴ� ������ �ϰ� Disable��Ų��

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Edit_Text_Dialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	m_Output_CP = m_Output_ComboBox.GetCurSel();
	GetDlgItemText (IDC_ET_EDIT_AFT, m_Output_tmp);

	if ((m_Output_CP != CB_ERR) && (m_Output_tmp.Compare(_T("")) == 0)) {
		AfxMessageBox (_T("�ؽ�Ʈ�� �ڵ��������� �Է����ּ���.")); return;
	}
	//***����� �ؽ�Ʈ�� Ȯ���ϰ� �� ���ڿ��� �ƴϸ� ok�Ѵ�

	CDialogEx::OnOK();
}