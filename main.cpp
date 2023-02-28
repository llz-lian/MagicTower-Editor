#include<vector>
#include"Editor.h"


#include <windows.h>
#include <DbgHelp.h>
#pragma comment( lib, "Dbghelp.lib" )

//程式异常捕获
LONG WINAPI ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    //创建 Dump 文件  
    HANDLE hDumpFile = CreateFile(TEXT("crash.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // crash.dmp 崩溃生成文件名
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump信息  
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        //写入Dump文件内容  
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }

    //这里弹出一个错误对话框并退出程序  
    //QMessageBox::critical(NULL, "程序崩溃", "抱歉，请联系JCXY解决！");

    return EXCEPTION_EXECUTE_HANDLER;
}

int main()
{
    //sf::Vector2u tile_size, std::string asset_dir, float choose_scale,float game_show_scale,int * __choose_map_tiles
    int choose_map_tiles[max_length];
    for (int i = 0; i < max_length; i++)
    {
        choose_map_tiles[i] = i;
    }


    Editor* edit = new Editor(sf::Vector2u(32, 32), "assets",0.7f,1.0f,choose_map_tiles);
    edit->run();
    return 0;
}