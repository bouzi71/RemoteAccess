#include "stdafx.h"

// Additional
#include "LocalProcess.h"
#include "WinsockClient.h"

int main()
{
	_wsetlocale(LC_ALL, L"Russian");
	Log::Init();

	WinsockClient client;

	system("pause");
	return 0;
}

