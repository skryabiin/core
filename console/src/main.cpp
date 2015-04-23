// DebugConsole.cpp : Defines the entry point for the console application.
//
// Echoes all input to stdout. This will be redirected by the redirect
// sample. Compile and build child.c as a Win32 Console application and
// put it in the same directory as the redirect sample.
//
#include "main.hpp"
#include "DebugConsole.hpp"



int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)	
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	auto& debugConsole = core::DebugConsole{};

	debugConsole.run();

	return 0;
}