#include "stdafx.h"

#include "Wolf_Pack.h"


extern CString Wolf_File_List;


int Set_FileOrDirName_Data (unsigned char *FileDirnames_Raw, unsigned int *FileDirnames_Pnt_Addr,
	unsigned int Dir_Idx, FOD_IN_DIR *Dir_Str_MetaCnt_Arr, unsigned int Dir_Idx_Size,
	FILE_METADATA *Metadata_Collect, unsigned int code, wchar_t ***Dir_File_Wstr_Pnt);
//*���� [���ϸ�+üũ��] ������ ���� �Լ�


int Set_Big_FileOrDirName_Data (unsigned char *FileDirnames_Raw, unsigned int *FileDirnames_Pnt_Addr,
	unsigned int Dir_Idx, FOD_IN_DIR *Dir_Str_MetaCnt_Arr, unsigned int Dir_Idx_Size,
	FILE_BIG_METADATA *Metadata_Collect, unsigned int code, wchar_t ***Dir_File_Wstr_Pnt);
//*���� [���ϸ�+üũ��] ������ ���� �Լ�(64bit wolf ���� ����)





void creating (Pack_Dialog* PakDlg, unsigned int type, unsigned int code)	
{
	char *List_Buff, *List_Buff_Pnt;			//����Ʈ ������ ������ ���� �� ������
	unsigned int List_Size;						//����Ʈ�� ũ��

	WOLF_HEADER Dxa_Head;						//wolf ������ ���

	unsigned char *Files_Buff;					//���ϵ��� �ѵ� ��Ƴ��� ����
	unsigned char *Each_File_Buff;				//�� ������ ����
	unsigned int FilesBuff_Size;				//���� ������ ũ��
	unsigned int Each_File_Size;				//�� ������ ũ��
	unsigned int Offset_Acumm;					//������ ����ġ�� ����� �ݿ��ϱ� ���� ����
	
	unsigned char *FileDirnames_Raw;			//[�����̸�+üũ��] �迭�� ��ٷ� ���� �Ұ��ϹǷ� �����̸� ������ ��ȯ�Ѵ�.
	unsigned int FileDirnames_Raw_Size;			//���� [�����̸�+üũ��]�� ũ��

	FILE_METADATA *Metadata_Collect;			//������ ��Ÿ�����͸� ������ ��
	unsigned int Metadata_Collect_Size;			//�����̸� ��ū���� ������ �� �� ����

	DIR_IDX *Dir_Idx_Collect;					//���丮�� ������ ������ ��
	unsigned int Dir_Idx_Collect_Size;			//���丮 ������ ������ �� �� ����

	FOD_IN_DIR *Dir_Str_MetaCnt_Arr;			//���丮�� ������ ��Ÿ������ �� �� ���丮 �̸� �����͸� �����ϴ� �迭
	unsigned int Dir_Idx_Tmp;					//�ӽÿ����� ���丮 �ε����� ������ ����
	unsigned int FoD_Idx_Tmp;					//�ӽÿ����� ���丮�� ������ [����/���丮] �ε����� ������ ����

	unsigned char *Total_Meta_Buff;				//�� 3���� �ѵ� ��� ������ ��
	unsigned int Total_Meta_Size;				//�� 3���� ������ ũ��

	char Parent_Dir [MAX_PATH];					//�θ� ���丮�� ��θ� ������ ����
	char *Each_Str;								//�� ����/���丮 ���ڿ� ������
	unsigned int FileOrDir_Length;				//����/���丮�� �̸� ����
	unsigned int Each_Str_Len;					//�� ����/���丮 ���ڿ��� ����

	HANDLE hEachFile;							//�� ������ ����� �� ���̴� �ڵ�
	HANDLE hDxa;								//�۾��� ��ģ �� �� wolf ������ ����� �� ���̴� �ڵ�


	CString tmpCstr;							//������ �����ϰ� ����� �� �ִ�
	wchar_t *List_Buff_Backup;					//*��θ� ���� �� ���� �������
	unsigned int Uni_List_Size;					//*�� ��������� ũ��
	wchar_t ***Dir_File_Wstr_Pnt;				//*��θ� �����ϴ� ������
		

	DWORD lp_read, lp_write;


	TCHAR dxaname[MAX_PATH];
	memset (dxaname, 0, sizeof(TCHAR) * MAX_PATH);
	PakDlg->GetDlgItemText (IDC_P_EDIT1, dxaname, MAX_PATH);
	//*���� �̸� ������

	if (code == JAP_CODE) { List_Buff = U2J (Wolf_File_List.GetBuffer()); }
	else if (code == KOR_CODE) { List_Buff = U2K (Wolf_File_List.GetBuffer()); }
	List_Size = (unsigned int)strlen(List_Buff);

	List_Buff_Backup = Wolf_File_List.GetBuffer();
	Uni_List_Size = (unsigned int)wcslen(List_Buff_Backup);
	//*List_Buff ������ ���ڿ��� �ܾ�� ����� �� ���� ������,
	//*List_Buff_Backup�� ���丮/������ �� �� ���� ������
	//*�׷��Ƿ� List_Buff�� ������ ����ڵ忡 ���� �ٲ۴�.
	//***0. ����Ʈ �о����


	
	Dir_Idx_Collect_Size = 0;
	for (unsigned int i = 0;i < List_Size-0x3;i++) {
		if (*(unsigned int*)(&List_Buff[i]) == 0x0A0D0A0D) { Dir_Idx_Collect_Size++; }
	}
	//*��Ʈ ���丮�� ������ �� ���丮 �� �߻�. ������ 2�� ���޾� �ִ� Ƚ���� �߻��ϸ� �ȴ�

	Dir_Str_MetaCnt_Arr = (FOD_IN_DIR*)malloc(sizeof(FOD_IN_DIR)*Dir_Idx_Collect_Size);
	memset (Dir_Str_MetaCnt_Arr, 0, sizeof(FOD_IN_DIR)*Dir_Idx_Collect_Size);
	//*���丮�� ���� ��Ÿ������ ���� �߻��� �����ϴ� �迭 ���� �� �ʱ�ȭ

	Dir_Str_MetaCnt_Arr[0].Dir_Str = List_Buff;
	//*��ħ ���� ó���� �ִ� �� ��Ʈ ���丮�Ƿ� �̷��� �ϸ� �ȴ�

	Metadata_Collect_Size = 1;
	Dir_Idx_Tmp = 0;
	for (unsigned int i = 0;i < List_Size-4;i++) {
		if (List_Buff[i] == 0x9) { Metadata_Collect_Size++; Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt++; }
		//*���� ������ ��. ��Ÿ������ �迭�� �Ӹ� �ƴ϶� �� ���丮 ���� ��Ÿ������ ���� �÷���

		else if ((List_Buff[i] == 0xD) && (List_Buff[i+1] == 0xA)) { 
			if ((List_Buff[i+2] == 0xD) && (List_Buff[i+3] == 0xA)) { 
				Dir_Idx_Tmp++; 
				Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Str = &List_Buff[i+4];
			}
			//*������ ���� 2���� �� -> ���� ���丮�� �Ѿ�� ����. �׷��Ƿ� �� ���������� ���ڿ��� ���� �Ѵ�
		}
		//*������ ������ �� �� �κ��� �ƿ� 0���� ����� ������ ������, ���� ���� ������ �ٷ� �׷����� �ʴ´�
	}
	//*��Ʈ ���丮�� ������ �� ��Ÿ������ �� �߻�. ���� ���� �������� �߻��ϸ� ������ ��Ʈ ���丮�� ���� �ȵǹǷ� 1�� �� ���Ѵ�
	//*�߰��� ���� �κ��� 0���� ���������� �����ϰ� ����/���丮�� ���� ������ ���ڿ��� �����, �ش� ���丮�� ���� ��Ÿ�����͸� �÷��� �Ѵ�

	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {
		Dir_Str_MetaCnt_Arr[i].FileOrDir_In_Dir_Arr = 
			(char**)malloc(sizeof(char*) * Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt);
		memset (Dir_Str_MetaCnt_Arr[i].FileOrDir_In_Dir_Arr, 0, sizeof(char*) * Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt);
	}
	//*���ڿ� ������ �迭 �Ҵ� �� �ʱ�ȭ
	


	Dir_File_Wstr_Pnt = (wchar_t***)malloc(sizeof(wchar_t**) * Dir_Idx_Collect_Size);
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {
		Dir_File_Wstr_Pnt[i] = (wchar_t**)malloc (sizeof(wchar_t*) * (Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt + 1));
	}
	Dir_File_Wstr_Pnt[0][0] = List_Buff_Backup;
	Dir_Idx_Tmp = 0;
	for (unsigned int i = 0;i < Uni_List_Size-4;i++) { 
		if ((List_Buff_Backup[i] == 0xD) && (List_Buff_Backup[i+1] == 0xA) 
			&& (List_Buff_Backup[i+2] == 0xD) && (List_Buff_Backup[i+3] == 0xA)) { 
				Dir_Idx_Tmp++;
				Dir_File_Wstr_Pnt[Dir_Idx_Tmp][0] = &List_Buff_Backup[i+4];
		} 
	}
	//***��ο� �����ϱ� ���� �����͵� �߰� �Ҵ�, +1�� �ش� ����� �����ϴ� ���丮�� ǥ���ؾ� �ϱ� ����



	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	for (unsigned int i = 0;i < List_Size-1;i++) {
		if (List_Buff[i] == 0x9) { 
			Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp] = &List_Buff[i+1];
			FoD_Idx_Tmp++;
		}
		//*���� ������ ��. �̹����� ������ ���ڿ����� ����� ��Ī�����ش�(���� ���� �ѱ��)

		else if ((List_Buff[i] == 0xD) && (List_Buff[i+1] == 0xA)) { 
			List_Buff[i] = List_Buff[i+1] = 0; 
			if ((List_Buff[i+2] == 0xD) && (List_Buff[i+3] == 0xA)) { 
				FoD_Idx_Tmp = 0;Dir_Idx_Tmp++; 
				if ((List_Buff[i+4] == 0xD) && (List_Buff[i+5] == 0xA)) { 
					i--; while ((List_Buff[i] != 0) && (i >= 0)) { i--; }
					if (code == JAP_CODE) { tmpCstr = J2U (&List_Buff[i+1]); }
					else if (code == KOR_CODE) { tmpCstr = K2U (&List_Buff[i+1]); }
					AfxMessageBox (tmpCstr + _T(" : ���丮�� ������ϴ�.\n����Ʈ���� �������ּ���."));
					for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
						free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
						free (Dir_File_Wstr_Pnt[jj]);
					}
					free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt); tmpCstr.Empty();
					return;
				}
			}
			//*������ ���� 2���� �� -> ���� ���丮�� �Ѿ�� ����.
			//*�׷��Ƿ� [����/���丮] �ε����� �ʱ�ȭ�ϰ� ���丮 �ε����� ���Ѵ�
		}
		//*�̹����߸��� ���� �κ��� 0���� �����
	}
	//*�� ���丮 ���� [����/���丮] �迭 ����



	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	for (unsigned int i = 0;i < Uni_List_Size-1;i++) {
		if (List_Buff_Backup[i] == 0x9) { 
			Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1] = &List_Buff_Backup[i+1];
			FoD_Idx_Tmp++;
		}
		//*���� ������ ��. �̹����� ������ ���ڿ����� ����� ��Ī�����ش�(���� ���� �ѱ��)

		else if ((List_Buff_Backup[i] == 0xD) && (List_Buff_Backup[i+1] == 0xA)) { 
			List_Buff_Backup[i] = List_Buff_Backup[i+1] = 0; 
			if ((List_Buff_Backup[i+2] == 0xD) && (List_Buff_Backup[i+3] == 0xA)) 
			{ FoD_Idx_Tmp = 0;Dir_Idx_Tmp++; }
			//*������ ���� 2���� �� -> ���� ���丮�� �Ѿ�� ����.
			//*�׷��Ƿ� [����/���丮] �ε����� �ʱ�ȭ�ϰ� ���丮 �ε����� ���Ѵ�
		}
		//*�̹����߸��� ���� �κ��� 0���� �����
	}
	//***������ ���� ��� �����Ϳ��� �߰��� ����



	FileDirnames_Raw_Size = 0;
	FilesBuff_Size = 0;
	//*[�����̸�+üũ��] �������� ũ�� �� ���� ���� ũ�� �ʱ�ȭ


	List_Buff_Pnt = List_Buff;
	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	for (unsigned int i = 0;i < Metadata_Collect_Size;i++) {

		if (i == 0) { FileDirnames_Raw_Size += (sizeof(short) * 2); }
		//*��Ʈ ���丮�� ��¡�ϴµ�, ���̰� �ƿ� �����Ƿ� 4�� ���Ѵ�

		else {

			if (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt == 0) { 
				FoD_Idx_Tmp = 0; Dir_Idx_Tmp++; continue; 
			}

			while (*List_Buff_Pnt != 0x9) { List_Buff_Pnt++; } List_Buff_Pnt++;
			//*�� �ٷ� �������� ã��(����� ����)

			FileOrDir_Length = (unsigned int)strlen(List_Buff_Pnt) - (Get_Last_SlashPos(List_Buff_Pnt, code)+1);
			FileOrDir_Length += (4-(FileOrDir_Length%4));
			//*������ ������ �ʸ��� ���ڿ� ���̸� ���� 4�� ���� �� ���ؾ� �Ѵ�

			FileDirnames_Raw_Size += ((sizeof(short) + FileOrDir_Length) * 2);
			//*(����*2) + 4��ŭ ���Ѵ�

			if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 1) { 
				hEachFile = CreateFileW (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				Each_File_Size = GetFileSize (hEachFile, NULL);
				FilesBuff_Size += Each_File_Size + (4-(Each_File_Size%4))%4;
				CloseHandle (hEachFile);
				//*������ �� ���� ũ�� ���ϱ�
				//*�̰� �������� �ʾ��� ���� ũ���̰�, ������ �����ϰ� �ȴ� �ѵ� ũ��鿡�� ����� �ʱ⿡
				//*�̷��� �� ���� ����. ����� ���� FilesBuff_Size ��ŭ�� �ϸ� �ȴ�
			}
			else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == -1) {
				tmpCstr = Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]; 
				AfxMessageBox (tmpCstr + _T(" : �ش� ����/���丮�� �����ϴ�."));
				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
					free (Dir_File_Wstr_Pnt[jj]);
				} 
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				return;
			}
			else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 0) 
			{;}
			//*������ ���� ������ ũ�⵵ Ȯ�����´�.(�Ź��������� ���� ���� �����µ� 4�� ������������ �Ѵ�)

			FoD_Idx_Tmp++;
			if (FoD_Idx_Tmp >= Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt) 
			{ FoD_Idx_Tmp = 0; Dir_Idx_Tmp++; }
			//*���� ������ �߸� ���� ������

		}
		//*�ٸ� ���� or ���丮

	}
	//*[���ϸ�+üũ��] ���� ��� ��, ���� �̸� ���̴� 4�� �÷� ������ ����Ѵ�
	//*���⼭ ������ ���� ���ۿ� �ʿ��� ���̵� ����Ѵ�

	Files_Buff = (unsigned char*)malloc(sizeof(char)*FilesBuff_Size);
	if (Files_Buff == NULL) {
#ifdef _WIN64
		AfxMessageBox (_T(" : �뷮�� ����ġ�� Ŀ�� �ռ��� �� �����ϴ�."));
#else
		AfxMessageBox (_T(" : ���� �뷮�� �ʹ� Ů�ϴ�. 64��Ʈ�� �����ʽÿ�."));
#endif
		return;
	}
	memset (Files_Buff, 0, FilesBuff_Size);
	//*���� ���� Ȱ��ȭ �� �ʱ�ȭ

	Total_Meta_Size = FileDirnames_Raw_Size + 
		sizeof(FILE_METADATA)*Metadata_Collect_Size + 
		sizeof(DIR_IDX)*Dir_Idx_Collect_Size;
	Total_Meta_Buff = (unsigned char*)malloc(sizeof(char)*Total_Meta_Size);
	memset (Total_Meta_Buff, 0, Total_Meta_Size);
	//*���� ��Ÿ������ ���� ��� �� �Ҵ� �� �ʱ�ȭ

	FileDirnames_Raw = Total_Meta_Buff;
	Metadata_Collect = (FILE_METADATA*)(Total_Meta_Buff + FileDirnames_Raw_Size);
	Dir_Idx_Collect = (DIR_IDX*)(Total_Meta_Buff + FileDirnames_Raw_Size + sizeof(FILE_METADATA)*Metadata_Collect_Size);
	//*�� ��ġ�� ������ ����

	//***1. ��Ʈ ���丮�� ������ �� [�����̸�+üũ��] �迭 ���� ��, ���� ���� ũ��, ���丮 �ε��� ��, �� ��Ÿ������ �� �߻�





	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {

		Dir_Idx_Collect[i].mt_number = (Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt);
		//*�� ���丮�� �����ϴ� ��Ÿ������ ���� �̸� ���������Ƿ� ������ �ش�

		if (i == 0) {
			Dir_Idx_Collect[i].dirmt_ofs = 0x0;
			//*��Ʈ ���丮 ��Ÿ������ �� ó���� �����Ƿ� �翬�� 0�� �־�� �Ѵ�
			Dir_Idx_Collect[i].parent_dir = -1;
			//*��Ʈ ���丮 �θ� ���丮�� �����Ƿ� -1
			Dir_Idx_Collect[i].mt_offset = (sizeof(FILE_METADATA));
			//*��Ʈ ���丮�� ������ ��Ÿ�����ʹ� ��Ʈ ���丮 ��Ÿ������ ������ �����Ƿ� �� ũ�⸸ŭ�� �����ָ� �ȴ�
		}
		//*��Ʈ ���丮 ����

		else {
			memset (Parent_Dir, 0, MAX_PATH);
			memcpy (Parent_Dir, 
				Dir_Str_MetaCnt_Arr[i].Dir_Str, 
				Get_Last_SlashPos (Dir_Str_MetaCnt_Arr[i].Dir_Str, code));
			//*������ ������ ���������� ���ڿ��� �����ϸ� �װ� �θ� ���丮�� �ȴ�

			Dir_Idx_Collect[i].parent_dir = -1;
			for (Dir_Idx_Tmp = 0 ; Dir_Idx_Tmp < i ; Dir_Idx_Tmp++) {
				if (strcmp (Parent_Dir, Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Str) == 0) {
					Dir_Idx_Collect[i].parent_dir = (sizeof(DIR_IDX) * Dir_Idx_Tmp);
					break;
				}
			}
			if (Dir_Idx_Tmp == i) {
				tmpCstr = Dir_File_Wstr_Pnt[i][0];
				AfxMessageBox (tmpCstr + _T(" : �θ� ���丮�� �����ϴ�.\n����Ʈ�� ���� �� �����ϴ�."));
				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
					free (Dir_File_Wstr_Pnt[jj]);
				}
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				free (Files_Buff); free (Total_Meta_Buff);
				return;
				//*���� ���⿡ �̸��������� �θ� ���丮�� ���ٰ� ������ �и� �߸��� ���̴�
			}
			//*���� �װ��� �ռ� ������ Dir_Str_MetaCnt_Arr�κ��� ���ڿ� �����͸� ��� ���ϸ� �ȴ�(�θ� ���丮 ����)
			//*Dir_Idx_Tmp�� ���� ���� ���� ���丮�� �θ� ���丮 �ε����� ������ �ȴ�

			Dir_Idx_Collect[i].mt_offset = (sizeof(FILE_METADATA));
			//*��Ʈ ���丮 ��Ÿ������ ���� ����
			for (unsigned int j = 0;j < i;j++) {
				Dir_Idx_Collect[i].mt_offset += (sizeof(FILE_METADATA) * Dir_Str_MetaCnt_Arr[j].Dir_Meta_Cnt);
			}
			//*�׳� [���� ���丮������ �� ��Ÿ������ ��] + [��Ʈ ���丮�� ��Ÿ������] �� ���ϸ� �� ���̴�
			//*���丮 ���� ��Ÿ������ ������ ����

			for (FoD_Idx_Tmp = 0 ; FoD_Idx_Tmp < Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt ; FoD_Idx_Tmp++) {
				if (strcmp (Dir_Str_MetaCnt_Arr[i].Dir_Str, 
					Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp]) == 0) {
					break;
				}
				//*���� ���丮 ���ڿ��� �θ� ���丮 �� [����/���丮] ���ڿ���� ��
			}
			if (FoD_Idx_Tmp == Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt) {
				tmpCstr = Dir_File_Wstr_Pnt[Dir_Idx_Tmp][0];
				tmpCstr += _T("\n");
				tmpCstr += Dir_File_Wstr_Pnt[i][0];
				tmpCstr += _T(" ���丮 ���� �ش� ���丮�� �����ϴ�.\n����Ʈ�� ���� �� �����ϴ�.");
				AfxMessageBox (tmpCstr);
				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr);
					free (Dir_File_Wstr_Pnt[jj]);
				}
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				free (Files_Buff); free (Total_Meta_Buff);
				return;
				//*���� ���� ���ߴٸ� ����ó��
			}
			//*�θ� ���丮�� ��Ÿ������ �������� �˰� �����Ƿ� (��Ʈ ���丮�� 0x0, ���� ���丮���� ������ �̸� ���Ѵ�)
			//*�θ� ���丮�κ��� ���������� �������� ��� ���ڿ��� '������' �ľ��ϸ� �ȴ�
			Dir_Idx_Collect[i].dirmt_ofs = (Dir_Idx_Collect[Dir_Idx_Tmp].mt_offset
				 + ((sizeof(FILE_METADATA) * FoD_Idx_Tmp)));
			//*�׷��� (�θ� ���丮�� ������) + (��Ÿ������ ����ũ��*������ ��)�� �� ���̴�
			//*���丮 ������(in ��Ÿ������) ����
			//*FoD_Idx_Tmp�� �θ� ���丮 ���� �ش� ���丮�� �ִ� �ε����� ����Ų��
		}
		//*�ٸ� ���丮 ����
	}
	//***2. [���丮 �ε���] �迭 ����. ������ ��Ÿ�����Ͱ� ���������� ���õǾ� �����Ƿ� ������θ� �� �ϸ� �ȴ�





	Dir_Idx_Tmp = (sizeof(short)*2);
	if (Set_FileOrDirName_Data (FileDirnames_Raw, &Dir_Idx_Tmp, 0, Dir_Str_MetaCnt_Arr, 
		Dir_Idx_Collect_Size, Metadata_Collect, code, Dir_File_Wstr_Pnt) == -1)
	{
		tmpCstr = dxaname; AfxMessageBox (tmpCstr + _T(" : ��Ÿ������ ������ �����߽��ϴ�.\n"));
		for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
			free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
			free (Dir_File_Wstr_Pnt[jj]);
		}
		free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt); 
		free (Files_Buff); free (Total_Meta_Buff);
		return;
		//*-1�� ��ȯ�ƴٸ� ����ó��
	}
	//***3. [���ϸ�+üũ��] �迭 ����. �� �� Dir_Str_MetaCnt_Arr�� �����Ѵ�
	//***(���� : FileDirnames_Raw, FileDirnames_Collect �ε���(������), Dir_Str_MetaCnt_Arr, 
	//***���丮 �ε����� �ε���(Dir_Idx_Tmp����), Metadata_Collect), �ڵ尪 -> int�� ��ȯ(-1�� ����)
	//***������ ���� ���丮�� �׳� 0�̴ϱ� �̸� 4����Ʈ�� �ѱ�� �����Ѵ�






	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	Offset_Acumm = 0;
	for (unsigned int i = 0;i < Metadata_Collect_Size;i++) {

		PakDlg->m_Pack_Progress.SetPos (PROGRESSVALUE(i, Metadata_Collect_Size));
		//*���α׷��� �� ����

		Metadata_Collect[i].CompSize = -1;
		//*�е����� ó���Ǽ� ��¿ ���� ���µ�

		if (i == 0) {
			Metadata_Collect[i].fltok_start = 0x0;
			Metadata_Collect[i].attr = (DIR);
			Metadata_Collect[i].generated = Metadata_Collect[i].last_modified = 
				Metadata_Collect[i].last_modified = 0x0;
			Metadata_Collect[i].offset = Metadata_Collect[i].UncompSize = 0x0;
		}
		//*��Ʈ ���丮�� ��. ���丮��, ����, ������ ����, ������ ���� ���� �� �ǹ̰� �����Ƿ� 0 ó���Ѵ�
		//*���� ������, ����������, �������� �ǹ̰� ����

		else {

			if (Is_File_Or_Dir(Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 0) {
				Metadata_Collect[i].attr = DIR;		//*�Ӽ� : ���丮

				hEachFile = CreateFileW (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				GetFileTime(hEachFile, 
					(LPFILETIME)&(Metadata_Collect[i].generated), 
					(LPFILETIME)&(Metadata_Collect[i].last_accessed),
					(LPFILETIME)&(Metadata_Collect[i].last_modified));
				CloseHandle (hEachFile);
				//*����, ���� �ð�, ���� �ð� �ݿ�

				Metadata_Collect[i].UncompSize = 0x0;
				//*�����ũ�� 0, ����ũ�� -1�� ó��

				Metadata_Collect[i].offset = 0x0;
				for (unsigned int j = 0;j < Dir_Idx_Collect_Size;j++) {
					if (strcmp(Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp], 
						Dir_Str_MetaCnt_Arr[j].Dir_Str) == 0) {
							Metadata_Collect[i].offset = (sizeof(DIR_IDX) * j); break;
					}
				}
				//*�������� ������ ���丮 �ε��� ���� �������� �ݿ��ؾ� �Ѵ�
			}
			//*���丮�� ��. ����, ����, ���ӽð�, ����������, ����������� �ǹ̰� ����

			else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 1) {


				if (!CanBeFile (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp], code)) {
					int ll = Get_Last_SlashPos (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp], code);
					if (code == JAP_CODE) { tmpCstr = J2U (&Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp][ll + 1]); }
					else if (code == KOR_CODE) { tmpCstr = K2U (&Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp][ll + 1]); }
					AfxMessageBox (tmpCstr + _T(" : ����ڵ尡 �޶� ������ �ռ��� �� �����ϴ�.\n����ڵ带 �ٲ㺸�ʽÿ�.")); tmpCstr.Empty();
					for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
						free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
						free (Dir_File_Wstr_Pnt[jj]);
					}
					free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
					free (Files_Buff); free (Total_Meta_Buff);
					return;
				}
				//***���� �̸��� �� ���ڿ� üũ : ����ǥ�� ���� ������ ���߿� ����� �� �� �����Ƿ� ����ó���Ѵ�.
				//***üũ ���� : ������ �����ñ��� ã�ư��ٰ� ã�� ���� ����ǥ ������ �Ұ���


				tmpCstr = _T("Packing : ");
				PakDlg->SetDlgItemText (IDC_P_TEXT, tmpCstr + Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]);
				//*��� ���ڿ� ����

				Metadata_Collect[i].attr = FILE_DB;	//*�Ӽ� : ����

				hEachFile = CreateFileW (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				Each_File_Size = GetFileSize (hEachFile, NULL);

				if (Each_File_Size != 0) { 
					Each_File_Buff = (unsigned char*)malloc(sizeof(char) * Each_File_Size * 2); 
					memset (Each_File_Buff, 0, Each_File_Size * 2);
					ReadFile (hEachFile, Each_File_Buff, Each_File_Size, &lp_read, NULL);
				}
				else { Each_File_Buff = NULL; }

				Metadata_Collect[i].UncompSize = Each_File_Size;
				//*���� �ڵ� ���� �о����, ����� ũ�� �ݿ�, ����ġ Ȯ��

				GetFileTime(hEachFile, 
					(LPFILETIME)&(Metadata_Collect[i].generated), 
					(LPFILETIME)&(Metadata_Collect[i].last_accessed),
					(LPFILETIME)&(Metadata_Collect[i].last_modified));
				CloseHandle (hEachFile);
				//*����, ���� �ð�, ���� �ð� �ݿ�

				Each_Str = Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp];
				Each_Str_Len = (unsigned int)strlen(Each_Str);

				//***���� ũ�Ⱑ 0�� ���� ���͸� �ɾ ó���� �ش�

				if ((strcmp(Each_Str + Each_Str_Len - strlen(".common"), ".common") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".dat"), ".dat") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".project"), ".project") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".xxxxx"), ".xxxxx") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".mps"), ".mps") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".mid"), ".mid") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".tile"), ".tile") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - strlen(".txt"), ".txt") == 0)) {

					if (strcmp(Each_Str + Each_Str_Len - strlen("Game.dat"), "Game.dat") == 0) {
						Metadata_Collect[i].CompSize = -1;
						//*���ܷ� Game.dat���� ������������ �ʴ´�
					}
					else {
						if (Each_File_Size != 0) {
							Encode (Each_File_Buff, Each_File_Size);
							Metadata_Collect[i].CompSize = (*(unsigned int*)(&Each_File_Buff[4]));
							Each_File_Size = (unsigned int)Metadata_Collect[i].CompSize;
							//*���� �Ϸ�� ���� ������ ����� ũ�� Ȯ��
						}
						else { Metadata_Collect[i].CompSize = -1; }
					}
				}
				else {
					Metadata_Collect[i].CompSize = -1;
					//*������ ������ �ƴ϶�� ����ũ�⸦ -1�� ��ƹ�����
				}
				//*���� ������ ���� ����, ������� �����Ѵ�
				//*���� Ȯ���ڰ� .dat, .project, .xxxxx, .mps, .mid�� �������� �̰͵��� ������ �����ؾ� �Ѵ�

				Metadata_Collect[i].offset = Offset_Acumm;
				if (Each_File_Size != 0) { memcpy (Files_Buff + Offset_Acumm, Each_File_Buff, Each_File_Size); }
				Offset_Acumm += (Each_File_Size + (4 - (Each_File_Size%4))%4);
				//*������ ����, ���� ���ۿ� ����, ������ ������Ű��

				if (Each_File_Size != 0) { free (Each_File_Buff); }
				//*���� ���� ����
			}
			//*������ ��. � ���ϵ��� ������ �ʿ䰡 �ִ�.

			else {
				tmpCstr = Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1];
				if (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp] == NULL) {
					tmpCstr += _T(" : �ش� ���丮�� �ٸ� ���� Ȥ�� ���丮�� �������� �ʽ��ϴ�.\n�� ���丮�� ����Ʈ���� ������ �� �ٽ� �õ��Ͻʽÿ�.");
				}
				else {
					tmpCstr += _T(" : �ش� ���ڿ��� [����/���丮]�μ� �������� �ʽ��ϴ�.");
				}
				AfxMessageBox (tmpCstr);

				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
					free (Dir_File_Wstr_Pnt[jj]);
				}
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				free (Files_Buff); free (Total_Meta_Buff);
				return;
			}
			//*�ش� ���ڿ��� ���丮�� ������ ��ΰ� �ƴ� �� -> ����

			FoD_Idx_Tmp++;
			if (FoD_Idx_Tmp == (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt)) {
				FoD_Idx_Tmp = 0; Dir_Idx_Tmp++;
			}
			//*���� �� ���丮 ���� ��Ÿ������ �� ������ ���� �ݿ��ߴٸ�,
			//*���� ���丮�� �Ѿ�� ���� �����ϰ� �ʱ�ȭ�Ѵ�

			//*[���丮 �ε���] ������ ���߾� �����Ѵ�
			//*��Ÿ�����ʹ� ���������� ���߾� ������ �����⿡ ���� ���� �ܼ��ϰ� �ص� ��������
		}
		//*�̿��� �ٸ� [���丮/����]�� ��. �� ���丮�� ��Ÿ������ ���� ���� �����Ͽ� �Ҷ�� �����Ѵ�
	}
	//*���丮(0x10) �ƴϸ� ����(0x20)���� �ϰ� �����ϵ��� �Ѵ�. ������� ���� �����ϴ� ���ҵ�.
	//*fltok_start�� �� �������� ���õǹǷ� ���� �ǵ��� �ʾƵ� �ȴ�
	//*�� �۾��� �� ��ġ�� Offset_Acumm�� ���� ���� ���� ũ�Ⱑ �ȴ�

	//***4. [��Ÿ������] �迭 �� ���� ���� ����, �̶� ���Ϲ��۵� ���� �����Ѵ�




	memset (&Dxa_Head, 0, sizeof(WOLF_HEADER));
	if (type == DXA_220) { Dxa_Head.magic = WOLF_MAGIC; }
	else if (type == DXA_THMK) { Dxa_Head.magic = DXA_MAGIC; }
	else { Dxa_Head.magic = WOLF_MAGIC; }
	Dxa_Head.total_toklen = Total_Meta_Size;
	Dxa_Head.header_size = (sizeof(WOLF_HEADER));
	Dxa_Head.totalmt_ofs = (Offset_Acumm + sizeof(WOLF_HEADER));
	Dxa_Head.filemt_ofs = (FileDirnames_Raw_Size);
	Dxa_Head.dir_hd_ofs = (Total_Meta_Size - (sizeof(DIR_IDX)*Dir_Idx_Collect_Size));

	unsigned char *pt;

	if (type == DXA_220) {
		pt = (unsigned char*)(&Dxa_Head);
		for (unsigned int i = 0;i < sizeof(WOLF_HEADER);i++) { pt[i] ^= WOLF_KEY2[(i)%KEY_SIZE]; }
		//*��� xor
		pt = Files_Buff;
		for (unsigned int i = 0;i < Offset_Acumm;i++) { pt[i] ^= WOLF_KEY2[(i + sizeof(WOLF_HEADER))%KEY_SIZE]; }
		//*���Ϲ��� xor
		pt = Total_Meta_Buff;
		for (unsigned int i = 0;i < Total_Meta_Size;i++) { pt[i] ^= WOLF_KEY2[(i + sizeof(WOLF_HEADER) + Offset_Acumm)%KEY_SIZE]; }
		//*��Ÿ������ xor
	}
	if (type == DXA_THMK) {
		pt = (unsigned char*)(&Dxa_Head);
		for (unsigned int i = 0;i < sizeof(WOLF_HEADER);i++) { pt[i] ^= THMK2_KEY[(i)%KEY_SIZE]; }
		//*��� xor
		pt = Files_Buff;
		for (unsigned int i = 0;i < Offset_Acumm;i++) { pt[i] ^= THMK2_KEY[(i + sizeof(WOLF_HEADER))%KEY_SIZE]; }
		//*���Ϲ��� xor
		pt = Total_Meta_Buff;
		for (unsigned int i = 0;i < Total_Meta_Size;i++) { pt[i] ^= THMK2_KEY[(i + sizeof(WOLF_HEADER) + Offset_Acumm)%KEY_SIZE]; }
		//*��Ÿ������ xor
	}
	else {
		pt = (unsigned char*)(&Dxa_Head);
		for (unsigned int i = 0;i < sizeof(WOLF_HEADER);i++) { pt[i] ^= WOLF_KEY[(i)%KEY_SIZE]; }
		//*��� xor
		pt = Files_Buff;
		for (unsigned int i = 0;i < Offset_Acumm;i++) { pt[i] ^= WOLF_KEY[(i + sizeof(WOLF_HEADER))%KEY_SIZE]; }
		//*���Ϲ��� xor
		pt = Total_Meta_Buff;
		for (unsigned int i = 0;i < Total_Meta_Size;i++) { pt[i] ^= WOLF_KEY[(i + sizeof(WOLF_HEADER) + Offset_Acumm)%KEY_SIZE]; }
		//*��Ÿ������ xor
	}
	//***5. ����� �ϳ� ��� �ùٸ� ���� ����ϰ� �ѹ��� ������ xor �Ѵ�




	hDxa = CreateFileW (dxaname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile (hDxa, &Dxa_Head, sizeof(WOLF_HEADER), &lp_write, NULL);
	WriteFile (hDxa, Files_Buff, Offset_Acumm, &lp_write, NULL);
	WriteFile (hDxa, Total_Meta_Buff, Total_Meta_Size, &lp_write, NULL);
	
	PakDlg->m_Pack_Progress.SetPos (100);
	PakDlg->SetDlgItemText (IDC_P_TEXT, _T("Packing Finished"));
	//***6. ���, ���ϰ���, ���� ��Ÿ������ ������ ������ ����Ѵ�




	CloseHandle (hDxa);
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) { 
		free (Dir_Str_MetaCnt_Arr[i].FileOrDir_In_Dir_Arr); 
		free (Dir_File_Wstr_Pnt[i]);
	}
	free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
	free (Files_Buff); free (Total_Meta_Buff);
	//***7. ���� ����
}



