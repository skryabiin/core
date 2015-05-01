#include "LuaState.hpp"
#include "Console.hpp"
#include <fstream>


namespace core {


	LuaState::LuaState() {
		_L = nullptr;
		_stackAdditions = 0;
	}

	LuaState::~LuaState() {
		lua_close(_L);
	}

	bool LuaState::createImpl(bool openStandardLibraries) {


		_L = luaL_newstate();
		if (_L == nullptr) {
			_lastError = "Could not initialize lua state.";
			error(_lastError);
			return false;
		}
		if (openStandardLibraries) {
			this->openStandardLibraries();
		}		

		return true;
	}

	bool LuaState::initializeImpl() {
		return true;
	}

	bool LuaState::resetImpl(bool clearBoundFunctions) {

		_lastError = "";

		if (clearBoundFunctions) {
			_boundFunctions.clear();
		}

		return true;
	}

	bool LuaState::destroyImpl() {


		lua_close(_L);

		return true;
	}

	void LuaState::clearStack() {
		if (_initStatus == InitStatus::INIT_TRUE) {
			lua_pop(_L, lua_gettop(_L));
		}
		else{
			warn("Attempting LuaState::clearStack() when state is not initialized.");
		}
	}

	void LuaState::printTable(int index, std::ostringstream& ss, int indent) {


		pushStack();
		while (lua_next(_L, index) != 0) {
			auto ss2 = std::ostringstream{};
			for (int i = 0; i < indent; i++){
				ss2 << "-";
			}
			

			
			int tipe = lua_type(_L, -2);

			switch (tipe) {


				case LUA_TSTRING:  /* strings */
					
					ss2 << lua_tostring(_L, -2);
					break;

				case LUA_TNUMBER:  /* numbers */
					ss2 << lua_tonumber(_L, -2);
					break;
			}

			ss2 << " = ";


			tipe = lua_type(_L, -1);
			switch (tipe) {


			case LUA_TSTRING:  /* strings */

				ss2 << lua_tostring(_L, -1);
				debug(ss2.str());
				break;

			case LUA_TBOOLEAN:  /* booleans */
				ss2 << (lua_toboolean(_L, -1) ? "true" : "false");
				debug(ss2.str());
				break;

			case LUA_TNUMBER:  /* numbers */
				ss2 << lua_tonumber(_L, -1);
				debug(ss2.str());
				break;

			case LUA_TTABLE:
				ss2 << "table:";
				debug(ss2.str());

				break;
			default:  /* other values */
				ss2 << lua_typename(_L, -1);
				debug(ss2.str());
				break;

			}

			popStack();
		}
	}

	std::string LuaState::printStack() {

		std::string out = "";

		if (_initStatus == InitStatus::INIT_TRUE) {
			


				int i;
				int top = lua_gettop(_L);

				
				debug("Lua stack:");
				for (i = top; i >= 1; --i) {  /* repeat for each level */
					int t = lua_type(_L, i);
					std::ostringstream ss;
					switch (t) {

					case LUA_TSTRING:  /* strings */
						debug(lua_tostring(_L, i));
						break;

					case LUA_TBOOLEAN:  /* booleans */
						debug((lua_toboolean(_L, i) ? "true" : "false"));
						break;

					case LUA_TNUMBER:  /* numbers */
						debug(lua_tonumber(_L, i));
						break;

					case LUA_TTABLE:
						debug("table:");
						
						printTable(i, ss, 1);
						debug(ss.str());
						break;
					default:  /* other values */
						debug(lua_typename(_L, t));
						break;

					}
				}				
		}
		else {
			out = "Attempting LuaState::printStack() when state is not initialized.";
		}

		return out;
	}

	int LuaState::createReference() {
		return luaL_ref(_L, LUA_REGISTRYINDEX);
	}

	void LuaState::retrieveReference(int ref) {
		lua_rawgeti(_L, LUA_REGISTRYINDEX, ref);
	}

	void LuaState::removeReference(int ref) {
		luaL_unref(_L, LUA_REGISTRYINDEX, ref);
	}

	bool LuaState::runString(std::string luaCode) {
		if (_initStatus == InitStatus::INIT_TRUE) {
			return luaL_dostring(_L, luaCode.c_str());
		}
		else {
			warn("Attemping LuaState::runString() when state is not initialized.");
			return false;
		}
	}

	bool LuaState::runFile(std::string filePath) {
		if (_initStatus == InitStatus::INIT_TRUE) {
			if (luaL_dofile(_L, filePath.c_str())) {
				_lastError = lua_tostring(_L, -1);

				if (single<Console>().getInitializedStatus() == InitStatus::INIT_TRUE) {
					error("Error running lua script: ", _lastError);
				}
				else {
					std::ofstream logFile;					
					std::string logFileName = (*this)("Config")["logFile"];
					logFile.open(logFileName, std::ios::out | std::ios::app);
					logFile << "[error] Error running lua script: " << _lastError << '\n';
					logFile.flush();
					logFile.close();
				}				
				
				return false;
			}
			return true;
		}
		else {
			warn("Attempting LuaState::runFile() when state is not initialized.");
			return false;
		}		
	}

