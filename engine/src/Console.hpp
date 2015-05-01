#ifndef CORE_CONSOLE_HPP
#define CORE_CONSOLE_HPP

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>

#include "lua.hpp"
#include "Config.hpp"
#include "Templates.hpp"
#include "SDLEvent.hpp"
#include "Event.hpp"




namespace core {

	typedef std::basic_ostream<char, std::char_traits<char>> basicOutStream;
	typedef std::basic_streambuf<char, std::char_traits<char>> basicStreamBuf;
	typedef std::basic_ios<char, std::char_traits<char>> basicIos;


	class console_out_buf : public basicStreamBuf {

	};

	struct DebugSettings {

		DebugSettings() : verbosityLevel{ 0 } {};
		std::map<std::string, bool> debugFlags;		
		int verbosityLevel;

	};



	inline void endl() {}

	class Core;
	class Console : public basicOutStream, public initializable<Console, void, void, void, void>, public singleton<Console> {

		friend class Core;
	public:

		Console();

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();
		
		template <typename ... Args>
		void log(int debugLevel, Args&& ... args) {
			if (_debugSettings.verbosityLevel < debugLevel) return;

			auto ss = std::ostringstream{};

			switch (debugLevel) {
			case -1:  //fatal errors accompany app shutdown, and are always displayed
				ss << "[fatal] ";
				break;
			case 0:  //notices are always displayed
				ss << "[notice] ";
				break;
			case 1: //errors that do not necessarily indicate an app shutdown
				ss << "[error] ";
				break;
			case 2: //warnings
				ss << "[warn] ";
				break;
			case 3: //helpful information
				ss << "[info] ";
				break;
			case 4: //all your "here" output lines
				ss << "[dbug] ";
				break;
			}

			logRec(ss, args...);			
			ss.flush(); 
			log(ss.str());
			addNewLogMessage(ss.str());

		}

		std::string getLogFilePath();

		void log(std::string msg);
		
		bool isRunning();

		bool getNewLogMessages(std::vector<std::string>& msgBuf);

		void addNewLogMessage(std::string msg);

		friend int consoleWriteProc(void* data);		

		void currentlyLogging(bool logging);

		static int setDebug_bind(LuaState& lua);

		static int doCommand_bind(LuaState& lua);

		static int debug_bind(LuaState& lua);

		static int info_bind(LuaState& lua);
		
		static int warn_bind(LuaState& lua);
		
		static int error_bind(LuaState& lua);

		bool getDebugFlag(std::string flagName) const;

		int getVerbosityLevel() const;

		void setVerbosityLevel(int verbosity);

		void setDebugFlag(std::string flagName, bool flagValue);

		bool getConsoleOut() const;

		void setConsoleOut(bool consoleOut);

		void shutdown();

	private:

		template <typename B>
		void logRec(std::ostringstream& ss, B&& b) {
			ss << b;
			
		}

		template <typename A, typename B>
		void logRec(std::ostringstream& ss, A&& a, B&& b) {
			ss << a;
			logRec<B>(ss, b);
		}

		template <typename A, typename B, typename ... Args>
		void logRec(std::ostringstream& ss, A&& a, B&& b, Args&& ...args) {
			ss << a;
			logRec(ss, b, args...);
		}

		std::ostringstream ss;

		DebugSettings _debugSettings;
		
		bool _debug;

		bool _consoleOut;

		bool _running;

		std::string _logFilePath;		

		console_out_buf _buffer;		

		int _keyToggle;
		int _buttonToggle;

		std::vector<std::string> _consoleLog;

		int _lastMsgPolled; 

		SDL_SpinLock _consoleLogLock;

		SDL_Thread* _debugOutProc;

		#ifdef _WIN32
		HANDLE _debugPipeServerEnd;
			void openDebugPipe();
		#endif
	};

	template <typename ... Args>
	inline void debug(Args&& ...args) {
		single<Console>().log(4, args...);
	}

	template <typename ... Args>
	inline void info(Args&& ...args) {
		single<Console>().log(3, args...);
	}
	
	template <typename ... Args>
	inline void warn(Args&& ...args) {
		single<Console>().log(2, args...);
	}
	
	template <typename ... Args>
	inline void error(Args&& ...args) {
		single<Console>().log(1, args...);
	}

	template <typename ... Args>
	inline void notice(Args&& ...args) {
		single<Console>().log(0, args...);
	}

	template <typename ... Args>
	inline void fatal(Args&& ...args) {
		single<Console>().log(-1, args...);
	}

} //end namespace core

#endif