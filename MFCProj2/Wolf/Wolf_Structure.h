#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <Windows.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#ifndef STRUCTURE
#define STRUCTURE

#define DIR 0x10
#define FILE_DB 0x20
#define DB 0x26	
#define MOV 0x2026						//�� �� ������ ���� ���丮����, ��������, Ȥ�� �����ͺ��̽�(.db)����, �������� �Ǵ��ϴ� ô���� ���� ���̴�.
#define FILESTRMAX_T 0x100

#define NONE_FILE		0xFFFFFFFF		//���� �ε��� ���� �� ������ �ƴ� ���� ��
#define LOADING_FILE	0x7FFFFFFF		//���� ����Ʈ�� �ε� ���� ���� ��

#define WOLF_MAGIC		0x00035844		//this should be the magic header of .wolf file. like 'DX��'
#define DXA_MAGIC		0x00045844		//this should be the magic header of .dxa file. like 'DX��'
#define WOLF_MAGIC_64	0x00065844		//this should be the magic header of .wolf file. like 'DX��'
#define DXA_MAGIC_64	WOLF_MAGIC_64	//this should be the magic header of .dxa file. like 'DX��'
#define HEAD_END		0x3A4			//maybe notifying that this is header end.
#define HEAD_END2		0x3B5			//maybe notifying that this is header end.
#define PADD			0xFFFFFFFF		//��Ÿ������ �� �е���




typedef struct file_info_big{
#pragma pack(push,1)
	unsigned int fltok_start;	//���� ��ū���� �ϳ��� ���� ���̰� �����ϴ� ��ġ������
	unsigned int attr;			//this presents whether it's DIR or FILE.
	time_t generated;			//generated time
	time_t last_modified;		//last modified time
	time_t last_accessed;		//last access time
	unsigned int offset;		//file's offset, if it's directory, this will get value of 0x10's multiple.
	unsigned int UncompSize;	//file's size.
	unsigned int CompSize;		//����� ũ��
#pragma pack(pop)
}FILE_METADATA;
//***32��Ʈ ���� ��Ÿ������

typedef struct file_biginfo{
#pragma pack(push,1)
	__int64 fltok_start;	//���� ��ū���� �ϳ��� ���� ���̰� �����ϴ� ��ġ������
	__int64 attr;			//this presents whether it's DIR or FILE.
	time_t generated;		//generated time
	time_t last_modified;	//last modified time
	time_t last_accessed;	//last access time
	__int64 offset;			//file's offset, if it's directory, this will get value of 0x10's multiple.
	__int64 UncompSize;		//����Ǭ ���� ũ��
	__int64 CompSize;		//����� ���� ũ��, ���� ������ �� �� �����̶�� -1(0xFFFFFFFFFFFFFFFF)�̾�� �Ѵ�
#pragma pack(pop)
}FILE_BIG_METADATA;
//***64��Ʈ ���� ��Ÿ������




typedef struct directory_idx{
#pragma pack(push,1)
	unsigned int dirmt_ofs;		//�ش� ���丮�� ��Ÿ�����Ͱ� ��ġ�� ��(��� ��Ÿ�������� ũ�⸸ŭ ������ ��).
	int parent_dir;				//-1(0xFFFFFFFF) if dir is root directory, 0 if parent is root directory, 0x10, 0x20...means that sub dir's order.
	unsigned int mt_number;		//���丮�� ���� ��Ÿ�������� ����.
	unsigned int mt_offset;		//�ش� ���丮�� �����ϴ� ��Ÿ�����͵��� ��ġ�� ��(��� ��Ÿ�������� ũ�⸸ŭ ������ ��).
#pragma pack(pop)
}DIR_IDX;
//***32��Ʈ ���� ���丮 �ε���

typedef struct directory_idx64{
#pragma pack(push,1)
	__int64 dirmt_ofs;		//�ش� ���丮�� ��Ÿ�����Ͱ� ��ġ�� ��(��� ��Ÿ�������� ũ�⸸ŭ ������ ��).
	__int64 parent_dir;		//-1(0xFFFFFFFF) if dir is root directory, 0 if parent is root directory, 0x10, 0x20...means that sub dir's order.
	__int64 mt_number;		//���丮�� ���� ��Ÿ�������� ����.
	__int64 mt_offset;		//�ش� ���丮�� �����ϴ� ��Ÿ�����͵��� ��ġ�� ��(��� ��Ÿ�������� ũ�⸸ŭ ������ ��).
#pragma pack(pop)
}DIR_BIG_IDX;
//***64��Ʈ ���� ���丮 �ε���




typedef struct filenm{
#pragma pack(push,1)
	unsigned int start;			//���� ��ũ���� ���������� �����ϱ� ���� ���
	unsigned short len;			//(len*4) means least need length to save filename. so if 'sdfs.pl' length is 7 -> len = (8/4) = 2
	unsigned short chksum;		//this should be the sum of each filename alphabet's ascii code. check string is big_str.
	char *big_str;				//this means big alphabet's conversion of filename. ex)"BIGDATA.TXT"
	char *real_str;				//this means real(original) filename. ex)"Bigdata.txt"
#pragma pack(pop)
}FILESTR_DATA;




