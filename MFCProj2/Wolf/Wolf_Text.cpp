#include "stdafx.h"

#include "Wolf_Text.h"


extern CListCtrl *m_G_ListCtrl_Files;
extern CListCtrl *m_G_ListCtrl_Text;
extern DIR_TXT_IDX m_Text_Idx_In_Dir;
//*extern���� ����Ʈ ������ ���� 

extern TCHAR *Prefix_Text_Str[];
extern unsigned int Prefix_Length;
//*Prefix�� ���ڿ�

extern TCHAR *T_Head_Start;
extern TCHAR *T_Head_End;
//*�ؽ�Ʈ ����� ���ڿ�

extern TCHAR *T_Head_Dirty;
extern unsigned int Dirty_Prefix_Length;
//*Dirty Prefix ���ڿ�

extern wchar_t N_table[];
//*�ݰ�->�������� ������ ���̺�

unsigned int Total_Cnt_File_Num;
unsigned int Cnt_File_Num;
unsigned int Cnt_Text_Num;
//*���� �ε��� �� �ؽ�Ʈ �ε����� ���������� ī��Ʈ�Ѵ�

wchar_t *JapHanja;
wchar_t *KorHanja;
unsigned int HanjaCnt;
//*�Ϻ����� <-> �ѱ����� ��ȯ�� ����, �׸��� ũ��


//*Game.dat�� ���⼭ �������� �ʴ´�.
//*�ٸ� �Լ��� ����� �ű⼭ ���������� �����ϵ��� ��.


inline void Lower (CString *str) 
{
	wchar_t *tt = str->GetBuffer();
	for (int i = 0;i < str->GetLength();i++) { 
		if ((tt[i] >= 'A') && (tt[i] <= 'Z')) { tt[i] += ('a' - 'A'); } 
		else if (tt[i] == '\\') { tt[i] = '/'; }
	}
}
//*���ڿ� �ҹ��ڷ� ���߱�, '\'�� '/'�� �ϰ� �����Ѵ�


void Load_Text_From_Directory_Data (Load_Text_Dialog *LoadTtxDlg, CString DirPath, unsigned int code, bool IsRoot)
{
	WIN32_FIND_DATAW FData;
	HANDLE hFind;
	//*���丮 ���ٿ� �ڵ�

	HANDLE hEachFile;
	unsigned char *EachFileBuff;
	unsigned int EachFileSize;
	DWORD lp_read;
	//*�� ���� ���ٿ� �ڵ�, ����

	CString FileName_Str;
	CString Cnt_File_Num_Str;
	CString File_FullPath;
	//*���� �̸��� ���� ���ڿ��� ��ü��θ� ���� ���ڿ��� �ȴ�
	
	CString Text_Str;
	wchar_t *tmpWTStr;
	//*�ӽ� �ؽ�Ʈ ���ڿ�



	if (IsRoot) { 

		LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Load Text From Files..."));

		Cnt_File_Num = Cnt_Text_Num = 0; 
		//*�ε����� �� ī���� �ʱ�ȭ

		Total_Cnt_File_Num = Get_Total_FileCount (DirPath, _T("Game.dat"));
		//*�� ���� �� ���ϱ�. //*���⼭�� Game.dat�� �����Ѵ�

		m_Text_Idx_In_Dir.File_Num = Total_Cnt_File_Num;
		m_Text_Idx_In_Dir.File_Idx_Arr = (FILE_TXT_ELEM*)malloc (sizeof(FILE_TXT_ELEM) * Total_Cnt_File_Num);
		memset (m_Text_Idx_In_Dir.File_Idx_Arr, 0, sizeof(FILE_TXT_ELEM) * Total_Cnt_File_Num);
		//*m_Text_Idx_In_Dir �Ҵ� �� �ʱ�ȭ

	}
	//*�� ó�� ȣ������ �� ����Ʈ ���� ī���� �ʱ�ȭ.
	//*�ؽ�Ʈ �ε� �Լ� (��ü ���Ͽ� ���Ͽ�. �ؽ�Ʈ�� �ִ� ������ ���� ǥ��)
	//*�켱������ [���丮��].tmptxt�� �ε��Ѵ�. �ٸ� �ش� ���丮�� ������ �ε��� �� ����,
	//*���丮�� �־ tmptxt ���� �ȿ� ����� ������ ���� ������ �׳� ���丮 ������ ������� �ε��Ѵ�(���޽���).
	//*�ϴ� ���丮�� �����ϰ�, �׿� �����ϴ� [���丮��].tmptxt ������ ������ �̰� �켱������ �ε��Ѵ�.



	CString DirPath_ast = DirPath + _T("/*");
	hFind = FindFirstFileW (DirPath_ast.GetBuffer(), &FData); 
	if (hFind == INVALID_HANDLE_VALUE) {
		AfxMessageBox (DirPath + _T(" : �ش� ����/������ �����ϴ�."));
		return;
	}
	//*�ڵ� üũ



	do {

		File_FullPath = DirPath + _T("/") + FData.cFileName;
		//*��ü��� ���

		if (lstrcmp(FData.cFileName, L"Game.dat") == 0){;}
		//*Game.dat ����

		else if(!(FData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			&& (lstrcmp(FData.cFileName, L".") != 0) 
			&& (lstrcmp(FData.cFileName, L"..") != 0))
		{
			FileName_Str = FData.cFileName;
			//*���� ���ڿ� ���

			Cnt_File_Num_Str.Format(_T("%d"), Cnt_File_Num);
			m_G_ListCtrl_Files->InsertItem (Cnt_File_Num, Cnt_File_Num_Str + _T(" : ") + FileName_Str);
			//***���ڿ� ����Ʈ�� �ֱ�***

			hEachFile = CreateFileW (File_FullPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			EachFileSize = GetFileSize (hEachFile, NULL);
			EachFileBuff = (unsigned char*)malloc (sizeof(char) * EachFileSize);
			if (!ReadFile (hEachFile, EachFileBuff, EachFileSize, &lp_read, NULL)){
				AfxMessageBox (File_FullPath + _T(" : ������ ���� ���߽��ϴ�."));
				LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Load Failed..."));
				All_List_Item_Delete(); All_Text_Data_Reset(); return;
				//*����Ʈ ���� ���� �� DIR_TXT_IDX ���� �ʱ�ȭ
			}
			CloseHandle (hEachFile);
			//*���� ����, ���� �Ҵ�, �б� �� ���� �ݱ�

			memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].File_Name, FileName_Str.GetBuffer(), sizeof(TCHAR) * FileName_Str.GetLength());
			memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].File_FullPath, File_FullPath.GetBuffer(), sizeof(TCHAR) * File_FullPath.GetLength());
			//*���� �̸�, ���� ��ü��� ����, �� �ؽ�Ʈ�� �̸� ����ڵ� �ʱ�ȭ.


			if ((wcsstr (FData.cFileName, L".common") != NULL)
				|| (wcsstr (FData.cFileName, L".dat") != NULL)
				|| (wcsstr (FData.cFileName, L".dbtype") != NULL)
				|| (wcsstr (FData.cFileName, L".mps") != NULL)
				|| (wcsstr (FData.cFileName, L".project") != NULL)
				|| (wcsstr (FData.cFileName, L".tile") != NULL)) {

				m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt = Get_Text_Count_Or_Set_Text (EachFileBuff, EachFileSize, code, NULL);
				//*�ؽ�Ʈ ���� ���� ���ϱ�
				if (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt != 0){
					m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr = 
						(TEXT_IDX*)malloc (sizeof(TEXT_IDX) * m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt);
					memset (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr, 0, 
						sizeof(TEXT_IDX) * m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt);
					memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].File_Name, FileName_Str.GetBuffer(), sizeof(TCHAR) * FileName_Str.GetLength());
					memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].File_FullPath, File_FullPath.GetBuffer(), sizeof(TCHAR) * File_FullPath.GetLength());
					for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt;i++) 
					{ m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr[i].Lang_Code = code; }
					Get_Text_Count_Or_Set_Text (EachFileBuff, EachFileSize, code, &m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num]);
					//*�ؽ�Ʈ�� 0�� �ƴ϶�� �ؽ�Ʈ �迭�� �Ҵ��ϰ� �ؽ�Ʈ ����, �ڵ常 �̸� �����صΰ� ����
				}
				//*�ؽ�Ʈ ���� ������� m_Text_Idx_In_Dir ������ ������ ����
			}
			else { m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt = 0; }
			//*�ؽ�Ʈ�� ���� ���� ���ϵ鸸 ��� üũ�Ѵ�. �������� �� �ؽ�Ʈ ���� �ɷ� ���


			if (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt != 0) {
				Text_Str = T_Head_Start; Text_Str += FileName_Str; Text_Str += T_Head_End;
				m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Idx_of_Text_Start = Cnt_Text_Num;
				m_G_ListCtrl_Text->InsertItem (Cnt_Text_Num++, Text_Str);
			}
			//***��� �ؽ�Ʈ ���(�ؽ�Ʈ ���� 0�� �ƴ� ����), �߰��� �ε����� ����Ѵ�***

			for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Cnt;i++) {

				if (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr[i].Lang_Code == JAP_CODE) {
					tmpWTStr = J2U ((char*)(EachFileBuff + m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr[i].Offset + sizeof(int)));
				}
				else if (m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr[i].Lang_Code == KOR_CODE) {
					tmpWTStr = K2U ((char*)(EachFileBuff + m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr[i].Offset + sizeof(int)));
				}
				//*���ڿ� ��ȯ

				Text_Str.Format(_T("%s[%06d]"), Prefix_Text_Str[m_Text_Idx_In_Dir.File_Idx_Arr[Cnt_File_Num].Text_Idx_Arr[i].Lang_Code], i);
				Text_Str += tmpWTStr;
				//*���ڿ� Ȯ��

				m_G_ListCtrl_Text->InsertItem (Cnt_Text_Num++, Text_Str);
				free (tmpWTStr);
			}
			//***�ؽ�Ʈ ����Ʈ�� �״�� ���(����ڵ带 ������.)***
			//***�׳� code ������ ���� �ʰ� [���丮��].tmptxt ���ϰ��� ȣȯ���� ����� ������ �����Ѵ�)***

			free (EachFileBuff);
			//*���� ���� ����

			Cnt_File_Num++;
			LoadTtxDlg->m_Load_Text_Progress.SetPos (PROGRESSVALUE(Cnt_File_Num, Total_Cnt_File_Num*2));
			//*�ؽ�Ʈ �ε� �� ���α׷��� �� ����
			//***[���丮��].tmptxt ������ ������ File_Str_Idx ���� ������ �� �����Ƿ� �׳� 1��� �����ϸ� �ȴ�***

		}
		//*���丮�� �ƴ϶��(Game.dat ����) Ȯ���غ��� �ε��Ѵ�

		else if((lstrcmp(FData.cFileName, L".") != 0) && (lstrcmp(FData.cFileName, L"..") != 0)) {
			Load_Text_From_Directory_Data (LoadTtxDlg, File_FullPath, code, false);
		}
		//*���� ���丮���(����, ���� ���丮 ����) ����Լ� ȣ��
		//***[���丮].tmptxt ������ ���� ���� ���ȣ���Ѵ�

	} while (FindNextFileW (hFind, &FData));
	//*���丮 ���� ��� ���� �� ���ƺ���
	//***[���丮].tmptxt ������ ���� ���� ���ȣ���Ѵ�. ������ �� ���� �� ���� ���Ƶ� ��.

	FindClose (hFind);
	//*�ڵ� �ݾ��ֱ�



	if (IsRoot) {

		LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Check Filenames..."));

		CString fstr, tstr;

		for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
			if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt != 0) {
				for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {

					tstr = m_G_ListCtrl_Text->GetItemText (j + m_Text_Idx_In_Dir.File_Idx_Arr[i].Idx_of_Text_Start + 1, 0); Lower (&tstr);
					//*��ҹ��� ������ ���ְ� ���Ѵ�

					m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].File_Str_Idx = NONE_FILE;
					for (unsigned int k = 0;k < m_Text_Idx_In_Dir.File_Num;k++) {

						fstr = m_Text_Idx_In_Dir.File_Idx_Arr[k].File_FullPath; Lower (&fstr);
						fstr = fstr.Right (fstr.GetLength() - DirPath.GetLength() - 1);
						//*��ҹ��� ������ ���ְ� ���Ѵ�(+1�� ������ ������)
						//*���� ���� ���ڿ��� �ƴ� ��쵵 ������ �׳� ����θ� ���ͼ� ���ϴ� �� ����

						if (wcsstr(&tstr.GetBuffer()[Prefix_Length], fstr.GetBuffer()) != 0) {
							m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].File_Str_Idx = k; 
							break;
						}
						//*Prefix�� ���ڿ��� �����ϰ� ���Ѵ�
					}

					if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].File_Str_Idx == NONE_FILE) {
						unsigned int T_List_Idx = Get_List_Index (i, j);
						Text_Str = m_G_ListCtrl_Text->GetItemText (T_List_Idx, 0);
						TCHAR *pt = Text_Str.GetBuffer();
						for (int j = 0;j < Text_Str.GetLength();j++) {
							if (pt[j] == 0x7E26) { pt[j] = 0x7E31; }
							if (pt[j] == 0x6A2A) { pt[j] = 0x6A6B; }
						}
						m_G_ListCtrl_Text->SetItemText (T_List_Idx, 0, Text_Str);
						//*"��Ⱦ"���ڴ� ������ �����⵵ �ϰ�, �������� ���� ���� �� ���� ������
						//*�׳� ���ڿ��̶�� �̸� �ٲ�δ� �� ���� �� ����
					}

					LoadTtxDlg->m_Load_Text_Progress.SetPos (50 + 
						PROGRESSVALUE(j + m_Text_Idx_In_Dir.File_Idx_Arr[i].Idx_of_Text_Start + 2, m_G_ListCtrl_Text->GetItemCount()*2));

				}
			}
		}
		//*�ؽ�Ʈ ������ ���ϸ�/���丮���� ��� (���� ����Ʈ ���� �� ��ü��� ���ڿ� �� ������ �ؽ�Ʈ ������ ã�� ���)
		//*���� �÷��׸� �������ش� (���丮���� �� �ڿ� '/'�� �ٴ´�)
		//***�ϴ� ���丮 ���� ���߿� ������ ����� �մ��� ����***

		LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Saving Tmptxt File..."));

		Save_Tmptxt (DirPath);
		//*tmptxt ������ ���� �� ������� �״� �״�� ����

		LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Text Loading Finished."));

	}

}


