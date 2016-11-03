// Mux.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream> 
#include <process.h> 
#include "Lock.h" 
using namespace std; 

//创建一个互斥对象 
Mutex g_Lock; 

//线程函数 
unsigned int __stdcall StartThread(void *pParam) 
{ 
	char *pMsg = (char *)pParam; 
	if (!pMsg) 
	{ 
		return (unsigned int)1; 
	} 
	//对被保护资源（以下打印语句）自动加锁 
	//线程函数结束前，自动解锁 
	CLock lock(g_Lock); 
	for( int i = 0; i < 5; i++ ) 
	{ 
		cout << pMsg << endl; 
		Sleep( 500 ); 
	} 
	return (unsigned int)0; 
} 


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hThread1, hThread2; 
	unsigned int uiThreadId1, uiThreadId2; 
	char *pMsg1 = "First print thread."; 
	char *pMsg2 = "Second print thread."; 

	//创建两个工作线程，分别打印不同的消息 
	//hThread1 = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartThread, (void *)pMsg1, 0, (LPDWORD)&uiThreadId1); 

	//hThread2 = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartThread, (void *)pMsg2, 0, (LPDWORD)&uiThreadId2); 

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, &StartThread, (void *)pMsg1, 0, &uiThreadId1); 

	hThread2 = (HANDLE)_beginthreadex(NULL, 0, &StartThread, (void *)pMsg2, 0, &uiThreadId2); 
	//等待线程结束 
	DWORD dwRet = WaitForSingleObject(hThread1,INFINITE); 
	if ( dwRet == WAIT_TIMEOUT ) 
	{ 
		TerminateThread(hThread1,0); 
	} 
	dwRet = WaitForSingleObject(hThread2,INFINITE); 
	if ( dwRet == WAIT_TIMEOUT ) 
	{ 
		TerminateThread(hThread2,0); 
	} 
	//关闭线程句柄，释放资源 
	::CloseHandle(hThread1); 
	::CloseHandle(hThread2); 
	system("pause"); 
	return 0; 
}

