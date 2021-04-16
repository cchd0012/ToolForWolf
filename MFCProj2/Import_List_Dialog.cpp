// Import_List_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Import_List_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_List.h"

extern CString Wolf_File_ListName;
extern CString Wolf_File_List;

// Import_List_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Import_List_Dialog, CDialogEx)

Import_List_Dialog::Import_List_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Import_List_Dialog::IDD, pParent)
{

}

Import_List_Dialog::~Import_List_Dialog()
{
}

void Import_List_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Import_List_Dialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Import_List_Dialog::OnBnClicked_ImportFromDir)
	ON_BN_CLICKED(IDC_BUTTON2, &Import_List_Dialog::OnBnClicked_ImportFromFile)
END_MESSAGE_MAP()


// Import_List_Dialog �޽��� ó�����Դϴ�.


void Import_List_Dialog::OnBnClicked_ImportFromDir()
{
	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];

	ZeroMemory (&BrInfo, sizeof(BROWSEINFO));
	ZeroMemory (szBuffer, 512);

	BrInfo.hwndOwner = m_hWnd;
	BrInfo.lpszTitle = _T("Select Folder to Make List");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	BrInfo.lParam = (LPARAM)(_T("C:\\"));
	LPITEMIDLIST pItemIdList = SHBrowseForFolder (&BrInfo);
	if (SHGetPathFromIDList (pItemIdList, szBuffer) == 0) { return; }
	//*���丮 ������

	FilePathCollect (szBuffer, NULL);
	//*����Ʈ ������ �Լ� ����

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CDialogEx::OnOK();
}


void Import_List_Dialog::OnBnClicked_ImportFromFile()
{
	TCHAR szFilter[] = _T("Text File (*.txt) |*.txt|");
	FILE *stream;
	wchar_t *tpBuff; unsigned int fsize;

	CFileDialog dlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

	if (dlg.DoModal() == IDOK) { Wolf_File_ListName = dlg.GetFileName(); }
	else { return; }
	//*�ؽ�Ʈ ���� ����, �� �� �̸��� ���� ����Ʈ ���� �̸����� �����

	_wfopen_s (&stream, dlg.GetPathName().GetBuffer(), L"r+b");
	fseek (stream, 0, SEEK_END); fsize = ftell (stream);
	tpBuff = (wchar_t*)malloc (fsize); tpBuff[(fsize/sizeof(wchar_t)) - 1] = 0;
	fseek (stream, sizeof(wchar_t), SEEK_SET); fread (tpBuff, 1, fsize-sizeof(wchar_t), stream);
	Wolf_File_List = tpBuff; free (tpBuff); fclose (stream);
	//*�״�� ����Ʈ ���� ������ �����ϱ�

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CDialogEx::OnOK();
}