void creating_64bit (Pack_Dialog* PakDlg, unsigned int type, unsigned int code)
{
	char *List_Buff, *List_Buff_Pnt;			//����Ʈ ������ ������ ���� �� ������
	unsigned int List_Size;						//����Ʈ�� ũ��

	WOLF_BIG_HEADER Dxa_Head;					//wolf ������ ���

	unsigned int Xor_Ofs;						//��Ÿ�����͸� ����ȭ��Ű�� ���� xor�� Ƚ���� �����Ѵ�

	unsigned char *Files_Buff;					//���ϵ��� �ѵ� ��Ƴ��� ����
	unsigned char *Each_File_Buff;				//�� ������ ����
	unsigned int FilesBuff_Size;				//���� ������ ũ��
	unsigned int Each_File_Size;				//�� ������ ũ��
	__int64 Offset_Acumm;						//������ ����ġ�� ����� �ݿ��ϱ� ���� ����
	
	unsigned char *FileDirnames_Raw;			//[�����̸�+üũ��] �迭�� ��ٷ� ���� �Ұ��ϹǷ� �����̸� ������ ��ȯ�Ѵ�.
	unsigned int FileDirnames_Raw_Size;			//���� [�����̸�+üũ��]�� ũ��

	FILE_BIG_METADATA *Metadata_Collect;		//������ ��Ÿ�����͸� ������ ��
	unsigned int Metadata_Collect_Size;			//�����̸� ��ū���� ������ �� �� ����

	DIR_BIG_IDX *Dir_Idx_Collect;				//���丮�� ������ ������ ��
	unsigned int Dir_Idx_Collect_Size;			//���丮 ������ ������ �� �� ����

	FOD_IN_DIR *Dir_Str_MetaCnt_Arr;			//���丮�� ������ ��Ÿ������ �� �� ���丮 �̸� �����͸� �����ϴ� �迭
	unsigned int Dir_Idx_Tmp;					//�ӽÿ����� ���丮 �ε����� ������ ����
	unsigned int FoD_Idx_Tmp;					//�ӽÿ����� ���丮�� ������ [����/���丮] �ε����� ������ ����

	unsigned char *Total_Meta_Buff;				//�� 3���� �ѵ� ��� ������ ��
	unsigned int Total_Meta_Size;				//�� 3���� ������ ũ��

	char Parent_Dir [MAX_PATH];					//�θ� ���丮�� ��θ� ������ ����
	char *Each_Str;								//�� ����/���丮 ���ڿ� ������
	unsigned int FileOrDir_Length;				//����/���丮�� �̸� ����
	unsigned int Each_Str_Len;					//�� ����/���丮 ���ڿ��� ����

	HANDLE hEachFile;							//�� ������ ����� �� ���̴� �ڵ�
	HANDLE hDxa;								//�۾��� ��ģ �� �� wolf ������ ����� �� ���̴� �ڵ�


	CString tmpCstr;							//������ �����ϰ� ����� �� �ִ�
	wchar_t *List_Buff_Backup;					//*��θ� ���� �� ���� �������
	unsigned int Uni_List_Size;					//*�� ��������� ũ��
	wchar_t ***Dir_File_Wstr_Pnt;				//*��θ� �����ϴ� ������
		

	DWORD lp_read, lp_write;


	TCHAR dxaname[MAX_PATH];
	memset (dxaname, 0, sizeof(TCHAR) * MAX_PATH);
	PakDlg->GetDlgItemText (IDC_P_EDIT1, dxaname, MAX_PATH);
	//*���� �̸� ������

	if (code == JAP_CODE) { List_Buff = U2J (Wolf_File_List.GetBuffer()); }
	else if (code == KOR_CODE) { List_Buff = U2K (Wolf_File_List.GetBuffer()); }
	List_Size = (unsigned int)strlen(List_Buff);

	List_Buff_Backup = Wolf_File_List.GetBuffer();
	Uni_List_Size = (unsigned int)wcslen(List_Buff_Backup);
	//*List_Buff ������ ���ڿ��� �ܾ�� ����� �� ���� ������,
	//*List_Buff_Backup�� ���丮/������ �� �� ���� ������
	//*�׷��Ƿ� List_Buff�� ������ ����ڵ忡 ���� �ٲ۴�.
	//***0. ����Ʈ �о����


	
	Dir_Idx_Collect_Size = 0;
	for (unsigned int i = 0;i < List_Size-0x3;i++) {
		if (*(unsigned int*)(&List_Buff[i]) == 0x0A0D0A0D) { Dir_Idx_Collect_Size++; }
	}
	//*��Ʈ ���丮�� ������ �� ���丮 �� �߻�. ������ 2�� ���޾� �ִ� Ƚ���� �߻��ϸ� �ȴ�

	Dir_Str_MetaCnt_Arr = (FOD_IN_DIR*)malloc(sizeof(FOD_IN_DIR)*Dir_Idx_Collect_Size);
	memset (Dir_Str_MetaCnt_Arr, 0, sizeof(FOD_IN_DIR)*Dir_Idx_Collect_Size);
	//*���丮�� ���� ��Ÿ������ ���� �߻��� �����ϴ� �迭 ���� �� �ʱ�ȭ

	Dir_Str_MetaCnt_Arr[0].Dir_Str = List_Buff;
	//*��ħ ���� ó���� �ִ� �� ��Ʈ ���丮�Ƿ� �̷��� �ϸ� �ȴ�

	Metadata_Collect_Size = 1;
	Dir_Idx_Tmp = 0;
	for (unsigned int i = 0;i < List_Size-4;i++) {
		if (List_Buff[i] == 0x9) { Metadata_Collect_Size++; Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt++; }
		//*���� ������ ��. ��Ÿ������ �迭�� �Ӹ� �ƴ϶� �� ���丮 ���� ��Ÿ������ ���� �÷���

		else if ((List_Buff[i] == 0xD) && (List_Buff[i+1] == 0xA)) { 
			if ((List_Buff[i+2] == 0xD) && (List_Buff[i+3] == 0xA)) { 
				Dir_Idx_Tmp++; 
				Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Str = &List_Buff[i+4];
			}
			//*������ ���� 2���� �� -> ���� ���丮�� �Ѿ�� ����. �׷��Ƿ� �� ���������� ���ڿ��� ���� �Ѵ�
		}
		//*������ ������ �� �� �κ��� �ƿ� 0���� ����� ������ ������, ���� ���� ������ �ٷ� �׷����� �ʴ´�
	}
	//*��Ʈ ���丮�� ������ �� ��Ÿ������ �� �߻�. ���� ���� �������� �߻��ϸ� ������ ��Ʈ ���丮�� ���� �ȵǹǷ� 1�� �� ���Ѵ�
	//*�߰��� ���� �κ��� 0���� ���������� �����ϰ� ����/���丮�� ���� ������ ���ڿ��� �����, �ش� ���丮�� ���� ��Ÿ�����͸� �÷��� �Ѵ�

	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {
		Dir_Str_MetaCnt_Arr[i].FileOrDir_In_Dir_Arr = 
			(char**)malloc(sizeof(char*) * Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt);
		memset (Dir_Str_MetaCnt_Arr[i].FileOrDir_In_Dir_Arr, 0, sizeof(char*) * Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt);
	}
	//*���ڿ� ������ �迭 �Ҵ� �� �ʱ�ȭ
	


	Dir_File_Wstr_Pnt = (wchar_t***)malloc(sizeof(wchar_t**) * Dir_Idx_Collect_Size);
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {
		Dir_File_Wstr_Pnt[i] = (wchar_t**)malloc (sizeof(wchar_t*) * (Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt + 1));
	}
	Dir_File_Wstr_Pnt[0][0] = List_Buff_Backup;
	Dir_Idx_Tmp = 0;
	for (unsigned int i = 0;i < Uni_List_Size-4;i++) { 
		if ((List_Buff_Backup[i] == 0xD) && (List_Buff_Backup[i+1] == 0xA) 
			&& (List_Buff_Backup[i+2] == 0xD) && (List_Buff_Backup[i+3] == 0xA)) { 
				Dir_Idx_Tmp++;
				Dir_File_Wstr_Pnt[Dir_Idx_Tmp][0] = &List_Buff_Backup[i+4];
		} 
	}
	//***��ο� �����ϱ� ���� �����͵� �߰� �Ҵ�, +1�� �ش� ����� �����ϴ� ���丮�� ǥ���ؾ� �ϱ� ����



	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	for (unsigned int i = 0;i < List_Size-1;i++) {
		if (List_Buff[i] == 0x9) { 
			Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp] = &List_Buff[i+1];
			FoD_Idx_Tmp++;
		}
		//*���� ������ ��. �̹����� ������ ���ڿ����� ����� ��Ī�����ش�(���� ���� �ѱ��)

		else if ((List_Buff[i] == 0xD) && (List_Buff[i+1] == 0xA)) { 
			List_Buff[i] = List_Buff[i+1] = 0; 
			if ((List_Buff[i+2] == 0xD) && (List_Buff[i+3] == 0xA)) { 
				FoD_Idx_Tmp = 0;Dir_Idx_Tmp++; 
				if ((List_Buff[i+4] == 0xD) && (List_Buff[i+5] == 0xA)) { 
					i--; while ((List_Buff[i] != 0) && (i >= 0)) { i--; }
					if (code == JAP_CODE) { tmpCstr = J2U (&List_Buff[i+1]); }
					else if (code == KOR_CODE) { tmpCstr = K2U (&List_Buff[i+1]); }
					AfxMessageBox (tmpCstr + _T(" : ���丮�� ������ϴ�.\n����Ʈ���� �������ּ���."));
					for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
						free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
						free (Dir_File_Wstr_Pnt[jj]);
					}
					free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt); tmpCstr.Empty();
					return;
				}
			}
			//*������ ���� 2���� �� -> ���� ���丮�� �Ѿ�� ����.
			//*�׷��Ƿ� [����/���丮] �ε����� �ʱ�ȭ�ϰ� ���丮 �ε����� ���Ѵ�
		}
		//*�̹����߸��� ���� �κ��� 0���� �����
	}
	//*�� ���丮 ���� [����/���丮] �迭 ����



	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	for (unsigned int i = 0;i < Uni_List_Size-1;i++) {
		if (List_Buff_Backup[i] == 0x9) { 
			Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1] = &List_Buff_Backup[i+1];
			FoD_Idx_Tmp++;
		}
		//*���� ������ ��. �̹����� ������ ���ڿ����� ����� ��Ī�����ش�(���� ���� �ѱ��)

		else if ((List_Buff_Backup[i] == 0xD) && (List_Buff_Backup[i+1] == 0xA)) { 
			List_Buff_Backup[i] = List_Buff_Backup[i+1] = 0; 
			if ((List_Buff_Backup[i+2] == 0xD) && (List_Buff_Backup[i+3] == 0xA)) 
			{ FoD_Idx_Tmp = 0;Dir_Idx_Tmp++; }
			//*������ ���� 2���� �� -> ���� ���丮�� �Ѿ�� ����.
			//*�׷��Ƿ� [����/���丮] �ε����� �ʱ�ȭ�ϰ� ���丮 �ε����� ���Ѵ�
		}
		//*�̹����߸��� ���� �κ��� 0���� �����
	}
	//***������ ���� ��� �����Ϳ��� �߰��� ����



	FileDirnames_Raw_Size = 0;
	FilesBuff_Size = 0;
	//*[�����̸�+üũ��] �������� ũ�� �� ���� ���� ũ�� �ʱ�ȭ


	List_Buff_Pnt = List_Buff;
	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	for (unsigned int i = 0;i < Metadata_Collect_Size;i++) {

		if (i == 0) { FileDirnames_Raw_Size += (sizeof(short) * 2); }
		//*��Ʈ ���丮�� ��¡�ϴµ�, ���̰� �ƿ� �����Ƿ� 4�� ���Ѵ�

		else {

			if (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt == 0) { 
				FoD_Idx_Tmp = 0; Dir_Idx_Tmp++; continue; 
			}

			while (*List_Buff_Pnt != 0x9) { List_Buff_Pnt++; } List_Buff_Pnt++;
			//*�� �ٷ� �������� ã��(����� ����)

			FileOrDir_Length = (unsigned int)strlen(List_Buff_Pnt) - (Get_Last_SlashPos(List_Buff_Pnt, code)+1);
			FileOrDir_Length += (4-(FileOrDir_Length%4));
			//*������ ������ �ʸ��� ���ڿ� ���̸� ���� 4�� ���� �� ���ؾ� �Ѵ�

			FileDirnames_Raw_Size += ((sizeof(short) + FileOrDir_Length) * 2);
			//*(����*2) + 4��ŭ ���Ѵ�

			if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 1) { 
				hEachFile = CreateFileW (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				Each_File_Size = GetFileSize (hEachFile, NULL);
				FilesBuff_Size += Each_File_Size + (4-(Each_File_Size%4))%4;
				CloseHandle (hEachFile);
				//*������ �� ���� ũ�� ���ϱ�
				//*�̰� �������� �ʾ��� ���� ũ���̰�, ������ �����ϰ� �ȴ� �ѵ� ũ��鿡�� ����� �ʱ⿡
				//*�̷��� �� ���� ����. ����� ���� FilesBuff_Size ��ŭ�� �ϸ� �ȴ�
			}
			else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == -1) {
				tmpCstr = Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]; 
				AfxMessageBox (tmpCstr + _T(" : �ش� ����/���丮�� �����ϴ�."));
				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
					free (Dir_File_Wstr_Pnt[jj]);
				} 
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				return;
			}
			else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 0) 
			{;}
			//*������ ���� ������ ũ�⵵ Ȯ�����´�.(�Ź��������� ���� ���� �����µ� 4�� ������������ �Ѵ�)

			FoD_Idx_Tmp++;
			if (FoD_Idx_Tmp >= Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt) 
			{ FoD_Idx_Tmp = 0; Dir_Idx_Tmp++; }
			//*���� ������ �߸� ���� ������

		}
		//*�ٸ� ���� or ���丮

	}
	//*[���ϸ�+üũ��] ���� ��� ��, ���� �̸� ���̴� 4�� �÷� ������ ����Ѵ�
	//*���⼭ ������ ���� ���ۿ� �ʿ��� ���̵� ����Ѵ�

	Files_Buff = (unsigned char*)malloc(sizeof(char)*FilesBuff_Size);
	if (Files_Buff == NULL) {
#ifdef _WIN64
		AfxMessageBox (_T(" : �뷮�� ����ġ�� Ŀ�� �ռ��� �� �����ϴ�."));
#else
		AfxMessageBox (_T(" : ���� �뷮�� �ʹ� Ů�ϴ�. 64��Ʈ�� �����ʽÿ�."));
#endif
		return;
	}
	memset (Files_Buff, 0, FilesBuff_Size);
	//*���� ���� Ȱ��ȭ �� �ʱ�ȭ

	Total_Meta_Size = FileDirnames_Raw_Size + 
		sizeof(FILE_BIG_METADATA)*Metadata_Collect_Size + 
		sizeof(DIR_BIG_IDX)*Dir_Idx_Collect_Size;
	Total_Meta_Buff = (unsigned char*)malloc(sizeof(char)*Total_Meta_Size);
	memset (Total_Meta_Buff, 0, Total_Meta_Size);
	//*���� ��Ÿ������ ���� ��� �� �Ҵ� �� �ʱ�ȭ

	FileDirnames_Raw = Total_Meta_Buff;
	Metadata_Collect = (FILE_BIG_METADATA*)(Total_Meta_Buff + FileDirnames_Raw_Size);
	Dir_Idx_Collect = (DIR_BIG_IDX*)(Total_Meta_Buff + FileDirnames_Raw_Size + sizeof(FILE_BIG_METADATA)*Metadata_Collect_Size);
	//*�� ��ġ�� ������ ����

	//***1. ��Ʈ ���丮�� ������ �� [�����̸�+üũ��] �迭 ���� ��, ���� ���� ũ��, ���丮 �ε��� ��, �� ��Ÿ������ �� �߻�





	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {

		Dir_Idx_Collect[i].mt_number = (__int64)(Dir_Str_MetaCnt_Arr[i].Dir_Meta_Cnt);
		//*�� ���丮�� �����ϴ� ��Ÿ������ ���� �̸� ���������Ƿ� ������ �ش�

		if (i == 0) {
			Dir_Idx_Collect[i].dirmt_ofs = (__int64)0x0;
			//*��Ʈ ���丮 ��Ÿ������ �� ó���� �����Ƿ� �翬�� 0�� �־�� �Ѵ�
			Dir_Idx_Collect[i].parent_dir = (__int64)-1;
			//*��Ʈ ���丮 �θ� ���丮�� �����Ƿ� -1
			Dir_Idx_Collect[i].mt_offset = (__int64)(sizeof(FILE_BIG_METADATA));
			//*��Ʈ ���丮�� ������ ��Ÿ�����ʹ� ��Ʈ ���丮 ��Ÿ������ ������ �����Ƿ� �� ũ�⸸ŭ�� �����ָ� �ȴ�
		}
		//*��Ʈ ���丮 ����

		else {
			memset (Parent_Dir, 0, MAX_PATH);
			memcpy (Parent_Dir, 
				Dir_Str_MetaCnt_Arr[i].Dir_Str, 
				Get_Last_SlashPos (Dir_Str_MetaCnt_Arr[i].Dir_Str, code));
			//*������ ������ ���������� ���ڿ��� �����ϸ� �װ� �θ� ���丮�� �ȴ�

			Dir_Idx_Collect[i].parent_dir = (__int64)-1;
			for (Dir_Idx_Tmp = 0 ; Dir_Idx_Tmp < i ; Dir_Idx_Tmp++) {
				if (strcmp (Parent_Dir, Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Str) == 0) {
					Dir_Idx_Collect[i].parent_dir = (__int64)(sizeof(DIR_BIG_IDX) * Dir_Idx_Tmp);
					break;
				}
			}
			if (Dir_Idx_Tmp == i) {
				tmpCstr = Dir_File_Wstr_Pnt[i][0];
				AfxMessageBox (tmpCstr + _T(" : �θ� ���丮�� �����ϴ�.\n����Ʈ�� ���� �� �����ϴ�."));
				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
					free (Dir_File_Wstr_Pnt[jj]);
				}
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				free (Files_Buff); free (Total_Meta_Buff);
				return;
				//*���� ���⿡ �̸��������� �θ� ���丮�� ���ٰ� ������ �и� �߸��� ���̴�
			}
			//*���� �װ��� �ռ� ������ Dir_Str_MetaCnt_Arr�κ��� ���ڿ� �����͸� ��� ���ϸ� �ȴ�(�θ� ���丮 ����)
			//*Dir_Idx_Tmp�� ���� ���� ���� ���丮�� �θ� ���丮 �ε����� ������ �ȴ�

			Dir_Idx_Collect[i].mt_offset = (__int64)(sizeof(FILE_BIG_METADATA));
			//*��Ʈ ���丮 ��Ÿ������ ���� ����
			for (unsigned int j = 0;j < i;j++) {
				Dir_Idx_Collect[i].mt_offset += (__int64)(sizeof(FILE_BIG_METADATA) * Dir_Str_MetaCnt_Arr[j].Dir_Meta_Cnt);
			}
			//*�׳� [���� ���丮������ �� ��Ÿ������ ��] + [��Ʈ ���丮�� ��Ÿ������] �� ���ϸ� �� ���̴�
			//*���丮 ���� ��Ÿ������ ������ ����

			for (FoD_Idx_Tmp = 0 ; FoD_Idx_Tmp < Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt ; FoD_Idx_Tmp++) {
				if (strcmp (Dir_Str_MetaCnt_Arr[i].Dir_Str, 
					Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp]) == 0) {
					break;
				}
				//*���� ���丮 ���ڿ��� �θ� ���丮 �� [����/���丮] ���ڿ���� ��
			}
			if (FoD_Idx_Tmp == Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt) {
				tmpCstr = Dir_File_Wstr_Pnt[Dir_Idx_Tmp][0];
				tmpCstr += _T("\n");
				tmpCstr += Dir_File_Wstr_Pnt[i][0];
				tmpCstr += _T(" ���丮 ���� �ش� ���丮�� �����ϴ�.\n����Ʈ�� ���� �� �����ϴ�.");
				AfxMessageBox (tmpCstr);
				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr);
					free (Dir_File_Wstr_Pnt[jj]);
				}
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				free (Files_Buff); free (Total_Meta_Buff);
				return;
				//*���� ���� ���ߴٸ� ����ó��
			}
			//*�θ� ���丮�� ��Ÿ������ �������� �˰� �����Ƿ� (��Ʈ ���丮�� 0x0, ���� ���丮���� ������ �̸� ���Ѵ�)
			//*�θ� ���丮�κ��� ���������� �������� ��� ���ڿ��� '������' �ľ��ϸ� �ȴ�
			Dir_Idx_Collect[i].dirmt_ofs = (__int64)(Dir_Idx_Collect[Dir_Idx_Tmp].mt_offset
				 + ((sizeof(FILE_BIG_METADATA) * FoD_Idx_Tmp)));
			//*�׷��� (�θ� ���丮�� ������) + (��Ÿ������ ����ũ��*������ ��)�� �� ���̴�
			//*���丮 ������(in ��Ÿ������) ����
			//*FoD_Idx_Tmp�� �θ� ���丮 ���� �ش� ���丮�� �ִ� �ε����� ����Ų��
		}
		//*�ٸ� ���丮 ����
	}
	//***2. [���丮 �ε���] �迭 ����. ������ ��Ÿ�����Ͱ� ���������� ���õǾ� �����Ƿ� ������θ� �� �ϸ� �ȴ�





	Dir_Idx_Tmp = (sizeof(short)*2);
	if (Set_Big_FileOrDirName_Data (FileDirnames_Raw, &Dir_Idx_Tmp, 0, Dir_Str_MetaCnt_Arr, 
		Dir_Idx_Collect_Size, Metadata_Collect, code, Dir_File_Wstr_Pnt) == -1)
	{
		tmpCstr = dxaname; AfxMessageBox (tmpCstr + _T(" : ��Ÿ������ ������ �����߽��ϴ�.\n"));
		for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
			free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
			free (Dir_File_Wstr_Pnt[jj]);
		}
		free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt); 
		free (Files_Buff); free (Total_Meta_Buff);
		return;
		//*-1�� ��ȯ�ƴٸ� ����ó��
	}
	//***3. [���ϸ�+üũ��] �迭 ����. �� �� Dir_Str_MetaCnt_Arr�� �����Ѵ�
	//***(���� : FileDirnames_Raw, FileDirnames_Collect �ε���(������), Dir_Str_MetaCnt_Arr, 
	//***���丮 �ε����� �ε���(Dir_Idx_Tmp����), Metadata_Collect), �ڵ尪 -> int�� ��ȯ(-1�� ����)
	//***������ ���� ���丮�� �׳� 0�̴ϱ� �̸� 4����Ʈ�� �ѱ�� �����Ѵ�






	Dir_Idx_Tmp = FoD_Idx_Tmp = 0;
	Offset_Acumm = 0;
	for (unsigned int i = 0;i < Metadata_Collect_Size;i++) {

		PakDlg->m_Pack_Progress.SetPos (PROGRESSVALUE(i, Metadata_Collect_Size));
		//*���α׷��� �� ����

		if (i == 0) {
			Metadata_Collect[i].fltok_start = (__int64)0x0;
			Metadata_Collect[i].attr = (__int64)(DIR);
			Metadata_Collect[i].generated = Metadata_Collect[i].last_modified = 
				Metadata_Collect[i].last_modified = (__int64)0x0;
			Metadata_Collect[i].offset = Metadata_Collect[i].UncompSize = (__int64)0x0;
			Metadata_Collect[i].CompSize = (__int64)-1;
		}
		//*��Ʈ ���丮�� ��. ���丮��, ����, ������ ����, ������ ���� ���� �� �ǹ̰� �����Ƿ� 0 ó���Ѵ�
		//*���� ������, ����������, �������� �ǹ̰� ����

		else {

			if (Is_File_Or_Dir(Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 0) {
				Metadata_Collect[i].attr = DIR;		//*�Ӽ� : ���丮

				hEachFile = CreateFileW (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				GetFileTime(hEachFile, 
					(LPFILETIME)&(Metadata_Collect[i].generated), 
					(LPFILETIME)&(Metadata_Collect[i].last_accessed),
					(LPFILETIME)&(Metadata_Collect[i].last_modified));
				CloseHandle (hEachFile);
				//*����, ���� �ð�, ���� �ð� �ݿ�

				Metadata_Collect[i].UncompSize = (__int64)0x0;
				Metadata_Collect[i].CompSize = (__int64)-1;
				//*�����ũ�� 0, ����ũ�� -1�� ó��

				Metadata_Collect[i].offset = (__int64)0x0;
				for (unsigned int j = 0;j < Dir_Idx_Collect_Size;j++) {
					if (strcmp(Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp], 
						Dir_Str_MetaCnt_Arr[j].Dir_Str) == 0) {
							Metadata_Collect[i].offset = (__int64)(sizeof(DIR_BIG_IDX) * j); break;
					}
				}
				//*�������� ������ ���丮 �ε��� ���� �������� �ݿ��ؾ� �Ѵ�
			}
			//*���丮�� ��. ����, ����, ���ӽð�, ����������, ����������� �ǹ̰� ����

			else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]) == 1) {


				if (!CanBeFile (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp], code)) {
					int ll = Get_Last_SlashPos (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp], code);
					if (code == JAP_CODE) { tmpCstr = J2U (&Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp][ll + 1]); }
					else if (code == KOR_CODE) { tmpCstr = K2U (&Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp][ll + 1]); }
					AfxMessageBox (tmpCstr + _T(" : ����ڵ尡 �޶� ������ �ռ��� �� �����ϴ�.\n����ڵ带 �ٲ㺸�ʽÿ�.")); tmpCstr.Empty();
					for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
						free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
						free (Dir_File_Wstr_Pnt[jj]);
					}
					free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
					free (Files_Buff); free (Total_Meta_Buff);
					return;
				}
				//***���� �̸��� �� ���ڿ� üũ : ����ǥ�� ���� ������ ���߿� ����� �� �� �����Ƿ� ����ó���Ѵ�.
				//***üũ ���� : ������ �����ñ��� ã�ư��ٰ� ã�� ���� ����ǥ ������ �Ұ���


				tmpCstr = _T("Packing : ");
				PakDlg->SetDlgItemText (IDC_P_TEXT, tmpCstr + Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1]);
				//*��� ���ڿ� ����

				
				Metadata_Collect[i].attr = FILE_DB;	//*�Ӽ� : ����

				hEachFile = CreateFileW (Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				Each_File_Size = GetFileSize (hEachFile, NULL);
				Xor_Ofs = (Each_File_Size % KEY_SIZE);

				if (Each_File_Size != 0) { 
					Each_File_Buff = (unsigned char*)malloc(sizeof(char) * Each_File_Size * 3); 
					memset (Each_File_Buff, 0, Each_File_Size * 3);
					ReadFile (hEachFile, Each_File_Buff, Each_File_Size, &lp_read, NULL);
				}
				else { Each_File_Buff = NULL; }
				//*����ũ�� 0�� �� ����

				Metadata_Collect[i].UncompSize = (__int64)Each_File_Size;
				//*���� �ڵ� ���� �о����, ����� ũ�� �ݿ�, ����ġ Ȯ��

				GetFileTime(hEachFile, 
					(LPFILETIME)&(Metadata_Collect[i].generated), 
					(LPFILETIME)&(Metadata_Collect[i].last_accessed),
					(LPFILETIME)&(Metadata_Collect[i].last_modified));
				CloseHandle (hEachFile);
				//*����, ���� �ð�, ���� �ð� �ݿ�

				//***���� ũ�Ⱑ 0�� ���� ���͸� �ɾ ó���� �ش�

				Each_Str = Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp];
				Each_Str_Len = (unsigned int)strlen(Each_Str);
				if ((strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".common"), ".common") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".dat"), ".dat") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".project"), ".project") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".xxxxx"), ".xxxxx") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".mps"), ".mps") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".mid"), ".mid") == 0)
					|| (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen(".txt"), ".txt") == 0)) {

					if (strcmp(Each_Str + Each_Str_Len - (unsigned int)strlen("Game.dat"), "Game.dat") == 0) {
						Metadata_Collect[i].CompSize = (__int64)-1;
						//*���ܷ� Game.dat���� ������������ �ʴ´�

//						if (Each_File_Buff[0x1F] == 0x1) { Each_File_Buff[0x1F] = 0x2; }
//						Each_File_Buff[0x1F] = 0x2;
//						//*�߰��� ���� �߿� 0x1F ��° ����Ʈ�� 0x1�� �Ǿ� �ִٸ� 0x2�� �ٲ�� �ѱ��� ���´�

						if (type == DXA_330) {
							for (unsigned int j = 0;j < Each_File_Size;j++) {
								Each_File_Buff[j] ^= WOLF_KEY3[(j+Xor_Ofs)%KEY_SIZE];
							}
						}
						else if (type == DXA_THMK_64) {
							for (unsigned int j = 0;j < Each_File_Size;j++) {
								Each_File_Buff[j] ^= THMK2_KEY[(j+Xor_Ofs)%KEY_SIZE];
							}
						}
						else {
							for (unsigned int j = 0;j < Each_File_Size;j++) {
								Each_File_Buff[j] ^= WOLF_KEY3[(j+Xor_Ofs)%KEY_SIZE];
							}
						}
						//*��ġ �����ϰ� xor�ϱ�
					}
					else {

						if (Each_File_Size != 0) { 
							Encode (Each_File_Buff, Each_File_Size);
							Metadata_Collect[i].CompSize = (__int64)(*(unsigned int*)(&Each_File_Buff[4]));
							Each_File_Size = (unsigned int)Metadata_Collect[i].CompSize;
							//*���� �Ϸ�� ���� ������ ����� ũ�� Ȯ��

							if (type == DXA_330) {
								for (unsigned int j = 0;j < (unsigned int)Metadata_Collect[i].CompSize;j++) {
									Each_File_Buff[j] ^= WOLF_KEY3[(j+Xor_Ofs)%KEY_SIZE];
								}
							}
							else if (type == DXA_THMK_64) {
								for (unsigned int j = 0;j < (unsigned int)Metadata_Collect[i].CompSize;j++) {
									Each_File_Buff[j] ^= THMK2_KEY[(j+Xor_Ofs)%KEY_SIZE];
								}
							}
							else {
								for (unsigned int j = 0;j < (unsigned int)Metadata_Collect[i].CompSize;j++) {
									Each_File_Buff[j] ^= WOLF_KEY3[(j+Xor_Ofs)%KEY_SIZE];
								}
							}
							//*��ġ �����ϰ� xor�ϱ�
						}
						//*���� ũ�Ⱑ 0���� Ŭ ���� �����Ѵ�
						else { Metadata_Collect[i].CompSize = (__int64)(-1); }
						//*���� ũ�Ⱑ 0�̶�� ������ ũ�Ⱑ �����Ƿ� -1�� ��Ƶ� �ɱ�?

					}
				}
				else {
					Metadata_Collect[i].CompSize = (__int64)-1;
					//*������ ������ �ƴ϶�� ����ũ�⸦ -1�� ��ƹ�����
					if (type == DXA_330) {
						for (unsigned int j = 0;j < Each_File_Size;j++) {
							Each_File_Buff[j] ^= WOLF_KEY3[(j+Xor_Ofs)%KEY_SIZE];
						}
					}
					if (type == DXA_THMK_64) {
						for (unsigned int j = 0;j < Each_File_Size;j++) {
							Each_File_Buff[j] ^= THMK2_KEY[(j+Xor_Ofs)%KEY_SIZE];
						}
					}
					else {
						for (unsigned int j = 0;j < Each_File_Size;j++) {
							Each_File_Buff[j] ^= WOLF_KEY3[(j+Xor_Ofs)%KEY_SIZE];
						}
					}
					//*��ġ �����ϰ� xor�ϱ�
				}
				//*���� ������ ���� ����, ������� �����Ѵ�
				//*���� Ȯ���ڰ� .dat, .project, .xxxxx, .mps, .mid�� �������� �̰͵��� ������ �����ؾ� �Ѵ�

				Metadata_Collect[i].offset = Offset_Acumm;
				if (Each_File_Size != 0) { memcpy (Files_Buff + Offset_Acumm, Each_File_Buff, Each_File_Size); }
				Offset_Acumm += (Each_File_Size + (4 - (Each_File_Size)%4)%4);
				//*������ ����, ���� ���ۿ� ����, ������ ������Ű��

				if (Each_File_Size != 0) { free (Each_File_Buff); }
				//*���� ���� ����
			}
			//*������ ��. � ���ϵ��� ������ �ʿ䰡 �ִ�.

			else {
				tmpCstr = Dir_File_Wstr_Pnt[Dir_Idx_Tmp][FoD_Idx_Tmp + 1];
				if (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].FileOrDir_In_Dir_Arr[FoD_Idx_Tmp] == NULL) {
					tmpCstr += _T(" : �ش� ���丮�� �ٸ� ���� Ȥ�� ���丮�� �������� �ʽ��ϴ�.\n�� ���丮�� ����Ʈ���� ������ �� �ٽ� �õ��Ͻʽÿ�.");
				}
				else {
					tmpCstr += _T(" : �ش� ���ڿ��� [����/���丮]�μ� �������� �ʽ��ϴ�.");
				}
				AfxMessageBox (tmpCstr);

				for (unsigned int jj = 0;jj < Dir_Idx_Collect_Size;jj++) { 
					free (Dir_Str_MetaCnt_Arr[jj].FileOrDir_In_Dir_Arr); 
					free (Dir_File_Wstr_Pnt[jj]);
				}
				free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
				free (Files_Buff); free (Total_Meta_Buff);
				return;
			}
			//*�ش� ���ڿ��� ���丮�� ������ ��ΰ� �ƴ� �� -> ����

			FoD_Idx_Tmp++;
			if (FoD_Idx_Tmp >= (Dir_Str_MetaCnt_Arr[Dir_Idx_Tmp].Dir_Meta_Cnt)) {
				FoD_Idx_Tmp = 0; Dir_Idx_Tmp++;
			}
			//*���� �� ���丮 ���� ��Ÿ������ �� ������ ���� �ݿ��ߴٸ�,
			//*���� ���丮�� �Ѿ�� ���� �����ϰ� �ʱ�ȭ�Ѵ�

			//*[���丮 �ε���] ������ ���߾� �����Ѵ�
			//*��Ÿ�����ʹ� ���������� ���߾� ������ �����⿡ ���� ���� �ܼ��ϰ� �ص� ��������
		}
		//*�̿��� �ٸ� [���丮/����]�� ��. �� ���丮�� ��Ÿ������ ���� ���� �����Ͽ� �Ҷ�� �����Ѵ�
	}
	//*���丮(0x10) �ƴϸ� ����(0x20)���� �ϰ� �����ϵ��� �Ѵ�. ������� ���� �����ϴ� ���ҵ�.
	//*fltok_start�� �� �������� ���õǹǷ� ���� �ǵ��� �ʾƵ� �ȴ�
	//*�� �۾��� �� ��ġ�� Offset_Acumm�� ���� ���� ���� ũ�Ⱑ �ȴ�

	if (type == DXA_330) {
		for (unsigned int j = 0;j < Total_Meta_Size;j++) {
			Total_Meta_Buff[j] ^= WOLF_KEY3[(j)%KEY_SIZE];
		}
	}
	if (type == DXA_THMK_64) {
		for (unsigned int j = 0;j < Total_Meta_Size;j++) {
			Total_Meta_Buff[j] ^= THMK2_KEY[(j)%KEY_SIZE];
		}
	}
	else {
		for (unsigned int j = 0;j < Total_Meta_Size;j++) {
			Total_Meta_Buff[j] ^= WOLF_KEY3[(j)%KEY_SIZE];
		}
	}
	//*���� ��ġ ���� xor

	//***4. [��Ÿ������] �迭 �� ���� ���� ����, �̶� ���Ϲ��۵� ���� �����Ѵ�




	memset (&Dxa_Head, 0, sizeof(WOLF_BIG_HEADER));
	if (type == DXA_330) { Dxa_Head.magic = WOLF_MAGIC_64; }
	else if (type == DXA_THMK_64) { Dxa_Head.magic = DXA_MAGIC_64; }
	else { Dxa_Head.magic = WOLF_MAGIC_64; }
	Dxa_Head.total_toklen = Total_Meta_Size;
	Dxa_Head.header_size = (__int64)(sizeof(WOLF_BIG_HEADER));
	Dxa_Head.totalmt_ofs = (__int64)(Offset_Acumm + sizeof(WOLF_BIG_HEADER));
	Dxa_Head.filemt_ofs = (__int64)(FileDirnames_Raw_Size);
	Dxa_Head.dir_hd_ofs = (__int64)(Total_Meta_Size - (sizeof(DIR_BIG_IDX)*Dir_Idx_Collect_Size));
	Dxa_Head.end = (__int64)(HEAD_END2);
	unsigned char *pt = (unsigned char*)(&Dxa_Head);
	if (type == DXA_330) {
		for (unsigned int i = 0;i < sizeof(WOLF_BIG_HEADER);i++) { pt[i] ^= WOLF_KEY3[(i)%KEY_SIZE]; }
	}
	if (type == DXA_THMK_64) {
		for (unsigned int i = 0;i < sizeof(WOLF_BIG_HEADER);i++) { pt[i] ^= THMK2_KEY[(i)%KEY_SIZE]; }
	}
	else {
		for (unsigned int i = 0;i < sizeof(WOLF_BIG_HEADER);i++) { pt[i] ^= WOLF_KEY3[(i)%KEY_SIZE]; }
	}
	//***5. ����� �ϳ� ��� �ùٸ� ���� ����ϰ� xor �Ѵ�




	hDxa = CreateFileW (dxaname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile (hDxa, &Dxa_Head, sizeof(WOLF_BIG_HEADER), &lp_write, NULL);
	WriteFile (hDxa, Files_Buff, (unsigned int)Offset_Acumm, &lp_write, NULL);
	WriteFile (hDxa, Total_Meta_Buff, Total_Meta_Size, &lp_write, NULL);
	
	PakDlg->m_Pack_Progress.SetPos (100);
	PakDlg->SetDlgItemText (IDC_P_TEXT, _T("Packing Finished"));
//	printf("\n�ռ� �Ϸ�\n");
	//***6. ���, ���ϰ���, ���� ��Ÿ������ ������ ������ ����Ѵ�




	CloseHandle (hDxa);
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) { 
		free (Dir_Str_MetaCnt_Arr[i].FileOrDir_In_Dir_Arr); 
		free (Dir_File_Wstr_Pnt[i]);
	}
	free (Dir_Str_MetaCnt_Arr); free (Dir_File_Wstr_Pnt);
	free (Files_Buff); free (Total_Meta_Buff);
	//***7. ���� ����
}