int Save_Text_To_Directory_Data (Save_Text_Dialog *SaveTxtDlg, CString DirPath)
{

	if (!m_Text_Idx_In_Dir.Is_Dirty) { return 0; }
	//*����� �ؽ�Ʈ�� ���� ���� ����, ������ �ƴϴϱ� 0�� ��ȯ�Ѵ�

	HANDLE hEachFile;
	unsigned char *org_buff, *chg_buff, *org_pnt, *chg_pnt;
	unsigned int *org_ofs;
	unsigned int org_size, chg_size;
	//*���ϱ�Ͽ� �ʿ��� ������

	int T_List_Idx, Text_Gap;
	//*�ؽ�Ʈ ����Ʈ ���Ҹ� ��� �� �ʿ��� ����, ���� �ؽ�Ʈ�� ������ �ؽ�Ʈ ���� ����

	CString tmpCStr, tmpNStr;
	char *tmpStr;
	unsigned int code, offset;
	//*������ ���ϴ�
	
	bool check = false;
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {

		if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Is_FileTxt_Dirty) {

			tmpCStr.Format(_T("Saving Text to File...(%d/%d)"), i, m_Text_Idx_In_Dir.File_Num);
			SaveTxtDlg->SetDlgItemText (IDC_ST_TEXT, tmpCStr);
			//*�ؽ�Ʈ ���

			hEachFile = CreateFileW (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, 
				GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hEachFile == INVALID_HANDLE_VALUE) {
				tmpNStr = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath;
				JPHan_2_KRHan (&tmpNStr);
				memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, tmpNStr.GetBuffer(), sizeof(TCHAR)*tmpNStr.GetLength());
				tmpNStr = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name;
				JPHan_2_KRHan (&tmpNStr);
				memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name, tmpNStr.GetBuffer(), sizeof(TCHAR)*tmpNStr.GetLength());
				hEachFile = CreateFileW (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, 
					GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hEachFile == INVALID_HANDLE_VALUE) {
					tmpCStr.Format (_T("%s : �ش� ������ �����ϴ�."), m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath);
					AfxMessageBox (tmpCStr);
					return -1;
				}
				//*������ �𸣰����� ���⼭ ���� �� �ٲ�� ���׳��µ�
			}
			org_size = chg_size = GetFileSize (hEachFile, NULL);
			org_buff = (unsigned char*)malloc (sizeof(char) * org_size);
			ReadFile (hEachFile, org_buff, org_size, NULL, NULL);
			CloseHandle (hEachFile);
			//*�� ���Ϲ���, ���Ϲ��� ���� �ʱ�ȭ, ���� ������ �� ���� -1 ��ȯ

			T_List_Idx = Get_List_Index (i, 0);
			//*�ش� ������ ù ���ڿ��� �ִ� �ε���
			for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {

				if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty) {

					tmpCStr = m_G_ListCtrl_Text->GetItemText((T_List_Idx+j), 0);
					tmpCStr = tmpCStr.Right (tmpCStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
					code = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Lang_Code;
					if (code == KOR_CODE) { tmpStr = U2K (tmpCStr.GetBuffer()); }
					else if (code == JAP_CODE) { tmpStr = U2J (tmpCStr.GetBuffer()); }
					//*�ڵ忡 ���� ansi ���ڿ� ���, prefix, Dirty ��� ���ڿ� ���

					Text_Gap = ((int)strlen(tmpStr)+1) - (int)m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Length;
					if (tmpStr != NULL) { free (tmpStr); }
					//*���� ���ڿ��� �ٲ� ���ڿ� ������ ���� �� ���� ����

					if (Text_Gap != 0) { chg_size = (unsigned int)((int)chg_size + Text_Gap); }
					//*�� ���� ũ�⿡ �ؽ�Ʈ ������ �ݿ�

				}

			}
			//*���� ������ �ؽ�Ʈ�� �ٲٰ� �Ҵ�� ũ�� �̸� ���(���� Dirty�� �ؽ�Ʈ�� ����ϸ� �ȴ�)
			//*���� ���ڿ��� ������ �� �� ������ �翬�� Dirty�� ���°� ���� �ʴ´�

			chg_buff = (unsigned char*)malloc (sizeof(char) * chg_size);
			memset (chg_buff, 0, (sizeof(char) * chg_size));
			//*�ٲ� ����� ������ �Ҵ�

			org_pnt = org_buff; chg_pnt = chg_buff;
			//*������ �ʱ�ȭ

			org_ofs = (unsigned int*)malloc (sizeof(int) * m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt);
			for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {
				org_ofs[j] = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Offset;
			}
			//*������ �������� �̸� �����س��´�

			T_List_Idx = Get_List_Index (i, 0);
			//*�ش� ������ ù ���ڿ��� �ִ� �ε���
			for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {

				if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty) {

					tmpCStr = m_G_ListCtrl_Text->GetItemText((T_List_Idx+j), 0);
					tmpCStr = tmpCStr.Right (tmpCStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
					code = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Lang_Code;
					if (code == KOR_CODE) { tmpStr = U2K (tmpCStr.GetBuffer()); }
					else if (code == JAP_CODE) { tmpStr = U2J (tmpCStr.GetBuffer()); }
					//*�ڵ忡 ���� ansi ���ڿ� ���
					
					offset = org_ofs[j];
					memcpy (chg_pnt, org_pnt, ((org_buff + offset) - org_pnt));
					//*�������� �������� ���� ���̸�ŭ �� ���ۿ� ����
					
					chg_pnt += ((org_buff + offset) - org_pnt);
					org_pnt = (org_buff + offset);
					//*������, ���� ����, �� ���� ������ ����

					*(unsigned int*)chg_pnt = ((unsigned int)strlen(tmpStr) + 1); chg_pnt += sizeof(int);
					memcpy (chg_pnt, tmpStr, (unsigned int)strlen(tmpStr)); chg_pnt += ((unsigned int)strlen(tmpStr) + 1);
					org_pnt += sizeof(int);
					org_pnt += m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Length;
					//*�� ���ۿ� �ٲ� �ؽ�Ʈ �̽�, ���� ������ ����
					
					tmpNStr.Format (_T("%s[%06d]%s"), Prefix_Text_Str[code], j, tmpCStr.GetBuffer());
					m_G_ListCtrl_Text->SetItemText ((T_List_Idx+j), 0, tmpNStr);
					//*�ؽ�Ʈ ����Ʈ�� �ٲ� �ؽ�Ʈ �̽�(Dirty ��� ���ڿ� ����)

					m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty = false;
					Text_Gap = ((int)strlen(tmpStr)+1) - (int)m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Length;
					m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Length = (unsigned int)(strlen(tmpStr)+1);
					if (tmpStr != NULL) { free (tmpStr); }
					//*�ؽ�Ʈ ��Ƽ ���� ���ֱ�, ���� ����, �ؽ�Ʈ ����

					if (Text_Gap != 0) { 
						for (unsigned int k = j+1;k < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;k++) {
							m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[k].Offset = 
								(unsigned int)((int)m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[k].Offset + Text_Gap);
						}
					}
					//*����� ���̿� ���� �������� ���� ����(Text_Gap)�� �̿��� ���Ŀ� �ִ� ������ �� ����

				}

			}

			if (org_pnt < (org_buff + org_size)) { 
				memcpy (chg_pnt, org_pnt, ((org_buff + org_size) - org_pnt)); 
			}
			//*������ ���� ��� ���� �ű��
			
			_wremove (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath);
			hEachFile = CreateFileW (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, 
				GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			WriteFile (hEachFile, chg_buff, chg_size, NULL, NULL);
			CloseHandle (hEachFile);
			//*���� ���� ����� ������ ���Ϲ��۷� ����

			free (chg_buff); free (org_buff); free (org_ofs);
			//*���� ���Ϲ���, ������ ���Ϲ��� ����, ������ ���� ����, �����ڵ� ����

			int FH_List_Idx = Get_List_Index (i, 0) - 1;
			tmpCStr = m_G_ListCtrl_Text->GetItemText(FH_List_Idx, 0);
			tmpCStr = tmpCStr.Right (tmpCStr.GetLength() - Dirty_Prefix_Length);
			m_G_ListCtrl_Text->SetItemText (FH_List_Idx, 0, tmpCStr);
			//*���� �ؽ�Ʈ ������� ��Ƽ ��� ����

			m_Text_Idx_In_Dir.File_Idx_Arr[i].Is_FileTxt_Dirty = false;
			//*���� ��Ƽ ���� ���ֱ�

		}
		//*����� �ؽ�Ʈ�� �ִ� ���ϸ� �մ�

		SaveTxtDlg->m_Save_Text_Progress.SetPos (PROGRESSVALUE(i, m_Text_Idx_In_Dir.File_Num));
		//*���α׷��� �� ����

	}

	m_Text_Idx_In_Dir.Is_Dirty = false;
	//*Dirty ���� ����

	Save_Tmptxt (DirPath);
	//*[���丮��].tmptxt�� ���� ���� ����

	SaveTxtDlg->m_Save_Text_Progress.SetPos (100);
	//*���α׷��� �� ����

	return 0;
}


	
//*���� ���ڿ� ���� �� �����ϴ� �Լ�, �ҷ��� ���� �߿� �״�� �����ϸ� �ߺ��Ǵ� ���� ���ڿ��� �ֳ� �����ϴ� �Լ�,
//*�׿� ���� ���ϸ��� �ٲ��ִ� �Լ��� �־�� ��. �� ��� ������ tmptxt�� ���ŵǾ� ����ȴ�.
//*���ϸ��� �ٲ��ִ� �Լ������� �ؽ�Ʈ ����Ʈ ������ ���ϸ� �α׸� �ٲ۴�.
//*���� ���ڿ��� ���⼭ �ٷ��� ���� �״� �̰� �ٸ� ���� ������






int Load_Tmptxt (Load_Text_Dialog *LoadTtxDlg, CString DirPath)
{
	HANDLE hTmpTxt;
	unsigned char *TmpTxtBuff, *TmpTxtOrg, *TmpTxtPnt;
	unsigned int TmpTxtSize, TmpOrgSize;
	//*tmptxt ������ ���� ������

	HANDLE hEachFile;
	unsigned char *EachFileBuff;
	unsigned int EachFileSize;
	//*�� ������ ���� ������

	wchar_t *tpstr;
	CString Text_In_File, Text_In_TmpTxt;
	//*�񱳿� ���ڿ�



	LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Load Text From Files..."));

	CString TmpTxtFile = DirPath + _T(".tmptxt");
	hTmpTxt = CreateFileW (TmpTxtFile.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hTmpTxt == INVALID_HANDLE_VALUE) { return -1; }
	//*���� ã��

	TmpTxtSize = GetFileSize (hTmpTxt, NULL);
	TmpTxtBuff = (unsigned char*)malloc (sizeof(char) * TmpTxtSize);
	ReadFile (hTmpTxt, TmpTxtBuff, TmpTxtSize, NULL, NULL);
	CloseHandle (hTmpTxt);
	//*���� �ε� �� �ڵ� �ݱ�

	TmpOrgSize = *(unsigned int*)TmpTxtBuff;
	TmpTxtOrg = (unsigned char*)malloc (sizeof(char) * TmpOrgSize);
	memset (TmpTxtOrg, 0, (sizeof(char) * TmpOrgSize));
	memcpy (TmpTxtOrg, TmpTxtBuff, TmpTxtSize);
	free (TmpTxtBuff);
	//*������ ���۷� �����ϰ� ������

	Decode (TmpTxtOrg, TmpOrgSize);
	TmpTxtPnt = TmpTxtOrg;
	//*���� ���� ���̷� ���� ����
	
	unsigned int File_Num;
	//*DIR_TXT_IDX�� ����

	TCHAR *File_Name;
	TCHAR *File_FullPath;
	unsigned int Idx_of_Text_Start;
	unsigned int Text_Cnt;
	//*FILE_TXT_ELEM�� ������
	
	unsigned int Lang_Code;
	unsigned int Offset;
	unsigned int Length;
	unsigned int File_Str_Idx;
	unsigned int Is_OneChar_Enter;
	TCHAR *Text, *Text_bak;
	//*TEXT_IDX�� ������� �����ڵ� �ؽ�Ʈ ������

	CString Text_Str;
	unsigned int F_List_Cnt_Idx, T_List_Cnt_Idx, Total_Text_Num;
	//*����Ʈ�� �߰��� ���ڿ�
	//*���� ����Ʈ �ε��� ��ȣ, �ؽ�Ʈ ����Ʈ �ε��� ��ȣ, ��ü �ؽ�Ʈ ��

	char *atpstr;
	//*������ ���� ����



	All_List_Item_Delete();
	All_Text_Data_Reset();
	F_List_Cnt_Idx = T_List_Cnt_Idx = 0;
	//*����Ʈ �� DIR_TXT_IDX ���� �ʱ�ȭ

	File_Num = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
	Total_Text_Num = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);

	m_Text_Idx_In_Dir.File_Num = File_Num;
	m_Text_Idx_In_Dir.File_Idx_Arr = (FILE_TXT_ELEM*)malloc (sizeof(FILE_TXT_ELEM) * File_Num);
	memset (m_Text_Idx_In_Dir.File_Idx_Arr, 0, (sizeof(FILE_TXT_ELEM) * File_Num));
	//*���� �� Ȯ�� �� �Ҵ�/�ʱ�ȭ
	
	bool loadTmpTxt = false;
	for (unsigned int i = 0;i < File_Num;i++) {

		if (TmpTxtPnt > (TmpTxtOrg + TmpOrgSize)) { 
			AfxMessageBox (_T("1 : tmptxt ������ �ùٸ��� �ʽ��ϴ�.\n�������� ���� �ҷ��ɴϴ�."));
			free (TmpTxtOrg); All_List_Item_Delete(); All_Text_Data_Reset(); return -1;
		}

		File_Name = (TCHAR*)TmpTxtPnt; TmpTxtPnt += (sizeof(TCHAR)*MAX_PATH);
		File_FullPath = (TCHAR*)TmpTxtPnt; TmpTxtPnt += (sizeof(TCHAR)*MAX_PATH);
		Idx_of_Text_Start = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
		Text_Cnt = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
		//*���� ��������

		memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name, File_Name, (sizeof(TCHAR)*MAX_PATH));
		memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, File_FullPath, (sizeof(TCHAR)*MAX_PATH));
		m_Text_Idx_In_Dir.File_Idx_Arr[i].Idx_of_Text_Start = Idx_of_Text_Start;
		m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt = Text_Cnt;
		m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr = (TEXT_IDX*)malloc (sizeof(TEXT_IDX) * Text_Cnt);
		memset (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr, 0, (sizeof(TEXT_IDX) * Text_Cnt));
		//*���� �Ҵ�/�ʱ�ȭ

		TCHAR *t;
		if (((t = _tcsstr (File_FullPath, L"Data/")) != NULL) || ((t = _tcsstr (File_FullPath, L"Data\\")) != NULL)) {
			TCHAR newFullPath [MAX_PATH]; memset (newFullPath, 0, sizeof(TCHAR)*MAX_PATH);
			GetFullPathName (t, MAX_PATH, newFullPath, NULL);
			File_FullPath = newFullPath;
			memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, File_FullPath, (sizeof(TCHAR)*MAX_PATH));
		}
		//*��ΰ� �ٲ������ File_FullPath�� �ٲ�ģ��

		hEachFile = CreateFileW (File_FullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hEachFile == INVALID_HANDLE_VALUE) {
			AfxMessageBox (File_FullPath);
			AfxMessageBox (_T("2 : tmptxt ������ �ùٸ��� �ʽ��ϴ�.\n�������� ���� �ҷ��ɴϴ�."));
			free (TmpTxtOrg); All_List_Item_Delete(); All_Text_Data_Reset(); return -1;
		}
		EachFileSize = GetFileSize (hEachFile, NULL);
		EachFileBuff = (unsigned char*)malloc (sizeof(char) * EachFileSize);
		ReadFile (hEachFile, EachFileBuff, EachFileSize, NULL, NULL);
		CloseHandle (hEachFile);
		//*���� ���� �ҷ����� �ڵ� �ݱ�, �� �θ��� ���� �ɷ� ó��

		Text_Str.Format (_T("%d : "), F_List_Cnt_Idx); Text_Str += File_Name;
		m_G_ListCtrl_Files->InsertItem (F_List_Cnt_Idx++, Text_Str);
		//***���ڿ��� ����� ���� ����Ʈ�� �߰��ϱ�

		if (Text_Cnt != 0){
			Text_Str = T_Head_Start; Text_Str += File_Name; Text_Str += T_Head_End;
			m_G_ListCtrl_Text->InsertItem (T_List_Cnt_Idx++, Text_Str);
			//***���ڿ��� ����� �ؽ�Ʈ ����Ʈ�� �߰��ϱ�(�̰� �ؽ�Ʈ ���� ���� ��)
		}

		for (unsigned int j = 0;j < Text_Cnt;j++) {

			if (TmpTxtPnt > (TmpTxtOrg + TmpOrgSize)) { 
				AfxMessageBox (_T("3 : tmptxt ������ �ùٸ��� �ʽ��ϴ�.\n�������� ���� �ҷ��ɴϴ�."));
				free (TmpTxtOrg); All_List_Item_Delete(); All_Text_Data_Reset(); 
				free (EachFileBuff); return -1;
			}

			Lang_Code = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
			Offset = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
			Length = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
			File_Str_Idx = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
			Is_OneChar_Enter = *(unsigned int*)TmpTxtPnt; TmpTxtPnt += sizeof(int);
			Text_bak = Text = (TCHAR*)TmpTxtPnt; TmpTxtPnt += (sizeof(TCHAR) * (wcslen(Text)+1));
			//*���� ��������, �ؽ�Ʈ�� (�����ڵ� ���ڿ�+NULL)�� �Ǿ� �ִ� ����

			Text_Str.Format(_T("%s[%06d]"), Prefix_Text_Str[Lang_Code], j);
			if (Lang_Code == KOR_CODE) { 
				atpstr = U2K (Text); Text = K2U (atpstr); free (atpstr);
				tpstr = K2U ((char*)(EachFileBuff + Offset + sizeof(int))); 
			}
			else if (Lang_Code == JAP_CODE) { 
				atpstr = U2J (Text); Text = J2U (atpstr); free (atpstr);
				tpstr = J2U ((char*)(EachFileBuff + Offset + sizeof(int))); 
			}
			Text_In_TmpTxt = Text; free (Text);
			Text_In_File = tpstr; free (tpstr);
			if (Text_In_TmpTxt.CompareNoCase (Text_In_File) != 0) {
				if (!loadTmpTxt) {
					AfxMessageBox (_T("4 : ������ tmptxt ���� ���� �ؽ�Ʈ�� �ٸ��ϴ�.\ntmptxt ������ �ؽ�Ʈ�� �ҷ��ɴϴ�."));
					Text_In_File = Text_In_TmpTxt;
					loadTmpTxt = true;
				}
				else { Text_In_TmpTxt = Text_In_File; }
				//*** ���⼭ ��/�� ���ڰ� �޶� �������� ��찡 �ִµ�
				//*** ��Ȯ���� tmptxt���� ���� �� ����ǰ� ������ �������� ���� ���
				//*** �׷��ٸ� �����ؽ�Ʈ�� �����Ͽ� ���ϴ� �� ģ��
				//*** �׷��� �ε��ؾ߰ڴٸ� tmptxt �ؽ�Ʈ�� �ҷ��´ٴ�
			}
			//*���ڿ� ��(��ҹ��ڴ� ������ �ʴ´�)
			//***���ߺ�ȯ�ϴ� ������ ���Ͽ� ������ �� Ȥ���� ������ ���ڿ����� �����ϱ� �����̴�
			//***�̿ܿ� �߰��� ���� �� ������ (���� ���ڿ� ���� ������) �� �߰��ϸ� �ȴ�
			
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Lang_Code = Lang_Code;
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Offset = Offset;
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Length = Length;
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].File_Str_Idx = File_Str_Idx;
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_OneChar_Enter = Is_OneChar_Enter;
			//*��Ȯ�ϴ� ������ ���� �Ҵ�/�ʱ�ȭ

			Text_In_TmpTxt = Text_bak;
			Text_Str += Text_In_TmpTxt;
			m_G_ListCtrl_Text->InsertItem (T_List_Cnt_Idx++, Text_Str);
			//***���ڿ��� ����� �ؽ�Ʈ ����Ʈ�� �߰��ϱ�(�̶��� ���� �� ���� ���ڿ��� �߰�)

			LoadTtxDlg->m_Load_Text_Progress.SetPos (PROGRESSVALUE(T_List_Cnt_Idx, Total_Text_Num));
			//*���α׷��� �� ����

		}

		free (EachFileBuff);
		//*���� ���� �����ϱ�

	}
	//*���� ������ ���� DIR_TXT_IDX ����ü�� üũ �� ����

	free (TmpTxtOrg);

	LoadTtxDlg->SetDlgItemText (IDC_LT_TEXT, _T("Loading Progress : Text Loading Finished."));

	return 0;
}
//*tmptxt ������ �ε��� DIR_TXT_IDX�� ����ֱ�, ����Ʈ�� ��±��� ����


