#include <SDL.h>
#include <ctime>
#include "lua.hpp"
#include "Console.hpp"
#include "Event.hpp"
#include "Core.hpp"
#include "Templates.hpp"
#include "EventProcessor.hpp"
#include "Interface.hpp"
#include <fstream>

namespace core {

	int consoleWriteProc(void* data) {
		
		auto tempLog = std::vector<std::string>{};
		auto& console = single<Console>();		

		std::ofstream logFile;


		bool gogogo = true;
		logFile.open(console.getLogFilePath(), std::ios::out | std::ios::app);

		while (gogogo) {			
			SDL_Delay(10);
			console.currentlyLogging(true);
			gogogo = console.getNewLogMessages(tempLog);

			for (auto& message : tempLog) {
				logFile << message << '\n';
			}
			logFile.flush();			
			tempLog.clear();
			console.currentlyLogging(false);
		}

		logFile.close();
		return 0;
	}

	Console::Console() : basicIos(&_buffer), basicOutStream(&_buffer), singleton<Console>(), _debug{ false }, _consoleOut{ true }, _running{ true }, _consoleLogLock{ 0 }, _lastMsgPolled{ 0 } {

		if (_debugOutProc == NULL) {
			_logFilePath = single<Core>().lua()("Config")["logFile"];
			_debugOutProc = SDL_CreateThread(consoleWriteProc, "DebugOutput", (void*)NULL);
		}

#ifdef _WIN32
		openDebugPipe();
#endif
		
	
	};

#ifdef _WIN32
	void Console::openDebugPipe() {


		SECURITY_ATTRIBUTES sa;

		// Set up the security attributes struct.
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		_debugPipeServerEnd =
			CreateNamedPipe("\\\\.\\pipe\\core_debug_pipe", PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, 1024, 1024, 0, &sa);


	}

#endif

	InitStatus Console::initializeImpl() {
		
		info("Initializing console...");

		auto& lua = single<Core>().lua();


		lua.bindFunction("setDebug_bind", Console::setDebug_bind);
		lua.bindFunction("debug_bind", Console::debug_bind);
		lua.bindFunction("info_bind", Console::info_bind);
		lua.bindFunction("warn_bind", Console::warn_bind);
		lua.bindFunction("error_bind", Console::error_bind);

		lua.bindFunction("doCommand_bind", Console::doCommand_bind);

		//set up event filter
		//create the callback for keyboard events
		std::function<void(Console*, WrappedSdlEvent&)> sdlEventCallback = std::mem_fn(&Console::handleToggleEvent);
		_consoleToggleFilter.init(this, sdlEventCallback);

		_keyToggle = lua("Config")["keyDebugToggle"];

		//set the predicate for only the toggle key
		_consoleToggleFilter.setPredicate(
			[&](WrappedSdlEvent& event)->bool {
			if (event._wrappedEvent->type == SDL_KEYDOWN && event._wrappedEvent->key.repeat == 0) {
				return event._wrappedEvent->key.keysym.sym == _keyToggle;
			}
			else if(event._wrappedEvent->type == SDL_CONTROLLERBUTTONDOWN) {
				return event._wrappedEvent->cbutton.button == _buttonToggle;
			}
			else {
				return false;
			}
		});


		single<EventProcessor>().addFilter(&_consoleToggleFilter);
		return InitStatus::INIT_TRUE;


	}

	InitStatus Console::resetImpl() {

		//do not allow reset
		return InitStatus::INIT_FALSE;

	}

	std::string Console::getLogFilePath() {
		return _logFilePath;
	}

	void Console::log(std::string msg) {

		//if (!_debug) return;
		//OutputDebugString(msg.c_str());
		//OutputDebugString("\n");
	}

	void Console::handleToggleEvent(WrappedSdlEvent& event) {
		_debug = !_debug;		
		auto debugEvent = DebugEvent{};
		single<EventProcessor>().process(debugEvent);		
		if (_debug) {
			single<Interface>().checkGamepadStatus();
		}
	}


	bool Console::getDebugFlag(std::string flagName) const {
		if (!_debug) return false;
		auto it = _debugSettings.debugFlags.find(flagName);

		//default for any non-specified flags is false
		return (it == _debugSettings.debugFlags.end()) ? false : it->second;		

	}

	void Console::setDebugFlag(std::string flagName, bool flagValue) {
		_debugSettings.debugFlags[flagName] = flagValue;
		auto debugEvent = DebugEvent{};
		single<EventProcessor>().process(debugEvent);

	}


	int Console::getVerbosityLevel() const {
		return _debugSettings.verbosityLevel;

	}

	void Console::setVerbosityLevel(int verbosity) {

		_debugSettings.verbosityLevel = verbosity;
	}



	bool Console::getConsoleOut() const {

		return _consoleOut;
	}

	void Console::setConsoleOut(bool consoleOut) {

		_consoleOut = consoleOut;
	}

	
	void Console::shutdown() {
		single<EventProcessor>().removeFilter(&_consoleToggleFilter);

		SDL_AtomicLock(&_consoleLogLock);
		_running = false;
		SDL_AtomicUnlock(&_consoleLogLock);

		SDL_Delay(1000);
	}


	bool Console::getNewLogMessages(std::vector<std::string>& msgBuf) {

		auto running = true;

		msgBuf.insert(std::end(msgBuf), std::begin(_consoleLog) + _lastMsgPolled, std::end(_consoleLog));
		_lastMsgPolled += msgBuf.size();
		running = _running;		

		return running;

	}

	void Console::currentlyLogging(bool logging) {
		if (logging) {
			SDL_AtomicLock(&_consoleLogLock);
		}
		else {
			SDL_AtomicUnlock(&_consoleLogLock);
		}

	}

	void Console::addNewLogMessage(std::string msg) {
		SDL_AtomicLock(&_consoleLogLock);
		_consoleLog.push_back(msg);
		SDL_AtomicUnlock(&_consoleLogLock);
	}
	int Console::setDebug_bind(LuaState& lua) {
		
		notice("----------------------------------------------------");
		notice("Starting Core at ", getTimestamp());

		int verbosity = lua("Config")["verbosityLevel"];
		notice("Verbosity =  ", verbosity);

		single<Console>().setVerbosityLevel(verbosity);


		return 0;
	}

	int Console::debug_bind(LuaState& lua) {
		auto msg = lua.pullStack<std::string>(1);
		debug(msg);
		return 0;
	}

	int Console::info_bind(LuaState& lua) {
		auto msg = lua.pullStack<std::string>(1);
		info(msg);
		return 0;
	}

	int Console::warn_bind(LuaState& lua) {
		auto msg = lua.pullStack<std::string>(1);
		warn(msg);
		return 0;
	}
	int Console::error_bind(LuaState& lua) {
		auto msg = lua.pullStack<std::string>(1);
		error(msg);
		return 0;
	}

	int Console::doCommand_bind(LuaState& lua) {

		return 0;
	}

} //end namespace core