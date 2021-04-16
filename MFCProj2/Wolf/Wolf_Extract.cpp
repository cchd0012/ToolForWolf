#include "stdafx.h"

#include "Wolf_Extract.h"


extern CString Wolf_File_ListName;
extern CString Wolf_File_List;
//*����Ʈ �̸��� ������ ����ϴ� �� ���� CString


void extracting (Extract_Dialog* ExtDlg, unsigned int type, unsigned int code)
{
	HANDLE hDxa;
	unsigned char *Dxa_Buff, *Dxa_Buff_Pnt;
	unsigned int Dxa_Size;						//64��Ʈ�������� �ϴ��� int������ ����
	WOLF_HEADER *Dxa_Head;

	unsigned char *Files_Buff;					//���ϵ��� �ѵ� ��Ƴ��� ����
	unsigned int FilesBuff_Size;				//���� ������ ũ��

	unsigned char *FileDirnames_Raw;			//�����̸� ��ū�� ��ٷ� ��ȭ��ų �� �����Ƿ�. ������ �о���� ���� �����̸� ������ ��ȯ�Ѵ�.
	FILESTR_DATA *FileDirnames_Collect;			//������ �����̸� ��ū�� ��ȭ���� ������ ��(��Ʈ ���丮���� ����, ��Ʈ�� ���� �̸� ���)
	unsigned int FileDirnames_Collect_Size;		//�����̸� ��ū���� ������ �� �� ����

	FILE_METADATA *Metadata_Collect;			//������ ��Ÿ�����͸� ������ ��
	unsigned int Metadata_Collect_Size;			//�����̸� ��ū���� ������ �� �� ����

	DIR_IDX *Dir_Idx_Collect;					//���丮�� ������ ������ ��
	unsigned int *ParentDir_Name_Len_Buff;		//�� �θ� ���丮�� ���̸� ��Ƴ��� ��
	unsigned int Dir_Idx_Collect_Size;			//���丮 ������ ������ �� �� ����

	unsigned int Parent_Dir_Idx;				//[���丮 �ε���] ���� �θ� ���丮�� ��ġ�� ����Ų��
	unsigned int Parent_Dir_FileDirnames_Pos;	//�θ� ���丮�� [�̸�+üũ��] ��ġ�� ����Ų��
	unsigned int Each_Dir_FileDirnames_Pos;		//�ڽ� ���丮�� [�̸�+üũ��] ��ġ�� ����Ų��
	unsigned int Each_FileDirnames_Pos;			//�ڽ�(���� or ���丮)�� [�̸�+üũ��] ��ġ�� ����Ų��
	unsigned int FileInDir_Meta_Start_Pos;		//���丮�� �����ϴ� ���ϵ� ��Ÿ�������� ��������
	unsigned int Ofs_Of_DirMeta;				//���丮�� ������ ���丮 ��Ÿ�����ͷ� ���� ���� ���� ����

	//*�ϴ� __int64�� �ؾ� �´� �� ���� �ѵ�, �ƹ����� ��߳� ����� �־� unsigned int�� �����Ѵ�

	char Root_Dir [MAX_PATH];					//��Ʈ ���丮�� ��θ� ������ ����
	char Upper_Dir [MAX_PATH];					//���� ���丮�� ��θ� ������ ����
	char FileOrDir_In_Dir [MAX_PATH];			//���丮 �ؿ� ���� ����/���丮�� ��θ� ������ ����

	char List_String [MAX_PATH];				//����Ʈ�� �α׸� �����ϱ� ���� ���̴� ����

	unsigned char *Each_File_Buff;				//�� ������ ����Ű�� �� ����
	unsigned int Each_File_Size;				//�� ������ ũ��

	HANDLE hEachFile;							//�� ������ ����� �� ���̴� �ڵ�
		
	DWORD lp_read, lp_write;

	wchar_t *tmpwstr;							//������ ���� ��Ա� ����



	hDxa = CreateFileW(ExtDlg->Wolf_Full_Path, 
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDxa == INVALID_HANDLE_VALUE){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� dxa ������ ���� ���߽��ϴ�."));
		return;
	}
	//*dxa ���� ����

	Dxa_Size = GetFileSize(hDxa, NULL);
	if(Dxa_Size <= sizeof(WOLF_HEADER)){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		CloseHandle(hDxa);return;
	}//*dxa ���� ũ�� ����. �ּ��� ������ٴ� Ŀ�� �Ѵ�

	Dxa_Buff = (unsigned char*)malloc(sizeof(char) * Dxa_Size);

	Dxa_Buff_Pnt = Dxa_Buff;
	if (ReadFile (hDxa, Dxa_Buff, (DWORD)Dxa_Size, &lp_read, NULL) == 0) {
#ifdef WIN64
		AfxMessageBox (_T(" : �뷮�� ����ġ�� Ŀ�� ������ �� �����ϴ�."));
#else
		AfxMessageBox (_T(" : �ش� ������ �뷮�� �ʹ� Ů�ϴ�. 64��Ʈ�� �����ʽÿ�."));
#endif
		return;
	}
	CloseHandle (hDxa);
	for (unsigned int i = 0;i < Dxa_Size;i++) { 
		if (type == DXA_220) { Dxa_Buff[i] ^= WOLF_KEY2[i%KEY_SIZE]; }
		else if (type == DXA_THMK) { Dxa_Buff[i] ^= THMK2_KEY[i%KEY_SIZE]; }
		else { Dxa_Buff[i] ^= WOLF_KEY[i%KEY_SIZE]; }
	}
	//*���� �о���� �� �ڵ� �ݰ� ���� xor ���
	//*�׷��� ���⼭ �Ҵ��� �� 32��Ʈ ���α׷��̶� 64��Ʈ�� __int64�� �������̰� ���µ�,
	//*�ϴ� �̰� �ѱ�ٽ��� �ϰ� �����ؾ߰���

	Dxa_Head = (WOLF_HEADER*)Dxa_Buff;
	if ((Dxa_Head->magic != WOLF_MAGIC) && (Dxa_Head->magic != DXA_MAGIC)) {
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� ������ ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
	}
	//*��� �����͸� ����� �� ������ ����Ű�� �� �� ������ ����

	if(Dxa_Size < (Dxa_Head->totalmt_ofs + Dxa_Head->total_toklen)){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
	}
	//*����� ũ�� ���� ����

	if((Dxa_Head->total_toklen <= Dxa_Head->filemt_ofs) || (Dxa_Head->total_toklen <= Dxa_Head->dir_hd_ofs)){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
	}
	//*�� ��Ÿ�����͵鰣�� ������ ���� ����

	if (*(unsigned int*)(Dxa_Buff + (Dxa_Head->totalmt_ofs)) != 0x00000000) {
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
		//*�̷��� 0�� �ƴ϶�� ����ó���Ѵ�
	}
	//*��Ÿ������ �κ� xor ��α�(����Ʈ ��Ͽ�)
	//***0. ���� ���� ���� ���� ���� �˻�(��Ÿ������ ������ ������ ��Ʈ���� �����ϱ⿡ 0�̾�� �Ѵ�)

	FilesBuff_Size = (Dxa_Head->totalmt_ofs) - (Dxa_Head->header_size);
	Files_Buff = Dxa_Buff+(Dxa_Head->header_size);
	//*[���� ����] ũ�� �ľ� �� ����(xor�� ���� ��� �� �Բ� �Ѵ�)
	
	FileDirnames_Raw = (unsigned char*)malloc(sizeof(char)*(Dxa_Head->filemt_ofs));
	memcpy (FileDirnames_Raw, Dxa_Buff+(Dxa_Head->totalmt_ofs), (Dxa_Head->filemt_ofs));
	FileDirnames_Collect_Size = Get_File_Count(FileDirnames_Raw, (Dxa_Head->filemt_ofs));
	if(FileDirnames_Collect_Size == 0xFFFFFFFF){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : ���� �̸� ��ū�� �������ϴ�."));
		free (FileDirnames_Raw); free (Files_Buff); free (Dxa_Buff); return;
	}
	//*[���� �̸�+üũ�� ��ū] ������ �� �ľ��ϱ�

	FileDirnames_Collect = (FILESTR_DATA*)malloc(sizeof(FILESTR_DATA)*FileDirnames_Collect_Size);
	Set_FileNames(FileDirnames_Collect, FileDirnames_Collect_Size, FileDirnames_Raw);
	free(FileDirnames_Raw);
	//*[���� �̸�+üũ�� ��ū] �迭 ���� �� �����ϱ�

	Metadata_Collect_Size = (((Dxa_Head->dir_hd_ofs) - (Dxa_Head->filemt_ofs))) / sizeof(FILE_METADATA);
	if(Metadata_Collect_Size * sizeof(FILE_METADATA) != ((Dxa_Head->dir_hd_ofs) - (Dxa_Head->filemt_ofs))){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : ���� ��Ÿ�����Ͱ� �������ϴ�."));
		for(unsigned int i = 0;i < FileDirnames_Collect_Size;i++){ free (FileDirnames_Collect[i].big_str); free (FileDirnames_Collect[i].real_str); }
		free(FileDirnames_Collect); free (Files_Buff); free (Dxa_Buff); return;
	}
	//*[��Ÿ������] ���� �� �ľ��ϱ�

	Metadata_Collect = (FILE_METADATA*) malloc(sizeof(FILE_METADATA) * Metadata_Collect_Size);
	memcpy (Metadata_Collect, 
		Dxa_Buff+(Dxa_Head->totalmt_ofs + Dxa_Head->filemt_ofs), 
		(sizeof(FILE_METADATA)*Metadata_Collect_Size));
	//*[��Ÿ������] �迭 �����ϱ�

	Dir_Idx_Collect_Size = ((Dxa_Head->total_toklen) - (Dxa_Head->dir_hd_ofs)) / sizeof(DIR_IDX);
	if(Dir_Idx_Collect_Size * sizeof(DIR_IDX) != ((Dxa_Head->total_toklen) - (Dxa_Head->dir_hd_ofs))){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : ���丮 ������ �������ϴ�."));
		free (Metadata_Collect);
		for(unsigned int i = 0;i < FileDirnames_Collect_Size;i++){ free (FileDirnames_Collect[i].big_str); free (FileDirnames_Collect[i].real_str); }
		free (FileDirnames_Collect); free (Files_Buff); free (Dxa_Buff); return;
	}
	//*[���丮 �ε���] ���� �� �ľ��ϱ�

	Dir_Idx_Collect = (DIR_IDX*) malloc(sizeof(DIR_IDX) * Dir_Idx_Collect_Size);
	memcpy (Dir_Idx_Collect, Dxa_Buff+(Dxa_Head->totalmt_ofs + Dxa_Head->dir_hd_ofs), (sizeof(DIR_IDX)*Dir_Idx_Collect_Size));
	//*[���丮 �ε���] �迭 �����ϱ�

	ParentDir_Name_Len_Buff = (unsigned int*)malloc(sizeof(int) * Dir_Idx_Collect_Size);
	memset (ParentDir_Name_Len_Buff, 0, sizeof(int) * Dir_Idx_Collect_Size);
	//*[�θ� ���丮 �̸� ����] �迭 �ʱ�ȭ

//	free (Dxa_Buff);
	//*���� ���۴� ����Ǹ� �ʿ������ �����
	//***1. ����� ������ �������� �� ��Ÿ�����Ϳ� �����Ǵ� ����ü�� ����. ��� ���縸 �� �ϸ� �ȴ�




	memset (Root_Dir, 0, MAX_PATH);
	char *tt = U2K (ExtDlg->Wolf_File_Title.GetBuffer());
	for(unsigned int i = 0;i < strlen(tt);i++){ Root_Dir[i] = tt[i]; }
	free(tt);

	Wolf_File_ListName = ExtDlg->Wolf_File_Title + _T(".txt");
	//*�̸� ��Ʈ ���丮, ����Ʈ ���� �̸� �����α�
	
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {
		if (Dir_Idx_Collect[i].parent_dir != -1)  {
			if (Dir_Idx_Collect[i].parent_dir == 0x00) {
				ParentDir_Name_Len_Buff[i] = (unsigned int)strlen(Root_Dir);
				//*�θ� ��Ʈ ���丮��� �׳� ��Ʈ ���̸� �����ָ� �ȴ�
			}
			else{
				Parent_Dir_Idx = (unsigned int)Dir_Idx_Collect[i].parent_dir / sizeof(DIR_IDX);
				//*[���丮 �ε���] �� �θ� ���丮�� ��ġ
				Parent_Dir_FileDirnames_Pos = Find_FileOrDirName_Idx (FileDirnames_Collect, FileDirnames_Collect_Size,
					Metadata_Collect[Dir_Idx_Collect[Parent_Dir_Idx].dirmt_ofs / sizeof(FILE_METADATA)].fltok_start);
				//*�θ� ���丮�� �����Ǵ� [�̸�+üũ��]�� �ִ� ��(�ε���)

				ParentDir_Name_Len_Buff[i] = ParentDir_Name_Len_Buff[Parent_Dir_Idx];
				//*�θ� ���丮�� [�θ� ���丮 ��� ����] ���ϱ�
				ParentDir_Name_Len_Buff[i]++;
				//*������('\') ���� ���ϱ�
				ParentDir_Name_Len_Buff[i] += (unsigned int)strlen(FileDirnames_Collect[Parent_Dir_FileDirnames_Pos].real_str);
				//*�θ� ���丮�� �����Ǵ� ���ڿ��� ã�� �� ���� ���ϱ�
			}
			//*�θ� ���丮�� ���̸� ���� �����ÿ� �Բ� ���ϱ�
		}//*��Ʈ ���丮�� �ƴ� ��(��Ʈ ���丮�� ���� �̹� 0���� ���õǾ� �����Ƿ� �������)
	}
	//*[�θ� ���丮 �̸� ����] �迭 �����ϱ�
	//*�� ó���� ��Ʈ ���丮���� �˰� �ؾ� �Ѵ�.(���� ��ġ : 0)
	//*�������� ������ ó������ �����ϸ� �������� ���� �ľ��� �����ϴ�

	//***1.5. ���� ���ظ� ����� ����Ʈ ����/[�θ� ���丮�� ��� ����] �迭 ����



	

	unsigned int Total_Cnt = 0, Cnt = 0;
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) { Total_Cnt += Dir_Idx_Collect[i].mt_number; }
	//*���α׷��� �� ���� �غ�

	memset(Upper_Dir, 0, MAX_PATH);
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {

		memset(Upper_Dir + ParentDir_Name_Len_Buff[i], MAX_PATH - ParentDir_Name_Len_Buff[i], 0);
		//*���� ���丮 ���� �غ�
		//*��Ȯ���� �θ� ���丮 ��θ� ���ܳ��ƾ� �Ѵ�

		Ofs_Of_DirMeta = 0;
		//*���丮 ���� ���丮�� ���� �������� ���� �ʱ�ȭ

		if (Dir_Idx_Collect[i].parent_dir == -1) {
			sprintf_s (Upper_Dir, MAX_PATH, "%s", Root_Dir);
			//*���� ���丮�� ��Ʈ ���丮 ����
		}//*�θ� ���丮�� ������ ��Ʈ ���丮���� �ǹ��Ѵ�
		else {
			Each_Dir_FileDirnames_Pos = Find_FileOrDirName_Idx (FileDirnames_Collect, FileDirnames_Collect_Size,
				Metadata_Collect[Dir_Idx_Collect[i].dirmt_ofs / sizeof(FILE_METADATA)].fltok_start);
			//*�̸� ��ġ�� ���α�

			sprintf_s (Upper_Dir + ParentDir_Name_Len_Buff[i], 
				MAX_PATH - ParentDir_Name_Len_Buff[i], 
				"\\%s", 
				FileDirnames_Collect[Each_Dir_FileDirnames_Pos].real_str);
			//*����� ���� ��ġ������ �ϰ�, �����ñ��� ���ؼ� ��´�
		}//*�θ� ���丮�� �ִٸ� ��Ʈ ���丮�� ���� ���丮�̹Ƿ�, �θ� ���丮�� �ٿ��� ����Ѵ�


		memset (List_String, 0, MAX_PATH);
		sprintf_s (List_String, MAX_PATH, "%s\r\n", Upper_Dir);

		if (code == JAP_CODE) { tmpwstr = J2U (List_String); }
		else if (code == KOR_CODE) { tmpwstr = K2U (List_String); }
		Wolf_File_List += tmpwstr; free (tmpwstr);
		//*���� ���丮 �̸� Ȯ�� �� ����Ʈ�� ���. ������ �������

		if (My_mkdir (Upper_Dir, code) != 0) {
			if (code == JAP_CODE) { tmpwstr = J2U (Upper_Dir); }
			else if (code == KOR_CODE) { tmpwstr = K2U (Upper_Dir); }
			CString t = tmpwstr;
			AfxMessageBox (t + _T(" : ������ ������ ���߽��ϴ�."));
			ExtDlg->m_Extract_Progress.SetPos (0);
			Wolf_File_ListName = _T(""); Wolf_File_List.Empty();
			free (tmpwstr); return;
		}
		//*���� ���丮 ����� (����ڵ� ���� ������ ���ߴٸ� ����ó��)

		FileInDir_Meta_Start_Pos = Dir_Idx_Collect[i].mt_offset / sizeof(FILE_METADATA);
		//*�ش� ���丮�� ������ ���ϵ� ��Ÿ������ �������� ���

		for (unsigned int j = 0;j < Dir_Idx_Collect[i].mt_number;j++) {

			ExtDlg->m_Extract_Progress.SetPos (PROGRESSVALUE(Cnt++, Total_Cnt));
			//*���α׷��� �� ��ġ ����

			Each_FileDirnames_Pos = Find_FileOrDirName_Idx (FileDirnames_Collect, FileDirnames_Collect_Size,
				Metadata_Collect[FileInDir_Meta_Start_Pos + j].fltok_start);
			//*���� �ش� ��Ÿ�����Ϳ� �����Ǵ� [����/���丮��+üũ��] ���� �ε����� ��´�

			memset(FileOrDir_In_Dir, MAX_PATH, 0);
			sprintf_s (FileOrDir_In_Dir, MAX_PATH, "%s\\%s",
				Upper_Dir, FileDirnames_Collect[Each_FileDirnames_Pos].real_str);
			//*����/���丮�� Ȯ��

			memset (List_String, 0, MAX_PATH);
			sprintf_s (List_String, MAX_PATH, "\t%s\r\n", FileOrDir_In_Dir);


			if (code == JAP_CODE) { tmpwstr = J2U (List_String); }
			else if (code == KOR_CODE) { tmpwstr = K2U (List_String); }
			Wolf_File_List += tmpwstr; free (tmpwstr);
			//*����Ʈ�� ���(���� �߰�)


			if ((Metadata_Collect [FileInDir_Meta_Start_Pos + j].attr & DIR) != 0) {}
			//*���丮�� ���� ���� ũ�� �Ű澵 �ʿ� ����
			else {
				bool IsDir = false;
				//*�������� �׳� �ص� ������, ����Ÿ���� ������ �߰��ؾ� �Ѵ�
				//*�߰��� ���� ������ �׳� �����̶�� ġ��

				if (!IsDir) {

					if (code == KOR_CODE) { tmpwstr = K2U(FileOrDir_In_Dir); }
					else { tmpwstr = J2U(FileOrDir_In_Dir); }

					CString Sst (_T("Extracting"));
					if (type == DXA_LOW) { Sst += _T(" (Type 1) : "); }
					else if (type == DXA_220) { Sst += _T(" (Type 2) : "); }
					else if (type == DXA_330) { Sst += _T(" (Type 3) : "); }
					else if (type == DXA_THMK) { Sst += _T(" (Type th_l_2) : "); }
					else { Sst += _T(" (Type th_l_64bit) : "); }
					Sst += tmpwstr;

					ExtDlg->SetDlgItemText (IDC_E_TEXT, Sst);
					//*��� ���ڿ� ����


					Each_File_Buff = (unsigned char*)malloc (sizeof(char)*Metadata_Collect [FileInDir_Meta_Start_Pos + j].UncompSize);
					Each_File_Size = Metadata_Collect [FileInDir_Meta_Start_Pos + j].UncompSize;

					if (Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize == -1) {
						memcpy (Each_File_Buff, 
							Files_Buff + Metadata_Collect [FileInDir_Meta_Start_Pos + j].offset,
							Metadata_Collect [FileInDir_Meta_Start_Pos + j].UncompSize);
						//*����ϰ� �� ���� ũ�� ���
					}
					else {
						memcpy (Each_File_Buff, 
							Files_Buff + Metadata_Collect [FileInDir_Meta_Start_Pos + j].offset,
							Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize);
						//*����ϰ� �� ���� ũ�� ���
					}
					//*����ϰ��� �ϴ� ���� ���� �����ؿ���(����/����� ����)

					Decode (Each_File_Buff, Each_File_Size);
					Each_File_Size = Metadata_Collect [FileInDir_Meta_Start_Pos + j].UncompSize;
					//*������ �Ǿ������� ���� Ǯ��
					
					hEachFile = CreateFileW (tmpwstr, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (!WriteFile (hEachFile, Each_File_Buff, Each_File_Size, &lp_write, NULL)) {
						CString t = tmpwstr;
						AfxMessageBox (t + _T(" : ������ ������ ���߽��ϴ�."));
						ExtDlg->m_Extract_Progress.SetPos (0);
						Wolf_File_ListName = _T(""); Wolf_File_List.Empty();
						CloseHandle (hEachFile); free (Each_File_Buff);
						free (tmpwstr); return;
					}
					CloseHandle (hEachFile); free (Each_File_Buff);
					//*���� ��� �� �ڵ�, ���� ����(����ڵ� ���� ������ ���ߴٸ� ����ó��)
				}

			}
			//*������ ��
			
			//*���丮�� ���� ������ ���� �����Ͽ� �����Ѵ�

		}
		//*���丮 �ε����� ��ϵ� ��Ÿ������ ���� ����ŭ ���� ���鼭 ����/���丮���� ����Ѵ�

		Wolf_File_List += _T("\r\n");
		//*����Ʈ ���� ����

		//*�� ���� ��Ʈ ���丮�� ����Ű��, ���������� ���鼭 ����Ѵ�
		//*�������� ���丮(0x10) �ƴϸ� ����(0x20)�θ� �����ϴµ�?
	}
	//*�ش� wolf ������ ���� ���丮 ����ŭ �ݺ��Ѵ�

	ExtDlg->m_Extract_Progress.SetPos (100);
	ExtDlg->SetDlgItemText (IDC_E_TEXT, _T("Extract Finished"));
	//***2. ��Ÿ������ ���� ����ü�� �̿��� ���� ���� �� ����Ʈ ���
	//(���� ��İ� ������ : (ROOT)���� wolf�� ���ϸ��� �״�� �ְ�, ���̾� �������� ���丮�鵵 �˴� �ٿ��ִ´�)
	//(�׷��� ���� ���������� ���� ��ŷ�� �� �� �ִ�. �� ���� �ȴٴ� ��.)

	

	
	free (ParentDir_Name_Len_Buff); free (Dir_Idx_Collect); free (Metadata_Collect);
	for(unsigned int i = 0;i < FileDirnames_Collect_Size;i++){ free (FileDirnames_Collect[i].big_str); free (FileDirnames_Collect[i].real_str); }
	free (FileDirnames_Collect); //free (Files_Buff);
	free (Dxa_Buff);
	//***3. ���� ����
}