int Set_FileOrDirName_Data (unsigned char *FileDirnames_Raw, unsigned int *FileDirnames_Pnt_Addr,
	unsigned int Dir_Idx, FOD_IN_DIR *Dir_Str_MetaCnt_Arr, unsigned int Dir_Idx_Size,
	FILE_METADATA *Metadata_Collect, unsigned int code, wchar_t ***Dir_File_Wstr_Pnt)
{
	unsigned int Def_Len, Set_Len, SlashPos, Meta_Idx, tmp, Ret;
	unsigned short CheckSum;

	for (unsigned int i = 0;i < Dir_Str_MetaCnt_Arr[Dir_Idx].Dir_Meta_Cnt;i++) {

		SlashPos = Get_Last_SlashPos(Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i], code);
		Def_Len = (unsigned int)strlen(Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i] + SlashPos + 1);
		Set_Len = Def_Len + (4 - (Def_Len%4));
		//*���ؾ� �� ���� Ȯ��

		Meta_Idx = 1;
		for (unsigned int j = 0;j < Dir_Idx;j++) { Meta_Idx += Dir_Str_MetaCnt_Arr[j].Dir_Meta_Cnt; }
		Meta_Idx += i; tmp = (*FileDirnames_Pnt_Addr);
		Metadata_Collect[Meta_Idx].fltok_start = (tmp);
		//*�ش� ��Ÿ�����Ϳ� �̸� ��ġ �ݿ�

		*(unsigned short*)(FileDirnames_Raw + (*FileDirnames_Pnt_Addr)) = (Set_Len/4);
		(*FileDirnames_Pnt_Addr) += sizeof(short);
		//*(�̸�����/4) �ݿ�

		Converting_Big ((char*)(FileDirnames_Raw + (*FileDirnames_Pnt_Addr) + sizeof(short)), 
			(Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i] + SlashPos + 1), Def_Len, code);
		//*�빮�ڿ� ��ȯ �� �ݿ�

		CheckSum = 0;
		for (unsigned int j = 0;j < Def_Len;j++) { 
			CheckSum += *(FileDirnames_Raw + (*FileDirnames_Pnt_Addr) + sizeof(short) + j);
		}
		*(unsigned short*)(FileDirnames_Raw + (*FileDirnames_Pnt_Addr)) = CheckSum;
		(*FileDirnames_Pnt_Addr) += (sizeof(short) + Set_Len);
		//*üũ�� �ݿ�

		memcpy (FileDirnames_Raw + (*FileDirnames_Pnt_Addr), 
			Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i] + SlashPos + 1, Def_Len);
		(*FileDirnames_Pnt_Addr) += Set_Len;
		//*�������ڿ� �ݿ�
		
		if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx][i + 1]) == 0) {
			tmp = Dir_Idx_Size;
			for (unsigned int j = 0;j < Dir_Idx_Size;j++) {
				if (strcmp (Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i], 
					Dir_Str_MetaCnt_Arr[j].Dir_Str) == 0) { tmp = j; break; }
			}
			if (tmp == Dir_Idx_Size) { return -1; }
			//*�ش� ���丮 ���ڿ��� ��ġ�ϴ� ���丮 ���ڿ� ã��. ������ -1 ��ȯ
			
			Ret = Set_FileOrDirName_Data (FileDirnames_Raw, FileDirnames_Pnt_Addr, tmp, Dir_Str_MetaCnt_Arr,
				Dir_Idx_Size, Metadata_Collect, code, Dir_File_Wstr_Pnt);
			if (Ret == -1) { return Ret; }
			//*����Լ� ȣ��. -1�̶�� �� ���� �߸��ƴٴ� �����̹Ƿ� �״�� �����Ѵ�
		}
		//*���� ���ڿ��� ���丮���, �ش� ���丮 �ε����� ã�� ���ȣ���ؾ� �Ѵ�
		else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx][i + 1]) == 1) { ; }
		//*�����̸� �׳ɱ׳� �����ϸ� �ȴ�
		else { return -1; }
		//*�̵� ���� �ƴϸ� ������
	}
	return 0;
}
//***�ش� ���ڿ��� �����Ǵ� ��Ÿ������ ��ġ�� (1 + Dir_Idx_Tmp���� ��� ���� �� + Fod)�� �ٽ� ����ϸ� ��


