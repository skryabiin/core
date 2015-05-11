
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


	// create the application instance
	auto& lua = single<Core>().lua();
	//lua.runFile("./lua-core/config.lua");
	//lua.runFile(lua("Config")["mainLuaFile"]);
	lua.runFile(lpCmdLine);

	single<Console>().reset();
	single<Console>().destroy();

	bool debugMemory = lua("Config")["system"]["debugMemory"];
	if (debugMemory) {
		_CrtDumpMemoryLeaks();
	}
	return 0;

}
