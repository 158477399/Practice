// ThreadPool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Thread.h"
#include "ThreadPoolExecutor.h"

class R : public Runnable
{
public:
	~R()
	{
	}
	void Run()
	{
		printf("Hello World/n");
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	CThreadPoolExecutor * pExecutor = new CThreadPoolExecutor();
	pExecutor->Init(1, 10, 50);
	R r;
	for(int i=0;i<100;i++)
	{
		while(!pExecutor->Execute(&r))
		{
		}
	}
	pExecutor->Terminate();
	delete pExecutor;
	getchar();
	return 0;
}