int Set_Big_FileOrDirName_Data (unsigned char *FileDirnames_Raw, unsigned int *FileDirnames_Pnt_Addr,
	unsigned int Dir_Idx, FOD_IN_DIR *Dir_Str_MetaCnt_Arr, unsigned int Dir_Idx_Size,
	FILE_BIG_METADATA *Metadata_Collect, unsigned int code, wchar_t ***Dir_File_Wstr_Pnt)
{
	unsigned int Def_Len, Set_Len, SlashPos, Meta_Idx, tmp, Ret;
	unsigned short CheckSum;

	for (unsigned int i = 0;i < Dir_Str_MetaCnt_Arr[Dir_Idx].Dir_Meta_Cnt;i++) {

		SlashPos = Get_Last_SlashPos(Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i], code);
		Def_Len = (unsigned int)strlen(Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i] + SlashPos + 1);
		Set_Len = Def_Len + (4 - (Def_Len%4));
		//*���ؾ� �� ���� Ȯ��

		Meta_Idx = 1;
		for (unsigned int j = 0;j < Dir_Idx;j++) { Meta_Idx += Dir_Str_MetaCnt_Arr[j].Dir_Meta_Cnt; }
		Meta_Idx += i; tmp = (*FileDirnames_Pnt_Addr);
		Metadata_Collect[Meta_Idx].fltok_start = (__int64)(tmp);
		//*�ش� ��Ÿ�����Ϳ� �̸� ��ġ �ݿ�

		*(unsigned short*)(FileDirnames_Raw + (*FileDirnames_Pnt_Addr)) = (Set_Len/4);
		(*FileDirnames_Pnt_Addr) += sizeof(short);
		//*(�̸�����/4) �ݿ�

		Converting_Big ((char*)(FileDirnames_Raw + (*FileDirnames_Pnt_Addr) + sizeof(short)), 
			(Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i] + SlashPos + 1), Def_Len, code);
		//*�빮�ڿ� ��ȯ �� �ݿ�

		CheckSum = 0;
		for (unsigned int j = 0;j < Def_Len;j++) { 
			CheckSum += *(FileDirnames_Raw + (*FileDirnames_Pnt_Addr) + sizeof(short) + j);
		}
		*(unsigned short*)(FileDirnames_Raw + (*FileDirnames_Pnt_Addr)) = CheckSum;
		(*FileDirnames_Pnt_Addr) += (sizeof(short) + Set_Len);
		//*üũ�� �ݿ�

		memcpy (FileDirnames_Raw + (*FileDirnames_Pnt_Addr), 
			Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i] + SlashPos + 1, Def_Len);
		(*FileDirnames_Pnt_Addr) += Set_Len;
		//*�������ڿ� �ݿ�

		if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx][i + 1]) == 0) {
			tmp = Dir_Idx_Size;
			for (unsigned int j = 0;j < Dir_Idx_Size;j++) {
				if (strcmp (Dir_Str_MetaCnt_Arr[Dir_Idx].FileOrDir_In_Dir_Arr[i], 
					Dir_Str_MetaCnt_Arr[j].Dir_Str) == 0) { tmp = j; break; }
			}
			if (tmp == Dir_Idx_Size) { return -1; }
			//*�ش� ���丮 ���ڿ��� ��ġ�ϴ� ���丮 ���ڿ� ã��. ������ -1 ��ȯ
			
			Ret = Set_Big_FileOrDirName_Data (FileDirnames_Raw, FileDirnames_Pnt_Addr, tmp, Dir_Str_MetaCnt_Arr,
				Dir_Idx_Size, Metadata_Collect, code, Dir_File_Wstr_Pnt);
			if (Ret == -1) { return Ret; }
			//*����Լ� ȣ��. -1�̶�� �� ���� �߸��ƴٴ� �����̹Ƿ� �״�� �����Ѵ�
		}
		//*���� ���ڿ��� ���丮���, �ش� ���丮 �ε����� ã�� ���ȣ���ؾ� �Ѵ�
		else if (Is_File_Or_Dir (Dir_File_Wstr_Pnt[Dir_Idx][i + 1]) == 1) { ; }
		//*�����̸� �׳ɱ׳� �����ϸ� �ȴ�
		else { return -1; }
		//*�̵� ���� �ƴϸ� ������
	}
	return 0;
}
//***�ش� ���ڿ��� �����Ǵ� ��Ÿ������ ��ġ�� (1 + Dir_Idx_Tmp���� ��� ���� �� + Fod)�� �ٽ� ����ϸ� ��(64bit)