//*tmptxt ���Ͽ� ����� ���� : 
//*�� ������ ��, �� �ؽ�Ʈ ��, �� ������ ����ŭ FILE_TXT_ELEM ����ü (TEXT_IDX�� ũ�� * �ؽ�Ʈ ���� ũ�⸦ ���� Text_Idx_Arr ����)
//*FILE_TXT_ELEM ����ü�� TEXT_IDX�� Dirty ������ �����Ѵ�


void Save_Tmptxt (CString DirPath)
{
	HANDLE hTmpTxt;
	unsigned char *TmpTxtBuff, *TmpTxtComp, *TmpTxtPnt;
	unsigned int TmpTxtSize = 0, TmpcompSize;
	//*tmptxt ������ ���� ������

	unsigned int List_Idx, List_Accum;
	CString List_Text;
	//*����Ʈ ��ġ�� ����ϰ� �ؽ�Ʈ�� ��� ���� �ʿ��� ����


	TmpTxtSize += sizeof(int);											//*File_Num �߰�
	TmpTxtSize += sizeof(int);											//*�� �ؽ�Ʈ �� �߰�
	
	List_Accum = 0;
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {

		TmpTxtSize += (sizeof(TCHAR)*MAX_PATH);							//*File_Name �߰�
		TmpTxtSize += (sizeof(TCHAR)*MAX_PATH);							//*File_FullPath �߰�
		TmpTxtSize += sizeof(int);										//*Idx_of_Text_Start �߰�
		TmpTxtSize += sizeof(int);										//*Text_Cnt �߰�
		//***�̰� 0�̳� �ƴϳĿ� ���� ���� �ʿ䰡 ���� �� ����***

		for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {

			TmpTxtSize += sizeof(int);									//*Lang_Code �߰�
			TmpTxtSize += sizeof(int);									//*Offset �߰�
			TmpTxtSize += sizeof(int);									//*Length �߰�
			TmpTxtSize += sizeof(int);									//*File_Str_Idx �߰�
			TmpTxtSize += sizeof(int);									//*Is_OneChar_Enter �߰�

			List_Idx = List_Accum + 1 + j;								//*����Ʈ ��ġ�� ���
			List_Text = m_G_ListCtrl_Text->GetItemText (List_Idx, 0);	
			List_Text = List_Text.Right ((unsigned int)List_Text.GetLength() - Prefix_Length);
			//*�ؽ�Ʈ ������, Prefix�� ���ڿ��� �����Ѵ�

			TmpTxtSize += (sizeof(TCHAR) * ((unsigned int)List_Text.GetLength()+1));	//*�ؽ�Ʈ �߰�

		} 

		if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt != 0) 
		{ List_Accum += (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt + 1); }
		//*�������� �߰�

	}
	//*����� ũ�� ����ϱ�


	TmpTxtBuff = (unsigned char*)malloc (sizeof(char) * TmpTxtSize);
	memset (TmpTxtBuff, 0, TmpTxtSize); TmpTxtPnt = TmpTxtBuff;
	//*���� �Ҵ� �� �ʱ�ȭ

	*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Num; 
	TmpTxtPnt += sizeof(int);
	//*File_Num �߰�

	unsigned int textnum = m_G_ListCtrl_Text->GetItemCount(); 
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) 
	{ if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt != 0) { textnum--; } }
	*(unsigned int*)TmpTxtPnt = textnum; 
	TmpTxtPnt += sizeof(int);
	//*�� �ؽ�Ʈ �� �߰�(����Ʈ ��ü ���ҿ��� �ؽ�Ʈ ���� 0�� �ƴ� ������鸸 ���� ��)
	
	List_Accum = 0;
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {

		memcpy (TmpTxtPnt, m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name, (sizeof(TCHAR)*MAX_PATH));
		TmpTxtPnt += (sizeof(TCHAR)*MAX_PATH);
		//*File_Name �߰�
		memcpy (TmpTxtPnt, m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, (sizeof(TCHAR)*MAX_PATH));
		TmpTxtPnt += (sizeof(TCHAR)*MAX_PATH);
		//*File_FullPath �߰�
		*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Idx_of_Text_Start; 
		TmpTxtPnt += sizeof(int);
		//*Idx_of_Text_Start �߰�
		*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt; 
		TmpTxtPnt += sizeof(int);
		//*Text_Cnt �߰�
		
		for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {
			*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Lang_Code; 
			TmpTxtPnt += sizeof(int);
			//*Lang_Code �߰�
			*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Offset; 
			TmpTxtPnt += sizeof(int);
			//*Offset �߰�
			*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Length; 
			TmpTxtPnt += sizeof(int);
			//*Length �߰�
			*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].File_Str_Idx; 
			TmpTxtPnt += sizeof(int);
			//*File_Str_Idx �߰�
			*(unsigned int*)TmpTxtPnt = m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_OneChar_Enter; 
			TmpTxtPnt += sizeof(int);
			//*Is_OneChar_Enter �߰�

			List_Idx = List_Accum + 1 + j;								//*����Ʈ ��ġ�� ���
			List_Text = m_G_ListCtrl_Text->GetItemText (List_Idx, 0);
			List_Text = List_Text.Right ((unsigned int)List_Text.GetLength() - Prefix_Length);
			//*�ؽ�Ʈ ������, Prefix�� ���ڿ��� �����Ѵ�

			memcpy (TmpTxtPnt, List_Text.GetBuffer(), (sizeof(TCHAR) * List_Text.GetLength()));
			TmpTxtPnt += (sizeof(TCHAR) * ((unsigned int)List_Text.GetLength()+1));
			//*�ؽ�Ʈ �߰�
		}

		if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt != 0) 
		{ List_Accum += (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt + 1); }
		//*�������� �߰�
	}
	//*���������� ���


	Encode (TmpTxtBuff, TmpTxtSize);
	TmpTxtComp = TmpTxtBuff;
	TmpcompSize = (*(unsigned int*)(&TmpTxtBuff[4]));
	//*���� ����
	
	CString TmpTxtFile = DirPath + _T(".tmptxt");
	hTmpTxt = CreateFileW (TmpTxtFile.GetBuffer(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile (hTmpTxt, TmpTxtComp, TmpcompSize, NULL, NULL);
	free (TmpTxtBuff); CloseHandle (hTmpTxt);
	//*���� ���

	//***Save_Text_To_Directory_Data �Լ����� ȣ���� ���� ���� �ؽ�Ʈ�� ���Ͽ� �����ϰ� �� �Լ��� ȣ���Ѵ�
	//***�׷��� �����°� ���̸� ��Ȯ�� ����� �� �ְ� ���ڿ��� ��Ȯ�� ���� �� �ִ�
}








void Narrow_to_Em()
{
	
	int T_List_Idx;
	bool Is_Narrow;
	CString tmpWStr, _tmp, apply_Txt;
	wchar_t tm_str[2];


	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {

		T_List_Idx = Get_List_Index (i, 0);
		for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {

			if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].File_Str_Idx != NONE_FILE) { continue; }
			//*���� ���ڿ��� �ƴ� ���� �ǵ帮��� �Ѵ�

			tmpWStr = m_G_ListCtrl_Text->GetItemText ((T_List_Idx + j), 0);
			if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty) {
				tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
			}
			else {
				tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length);
			}
			//*Dirty ����� ������ ���� �ؽ�Ʈ ���

			Is_Narrow = false;
			for (unsigned int k = 0;k < (unsigned int)tmpWStr.GetLength();k++) {
				if ((tmpWStr[k] >= 0xFF61) && (tmpWStr[k] <= 0xFF9F)) { Is_Narrow = true; break; }
			}
			//*�ݰ� ���ڰ� �ִ��� üũ, �ݰ� ���ڰ� ���� ���� �ݿ��ϸ� �ȴ�

			if (Is_Narrow) {

				_tmp = _T("");
				for (unsigned int k = 0;k < (unsigned int)tmpWStr.GetLength();k++) {

					tm_str[0] = tm_str[1] = 0;

					if (BETWEEN(tmpWStr[k],0xFF61,0xFF9F)) {
						if (tmpWStr[k] == 0xFF9E) {
							if (_tmp.GetLength() == 0) { tm_str[0] = 0x3F; }
							else if (BETWEEN((_tmp.GetBuffer()[_tmp.GetLength()-1]),0x30AB,0x30C8) 
								|| BETWEEN((_tmp.GetBuffer()[_tmp.GetLength()-1]),0x30CF,0x30DB)) {
								if (BETWEEN(tmpWStr[k-1],0xFF61,0xFF9F)) { _tmp.GetBuffer()[_tmp.GetLength()-1] += 1; }
								else { tm_str[0] = 0x3F; }
							}
							else if (_tmp.GetBuffer()[_tmp.GetLength()-1] == 0x30A6) { _tmp.GetBuffer()[_tmp.GetLength()-1] = 0x30F4; }
							else { tm_str[0] = 0x3F; }
							//*1��° ����, ���� ���� ���ڰ� �ݰ��� �ƴϾ����� ����ǥ�� �ٲپ� �ִ´�
						}
						else if (tmpWStr[k] == 0xFF9F) {
							if (_tmp.GetLength() == 0) { tm_str[0] = 0x3F; }
							else if (BETWEEN((_tmp.GetBuffer()[_tmp.GetLength()-1]),0x30CF,0x30DB)) {
								if (BETWEEN(tmpWStr[k-1],0xFF61,0xFF9F)) { _tmp.GetBuffer()[_tmp.GetLength()-1] += 2; }
								else { tm_str[0] = 0x3F; }
							}
							else { tm_str[0] = 0x3F; }
							//*2��° ����, ���� ���� ���ڰ� �ݰ��� �ƴϾ����� ����ǥ�� �ٲپ� �ִ´�
						}
						else {
							tm_str[0] = N_table[tmpWStr[k]-0xFF61];
							//*�Ϲ� ����
						}
						//*�����Ͽ� �߰�, Ź�� ���� �� ������ ������ �����ϸ� �ȴ�
					}
					else { tm_str[0] = tmpWStr[k]; } //*�׳� �߰�
					//*�ݰ� -> ����, ������ �״�� �߰�

					_tmp += tm_str;
					//*���ڿ� ���ϱ�
				}
				//*�ݰ� ���ڸ� �������� �ٲ� �� ���ڿ� �����

				m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty = true;
				//*�ڵ� �ٲٱ�, Dirty ���� ����
				apply_Txt.Format (_T("%s[%06d]%s%s"), 
					Prefix_Text_Str[m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Lang_Code], 
					j, T_Head_Dirty, _tmp);
				m_G_ListCtrl_Text->SetItemText ((T_List_Idx + j), 0, apply_Txt);
				//*Dirty ���, ����� �ڵ� ����� ����� �ؽ�Ʈ�� ����ֱ�

				if (!m_Text_Idx_In_Dir.File_Idx_Arr[i].Is_FileTxt_Dirty) {
					apply_Txt = T_Head_Dirty; apply_Txt += m_G_ListCtrl_Text->GetItemText ((T_List_Idx - 1), 0);
					m_G_ListCtrl_Text->SetItemText ((T_List_Idx - 1), 0, apply_Txt);
					m_Text_Idx_In_Dir.File_Idx_Arr[i].Is_FileTxt_Dirty = true;
				}
				//*�ݰ� ���ڰ� �ִٸ� ���� ����� Dirty ���·� �ٲٱ�
				if (!m_Text_Idx_In_Dir.Is_Dirty) {
					m_Text_Idx_In_Dir.Is_Dirty = true;
				}
				//*����, �������� Dirty ���� ����
			}
			//*�ݰ� �ؽ�Ʈ�� ������ �翬�� ���� Dirty�� �����Ѵ�

		}

	}
	//*�ݰ� �Ϻ��� �����ڵ带 ���� �Ϻ��� �����ڵ�� �ٲٱ�

}