void extracting_64bit (Extract_Dialog* ExtDlg, unsigned int dxa_type, unsigned int code)
{
	HANDLE hDxa;
	unsigned char *Dxa_Buff, *Dxa_Buff_Pnt;
	unsigned int Dxa_Size;						//64��Ʈ�������� �ϴ��� int������ ����
	WOLF_BIG_HEADER *Dxa_Head;					//wolf ������ ���

	unsigned int Xor_Ofs;						//��Ÿ�����͸� ����ȭ��Ű�� ���� xor�� Ƚ���� �����Ѵ�

	unsigned char *Files_Buff;					//���ϵ��� �ѵ� ��Ƴ��� ����
	unsigned int FilesBuff_Size;				//���� ������ ũ��

	unsigned char *FileDirnames_Raw;			//�����̸� ��ū�� ��ٷ� ��ȭ��ų �� �����Ƿ�. ������ �о���� ���� �����̸� ������ ��ȯ�Ѵ�.
	FILESTR_DATA *FileDirnames_Collect;			//������ �����̸� ��ū�� ��ȭ���� ������ ��(��Ʈ ���丮���� ����, ��Ʈ�� ���� �̸� ���)
	unsigned int FileDirnames_Collect_Size;		//�����̸� ��ū���� ������ �� �� ����

	FILE_BIG_METADATA *Metadata_Collect;		//������ ��Ÿ�����͸� ������ ��
	unsigned int Metadata_Collect_Size;			//�����̸� ��ū���� ������ �� �� ����

	DIR_BIG_IDX *Dir_Idx_Collect;				//���丮�� ������ ������ ��
	unsigned int *ParentDir_Name_Len_Buff;		//�� �θ� ���丮�� ���̸� ��Ƴ��� ��
	unsigned int Dir_Idx_Collect_Size;			//���丮 ������ ������ �� �� ����

	unsigned int Parent_Dir_Idx;				//[���丮 �ε���] ���� �θ� ���丮�� ��ġ�� ����Ų��
	unsigned int Parent_Dir_FileDirnames_Pos;	//�θ� ���丮�� [�̸�+üũ��] ��ġ�� ����Ų��
	unsigned int Each_Dir_FileDirnames_Pos;		//�ڽ� ���丮�� [�̸�+üũ��] ��ġ�� ����Ų��
	unsigned int Each_FileDirnames_Pos;			//�ڽ�(���� or ���丮)�� [�̸�+üũ��] ��ġ�� ����Ų��
	unsigned int FileInDir_Meta_Start_Pos;		//���丮�� �����ϴ� ���ϵ� ��Ÿ�������� ��������
	unsigned int Ofs_Of_DirMeta;				//���丮�� ������ ���丮 ��Ÿ�����ͷ� ���� ���� ���� ����

	//*�ϴ� __int64�� �ؾ� �´� �� ���� �ѵ�, �ƹ����� ��߳� ����� �־� unsigned int�� �����Ѵ�

	char Root_Dir [MAX_PATH];					//��Ʈ ���丮�� ��θ� ������ ����
	char Upper_Dir [MAX_PATH];					//���� ���丮�� ��θ� ������ ����
	char FileOrDir_In_Dir [MAX_PATH];			//���丮 �ؿ� ���� ����/���丮�� ��θ� ������ ����

	char List_String [MAX_PATH];				//����Ʈ�� �α׸� �����ϱ� ���� ���̴� ����

	unsigned char *Each_File_Buff;				//�� ������ ����Ű�� �� ����
	unsigned int Each_File_Size;				//�� ������ ũ��

	HANDLE hEachFile;							//�� ������ ����� �� ���̴� �ڵ�
		
	DWORD lp_read, lp_write;

	wchar_t *tmpwstr;							//*������ ��Ա� ����

	unsigned char *XORKEY;
	if (dxa_type == DXA_330) { XORKEY = WOLF_KEY3; }
	else if (dxa_type == DXA_THMK_64) { XORKEY = THMK2_KEY; }
	else { XORKEY = WOLF_KEY3; }



	hDxa = CreateFileW(((wchar_t*)(LPCTSTR)(ExtDlg->Wolf_Full_Path)), 
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDxa == INVALID_HANDLE_VALUE){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� dxa ������ ���� ���߽��ϴ�."));
		return;
	}
	//*dxa ���� ����

	Dxa_Size = GetFileSize(hDxa, NULL);
	if(Dxa_Size <= sizeof(WOLF_BIG_HEADER)){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		CloseHandle(hDxa);return;
	}//*dxa ���� ũ�� ����. �ּ��� ������ٴ� Ŀ�� �Ѵ�

	Dxa_Buff = (unsigned char*)malloc(sizeof(char) * Dxa_Size);
	Dxa_Buff_Pnt = Dxa_Buff;
	if (ReadFile (hDxa, Dxa_Buff, (DWORD)Dxa_Size, &lp_read, NULL) == 0) {
#ifdef WIN64
		AfxMessageBox (_T(" : �뷮�� ����ġ�� Ŀ�� ������ �� �����ϴ�."));
#else
		AfxMessageBox (_T(" : �ش� ������ �뷮�� �ʹ� Ů�ϴ�. 64��Ʈ�� �����ʽÿ�."));
#endif
		return;
	}
	CloseHandle (hDxa);
	for (unsigned int i = 0;i < sizeof(WOLF_BIG_HEADER);i++) { Dxa_Buff[i] ^= XORKEY[i%KEY_SIZE]; }
	//*���� �о���� �� �ڵ� �ݱ�, �׸��� ����� xor ���
	//*�׷��� ���⼭ �Ҵ��� �� 32��Ʈ ���α׷��̶� 64��Ʈ�� __int64�� �������̰� ���µ�,
	//*�ϴ� �̰� �ѱ�ٽ��� �ϰ� �����ؾ߰���.

	Dxa_Head = (WOLF_BIG_HEADER*)Dxa_Buff;
	if ((Dxa_Head->magic != WOLF_MAGIC_64) && (Dxa_Head->magic != DXA_MAGIC_64)) {
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� ������ ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
	}
	//*��� �����͸� ����� �� ������ ����Ű�� �� �� ������ ����

	if(Dxa_Size < ((unsigned int)(Dxa_Head->totalmt_ofs) + Dxa_Head->total_toklen)){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
	}
	//*����� ũ�� ���� ����

	if (((unsigned int)Dxa_Head->end != HEAD_END) && ((unsigned int)Dxa_Head->end != HEAD_END2)) {
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� dxa ������ ����� �������ϴ�."));
		free (Dxa_Buff); return;
	}
	//*��� ���κ� ����

	if(((__int64)(Dxa_Head->total_toklen) <= Dxa_Head->filemt_ofs) || 
		((__int64)(Dxa_Head->total_toklen) <= Dxa_Head->dir_hd_ofs)){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
	}
	//*�� ��Ÿ�����͵鰣�� ������ ���� ����

	Xor_Ofs = (unsigned int)(Dxa_Head->totalmt_ofs)%KEY_SIZE;
	for (__int64 i = Dxa_Head->totalmt_ofs;i < Dxa_Size;i++) {
		Dxa_Buff[i] ^= XORKEY[((i+(KEY_SIZE-Xor_Ofs))%KEY_SIZE)];
	}
	//*xor�� �� ��, �������� ������ �Ǵ� totalmt_ofs�� ���߾� �������� ����ؾ� �Ѵ�
	if (*(unsigned int*)(Dxa_Buff + (unsigned int)(Dxa_Head->totalmt_ofs)) != 0x00000000) {
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : �ش� ������ �������� dxa ������ �ƴմϴ�."));
		free (Dxa_Buff); return;
		//*�̷��� 0�� �ƴ϶�� ����ó���Ѵ�
	}
	//*��Ÿ������ �κ� xor ��α�(����Ʈ ��Ͽ�)
	//***0. ���� ���� ���� ���� ���� �˻�(��Ÿ������ ������ ������ ��Ʈ���� �����ϱ⿡ 0�̾�� �Ѵ�)

	FilesBuff_Size = (unsigned int)((Dxa_Head->totalmt_ofs) - (Dxa_Head->header_size));

	Files_Buff = Dxa_Buff+(unsigned int)(Dxa_Head->header_size);
	//*[���� ����] ũ�� �ľ� �� ����(xor�� ���� ��� �� �Բ� �Ѵ�)
	//*(�ʹ� ũ�� �Ҵ��ϸ� �����ϱ⵵ �ϳ� ����. �׷��� �����ͷμ� �̿��ϰ� �Ѵ�)
	
	FileDirnames_Raw = (unsigned char*)malloc(sizeof(char)*(unsigned int)(Dxa_Head->filemt_ofs));
	memcpy (FileDirnames_Raw, 
		Dxa_Buff+(unsigned int)(Dxa_Head->totalmt_ofs), 
		(unsigned int)(Dxa_Head->filemt_ofs));
	FileDirnames_Collect_Size = Get_File_Count(FileDirnames_Raw, (unsigned int)(Dxa_Head->filemt_ofs));
	if(FileDirnames_Collect_Size == 0xFFFFFFFF){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : ���� �̸� ��ū�� �������ϴ�."));
		free (FileDirnames_Raw); free (Files_Buff); free (Dxa_Buff); return;
	}
	//*[���� �̸�+üũ�� ��ū] ������ �� �ľ��ϱ�

	FileDirnames_Collect = (FILESTR_DATA*)malloc(sizeof(FILESTR_DATA)*FileDirnames_Collect_Size);
	Set_FileNames(FileDirnames_Collect, FileDirnames_Collect_Size, FileDirnames_Raw);
	free(FileDirnames_Raw);
	//*[���� �̸�+üũ�� ��ū] �迭 ���� �� �����ϱ�

	Metadata_Collect_Size = ((unsigned int)((Dxa_Head->dir_hd_ofs) - (Dxa_Head->filemt_ofs))) / sizeof(FILE_BIG_METADATA);
	if(Metadata_Collect_Size * sizeof(FILE_BIG_METADATA) != (unsigned int)((Dxa_Head->dir_hd_ofs) - (Dxa_Head->filemt_ofs))){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : ���� ��Ÿ�����Ͱ� �������ϴ�."));
		for(unsigned int i = 0;i < FileDirnames_Collect_Size;i++){ free (FileDirnames_Collect[i].big_str); free (FileDirnames_Collect[i].real_str); }
		free(FileDirnames_Collect); free (Files_Buff); free (Dxa_Buff); return;
	}
	//*[��Ÿ������] ���� �� �ľ��ϱ�

	Metadata_Collect = (FILE_BIG_METADATA*) malloc(sizeof(FILE_BIG_METADATA) * Metadata_Collect_Size);
	memcpy (Metadata_Collect, 
		Dxa_Buff+(unsigned int)(Dxa_Head->totalmt_ofs + Dxa_Head->filemt_ofs), 
		(sizeof(FILE_BIG_METADATA)*Metadata_Collect_Size));
	//*[��Ÿ������] �迭 �����ϱ�

	Dir_Idx_Collect_Size = (unsigned int)((Dxa_Head->total_toklen) - (Dxa_Head->dir_hd_ofs)) / sizeof(DIR_BIG_IDX);
	if(Dir_Idx_Collect_Size * sizeof(DIR_BIG_IDX) != (unsigned int)((Dxa_Head->total_toklen) - (Dxa_Head->dir_hd_ofs))){
		AfxMessageBox (ExtDlg->Wolf_File_Name + _T(" : ���丮 ������ �������ϴ�."));
		free (Metadata_Collect);
		for(unsigned int i = 0;i < FileDirnames_Collect_Size;i++){ free (FileDirnames_Collect[i].big_str); free (FileDirnames_Collect[i].real_str); }
		free (FileDirnames_Collect); free (Files_Buff); free (Dxa_Buff); return;
	}
	//*[���丮 �ε���] ���� �� �ľ��ϱ�

	Dir_Idx_Collect = (DIR_BIG_IDX*) malloc(sizeof(DIR_BIG_IDX) * Dir_Idx_Collect_Size);
	memcpy (Dir_Idx_Collect,
		Dxa_Buff+(unsigned int)(Dxa_Head->totalmt_ofs + Dxa_Head->dir_hd_ofs), 
		(sizeof(DIR_BIG_IDX)*Dir_Idx_Collect_Size));
	//*[���丮 �ε���] �迭 �����ϱ�

	ParentDir_Name_Len_Buff = (unsigned int*)malloc(sizeof(int) * Dir_Idx_Collect_Size);
	memset (ParentDir_Name_Len_Buff, 0, sizeof(int) * Dir_Idx_Collect_Size);
	//*[�θ� ���丮 �̸� ����] �迭 �ʱ�ȭ

//	free (Dxa_Buff);
	//*���� ���۴� ����Ǹ� �ʿ������ ����� => �������� �����. ������ �ʹ� Ŀ���� �� ���� �о�� ��
	//***1. ����� ������ �������� �� ��Ÿ�����Ϳ� �����Ǵ� ����ü�� ����. ��� ���縸 �� �ϸ� �ȴ�




	
	memset (Root_Dir, 0, MAX_PATH);
	char *tt = U2K (ExtDlg->Wolf_File_Title.GetBuffer());
	for(unsigned int i = 0;i < strlen(tt);i++){ Root_Dir[i] = tt[i]; }
	free(tt);

	Wolf_File_ListName = ExtDlg->Wolf_File_Title + _T(".txt");
	//*�̸� ��Ʈ ���丮, ����Ʈ ���� �̸� �����α�
	
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {
		if (Dir_Idx_Collect[i].parent_dir != (__int64)(-1))  {
			if (Dir_Idx_Collect[i].parent_dir == 0x00) {
				ParentDir_Name_Len_Buff[i] = (unsigned int)strlen(Root_Dir);
				//*�θ� ��Ʈ ���丮��� �׳� ��Ʈ ���̸� �����ָ� �ȴ�
			}
			else{
				Parent_Dir_Idx = (unsigned int)Dir_Idx_Collect[i].parent_dir / sizeof(DIR_BIG_IDX);
				//*[���丮 �ε���] �� �θ� ���丮�� ��ġ
				Parent_Dir_FileDirnames_Pos = Find_FileOrDirName_Idx (FileDirnames_Collect, FileDirnames_Collect_Size,
					(unsigned int)Metadata_Collect[(unsigned int)Dir_Idx_Collect[Parent_Dir_Idx].dirmt_ofs / sizeof(FILE_BIG_METADATA)].fltok_start);
				//*�θ� ���丮�� �����Ǵ� [�̸�+üũ��]�� �ִ� ��(�ε���)

				ParentDir_Name_Len_Buff[i] = ParentDir_Name_Len_Buff[Parent_Dir_Idx];
				//*�θ� ���丮�� [�θ� ���丮 ��� ����] ���ϱ�
				ParentDir_Name_Len_Buff[i]++;
				//*������('\') ���� ���ϱ�
				ParentDir_Name_Len_Buff[i] += (unsigned int)strlen(FileDirnames_Collect[Parent_Dir_FileDirnames_Pos].real_str);
				//*�θ� ���丮�� �����Ǵ� ���ڿ��� ã�� �� ���� ���ϱ�
			}
			//*�θ� ���丮�� ���̸� ���� �����ÿ� �Բ� ���ϱ�
		}//*��Ʈ ���丮�� �ƴ� ��(��Ʈ ���丮�� ���� �̹� 0���� ���õǾ� �����Ƿ� �������)
	}
	//*[�θ� ���丮 �̸� ����] �迭 �����ϱ�
	//*�� ó���� ��Ʈ ���丮���� �˰� �ؾ� �Ѵ�.(���� ��ġ : 0)
	//*�������� ������ ó������ �����ϸ� �������� ���� �ľ��� �����ϴ�

	//***1.5. ���� ���ظ� ����� ����Ʈ ����/[�θ� ���丮�� ��� ����] �迭 ����




	__int64 Total_Cnt = 0, Cnt = 0;
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) { Total_Cnt += Dir_Idx_Collect[i].mt_number; }
	//*���α׷��� �� ���� �غ�

	memset (Upper_Dir, 0, MAX_PATH);
	for (unsigned int i = 0;i < Dir_Idx_Collect_Size;i++) {

		memset(Upper_Dir + ParentDir_Name_Len_Buff[i], MAX_PATH - ParentDir_Name_Len_Buff[i], 0);
		//*���� ���丮 ���� �غ�
		//*��Ȯ���� �θ� ���丮 ��θ� ���ܳ��ƾ� �Ѵ�

		Ofs_Of_DirMeta = 0;
		//*���丮 ���� ���丮�� ���� �������� ���� �ʱ�ȭ

		if (Dir_Idx_Collect[i].parent_dir == (__int64)-1) {
			sprintf_s (Upper_Dir, MAX_PATH, "%s", Root_Dir);
			//*���� ���丮�� ��Ʈ ���丮 ����
		}//*�θ� ���丮�� ������ ��Ʈ ���丮���� �ǹ��Ѵ�
		else {
			Each_Dir_FileDirnames_Pos = Find_FileOrDirName_Idx (FileDirnames_Collect, FileDirnames_Collect_Size,
				(unsigned int)Metadata_Collect[(unsigned int)Dir_Idx_Collect[i].dirmt_ofs / sizeof(FILE_BIG_METADATA)].fltok_start);
			//*�̸� ��ġ�� ���α�

			sprintf_s (Upper_Dir + ParentDir_Name_Len_Buff[i], 
				MAX_PATH - ParentDir_Name_Len_Buff[i], 
				"\\%s", 
				FileDirnames_Collect[Each_Dir_FileDirnames_Pos].real_str);
			//*����� ���� ��ġ������ �ϰ�, �����ñ��� ���ؼ� ��´�
		}//*�θ� ���丮�� �ִٸ� ��Ʈ ���丮�� ���� ���丮�̹Ƿ�, �θ� ���丮�� �ٿ��� ����Ѵ�


		memset (List_String, 0, MAX_PATH);
		sprintf_s (List_String, MAX_PATH, "%s\r\n", Upper_Dir);

		if (code == JAP_CODE) { tmpwstr = J2U (List_String); }
		else if (code == KOR_CODE) { tmpwstr = K2U (List_String); }
		Wolf_File_List += tmpwstr; free (tmpwstr);
		//*���� ���丮 �̸� Ȯ�� �� ����Ʈ�� ���. ������ �������


		if (My_mkdir (Upper_Dir, code) != 0) {
			if (code == JAP_CODE) { tmpwstr = J2U (Upper_Dir); }
			else if (code == KOR_CODE) { tmpwstr = K2U (Upper_Dir); }
			CString t = tmpwstr;
			AfxMessageBox (t + _T(" : ������ ������ ���߽��ϴ�."));
			ExtDlg->m_Extract_Progress.SetPos (0);
			Wolf_File_ListName = _T(""); Wolf_File_List.Empty();
			free (tmpwstr); return;
		}
		//*���� ���丮 �����(������ ���ߴٸ� ����ó��)


		FileInDir_Meta_Start_Pos = (unsigned int)Dir_Idx_Collect[i].mt_offset / sizeof(FILE_BIG_METADATA);
		//*�ش� ���丮�� ������ ���ϵ� ��Ÿ������ �������� ���

		for (unsigned int j = 0;j < (unsigned int)Dir_Idx_Collect[i].mt_number;j++) {

			ExtDlg->m_Extract_Progress.SetPos (PROGRESSVALUE(Cnt++, Total_Cnt));
			//*���α׷��� �� ��ġ ����

			Each_FileDirnames_Pos = Find_FileOrDirName_Idx (FileDirnames_Collect, FileDirnames_Collect_Size,
				(unsigned int)Metadata_Collect[FileInDir_Meta_Start_Pos + j].fltok_start);
			//*���� �ش� ��Ÿ�����Ϳ� �����Ǵ� [����/���丮��+üũ��] ���� �ε����� ��´�

			memset(FileOrDir_In_Dir, MAX_PATH, 0);
			sprintf_s (FileOrDir_In_Dir, MAX_PATH, "%s\\%s",
				Upper_Dir, FileDirnames_Collect[Each_FileDirnames_Pos].real_str);
			//*����/���丮�� Ȯ��

			memset (List_String, 0, MAX_PATH);
			sprintf_s (List_String, MAX_PATH, "\t%s\r\n", FileOrDir_In_Dir);
			if (code == JAP_CODE) { tmpwstr = J2U (List_String); }
			else if (code == KOR_CODE) { tmpwstr = K2U (List_String); }
			Wolf_File_List += tmpwstr; free (tmpwstr);
			//*����Ʈ�� ���(���� �߰�)

			if ((Metadata_Collect [FileInDir_Meta_Start_Pos + j].attr & (__int64)(DIR)) != 0) {

				//*���丮�� �� �� ������ �ʰ�, ���� ���ϼ���ŭ ���� �ѱ�..�ʿ� �������� �𸥴�

			}
			//*���丮�� ��

			else {

				bool IsDir = false;
				//*�������� �׳� �ص� ������, ����Ÿ���� ������ �߰��ؾ� �Ѵ�
				//*�߰��� ���� ������ �׳� �����̶�� ġ��

				if (!IsDir) {


					if (code == KOR_CODE) { tmpwstr = K2U(FileOrDir_In_Dir); }
					else { tmpwstr = J2U(FileOrDir_In_Dir); }

					CString Sst (_T("Extracting (Type 3) : "));
					Sst += tmpwstr;

					ExtDlg->SetDlgItemText (IDC_E_TEXT, Sst);
					//*��� ���ڿ� ����


					Each_File_Size = (unsigned int)Metadata_Collect [FileInDir_Meta_Start_Pos + j].UncompSize;
					//*����ϰ� �� ���� ũ�� ���

					Xor_Ofs = Each_File_Size%KEY_SIZE;
					//*xor ������ ������

					if (Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize != (__int64)-1) {
						Each_File_Buff = (unsigned char*)malloc (sizeof(char) * Each_File_Size);
						memset (Each_File_Buff, 0, Each_File_Size);

						for (unsigned int jj = 0;jj < (unsigned int)Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize;jj++) { 
							(Files_Buff + (unsigned int)Metadata_Collect [FileInDir_Meta_Start_Pos + j].offset)[jj] ^= XORKEY[(jj+Xor_Ofs)%KEY_SIZE]; 
						}
						//*�������� ������ �� xor ���

						memcpy (Each_File_Buff, 
							Files_Buff + (unsigned int)Metadata_Collect [FileInDir_Meta_Start_Pos + j].offset,
							(unsigned int)Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize);
						//*���� Ǯ ���۷� ����

						Decode (Each_File_Buff, Each_File_Size);
						//*�ش� ��ġ�� �� ���� ������ Ǯ���ش�
					}
					//*�����
					else {
						Each_File_Buff = Files_Buff + (unsigned int)Metadata_Collect [FileInDir_Meta_Start_Pos + j].offset;
						//*�ش� ��ġ�� �� �״�� ���� ���� ���� �����͸� ��ġ��Ų��

						for (unsigned int jj = 0;jj < Each_File_Size;jj++) { Each_File_Buff[jj] ^= XORKEY[(jj+Xor_Ofs)%KEY_SIZE]; }
						//*�������� ������ �� xor ���
					}
					//*������
					//*����� ������� ������ �� �ִ�(���� �ÿ��� ����ũ��/�����ũ�Ⱑ �� ���� �ִ�)
					
					hEachFile = CreateFileW (tmpwstr, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (!WriteFile (hEachFile, Each_File_Buff, Each_File_Size, &lp_write, NULL)) {
						CString t = tmpwstr;
						AfxMessageBox (t + _T(" : ������ ������ ���߽��ϴ�."));
						ExtDlg->m_Extract_Progress.SetPos (0);
						Wolf_File_ListName = _T(""); Wolf_File_List.Empty();
						if (Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize != (__int64)-1) { free (Each_File_Buff); }
						CloseHandle (hEachFile); free (tmpwstr); return;
					}
					CloseHandle (hEachFile);
					if (Metadata_Collect [FileInDir_Meta_Start_Pos + j].CompSize != (__int64)-1) { free (Each_File_Buff); }
					//*���� ��� �� �ڵ�, ���� ����(������ ���ߴٸ� ����ó��)
				}

			}
			//*������ ��
			
			//*���丮�� ���� ������ ���� �����Ͽ� �����Ѵ�

		}
		//*���丮 �ε����� ��ϵ� ��Ÿ������ ���� ����ŭ ���� ���鼭 ����/���丮���� ����Ѵ�

		Wolf_File_List += _T("\r\n");
		//*����Ʈ ���� ����

		//*�� ���� ��Ʈ ���丮�� ����Ű��, ���������� ���鼭 ����Ѵ�
	}
	//*�ش� wolf ������ ���� ���丮 ����ŭ �ݺ��Ѵ�

	ExtDlg->m_Extract_Progress.SetPos (100);
	ExtDlg->SetDlgItemText (IDC_E_TEXT, _T("Extract Finished"));
	//***2. ��Ÿ������ ���� ����ü�� �̿��� ���� ���� �� ����Ʈ ���
	//(���� ��İ� ������ : (ROOT)���� wolf�� ���ϸ��� �״�� �ְ�, ���̾� �������� ���丮�鵵 �˴� �ٿ��ִ´�)
	//(�׷��� ���� ���������� ���� ��ŷ�� �� �� �ִ�. �� ���� �ȴٴ� ��.)

	

	
	free (ParentDir_Name_Len_Buff); free (Dir_Idx_Collect); free (Metadata_Collect);
	for(unsigned int i = 0;i < FileDirnames_Collect_Size;i++){ free (FileDirnames_Collect[i].big_str); free (FileDirnames_Collect[i].real_str); }
	free (FileDirnames_Collect); //free (Files_Buff);
	free (Dxa_Buff);
	//***3. ���� ����
}