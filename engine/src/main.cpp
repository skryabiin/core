
#include "lua.hpp"
#include "main.hpp"
#include "Core.hpp"


using namespace core;




int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// create the application instance
	auto& lua = single<Core>().lua();
	//lua.runFile("./lua-core/config.lua");
	//lua.runFile(lua("Config")["mainLuaFile"]);
	lua.runFile(lpCmdLine);


	//_CrtDumpMemoryLeaks();
	return 0;

}
