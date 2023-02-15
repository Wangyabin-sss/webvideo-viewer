

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

// Recursive��1��ʾ�ݹ���ң������ֻ�г�����Ŀ¼ 
int ListDirectory(char* Path, int Recursive)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
    char FileName[MAX_PATH] = { 0 };
    int Ret = -1;

    strcpy(FileName, Path);
    strcat(FileName, "/");
    strcat(FileName, "*.*");

    // ���ҵ�һ���ļ�
    hFind = FindFirstFile(FileName, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error when list %s\n", Path);
        return Ret;
    }
    do
    {
        // �����ļ���
        strcpy(FileName, Path);
        strcat(FileName, "/");
        strcat(FileName, FindFileData.cFileName);
        printf("%s\n", FileName);

        // ����ǵݹ���ң������ļ�������.��..�������ļ���һ��Ŀ¼����ôִ�еݹ����
        if (Recursive != 0 
            && strcmp(FindFileData.cFileName, ".") != 0
            && strcmp(FindFileData.cFileName, "..") != 0
            && FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            ListDirectory(FileName, Recursive);
        }
        // ������һ���ļ�
        if (FindNextFile(hFind, &FindFileData) == FALSE)
        {
            // ERROR_NO_MORE_FILES ��ʾ�Ѿ�ȫ���������
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                printf("Error when get next file in %s\n", Path);
            }
            else
            {
                Ret = 0;
            }
            break;
        }
    } while (TRUE);
    
    // �رվ��
    FindClose(hFind);
    return Ret;
}

int main()
{
    char Path[MAX_PATH + 1] = { 0 };

    // ��Ϊgets��VS2019�ﲻ���ã�������fgets���
    //fgets(Path, sizeof(Path), stdin);  
    strcpy(Path,"./www/video/���Ĳ���/");
    // ��Ϊʹ����fgets������Ҫȡ����β����Ļ��з�
    while (Path[strlen(Path) - 1] == '\n'
           || Path[strlen(Path) - 1] == '\r')
    {
        Path[strlen(Path) - 1] = '\0';
    }
    ListDirectory(Path, 1);
    return 0;
}