typedef struct ddrt{
	char file_Str[FILESTRMAX_T];
	unsigned int offset;
}FN_OFS_BUNCH;




typedef struct wolf_head{
#pragma pack(push,1)
	unsigned int magic;			//wolf ������ �������� �ȴ�(0x35844)
	unsigned int total_toklen;	//this should be (DIR_IDX's chunk's len)+(FILESTR_DATA's chunk's len)
	unsigned int header_size;	//Dxa header size.(should be 0x18)
	unsigned int totalmt_ofs;	//this means that start of total metadata(filename's chunk+file metadata's chunk+dir metadata's chunk)
	unsigned int filemt_ofs;	//this means that start of file metadata chunk's. so this is also means filename token's size
	unsigned int dir_hd_ofs;	//this means that start of DIR_IDX's chunks. so this is also (total_toklen - DIR_IDX's chunk lennth)
#pragma pack(pop)
}WOLF_HEADER;
//***32��Ʈ ���� �������� ���

typedef struct wolf_head2{
#pragma pack(push,1)
	unsigned int magic;			//���� wolf ������ �������� �ȴ�(0x65844)
	unsigned int total_toklen;	//this should be (DIR_IDX's chunk's len)+(FILESTR_DATA's chunk's len)
	__int64 header_size;		//Dxa header size.(should be 0x30)
	__int64 totalmt_ofs;		//this means that start of total metadata(filename's chunk+file metadata's chunk+dir metadata's chunk)
	__int64 filemt_ofs;			//this means that start of file metadata chunk's. so this is also means filename token's size
	__int64 dir_hd_ofs;			//this means that start of DIR_IDX's chunks. so this is also (total_toklen - DIR_IDX's chunk lennth)
	__int64 end;				//this should be header end value(0x3A4).
#pragma pack(pop)
}WOLF_BIG_HEADER;
//***64��Ʈ ���� �������� ���




typedef struct Dir_Cnt_String{
#pragma pack(push,1)
	char *Dir_Str;					//���丮�� ���ڿ�
	unsigned int Dir_Meta_Cnt;		//���丮�� ���� [����/���丮]�� ��
	char **FileOrDir_In_Dir_Arr;	//�� ���丮�� ���� [����/���丮]���� ����Ű�� �ϴ� ���ڿ� ������ �迭
#pragma pack(pop)
}FOD_IN_DIR;
//***�ռ� �� �ӽ÷� ���丮�� ���� ��Ÿ������ ���� ���ڿ� ������(���丮��)�� ����� �ʿ䰡 �ֱ⿡ �����ϴ� ����ü




typedef struct text_idx {
	unsigned int Lang_Code;
	unsigned int Offset;
	unsigned int Length;
	unsigned int File_Str_Idx;
	unsigned int Is_OneChar_Enter;
	unsigned int Is_Text_Dirty;
}TEXT_IDX;
//*�� �ؽ�Ʈ�� ��� �ڵ� ����, ������, ���̸� ��ģ ����ü, ���� ���ڿ� �ε���, 
//*0xA������ ������ �ϴ°�, �����ϰ� ������ �ߴ°� �� �ߴ°�
//*���ڿ� ���̴� �����ڵ尡 �ƴ� ���� �ڵ带 �������� ����Ѵ�
//***Dirty �迭 ������ tmptxt�� ������� �ʴ´�
//*���� ���� �� ���� ������ �� ������,�ִ� 8�������� �����ұ� ������. �迭�� ó��

typedef struct file_idx {
	TCHAR File_Name[MAX_PATH];
	TCHAR File_FullPath[MAX_PATH];
	unsigned int Idx_of_Text_Start;
	unsigned int Text_Cnt;
	unsigned int Is_FileTxt_Dirty;
	TEXT_IDX *Text_Idx_Arr;
}FILE_TXT_ELEM;
//*�� ���� ������ ����ü(���� �̸�, ��ü ���� ���, �ؽ�Ʈ ����Ʈ�� ���� �ε���, �ؽ�Ʈ ����ü�� ��, 
//*                  �ؽ�Ʈ ����ü ������(�Ҵ��), ������ �ؽ�Ʈ �����Ͱ� �ϳ��� ���� ���ΰ�)
//***Dirty �迭 ������ tmptxt�� ������� �ʴ´�

typedef struct dir_file_txt {
	unsigned int File_Num;
	FILE_TXT_ELEM *File_Idx_Arr;
	unsigned int Is_Dirty;
}DIR_TXT_IDX;
//*�� ���丮 ���� �ִ� ��� �ؽ�Ʈ�� �Ѱ��ϴ� ����ü.
//***Dirty �迭 ������ tmptxt�� ������� �ʴ´�

#endif