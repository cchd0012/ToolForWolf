#include "stdafx.h"

#include "Wolf_List.h"
#include "Wolf_Functions.h"

extern CString Wolf_File_ListName;
extern CString Wolf_File_List;

unsigned int RootDir_StartPos;
//*��¥ ���ڿ��� ���۵Ǵ� ��

void FilePathCollect (wchar_t *DirPath, int Pos)
{
	HANDLE hFind;						//���丮 �� ��� ������ ã�� �� ���� �ڵ�
	WIN32_FIND_DATAW FData;				//ã�Ƴ� ������ ������ ���� ����
	wchar_t ListPath[MAX_PATH];			//�� ����Ʈ ������ ���
	wchar_t PathChecker[MAX_PATH];		//���� ���� �̸��� �� ���� �̸�
	wchar_t PathChecker_fc[MAX_PATH];	//Pathchecker�� ���ϸ�� �����ϱ� ���� ����
	wchar_t *EachFDName;				//�� ����/���丮 ���� �̸�
	wchar_t **FDStrings;				//����/���丮 ���ڿ� �迭
	bool *DirFlags;						//���ڿ��� ���丮���� ǥ�����ִ� �÷��� �迭

	unsigned int NumOfFD;				//���� ���丮�� ������ ����/���丮 �� ��

	wchar_t EachListLine[MAX_STRLEN];	//����Ʈ�� �� ����
	unsigned int FirstSlashPoint;		//���ڿ��� ù° ������ ��ġ�� ����Ѵ�
				
	memset (ListPath, 0, sizeof(wchar_t) * MAX_PATH);
	swprintf_s (ListPath, MAX_PATH, L"%s.txt", DirPath);

	if (Pos == 0) {
		RootDir_StartPos = 0;
		for (int i = (int)wcslen(DirPath) - 1;i >= 0;i--) {
			if ((DirPath[i] == '\\') || (DirPath[i] == '/')) { RootDir_StartPos = i+1; break; }
		}
		//*���⼭ �̸� ����س��´�

		Wolf_File_ListName = &DirPath[RootDir_StartPos];
		Wolf_File_ListName += _T(".txt");
		RootDir_StartPos = 0;
		//*����Ʈ�� ����

		memset (EachListLine, 0, sizeof(wchar_t) * MAX_PATH);
		swprintf_s (EachListLine, MAX_PATH, L"%s\r\n", &DirPath[RootDir_StartPos]);
		Wolf_File_List += EachListLine;
		//*����Ʈ ���
	}
	else {
		memset (EachListLine, 0, sizeof(wchar_t) * MAX_PATH);

		for (FirstSlashPoint = 0;FirstSlashPoint < wcslen(DirPath);FirstSlashPoint++) {
			if ((DirPath[FirstSlashPoint] == '\\') || (DirPath[FirstSlashPoint] == '/')) { break; }
		}
		//*ù° ������ ���� ���� �ʱ�ȭ �� ���.

		swprintf_s (EachListLine, MAX_PATH, L"\r\n%s\r\n", &DirPath[RootDir_StartPos]);
		Wolf_File_List += EachListLine;
		//*����Ʈ ���
	}
	//*���� ����Ʈ ���� �ڵ��� ���ٸ�, ��� ����� ���� �ڵ� ����
	//*���� �� ó���̶�� �Ҹ��̹Ƿ� (ROOT)0D0A�� ���ۿ� ����ϰ� ����Ѵ�
	//*�׸��� ���⼭ �ݾƾ� �ϹǷ� IsRootFunction�� true�� �Ѵ�
	//*���� ����Ʈ �ڵ��� �ִٸ�, �̹� �������̶�� �Ҹ��Ƿ� DirPath�� �״�� �ھƳִ´�

	memset (PathChecker, 0, sizeof(wchar_t) * MAX_PATH);
	memcpy (PathChecker, DirPath, sizeof(wchar_t) * wcslen(DirPath));
	wcscat_s (PathChecker, (wcslen(PathChecker) + 3), L"\\*");
	//[���丮]\*�� �����ϸ� ���� ��� ������ ã�� �� �ִ�.
	//".???" ������ �ϸ� ����ǥ�� ���� ���� Ȯ���ڸ� ������ �� �ִµ�
	//*��� ������ ã�� ���� ���� ����
	
	memset (PathChecker_fc, 0, sizeof(wchar_t) * MAX_PATH);
	memcpy (PathChecker_fc, DirPath, sizeof(wchar_t) * wcslen(DirPath));
	wcscat_s (PathChecker_fc, (wcslen(PathChecker_fc) + 3), L"\\");
	//*���� ���տ����� �ϳ� �� ��������

	NumOfFD = 0;
	hFind = FindFirstFileW (PathChecker, &FData);	//*ù ���� ã�Ƶα�
	do {
		if((lstrcmp(FData.cFileName, L".") != 0) && (lstrcmp(FData.cFileName, L"..") != 0)){ NumOfFD++; } //., .. ���� ����
	} while (FindNextFileW (hFind, &FData));
	FindClose (hFind);
	//*����/���丮 �� Ȯ��

	if (NumOfFD == 0) { 
		memset (EachListLine, 0, sizeof(wchar_t) * MAX_PATH);
		swprintf_s (EachListLine, MAX_PATH, L"\r\n");
		Wolf_File_List += EachListLine;
		return; 
		//*������ ���� ���� ġ�� �׳� �����Ѵ�
	}
	else {
		FDStrings = (wchar_t**)malloc(sizeof(wchar_t*)*NumOfFD);
		DirFlags = (bool*)malloc(sizeof(bool)*NumOfFD);
		for (unsigned int i = 0;i < NumOfFD;i++) {
			FDStrings[i] = (wchar_t*)malloc(sizeof(wchar_t)*MAX_PATH);
			memset (FDStrings[i], 0, sizeof(wchar_t) * MAX_PATH);
			DirFlags[i] = false;
		}
		//*Ȯ���� ���� �������� ����/���丮 ���ڿ� �迭, ���丮 �÷��� �迭 ����(�⺻ false).
	}

	for (unsigned int i = 0;i < NumOfFD;i++) {
		if (i == 0) { hFind = FindFirstFileW (PathChecker, &FData); }		//*ù ����/���丮 ã�Ƶα�
		else { FindNextFileW (hFind, &FData); }								//*���� ����/���丮 ã�Ƶα�

		do {
			if((lstrcmp(FData.cFileName, L".") != 0) && (lstrcmp(FData.cFileName, L"..") != 0)){ break; }
		} while (FindNextFileW (hFind, &FData));
		//* '.', '..'�� �ֳ� Ȯ���ϰ�, ���� ������ ������

		EachFDName = FData.cFileName;

		if (FData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { DirFlags[i] = true; }
		//*���丮 �÷��� üũ

		for (FirstSlashPoint = (unsigned int)wcslen(PathChecker_fc);FirstSlashPoint < (unsigned int)wcslen(PathChecker_fc);FirstSlashPoint++) {
			if ((PathChecker_fc[FirstSlashPoint] == '\\') || (PathChecker_fc[FirstSlashPoint] == '/')) { break; }
		}
		//*ù° ������ ���� ���� �ʱ�ȭ �� ���.

		swprintf_s (FDStrings[i], MAX_PATH, L"%s%s", PathChecker_fc, EachFDName);
		//*����/���丮 ���ڿ� �����

		memset (EachListLine, 0, sizeof(wchar_t) * MAX_PATH);
		swprintf_s (EachListLine, MAX_PATH, L"\t%s\\%s\r\n", &DirPath[RootDir_StartPos], EachFDName);
		Wolf_File_List += EachListLine;
		//*���ڿ��� ���. ó���� ������ �Ͱ� ��ġ��, �� ���� �����ø� ã�Ƽ� �պκ��� ���ּ� ���δ�
		//*�� "\t%s\\%s\r\n", ù��°�� ù° �����ñ��� ���� �� ���� PathChecker, ��°�� �� �����̸��� �Ϻ���/�ѱۺ�ȯ

	}
	//*���� ���� ���Ե� ��� ���� �ִ� ���丮/���ϵ���� ���� ó���� ��,

	for (unsigned int i = 0;i < NumOfFD;i++) {
		if (DirFlags[i]){ FilePathCollect (FDStrings[i], (Pos+1)); }
		//*���丮�� �͵��� ��� ������ ��ͷ� �����Ѵ�
		free (FDStrings[i]);
		//*�׸��� �� �۾��� ��ģ �� �� ���ڿ��� ����ϰ� �����Ѵ�
	}
	//*���丮 ��ȸ �۾�

	free (FDStrings); free (DirFlags);

	if (Pos == 0) { 
		memset (EachListLine, 0, sizeof(wchar_t) * MAX_PATH);
		swprintf_s (EachListLine, MAX_PATH, L"\r\n");
		Wolf_File_List += EachListLine;
	}
	//*���丮 �迭, ���ڿ� �迭, ����Ʈ �ڵ�, ���丮 ���ڿ� ����. ���������� ���� �ϳ� �� �ڰ� ������
}//���� ��θ� ��� ���ο� ����Ʈ ������ ����� �Լ