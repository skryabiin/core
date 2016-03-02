#ifndef CORE_CONFIG_HPP
#define CORE_CONFIG_HPP

#include <string>
#include "Geometry.hpp"
#include "Templates.hpp"
#include "LuaTable.hpp"
#include "LuaVector.hpp"

namespace core {

	struct WindowConfig : public LuaTable {

		WindowConfig() {
			lua_reg("title", &title);
			lua_reg("dimensions", &dimensions);
			lua_reg("position", &position);
			lua_reg("fullscreen", &fullscreen);
			lua_reg("centered", &centered);
			lua_reg("resizable", &resizable);
			lua_reg("borderless", &borderless);
			lua_reg("grabMouse", &grabMouse);
		}

		std::string title;
		LuaDimension dimensions;
		LuaPixel position;
		bool fullscreen;
		bool centered;		
		bool resizable;
		bool borderless;
		bool grabMouse;
	};

	struct LoggingConfig : public LuaTable {
		LoggingConfig() {
			lua_reg("logFile", &logFile);
			lua_reg("verbosityLevel", &verbosityLevel);
		}
		std::string logFile;
		unsigned short verbosityLevel;
	};

	struct SystemConfig : public LuaTable {
		SystemConfig() {
			lua_reg("maxUpdatesPerSecond", &maxUpdatesPerSecond);
			lua_reg("debugMemory", &debugMemory);
		}
		unsigned short maxUpdatesPerSecond;
		bool debugMemory;
	};

	struct GraphicsConfig : public LuaTable {
		GraphicsConfig() {
			lua_reg("maxFramesPerSecond", &maxFramesPerSecond);
			lua_reg("renderMultithreaded", &renderMultithreaded);
			lua_reg("maxWaitFreeQueueDepth", &maxWaitFreeQueueDepth);
			lua_reg("debugOpenGl", &debugOpenGl);
			lua_reg("vsync", &vsync);
		}
		unsigned short maxFramesPerSecond;
		bool renderMultithreaded;
		unsigned short maxWaitFreeQueueDepth;
		bool debugOpenGl;
		short vsync;
	};

	struct AudioConfig : public LuaTable {
		AudioConfig() {
			lua_reg("samplingFrequency", &samplingFrequency);
			lua_reg("stereo", &stereo);
			lua_reg("chunkSize", &chunkSize);
		}
		unsigned int samplingFrequency;
		bool stereo;
		unsigned short chunkSize;
	};



	struct Config : public LuaTable, public singleton<Config>, public initializable<Config, void, void, void, void> {
		
		Config() {
			lua_reg("window", &window);
			lua_reg("logging", &logging);
			lua_reg("system", &system);
			lua_reg("graphics", &graphics);
			lua_reg("audio", &audio);
		}

		WindowConfig window;
		LoggingConfig logging;
		SystemConfig system;
		GraphicsConfig graphics;
		AudioConfig audio;

		//initializable
		bool createImpl() { return reload(); };
		bool initializeImpl() { return true; };
		bool resetImpl() { return true; }
		bool destroyImpl() { return true; }


		bool reload();
	};


} //end namespace core


#endif // CONFIG_H_INCLUDED
