#include<vector>
#include"Editor.h"


#include <windows.h>
#include <DbgHelp.h>
#pragma comment( lib, "Dbghelp.lib" )

//��ʽ�쳣����
LONG WINAPI ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    //���� Dump �ļ�  
    HANDLE hDumpFile = CreateFile(TEXT("crash.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // crash.dmp ���������ļ���
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump��Ϣ  
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        //д��Dump�ļ�����  
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }

    //���ﵯ��һ������Ի����˳�����  
    //QMessageBox::critical(NULL, "�������", "��Ǹ������ϵJCXY�����");

    return EXCEPTION_EXECUTE_HANDLER;
}

int main()
{
    Editor* edit = new Editor(sf::Vector2u(32, 32), "assets");
    edit->run();
    return 0;
}