void Check_Filename_For_Codepage (Check_FDName_Dialog *ChkDlg, unsigned int code)
{

	CString Root_Dir;
	CString WStr_Name, WStr_Name_chg, WStr_FullPath, WStr_FullPath_bak;
	CString chg_Name;
	CString cc, cc_t;
	TCHAR **Dir_List;
	unsigned int Dir_Num;
	//*���丮 ����� ���� �� �ʿ��� ����

	char *tmpCStr; 
	wchar_t *tmpWStr;
	//*���丮 ���ڿ� �񱳿� �ʿ��� ����

	unsigned int fidx, tidx;
	CString List_str;
	//*����Ʈ �ݿ�/ȹ�� ����

	Root_Dir = _T("");
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
		if (Root_Dir.Compare(_T("")) == 0) { Root_Dir = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath; }
		else {
			while (wcsstr(m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, Root_Dir.GetBuffer()) == 0) {
				Root_Dir = Root_Dir.Left (Root_Dir.GetLength() - 1);
			}
		}
	}
	//*���� ��ü�� File_FullPath���� ��Ʈ ���丮 �߷�����(�������� ��ġ�ϴ� �κи� ã�Ƴ��� �ȴ�)

	Dir_Num = Get_Dir_Num_Or_DirList (Root_Dir.Left(Root_Dir.GetLength()-1), NULL);
	Dir_List = (TCHAR**)malloc (sizeof(TCHAR*) * Dir_Num);
	for (unsigned int i = 0;i < Dir_Num;i++) { 
		Dir_List[i] = (TCHAR*)malloc(sizeof(TCHAR) * MAX_PATH); 
		memset (Dir_List[i], 0, (sizeof(TCHAR) * MAX_PATH));
	}
	Get_Dir_Num_Or_DirList (Root_Dir.Left(Root_Dir.GetLength()-1), Dir_List);
	//*���丮 ����Ʈ �ʱ�ȭ, ��Ʈ ���丮 �ؿ� �ִ� ���丮 ����Ʈ ���
	//*�����ð� �տ� �پ������� �����ø� ���� �־�� ���������� ���ڿ��� ���� �� �ִ�

	unsigned int Total_Task = Dir_Num + m_Text_Idx_In_Dir.File_Num;
	
	for (unsigned int i = 0;i < Dir_Num;i++) {

		WStr_Name = Dir_List[i];
		WStr_Name = WStr_Name.Right (WStr_Name.GetLength() - Root_Dir.GetLength());

		cc.Format (_T("Check Folder Name : %s"), WStr_Name);
		ChkDlg->SetDlgItemText(IDC_CHK_FD_TEXT, cc);
		ChkDlg->m_Check_FDName_Progress.SetPos (PROGRESSVALUE(i, Total_Task));
		//*�ؽ�Ʈ ���� �� ���α׷��� �� ����
		
		cc = WStr_Name;
		if (code == KOR_CODE) { JPHan_2_KRHan (&cc); }
		//***�̸� �ణ ���� �� �α�� �Ѵ�

		if (code == JAP_CODE) { tmpCStr = U2J(cc.GetBuffer()); tmpWStr = J2U(tmpCStr); }
		else if (code == KOR_CODE) { tmpCStr = U2K(cc.GetBuffer()); tmpWStr = K2U(tmpCStr); }
		WStr_Name_chg = tmpWStr; free (tmpCStr); free (tmpWStr);
		//*���ڿ� ��ȯ���� �� ȣȯ �������� üũ

		if (WStr_Name != WStr_Name_chg) {

			Change_FDName_Dialog dlg;

			if (cc == WStr_Name_chg) { goto FOLDER_LABEL_2; }
			//*�̶��� ���̾�α� ȣ�� ���� �״�� �Ѿ�� �ȴ�

			WStr_FullPath.Format (_T("%s : \n�ڵ����������� �������� �ν��� �� �����ϴ�.\n������ ���ڿ��� �ٲټ���."), 
				WStr_Name);
			AfxMessageBox (WStr_FullPath);
			//*�ٲ� ���ڿ��� ���� ���ڿ��� �ٸ��� �̸��� �ٲٶ�� �޽����ڽ��� �ϳ� ����

			dlg.Input_WStr = WStr_Name;
			dlg.Output_WStr = WStr_Name_chg;
			dlg.code = code;
			dlg.Meta_type = DIR;
			dlg.ShouldChange = true;
			//*���̾�α� ȣ�� �� �غ�

FOLDER_LABEL:
			if (dlg.DoModal() == IDOK) {

				WStr_Name_chg = dlg.Output_WStr;

FOLDER_LABEL_2:
				chg_Name = Root_Dir + WStr_Name_chg;
				//*���丮 �̸� �ٲ� �غ�
				
				for (unsigned int j = 0;j < Dir_Num;j++) {
					if ((j != i) && (dlg.Output_WStr.Compare(Dir_List[j]) == 0)) {
						AfxMessageBox (_T("�������� �ߺ��˴ϴ�.\n�ٽ� �Է��ϼ���."));
						goto FOLDER_LABEL;
					}
				}
				//***���� �����ϴ� �������� �ߺ� üũ�� �ؾ� �Ѵ�

				Dir_List[i][wcslen(Dir_List[i]) - 1] = 0;
				chg_Name = chg_Name.Left (chg_Name.GetLength() - 1);

				if (FD_Rename (Dir_List[i], chg_Name.GetBuffer()) == -1) {
					AfxMessageBox (_T("���� ���� ���� �����ǰų�\n���� �� ������ ������Դϴ�."));
					for (unsigned int i = 0;i < Dir_Num;i++) { free (Dir_List[i]); } free (Dir_List);
					return;
					//*��θ� �ٲ��� ���ϸ� �׳� �����Ѵ�
					//*�̶��� ��� ������ �����ð��� ������ ����
				}
				else { 

					Dir_List[i][wcslen(Dir_List[i])] = '/';
					chg_Name += _T("/");
					
					cc = Dir_List[i];
					memset (Dir_List[i], 0, (sizeof(TCHAR) * MAX_PATH));
					memcpy (Dir_List[i], chg_Name.GetBuffer(), (sizeof(TCHAR) * chg_Name.GetLength()));
					for (unsigned int j = 0;j < Dir_Num;j++) {
						WStr_FullPath_bak = WStr_FullPath = Dir_List[j];
						WStr_FullPath.Replace (cc, chg_Name);
						if (WStr_FullPath_bak != WStr_FullPath) {
							memset (Dir_List[j], 0, (sizeof(TCHAR) * MAX_PATH));
							memcpy (Dir_List[j], WStr_FullPath.GetBuffer(), 
								(sizeof(TCHAR) * WStr_FullPath.GetLength()));
						}
					}
					//*Dir_List�� ���� �ٲ��ֱ�

					for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Num;j++) {

						WStr_FullPath = m_Text_Idx_In_Dir.File_Idx_Arr[j].File_FullPath;
						WStr_FullPath = WStr_FullPath.Right (WStr_FullPath.GetLength() - Root_Dir.GetLength());
						WStr_FullPath_bak = WStr_FullPath;
						WStr_FullPath.Replace (WStr_Name, WStr_Name_chg);
						//*WStr_FullPath�� Data ���� ���� ����θ� �ǹ��Ѵ�

						if (WStr_FullPath_bak != WStr_FullPath) {

							memset (&m_Text_Idx_In_Dir.File_Idx_Arr[j].File_FullPath[Root_Dir.GetLength()], 
								0, (sizeof(TCHAR) * (MAX_PATH - Root_Dir.GetLength())));
							memcpy (&m_Text_Idx_In_Dir.File_Idx_Arr[j].File_FullPath[Root_Dir.GetLength()], 
								WStr_FullPath.GetBuffer(), (sizeof(TCHAR) * WStr_FullPath.GetLength()));
							//*File_FullPath�� ���� ���

							for (unsigned int k = 0;k < (unsigned int)m_G_ListCtrl_Text->GetItemCount();k++) {
								Get_Fidx_Tidx (k, &fidx, &tidx);
								if ((tidx != 0) && (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].File_Str_Idx == j)) {

									List_str = m_G_ListCtrl_Text->GetItemText (k, 0);
									List_str.Replace (WStr_FullPath_bak, WStr_FullPath);
									if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Is_Text_Dirty) {
										m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Is_Text_Dirty = true;
										List_str.Insert(Prefix_Length, T_Head_Dirty);
									}
									m_G_ListCtrl_Text->SetItemText (k, 0, List_str);
									//*�ؽ�Ʈ �ٲ��ֱ�, Dirty ���� ���

									if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
										m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
										List_str = m_G_ListCtrl_Text->GetItemText (Get_List_Index(fidx,0)-1, 0);
										List_str.Insert (0, T_Head_Dirty);
										m_G_ListCtrl_Text->SetItemText (Get_List_Index(fidx,0)-1, 0, List_str);
									}
									//*���� ��� �ٲ��ֱ�, Dirty ���� ���

									if (!m_Text_Idx_In_Dir.Is_Dirty) { m_Text_Idx_In_Dir.Is_Dirty = true; }
									//*��ü Dirty ����

								}
							}
							//*�׸��� �ؽ�Ʈ �� �ش� ������ �ε����� ��� �ִٸ� �� ���� �ٲ��ش�
						}

					}
					//*���� �� ��� File_FullPath ���ڿ��� ������� �˻��Ͽ�, 
					//*��ü ��θ� �ٲ� �ʿ䰡 �ִٸ� �ٲٰ� �ٽ� ������ش�
					//*���� �ؽ�Ʈ ����Ʈ ���� ���ڿ� �߿��� �ش� ������ �ε����� �����ٸ� ���� ��ε� �ٲ�� �Ѵ�

				}
				//*��� ���� ������
			}

			//*�ٲ� ���ڿ��� ���, ���ڿ��� �κ� ��η� ���� ������ File_FullPath,
			//*��� �ؽ�Ʈ���� ���� ���ڿ��� ������ ���� �ؽ�Ʈ�� �α׸� �ٲ������

		}
		//*��ȯ�� ������ ���ڿ��� ���� ���ڿ� ��

	}
	//***���丮 üũ(File_FullPath �ٲٱ�)


	for (unsigned int i = 0;i < (unsigned int)m_G_ListCtrl_Text->GetItemCount();i++) {

		Get_Fidx_Tidx (i, &fidx, &tidx);
					
		if ((tidx > 0) && (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].File_Str_Idx != NONE_FILE)
			&& (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Lang_Code != code)) {

			cc = m_G_ListCtrl_Text->GetItemText (i, 0);
			if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Is_Text_Dirty) {
				cc = cc.Right (cc.GetLength() - Prefix_Length - Dirty_Prefix_Length);
			}
			else {
				cc = cc.Right (cc.GetLength() - Prefix_Length);
			}
			//*Dirty ����� ������ ���� �ؽ�Ʈ ���

			m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Lang_Code = code;
			m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Is_Text_Dirty = true;
			//*�ڵ� �ٲٱ�, Dirty ���� ����

			cc_t.Format (_T("%s[%06d]%s%s"), Prefix_Text_Str[code], tidx-1, T_Head_Dirty, cc.GetBuffer());
			m_G_ListCtrl_Text->SetItemText (i, 0, cc_t);
			//*Dirty ���, ����� �ڵ� ����� ����� �ؽ�Ʈ�� ����ֱ�

			if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
				m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
				List_str = m_G_ListCtrl_Text->GetItemText (Get_List_Index(fidx,0)-1, 0);
				List_str.Insert (0, T_Head_Dirty);
				m_G_ListCtrl_Text->SetItemText (Get_List_Index(fidx,0)-1, 0, List_str);
			}
			//*���� ��� �ٲ��ֱ�, Dirty ���� ���

			if (!m_Text_Idx_In_Dir.Is_Dirty) { m_Text_Idx_In_Dir.Is_Dirty = true; }
			//*��ü Dirty ����
		}

	}
	//***�ڵ�� ��ġ���� ���� ��� ���� �� �ٲ��ֱ�
	

	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {

		cc.Format (_T("Check File Name : %s"), m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name);
		ChkDlg->SetDlgItemText(IDC_CHK_FD_TEXT, cc);
		ChkDlg->m_Check_FDName_Progress.SetPos (PROGRESSVALUE((i+Dir_Num), Total_Task));
		//*�ؽ�Ʈ ���� �� ���α׷��� �� ����

		WStr_Name = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name;
		
		cc = WStr_Name; 
		if (code == KOR_CODE) { JPHan_2_KRHan (&cc); }
		//***�̸� �ణ ���� �� �α�� �Ѵ�

		if (code == JAP_CODE) { tmpCStr = U2J(cc.GetBuffer()); tmpWStr = J2U(tmpCStr); }
		else if (code == KOR_CODE) { tmpCStr = U2K(cc.GetBuffer()); tmpWStr = K2U(tmpCStr); }
		WStr_Name_chg = tmpWStr; free (tmpCStr); free (tmpWStr);
		//*���ڿ� ��ȯ���� �� ȣȯ �������� üũ

		if (WStr_Name != WStr_Name_chg) {

			Change_FDName_Dialog dlg;

			if (cc == WStr_Name_chg) { goto FILE_LABEL_2; }
			//*�̶��� ���̾�α� ȣ�� ���� �״�� �Ѿ�� �ȴ�

			WStr_FullPath.Format (_T("%s : \n�ڵ����������� ���ϸ��� �ν��� �� �����ϴ�.\n������ ���ڿ��� �ٲټ���."), 
				WStr_Name);
			AfxMessageBox (WStr_FullPath);
			//*�ٲ� ���ڿ��� ���� ���ڿ��� �ٸ��� �̸��� �ٲٶ�� �޽����ڽ��� �ϳ� ����

			dlg.Input_WStr = WStr_Name;
			dlg.Output_WStr = WStr_Name_chg;
			dlg.code = code;
			dlg.Meta_type = FILE_DB;
			dlg.ShouldChange = true;
			//*���̾�α� ȣ�� �� �غ�

FILE_LABEL:
			if (dlg.DoModal() == IDOK) {
				
				WStr_Name_chg = dlg.Output_WStr;
				
FILE_LABEL_2:
				cc = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath;
				cc = cc.Right (cc.GetLength() - Root_Dir.GetLength());
				cc.Replace (WStr_Name, WStr_Name_chg);
				for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Num;j++) {
					WStr_FullPath = m_Text_Idx_In_Dir.File_Idx_Arr[j].File_FullPath;
					WStr_FullPath = WStr_FullPath.Right (WStr_FullPath.GetLength() - Root_Dir.GetLength());
					if ((j != i) && (cc == WStr_FullPath)) {
						AfxMessageBox (_T("���ϸ��� �ߺ��˴ϴ�.\n�ٽ� �Է��ϼ���."));
						goto FILE_LABEL;
					}
				}
				//***���� �����ϴ� ���ϸ��� �ߺ� üũ�� �ؾ� �Ѵ�, �̰� ��ü ��� �����̶� �� ������

				cc = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath;
				cc = cc.Left (cc.GetLength() - (unsigned int)wcslen(m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name));
				cc += WStr_Name_chg;
				if (code == KOR_CODE) { JPHan_2_KRHan (&WStr_Name_chg); JPHan_2_KRHan (&cc); }
				FD_Rename (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, cc.GetBuffer());
				//***���ϸ� �ٲ��ֱ�

				memset (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name, 0, MAX_PATH);
				memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_Name, WStr_Name_chg.GetBuffer(), 
					(sizeof(TCHAR) * WStr_Name_chg.GetLength()));
				memset (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, 0, MAX_PATH);
				if (i == 458) { 
					int s = 0; 
				}
				memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, cc.GetBuffer(), 
					(sizeof(TCHAR) * cc.GetLength()));
				//***���� �ٲ��ֱ�

				CString sfsf; sfsf.Format (_T("%d : %s"), i, WStr_Name_chg);
				m_G_ListCtrl_Files->SetItemText (i, 0, sfsf);
				//*���� ����Ʈ�� ��� ���� ���ֱ�

				for (unsigned int j = 0;j < (unsigned int)m_G_ListCtrl_Text->GetItemCount();j++) {

					Get_Fidx_Tidx (j, &fidx, &tidx);

					if ((tidx != 0) && (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].File_Str_Idx == i)) {

						List_str = m_G_ListCtrl_Text->GetItemText (j, 0);
						List_str.Replace (WStr_Name, WStr_Name_chg);
						if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Is_Text_Dirty) {
							m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx-1].Is_Text_Dirty = true;
							List_str.Insert(Prefix_Length, T_Head_Dirty);
						}
						m_G_ListCtrl_Text->SetItemText (j, 0, List_str);
						//*�ؽ�Ʈ �ٲ��ֱ�, Dirty ���� ���

						if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
							m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
							List_str = m_G_ListCtrl_Text->GetItemText (Get_List_Index(fidx,0)-1, 0);
							List_str.Insert (0, T_Head_Dirty);
							m_G_ListCtrl_Text->SetItemText (Get_List_Index(fidx,0)-1, 0, List_str);
						}
						//*���� ��� �ٲ��ֱ�, Dirty ���� ���

						if (!m_Text_Idx_In_Dir.Is_Dirty) { m_Text_Idx_In_Dir.Is_Dirty = true; }
						//*��ü Dirty ����

					}

				}
				//*���� ���ڿ� �� �ش� ������ �ε����� ���� �ִ� ���� �α׸� �ٲٱ�

			}

		}
		//*��ȯ�� ������ ���ڿ��� ���� ���ڿ� ��

	}
	//***���� ���� üũ(Filename_Str�� �ٲٱ�)

	for (unsigned int i = 0;i < Dir_Num;i++) { free (Dir_List[i]); } free (Dir_List);
	//*���丮 ����Ʈ ���� ����

	ChkDlg->SetDlgItemText(IDC_CHK_FD_TEXT, _T("Check File/Directory Finished."));
	ChkDlg->m_Check_FDName_Progress.SetPos (100);
	//*������ �ؽ�Ʈ �� ���α׷��� �� ����

}








