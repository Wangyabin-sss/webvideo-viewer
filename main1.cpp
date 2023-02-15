

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

// Recursive是1表示递归查找，否则就只列出本级目录 
int ListDirectory(char* Path, int Recursive)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
    char FileName[MAX_PATH] = { 0 };
    int Ret = -1;

    strcpy(FileName, Path);
    strcat(FileName, "/");
    strcat(FileName, "*.*");

    // 查找第一个文件
    hFind = FindFirstFile(FileName, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error when list %s\n", Path);
        return Ret;
    }
    do
    {
        // 构造文件名
        strcpy(FileName, Path);
        strcat(FileName, "/");
        strcat(FileName, FindFileData.cFileName);
        printf("%s\n", FileName);

        // 如果是递归查找，并且文件名不是.和..，并且文件是一个目录，那么执行递归操作
        if (Recursive != 0 
            && strcmp(FindFileData.cFileName, ".") != 0
            && strcmp(FindFileData.cFileName, "..") != 0
            && FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            ListDirectory(FileName, Recursive);
        }
        // 查找下一个文件
        if (FindNextFile(hFind, &FindFileData) == FALSE)
        {
            // ERROR_NO_MORE_FILES 表示已经全部查找完成
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
    
    // 关闭句柄
    FindClose(hFind);
    return Ret;
}

int main()
{
    char Path[MAX_PATH + 1] = { 0 };

    // 因为gets在VS2019里不可用，所以用fgets替代
    //fgets(Path, sizeof(Path), stdin);  
    strcpy(Path,"./www/video/中文测试/");
    // 因为使用了fgets，所以要取掉结尾多余的换行符
    while (Path[strlen(Path) - 1] == '\n'
           || Path[strlen(Path) - 1] == '\r')
    {
        Path[strlen(Path) - 1] = '\0';
    }
    ListDirectory(Path, 1);
    return 0;
}