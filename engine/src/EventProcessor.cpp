#include "EventProcessor.hpp"
#include "Core.hpp"
#include "SDLEvent.hpp"
#include "MouseButtonEvent.hpp"
#include "MouseMoveEvent.hpp"
#include "KeyboardEvent.hpp"
#include "GamepadEvent.hpp"
#include "TimedEvent.hpp"
#include "LuaState.hpp"
namespace core {

	EventProcessor::EventProcessor() {

	}


	InitStatus EventProcessor::initializeImpl() {



		auto& lua = single<Core>().lua();

		lua.bindFunction("addEventFilter_bind", EventProcessor::addEventFilter_bind);		
		lua.bindFunction("removeEventFilter_bind", EventProcessor::removeEventFilter_bind);
		lua.bindFunction("processEvent_bind", EventProcessor::processEvent_bind);
		lua.bindFunction("scheduleTimedEvent_bind", EventProcessor::scheduleTimedEvent_bind);
		

		return InitStatus::INIT_TRUE;
	}

	InitStatus EventProcessor::resetImpl() {

		for (auto topic : _topics) {
			delete topic;
		}
		_topics.clear();
		_topics.shrink_to_fit();
		//todo?
		return InitStatus::INIT_FALSE;

	}



	int EventProcessor::removeEventFilter_bind(LuaState& lua) {

		auto filterId = lua.pullStack<long>(1);

		auto& evp = single <EventProcessor>();

		for (auto& it = std::begin(evp._luaFilters); it != std::end(evp._luaFilters); ++it) {

			if (it->id() == filterId) {
				evp._luaFilters.erase(it);
				break;
			}
		}

		return 0;

	}
	

	int EventProcessor::addEventFilter_bind(LuaState& lua) {

		auto lef = LuaEventFilter{};
		
		lef.callback = lua.createReference();

		lef.participant = lua.pullStack<int>(1);
		lef.eventTypeName = lua.pullStack<std::string>(2);

		single<EventProcessor>()._luaFilters.push_back(std::move(lef));

		lua.pushStack(lef.id());

		return 1;

	}

	int EventProcessor::scheduleTimedEvent_bind(LuaState& lua) {

		

		auto timedEvent = TimedEvent{};

		timedEvent.interval = lua.pullStack<int>(1);
		auto e = lua.pullStack<int>(2);
		if (e != 0) {		
			timedEvent.addParticipant(e);
			timedEvent.entity = e;
		}		
		timedEvent.eventName = lua.pullStack<std::string>(3);
		
		//timedEvent.callbackThreadState = lua_newthread(L);
		
		
		
		//lua_xmove(L, timedEvent.callbackThreadState, 1);

		timedEvent.luaCallbackRef = luaL_ref(timedEvent.callbackThreadState, LUA_REGISTRYINDEX);

		
		
		//debug("Dumping second stack: ", lua.printStack(timedEvent.callbackThreadState));

		auto& ep = single<EventProcessor>();
		SDL_AtomicLock(&ep._timedEventLock);
		ep._timedEvents.push_back(timedEvent);
		auto timedEventPtr = &ep._timedEvents.back();
		SDL_AtomicUnlock(&ep._timedEventLock);
		SDL_AddTimer(timedEvent.interval, EventProcessor::doTimedEventCallback, (void*)timedEventPtr);

		return 0;
	}
	Uint32 EventProcessor::doTimedEventCallback(Uint32 interval, void* timedEventPtr) {

		auto timedEvent = static_cast<TimedEvent*>(timedEventPtr);

		auto L = timedEvent->callbackThreadState;

		lua_rawgeti(L, LUA_REGISTRYINDEX, timedEvent->luaCallbackRef);

		//debug("Lua stack in doTimedEventCallback ", dumpLuaStack(L));

		//timedEvent->toLua(L);
		
  		if (lua_pcall(L, 1, 1, 0)) {
			error("Error in timed event callback: ", lua_tostring(L, -1));
			return 0;
		}


		auto newInterval = luaCast<int>(L, -1);
		lua_pop(L, 1);

		auto& ep = single<EventProcessor>();
		bool foundTimedEvent = false;
		SDL_AtomicLock(&ep._timedEventLock);		
		for (auto it = std::begin(ep._timedEvents); it != std::end(ep._timedEvents); it++) {
				if (*it == *timedEvent) {
					foundTimedEvent = true;
					if (newInterval <= 0) {
						debug("Timed event not repeating.");
						ep._timedEvents.erase(it);
					}
					else {
						it->interval = newInterval;
					}
					break;
				}
		}
		SDL_AtomicUnlock(&ep._timedEventLock);

		if (!foundTimedEvent) {
			newInterval = 0;
			warn("Timed event not found!");
		}
		else if (newInterval > 0) {
			debug("Timed event repeating in ", newInterval);			
		}

		return newInterval;

	}

	int EventProcessor::processEvent_bind(LuaState& lua) {

		std::string eventType = lua["typeName"];

		auto it = EventProcessor::registeredLuaHandlers().find(eventType);

		auto doReturn = false;
		if (it != EventProcessor::registeredLuaHandlers().end()) {

			doReturn = it->second(lua);
		}
		
		return (doReturn) ? 1 : 0;
		
	}



	std::map<std::string, std::function<bool(LuaState&)>>& EventProcessor::registeredLuaHandlers() {

		static std::map<std::string, std::function<bool(LuaState&)>> _registeredLuaHandlers;
		return _registeredLuaHandlers;

	}




} //end namespace core