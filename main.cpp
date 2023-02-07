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
    Editor* edit = new Editor(sf::Vector2u(32, 32), "assets");
    edit->run();
    return 0;
}