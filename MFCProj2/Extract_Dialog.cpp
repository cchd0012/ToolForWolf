// Extract_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Extract_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Extract.h"
//*������ �� �ֵ��� ���� ���� �Լ��� ����� ���� ����

extern CString Wolf_File_ListName;
extern CString Wolf_File_List;
extern TCHAR *CodePageMenu[];

// Extract_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Extract_Dialog, CDialogEx)


Extract_Dialog::Extract_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Extract_Dialog::IDD, pParent)
	, Wolf_Full_Path(_T(""))
	, Wolf_File_Name(_T(""))
	, Wolf_File_Title(_T(""))
	, Extract_Thread(NULL)
{

}

Extract_Dialog::~Extract_Dialog()
{
}

void Extract_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_EDIT1, Wolf_Full_Path);
	DDX_Control(pDX, IDC_E_TEXT, m_Extract_Print_String);
	DDX_Control(pDX, IDC_E_CP_RADIO1, m_CP_Radio1);
	DDX_Control(pDX, IDC_E_CP_RADIO2, m_CP_Radio2);
	DDX_Control(pDX, IDC_E_PROGRESS1, m_Extract_Progress);
}


BEGIN_MESSAGE_MAP(Extract_Dialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Extract_Dialog::OnBnClickedSelectButton)
	ON_BN_CLICKED(IDOK, &Extract_Dialog::OnBnClickedExtract)
END_MESSAGE_MAP()


// Extract_Dialog �޽��� ó�����Դϴ�.


void Extract_Dialog::OnBnClickedSelectButton()
{
	TCHAR szFilter[] = _T("Wolf Package File (*.wolf) |*.wolf|DXA Files(*.dxa) |*.dxa||");
	CFileDialog dlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

	if (dlg.DoModal() == IDOK) {
		UpdateData(TRUE);
		Wolf_Full_Path = dlg.GetPathName();
		Wolf_File_Name = dlg.GetFileName();
		Wolf_File_Title = dlg.GetFileTitle();
		UpdateData(FALSE);
	}
	//* wolf_Full_path ����

	//***GetFileName : Ȯ���ڸ� ������ ������ �̸�
	//***GetFileTitle : Ȯ���ڸ� ������ ������ �̸�
	//***GetFolderPath : ������ ������ ���� ���
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void Extract_Dialog::OnBnClickedExtract()
{
	if (Wolf_Full_Path.Compare(_T("")) == 0) {
		AfxMessageBox (_T("Wolf ������ �����ϼ���."));
		return;
	}
	//*���ϸ� üũ, ���ϸ��� ������ �������� �ʰ� �׳� ���ư�

	int DXA_Code = GetWolfFileVersion (Wolf_Full_Path);
	//*wolf ���� ���� Ȯ��

	if (DXA_Code == -2) {
		AfxMessageBox (_T("�ش� Wolf ������ �����ϴ�."));
		return;
	}
	else if (DXA_Code == -1) {
		AfxMessageBox (_T("�ùٸ� Wolf ������ �ƴմϴ�."));
		return;
	}
	//*���� ���� üũ, ������ ���ų� ���� ������ �ùٸ��� �ʴٸ� ���ư�

	Extract_Thread = AfxBeginThread (Extract_Func, this);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	//*���� ������ ����, Extract ��ư ��Ȱ��ȭ

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


UINT Extract_Dialog::Extract_Func (LPVOID lParam)
{
	Extract_Dialog* dlg;
	int DXA_Code, LANG_Code;
	//*wolf ���� ����, ������ ���

	Wolf_File_ListName = _T("");
	Wolf_File_List.Empty();
	//*����Ʈ �ʱ�ȭ

	dlg = (Extract_Dialog*)lParam;
	//*��ü ���

	DXA_Code = GetWolfFileVersion (dlg->Wolf_Full_Path);
	//*wolf ���� ���� ��Ȯ��. ��Ȯ���� ���ؼ�.

	if (dlg->m_CP_Radio1.GetCheck() == BST_CHECKED) { LANG_Code = KOR_CODE; }
	else if (dlg->m_CP_Radio2.GetCheck() == BST_CHECKED) { LANG_Code = JAP_CODE; }
	//*��� �� �����Ͽ� ������ ���ΰ�.

	if ((DXA_Code == DXA_330) || (DXA_Code == DXA_THMK_64)) { extracting_64bit (dlg, DXA_Code, LANG_Code); }
	else { extracting (dlg, DXA_Code, LANG_Code); }
	//*���� & ����Ʈ ���� & ���α׷��� ����*

	CString Check;
	dlg->GetDlgItemText (IDC_E_TEXT, Check);
	if (Check.Compare(_T("Extract Finished")) == 0) {
		CString tt = dlg->Wolf_File_Name + _T(" ���� �Ϸ�! (");
		if (DXA_Code == DXA_LOW) { tt += _T("type 1)"); }
		else if (DXA_Code == DXA_220) { tt += _T("type 2)"); }
		else if (DXA_Code == DXA_330) { tt += _T("type 3)"); }
		else if (DXA_Code == DXA_THMK) { tt += _T("type th_l_2)"); }
		else if (DXA_Code == DXA_THMK_64) { tt += _T("type th_l_2_64bit)"); }
		else { tt += _T("type Unknown)"); }
		AfxMessageBox (tt);
	}

	dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	//*��ư Ȱ��ȭ

	return 0;
}


BOOL Extract_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText (IDC_E_TEXT, _T("Extracting : "));
	SetDlgItemText (IDC_E_CP_RADIO1, CodePageMenu[KOR_CODE]);
	SetDlgItemText (IDC_E_CP_RADIO2, CodePageMenu[JAP_CODE]);
	//*�ؽ�Ʈ ����

	m_CP_Radio1.SetCheck(BST_UNCHECKED);
	m_CP_Radio2.SetCheck(BST_CHECKED);
	//*���� ����

	m_Extract_Progress.SetRange32 (0, 100);
	//*���α׷��� �� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Extract_Dialog::OnCancel()
{
	DWORD ExitCode;

	if (Extract_Thread != NULL) {
		GetExitCodeThread (Extract_Thread->m_hThread, &ExitCode);
		if (ExitCode == STILL_ACTIVE) {
			
			Extract_Thread->SuspendThread();
			int ret = AfxMessageBox(_T("���ظ� ���߰ڽ��ϱ�?"), MB_YESNO);
			//*���� ���� ���� �����尡 ���ư��� �ִٸ� �Ͻ�������Ų �� ����� �ų� ���´�.

			if (ret == IDYES) {
				TerminateThread (Extract_Thread->m_hThread, 0);
				CloseHandle (Extract_Thread->m_hThread);
				Wolf_File_ListName = _T("");
				Wolf_File_List.Empty();
				//*����� ���� ����Ʈ�� �ʱ�ȭ�ؾ� �Ѵ�
			}
			else {
				Extract_Thread->ResumeThread();
				return;
			}
			//*����Ѵٸ� �ٽ� �簳 �� ���ư���, �׸��дٸ� �״�� �����带 �����Ų �� OnCancel�� ȣ���Ѵ�

		}
	}

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnCancel();
}