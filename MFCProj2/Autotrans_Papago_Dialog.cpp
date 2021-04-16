// Autotrans_Papago_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Autotrans_Papago_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Auto_Papago_Translate.h"
// Autotrans_Papago_Dialog ��ȭ �����Դϴ�.

extern DIR_TXT_IDX m_Text_Idx_In_Dir;

IMPLEMENT_DYNAMIC(Autotrans_Papago_Dialog, CDialogEx)

Autotrans_Papago_Dialog::Autotrans_Papago_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Autotrans_Papago_Dialog::IDD, pParent)
{

}

Autotrans_Papago_Dialog::~Autotrans_Papago_Dialog()
{
}

void Autotrans_Papago_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ORG, m_Src_Language_List);
	DDX_Control(pDX, IDC_COMBO_ORG2, m_Dst_Language_List);
	DDX_Control(pDX, IDC_TRANS_PROGRESS, m_Translate_Progress);
	DDX_Control(pDX, IDC_COMBO_TRANS_FILE, m_Text_File_List);
	DDX_Control(pDX, IDC_START_IDX, m_Start_Idx_Str);
	DDX_Control(pDX, IDC_END_IDX, m_End_Idx_Str);
}


BEGIN_MESSAGE_MAP(Autotrans_Papago_Dialog, CDialogEx)
	ON_BN_CLICKED(ID_TRANSLATE, &Autotrans_Papago_Dialog::OnBnClickedTranslate)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANS_FILE, &Autotrans_Papago_Dialog::OnCbnSelchangeComboTransFile)
END_MESSAGE_MAP()


// Autotrans_Papago_Dialog �޽��� ó�����Դϴ�.


BOOL Autotrans_Papago_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Src_Language_List.AddString (_T("en"));
	m_Src_Language_List.AddString (_T("ja"));
	m_Src_Language_List.AddString (_T("ko"));
	m_Src_Language_List.SetCurSel (1);

	m_Dst_Language_List.AddString (_T("en"));
	m_Dst_Language_List.AddString (_T("ja"));
	m_Dst_Language_List.AddString (_T("ko"));
	m_Dst_Language_List.SetCurSel (2);
	// ���� ��� ��� �߰�, �⺻������ ����Ʈ�� ������ �ϸ� ��

	progressing_file_idx = 0;
	//*(�߰��� ���� ��)���� ���̾��� ���� �ε��� ����

	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
		if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt != 0) {
			TCHAR *t = _tcsstr(m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, _T("data/"));
			if (t == NULL){ t = _tcsstr(m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, _T("data\\")); }
			if (t == NULL){ t = _tcsstr(m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, _T("Data/")); }
			if (t == NULL){ t = _tcsstr(m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, _T("Data\\")); }
			m_Text_File_List.AddString (t);
		}
	}
	CString _t;
	m_Text_File_List.SetCurSel (0);
	_t.Format(_T("%06d"), 0); 
	m_Start_Idx_Str.SetWindowTextW (_t);
	s_idx = 0;
	_t.Format(_T("%06d"), (m_Text_Idx_In_Dir.File_Idx_Arr[0].Text_Cnt-1)); 
	m_End_Idx_Str.SetWindowTextW (_t);
	e_idx = (m_Text_Idx_In_Dir.File_Idx_Arr[0].Text_Cnt-1);
	//*�ؽ�Ʈ�� ��� �ִ� ���� ����Ʈ �� ���� ����

	m_Translate_Progress.SetRange32 (0, 100);
	//*���α׷��� �� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


UINT Autotrans_Papago_Dialog::Translate_Func (LPVOID lParam)
{
	Autotrans_Papago_Dialog* dlg;

	dlg = (Autotrans_Papago_Dialog*)lParam;
	//*��ü ���

	CString srcLang, dstLang, FilePath, startIdx, endIdx;
	dlg->m_Src_Language_List.GetLBText (dlg->m_Src_Language_List.GetCurSel(), srcLang);
	dlg->m_Dst_Language_List.GetLBText (dlg->m_Dst_Language_List.GetCurSel(), dstLang);
	dlg->m_Text_File_List.GetLBText (dlg->m_Text_File_List.GetCurSel(), FilePath);
	Papago_Translate (dlg, FilePath.GetBuffer(), srcLang.GetBuffer(), dstLang.GetBuffer(), 
		dlg->s_idx, dlg->e_idx);
	//*���� ����

	CString Check;
	dlg->GetDlgItemText (IDC_TRANS_PROG_TEXT, Check);
	if (Check.Compare(_T("Translating Finished")) == 0) {
		AfxMessageBox (FilePath + _T(" Papago �ڵ����� �Ϸ�!"));
	}

	dlg->GetDlgItem(ID_TRANSLATE)->EnableWindow(TRUE);
	//*��ư Ȱ��ȭ

	return 0;
}
//���� ������


void Autotrans_Papago_Dialog::OnOK()
{
	//*����Ű�� ������ �� �� �Ű���ָ� �ɵ�?
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnOK();
}


void Autotrans_Papago_Dialog::OnCancel()
{
	//*Cancel ��ư�� ������ ��

	DWORD ExitCode;

	if (Translate_Thread != NULL) {
		GetExitCodeThread (Translate_Thread->m_hThread, &ExitCode);
		if (ExitCode == STILL_ACTIVE) {
			
			Translate_Thread->SuspendThread();
			int ret = AfxMessageBox(_T("������ ���߰ڽ��ϱ�?"), MB_YESNO);
			//*���� ���� ���� �����尡 ���ư��� �ִٸ� �Ͻ�������Ų �� ����� �ų� ���´�.

			if (ret == IDYES) {
				TerminateThread (Translate_Thread->m_hThread, 0);
				CloseHandle (Translate_Thread->m_hThread);
				//*����� ���� �׳� ������� ������ ��ŭ�� �����ϵ��� �Ѵ�
			}
			else {
				Translate_Thread->ResumeThread();
				return;
			}
			//*����Ѵٸ� �ٽ� �簳 �� ���ư���, �׸��дٸ� �״�� �����带 �����Ų �� OnCancel�� ȣ���Ѵ�

		}
	}


	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnCancel();
}


void Autotrans_Papago_Dialog::OnBnClickedTranslate()
{
	//*Translate ��ư�� ������ ��

	if (m_Src_Language_List.GetCurSel() == m_Dst_Language_List.GetCurSel()) {
		AfxMessageBox (_T("���� ���� ���� �� ������ �� �˴ϴ�."));
		return;
	}
	//*���۾�� ����� �Ȱ����� �� �ȴ�

	CString FilePath;
	m_Text_File_List.GetLBText (m_Text_File_List.GetCurSel(), FilePath);
	unsigned int File_Idx = 0;
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
		if (_tcsstr (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, FilePath) != NULL) {
			File_Idx = i; break;
		}
	}
	//*������ ������ �ε��� ���ϱ�
	
	CString startIdx, endIdx;
	m_Start_Idx_Str.GetWindowTextW (startIdx);
	for (int i = 0;i < startIdx.GetLength();i++) {
		if ((startIdx[i] < '0') || (startIdx[i] > '9')) {
			AfxMessageBox (_T("�ùٸ� ���� �ε��� ���� �ƴմϴ�."));
			return;
		}
	}
	m_End_Idx_Str.GetWindowTextW (endIdx);
	for (int i = 0;i < endIdx.GetLength();i++) {
		if ((endIdx[i] < '0') || (endIdx[i] > '9')) {
			AfxMessageBox (_T("�ùٸ� ���� �ε��� ���� �ƴմϴ�."));
			return;
		}
	}
	//*����/���� �ε����� ���� �ùٸ��� Ȯ��

	s_idx = _ttoi(startIdx);
	e_idx = _ttoi(endIdx);
	if (e_idx < s_idx) {
		AfxMessageBox (_T("���� �ε����� ���� �ε������� ���� �� �����ϴ�."));
		return;
	}
	//*����/���� �ε����� ���� Ȯ��

	if (s_idx < 0) { 
		AfxMessageBox (_T("���� �ε��� ���� 0���� �۽��ϴ�.\n0���� �������ϴ�."));
	}
	if (e_idx >= m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Cnt) { 
		AfxMessageBox (_T("���� �ε��� ���� ���� ������ ����ϴ�.\n�ִ�ġ�� �������ϴ�."));
		e_idx = (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Cnt - 1);
	}
	//*����/���� �ε����� �ؽ�Ʈ ������ ����� �˾Ƽ� ���߱�

	Translate_Thread = AfxBeginThread (Translate_Func, this);
	GetDlgItem(ID_TRANSLATE)->EnableWindow(FALSE);
	//*���� ������ ����, Translate ��ư ��Ȱ��ȭ

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void Autotrans_Papago_Dialog::OnCbnSelchangeComboTransFile()
{
	CString FilePath;
	m_Text_File_List.GetLBText (m_Text_File_List.GetCurSel(), FilePath);
	unsigned int File_Idx = 0;
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
		if (_tcsstr (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, FilePath) != NULL) {
			File_Idx = i; break;
		}
	}
	//*������ ������ �ε��� ���ϱ�
	
	CString _t;
	_t.Format(_T("%06d"), 0); 
	m_Start_Idx_Str.SetWindowTextW (_t);
	s_idx = 0;
	_t.Format(_T("%06d"), (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Cnt-1)); 
	m_End_Idx_Str.SetWindowTextW (_t);
	e_idx = (m_Text_Idx_In_Dir.File_Idx_Arr[0].Text_Cnt-1);
	//*�ؽ�Ʈ�� ��� �ִ� ���� ����Ʈ �� ���� ����

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
