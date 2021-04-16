// Pack_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Pack_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Pack.h"
//*������ �� �ֵ��� ���� ���� �Լ��� ����� ���� ����

extern CString Wolf_File_ListName;
extern CString Wolf_File_List;
extern TCHAR *CodePageMenu[];

CString Wolf_File_List_Backup;
//*�����

// Pack_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Pack_Dialog, CDialogEx)

Pack_Dialog::Pack_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Pack_Dialog::IDD, pParent)
	, Pack_Thread(NULL)
{

}

Pack_Dialog::~Pack_Dialog()
{
}

void Pack_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_P_EDIT1, m_Wolf_Package_Name);
	DDX_Control(pDX, IDC_P_TEXT, m_Pack_Print_String);
	DDX_Control(pDX, IDC_P_PROGRESS1, m_Pack_Progress);
	DDX_Control(pDX, IDC_P_CP_RADIO1, m_CP_Radio1);
	DDX_Control(pDX, IDC_P_CP_RADIO2, m_CP_Radio2);
	DDX_Control(pDX, IDC_P_TP_RADIO1, m_Type_Radio1);
	DDX_Control(pDX, IDC_P_TP_RADIO2, m_Type_Radio2);
	DDX_Control(pDX, IDC_P_TP_RADIO3, m_Type_Radio3);
	DDX_Control(pDX, IDC_P_TP_RADIO4, m_Type_Radio4);
	DDX_Control(pDX, IDC_P_TP_RADIO5, m_Type_Radio5);
}


BEGIN_MESSAGE_MAP(Pack_Dialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &Pack_Dialog::OnBnClickedPack)
END_MESSAGE_MAP()


// Pack_Dialog �޽��� ó�����Դϴ�.


BOOL Pack_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	SetDlgItemText (IDC_P_CP_RADIO1, CodePageMenu[KOR_CODE]);
	SetDlgItemText (IDC_P_CP_RADIO2, CodePageMenu[JAP_CODE]);
	//*�ؽ�Ʈ ����

	SetDlgItemText (IDC_P_EDIT1, Wolf_File_ListName.Left(Wolf_File_ListName.GetLength() - 4) + _T(".wolf"));
	//*����Ʈ �̸� ����(����Ʈ �̸����� .txt�� ���� .wolf�� ������)

	m_CP_Radio1.SetCheck(BST_CHECKED);
	m_CP_Radio2.SetCheck(BST_UNCHECKED);

	m_Type_Radio1.SetCheck(BST_CHECKED);
	m_Type_Radio2.SetCheck(BST_UNCHECKED);
	m_Type_Radio3.SetCheck(BST_UNCHECKED);
	m_Type_Radio4.SetCheck(BST_UNCHECKED);
	m_Type_Radio5.SetCheck(BST_UNCHECKED);
	//*���� ����

	m_Pack_Progress.SetRange32 (0, 100);
	//*���α׷��� �� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Pack_Dialog::OnBnClickedPack()
{
	//***Pack***
	CString Pack_Name;

	GetDlgItemText (IDC_P_EDIT1, Pack_Name);
	if (Pack_Name.Compare(_T("")) == 0) {
		AfxMessageBox (_T("Wolf ���ϸ��� �Է��ϼ���."));
		return;
	}
	//*�Է��� ���ϸ� ����

	Wolf_File_List_Backup = Wolf_File_List;
	//*����Ʈ ���

	Pack_Thread = AfxBeginThread (Pack_Func, this);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_P_EDIT1)->EnableWindow(FALSE);
	//*���� ������ ����, Pack ��ư, �Է�â ��Ȱ��ȭ

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


UINT Pack_Dialog::Pack_Func (LPVOID lParam)
{
	Pack_Dialog* dlg;
	int DXA_Code, LANG_Code;
	//*wolf ���� ����, ������ ���

	dlg = (Pack_Dialog*)lParam;
	//*��ü ���

	CString Pack_Name;
	dlg->GetDlgItemText (IDC_P_EDIT1, Pack_Name);
	//*�ռ��� �̸� ���

	if (dlg->m_CP_Radio1.GetCheck() == BST_CHECKED) { LANG_Code = KOR_CODE; }
	else if (dlg->m_CP_Radio2.GetCheck() == BST_CHECKED) { LANG_Code = JAP_CODE; }
	//*��� �� �����Ͽ� ������ ���ΰ�.
	
	if (dlg->m_Type_Radio1.GetCheck() == BST_CHECKED) { DXA_Code = DXA_LOW; }
	else if (dlg->m_Type_Radio2.GetCheck() == BST_CHECKED) { DXA_Code = DXA_220; }
	else if (dlg->m_Type_Radio3.GetCheck() == BST_CHECKED) { DXA_Code = DXA_330; }
	else if (dlg->m_Type_Radio4.GetCheck() == BST_CHECKED) { DXA_Code = DXA_THMK; }
	else if (dlg->m_Type_Radio5.GetCheck() == BST_CHECKED) { DXA_Code = DXA_THMK_64; }
	//*��� �������� ��ŷ�� ���ΰ�.

	if ((DXA_Code == DXA_330) || DXA_Code == DXA_THMK_64) { creating_64bit (dlg, DXA_Code, LANG_Code); }
	else { creating (dlg, DXA_Code, LANG_Code); }
	//*���� & ����Ʈ ���� & ���α׷��� ����*

	CString Check;
	dlg->GetDlgItemText (IDC_P_TEXT, Check);
	if (Check.Compare(_T("Packing Finished")) == 0) {
		AfxMessageBox (Pack_Name + _T(" �ռ� �Ϸ�!"));
	}

	dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	dlg->GetDlgItem(IDC_P_EDIT1)->EnableWindow(TRUE);
	//*��ư Ȱ��ȭ

	Wolf_File_List = Wolf_File_List_Backup;
	//*����Ʈ ����

	return 0;
}


void Pack_Dialog::OnCancel()
{
	//***Cancel***

	DWORD ExitCode;

	if (Pack_Thread != NULL) {
		GetExitCodeThread (Pack_Thread->m_hThread, &ExitCode);
		if (ExitCode == STILL_ACTIVE) {
			
			Pack_Thread->SuspendThread();
			int ret = AfxMessageBox(_T("�ռ��� ���߰ڽ��ϱ�?"), MB_YESNO);
			//*���� ���� ���� �����尡 ���ư��� �ִٸ� �Ͻ�������Ų �� ����� �ų� ���´�.

			if (ret == IDYES) {
				TerminateThread (Pack_Thread->m_hThread, 0);
				CloseHandle (Pack_Thread->m_hThread);
				Wolf_File_List = Wolf_File_List_Backup;
				//*����� ���� ����� ����Ʈ�� �̿��� �����ؾ� �Ѵ�. �̸��� �������
			}
			else {
				Pack_Thread->ResumeThread();
				return;
			}
			//*����Ѵٸ� �ٽ� �簳 �� ���ư���, �׸��дٸ� �״�� �����带 �����Ų �� OnCancel�� ȣ���Ѵ�

		}
	}

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnCancel();
}