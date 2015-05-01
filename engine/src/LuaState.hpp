#ifndef CORE_LUA_STATE_HPP
#define CORE_LUA_STATE_HPP

#include <map>
#include <functional>

#include "lua.hpp"
#include "Templates.hpp"
#include "LuaTable.hpp"
#include "LuaFunction.hpp"

namespace core {
	
	class LuaState;

	typedef int(*BoundFunction)(LuaState&);


	class LuaState : public initializable<LuaState, bool, void, void, bool> {
		friend class LuaTable;
	public:

		LuaState();

		LuaState(LuaState& other) = delete;

		LuaState& operator=(LuaState& other) = delete;

		LuaState(LuaState&& other) = delete;

		LuaState& operator=(LuaState&& other) = delete;

		bool createImpl(bool loadLibs);

		bool initializeImpl();

		bool resetImpl(bool clearBoundFunctions);

		bool destroyImpl();

		std::string printStack();

		void printTable(int index, std::ostringstream& ss, int indent);

		void openStandardLibraries();

		void bindFunctions();

		std::string getError();

		void bindFunction(std::string name, BoundFunction fun);

		void clearStack();

		int createReference();

		void retrieveReference(int ref);

		void removeReference(int ref);

		bool runString(std::string luaCode);

		bool runFile(std::string filePath);

		LuaState& operator[](const char* key);
	
		LuaState& operator[](int index);

		void global(std::string name);

		LuaState& operator()(const char* key);	

		void createTable(int intKeys, int stringKeys);

		void setTable();

		void popStack(int count);

		void popStack();



		template<typename T>
		T pullStack(int index) {

		}

		template<>
		int pullStack(int index) {
			return lua_tointeger(_L, index);
		}

		template<>
		unsigned int pullStack(int index) {
			return lua_tounsigned(_L, index);
		}

		template<>
		long pullStack(int index) {
			return long(lua_tointeger(_L, index));
		}

		template<>
		float pullStack(int index) {
			return float(lua_tonumber(_L, index));
		}

		template<>
		double pullStack(int index) {
			return lua_tonumber(_L, index);
		}

		template<>
		unsigned short pullStack(int index) {
			return lua_tounsigned(_L, index);
		}

		template<>
		short pullStack(int index) {
			return lua_tointeger(_L, index);
		}

		template<>
		bool pullStack(int index) {
			return lua_toboolean(_L, index) == 1;
		}

		template<>
		std::string pullStack(int index) {
			return std::string{ lua_tostring(_L, index) };
		}

		template<>
		LuaFunction pullStack(int index) {
			LuaFunction lf = LuaFunction{};
			lua_pushvalue(_L, index);
			lf._callbackRef = createReference();
			return lf;
		}


		template<typename T>
		T pullStack() {
			return pullStack<T>(-1);
		}

		template<typename T>
		void pushStack(const T& value) {
			static_cast<LuaTable>(value).toLua(*this);
		}

		template<>
		void pushStack<int>(const int& value) {
			lua_pushinteger(_L, value);
		}
		
		template<>
		void pushStack<unsigned int>(const unsigned int& value) {
			lua_pushinteger(_L, value);
		}

		template<>
		void pushStack<std::string>(const std::string& value) {
			lua_pushstring(_L, value.c_str());
		}

		template<>
		void pushStack<float>(const float& value) {
			lua_pushnumber(_L, value);
		}

		template<>
		void pushStack<double>(const double& value) {
			lua_pushnumber(_L, value);
		}

		template<>
		void pushStack<long>(const long& value) {
			lua_pushnumber(_L, value);
		}

		template<>
		void pushStack<unsigned short>(const unsigned short& value) {
			lua_pushnumber(_L, value);
		}

		template<>
		void pushStack<short>(const short& value) {
			lua_pushnumber(_L, value);
		}

		template<>
		void pushStack<bool>(const bool& value) {
			lua_pushboolean(_L, value);
		}

		template<>
		void pushStack<LuaFunction>(const LuaFunction& f) {
			lua_rawgeti(_L, LUA_REGISTRYINDEX, f._callbackRef);
		}

		void pushStack() {
			lua_pushnil(_L);
		}

		void pushStackPointer(void* ptr) {
			lua_pushlightuserdata(_L, ptr);
		}

		void pushStack(const char* value) {
			lua_pushstring(_L, value);
		}

		void foreachKey(std::function<void(std::string, LuaState&)>);
		void foreachIndex(std::function<void(int, LuaState&)>);
		
		void tempLog(std::string msg);

		template<typename T>
		void getTable(T& key) {
			pushStack(key);			
			lua_gettable(_L, -2);
		}

		
		void call(LuaFunction& f) {
			clearStack();
			retrieveReference(f._callbackRef);
			lua_pcall(_L, 0, LUA_MULTRET, 0);
		}
		
		template<typename ...Args>
		void call(LuaFunction& f, Args&&... args) {

			clearStack();
			retrieveReference(f._callbackRef);
			auto numArgs = 0;
			callRec(numArgs, args...);
			lua_pcall(_L, numArgs, LUA_MULTRET, 0);

		}

		template<typename ...Args>
		void call(int callbackRef, Args&... args) {

			//clear the stack for this
			clearStack();
			retrieveReference(callbackRef);
			auto numArgs = 0;
			callRec(numArgs, args...);
			lua_pcall(_L, numArgs, LUA_MULTRET, 0);

		}




		static int dispatcher(lua_State* L);

		~LuaState();

		operator long();
		operator bool();
		operator int();
		operator unsigned int();
		operator float();
		operator unsigned short();
		operator short();
		operator std::string();
		operator LuaFunction();

	private:

		void addStackAdditions(int n);

		template<typename Arg1>
		void callRec(int& numArgs, Arg1&& arg1) {
			numArgs++;
			pushStack(arg1);
		}

		template<typename Arg1, typename Arg2>
		void callRec(int& numArgs, Arg1&& arg1, Arg2&& arg2) {
			numArgs++;
			pushStack(arg1);
			callRec(numArgs, arg2);
		}

		template<typename Arg1, typename Arg2, typename ...Args>
		void callRec(int& numArgs, Arg1&& arg1, Arg2&& arg2, Args&&... args) {
			numArgs++;
			pushStack(arg1);
			callRec(numArgs, arg2, args...);

		}

		void checkCallStatus(int status);

		void popStackAdditions();

		int _stackAdditions;

		lua_State* _L;

		std::string _lastError;

		std::map<std::string, BoundFunction> _boundFunctions;

	};


} //end namespace core

#endif