	void LuaState::openStandardLibraries() {
			luaL_openlibs(_L);
	}

	std::string LuaState::getError() {

		auto out = _lastError;
		_lastError = "";
		return out;

	}

	void LuaState::tempLog(std::string msg) {

		debug(msg);
		printStack();

	}
	void LuaState::createTable(int intKeys, int stringKeys) {
		lua_createtable(_L, intKeys, stringKeys);
	}

	void LuaState::setTable() {

		lua_settable(_L, -3);
	}

	void LuaState::popStack(int count) {
		lua_pop(_L, count);
	}

	void LuaState::popStack() {
		popStack(1);
	}

	int LuaState::dispatcher(lua_State* L) {
		
		auto* state = static_cast<LuaState*>(lua_touserdata(L, lua_upvalueindex(1)));
		auto boundFunction = static_cast<BoundFunction>(lua_touserdata(L, lua_upvalueindex(2)));

		return boundFunction(*state);		
	}

	void LuaState::bindFunction(std::string name, BoundFunction fun) {
		
		if (_boundFunctions.find(name) != std::end(_boundFunctions)) {
			warn("Overwriting registered lua bound function '", name, "'");
			return;
		}
		
		_boundFunctions[name] = fun;

		if (_initStatus == InitStatus::INIT_TRUE) {
			lua_pushlightuserdata(_L, this);
			lua_pushlightuserdata(_L, fun);
			lua_pushcclosure(_L, LuaState::dispatcher, 2);
			lua_setglobal(_L, name.c_str());
		}		

	}




	LuaState::operator bool() {
		auto out = pullStack<bool>();
		popStackAdditions();		
		return out;

	}

	LuaState::operator long() {
		auto out = pullStack<long>();
		popStackAdditions();
		return out;
	}

	LuaState::operator int() {
		auto out = pullStack<int>();
		popStackAdditions();
		return out;
	}

	LuaState::operator short() {
		auto out = pullStack<short>();
		popStackAdditions();
		return out;
	}

	LuaState::operator unsigned short() {
		auto out = pullStack<unsigned short>();
		popStackAdditions();
		return out;
	}

	LuaState::operator float() {
		auto out = pullStack<float>();
		popStackAdditions();
		return out;
	}

	LuaState::operator std::string() {
		auto out = pullStack<std::string>();
		popStackAdditions();
		return out;
	}

	LuaState::operator LuaFunction() {
		auto out = pullStack<LuaFunction>();
		popStackAdditions();
		return out;
	}

	LuaState& LuaState::operator[](const char* key) {

		lua_pushstring(_L, key);
		lua_gettable(_L, -2);

		addStackAdditions(1);
		return *this;

	}

	LuaState& LuaState::operator[](int index) {
		lua_pushinteger(_L, index);
		lua_gettable(_L, -2);

		addStackAdditions(1);
		return *this;
	}


	void LuaState::global(std::string name) {
		lua_getglobal(_L, name.c_str());
		addStackAdditions(1);
	}

	LuaState& LuaState::operator()(const char* key) {
		global(key);
		return *this;
	}

	void LuaState::foreachKey(std::function<void(std::string, LuaState&)> fun) {
		pushStack();
		while (lua_next(_L, -2) != 0) {
			fun(pullStack<std::string>(-2), *this);
			popStack();
		}

	}

	void LuaState::foreachIndex(std::function<void(int, LuaState&)> fun) {

		pushStack();
		while (lua_next(_L, -2) != 0) {
			fun(pullStack<int>(-2), *this);
			popStack();
		}

	}

	void LuaState::addStackAdditions(int n) {
		_stackAdditions += n;
	}

	void LuaState::popStackAdditions() {
		popStack(_stackAdditions);
		_stackAdditions = 0;
	}


	void LuaState::checkCallStatus(int status) {
		if (status == LUA_OK) return;

		std::string errorMessage = pullStack<std::string>();

		switch (status) {
		case LUA_ERRRUN:
			error("Lua runtime error: ", errorMessage);
			break;
		case LUA_ERRMEM:
			error("Lua memory allocation error: ", errorMessage);
			break;
		case LUA_ERRERR:
			error("Lua error handler error: ", errorMessage);
			break;
		case LUA_ERRGCMM:
			error("Lua  garbage collection error: ", errorMessage);
			break;
		}
	}


} //end namespace core