#include "stdafx.h"

// Additional
#include "WinsockClient.h"

int main()
{
	_wsetlocale(LC_ALL, L"Russian");
	Log::Init();

	WinsockClient client;

	return 0;
}

