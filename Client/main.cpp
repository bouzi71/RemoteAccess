#include "stdafx.h"

// Additional
#include "WinsockClient.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	Log::Init();

	Sleep(500);

	WinsockClient client;

	return 0;
}