void Load_Hanja()
{
	JapHanja = KorHanja = NULL;
	HanjaCnt = 0;
	//*�̸� �ʱ�ȭ�ϱ�

	HANDLE hHanja = CreateFileW (L"Hanja.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hHanja == INVALID_HANDLE_VALUE) {
		MessageBoxW (NULL, L"Hanja.txt ������ �����ϴ�.\n�� ������ ������ ���� �̸��� �������� �����ؾ� �մϴ�.", L"Alert", MB_OK);
		return;
	}

	unsigned int HanjaListSize = (GetFileSize(hHanja, NULL)/sizeof(wchar_t)) - 1;
	wchar_t *buff = (wchar_t*)malloc ((HanjaListSize+1) * sizeof(wchar_t));
	memset (buff, 0, ((HanjaListSize+1) * sizeof(wchar_t)));
	SetFilePointer (hHanja, sizeof(wchar_t), NULL, FILE_BEGIN);
	ReadFile (hHanja, buff, (HanjaListSize * sizeof(wchar_t)), NULL, NULL);
	CloseHandle (hHanja);
	//*���� �о���̱�

	for (unsigned int i = 0;i < HanjaListSize - 3;i++) {
		if ((buff[i] == '<') && (buff[i+1] == '-') && (buff[i+2] == '>')) { HanjaCnt++; }
	}
	//*�ε��ϱ�

	KorHanja = (wchar_t*)malloc (sizeof(wchar_t) * HanjaCnt);
	memset (KorHanja, 0, (sizeof(wchar_t) * HanjaCnt));
	JapHanja = (wchar_t*)malloc (sizeof(wchar_t) * HanjaCnt);
	memset (JapHanja, 0, (sizeof(wchar_t) * HanjaCnt));
	//*�Ҵ� �� �ʱ�ȭ

	unsigned int buffpntidx = 0;
	for (unsigned int i = 0;i < HanjaCnt;i++) {
		while (!((buff[buffpntidx] == '<') && (buff[buffpntidx+1] == '-') && (buff[buffpntidx+2] == '>'))) { buffpntidx++; }
		JapHanja[i] = buff[buffpntidx - 2]; KorHanja[i] = buff[buffpntidx + 4];
		buffpntidx++;
	}
	//*���ۿ��� ���� �Ѹ���

	free (buff);
	//*���� ����
}


void Release_Hanja ()
{
	if (JapHanja != NULL) { free (JapHanja); }
	if (KorHanja != NULL) { free (KorHanja); }
}


void JPHan_2_KRHan (CString *str)
{
	for (unsigned int j = 0;j < (unsigned int)str->GetLength();j++) {
		if (str->GetBuffer()[j] == 0x30FC){ str->GetBuffer()[j] = '-'; }
		else if (str->GetBuffer()[j] == 0x3005){ 
			if (j > 0) { str->GetBuffer()[j] = str->GetBuffer()[j-1]; } 
			else { str->GetBuffer()[j] = '-'; }
		}
		else {
			for (unsigned int k = 0;k < HanjaCnt;k++) {
				if (str->GetBuffer()[j] == JapHanja[k]){ str->GetBuffer()[j] = KorHanja[k]; break; }
			}
			//*��Ī�Ǵ� ���ڸ� ã�Ƽ� �ٲ�ġ��
		}
	}
}
//*�ٲ� �� �ִ� ���ڴ� ��� �ѱ����ڷ� �ٲٴ� �Լ