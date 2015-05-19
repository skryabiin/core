#include "EventProcessor.hpp"
#include "Core.hpp"
#include "SDLEvent.hpp"
#include "MouseButtonEvent.hpp"
#include "MouseMoveEvent.hpp"
#include "KeyboardEvent.hpp"
#include "GamepadEvent.hpp"
#include "TimedEvent.hpp"
#include "LuaState.hpp"
#include "EventRegistry.hpp"

namespace core {

	EventProcessor::EventProcessor() {

	}
	bool EventProcessor::createImpl() {
		info("Creating event processor...");
		info("Initializing SDL events...");
		if (SDL_Init(SDL_INIT_EVENTS) < 0) {
			error("Error initializing SDL events: ", SDL_GetError());
			return false;
		}
		info("Initializing SDL timer...");
		if (SDL_Init(SDL_INIT_TIMER) < 0) {
			error("Erorr initializing SDL timer: ", SDL_GetError());
			return false;
		}

		auto& lua = single<Core>().lua();

		lua.bindFunction("addEventFilter_bind", EventProcessor::addEventFilter_bind);
		lua.bindFunction("removeEventFilter_bind", EventProcessor::removeEventFilter_bind);
		lua.bindFunction("processEvent_bind", EventProcessor::processEvent_bind);
		return true;
	}



	bool EventProcessor::initializeImpl() {
		return true;
	}

	bool EventProcessor::resetImpl() {

		_luaFilters.clear();




		//todo?
		return true;

	}

	bool EventProcessor::destroyImpl() {
		for (auto topic : _topics) {
			delete topic;
		}
		_topics.clear();
		return true;
	}

	void EventProcessor::processForLua(std::string eventTypeName, LuaTable event) {
		auto& lua = single<Core>().lua();
		std::for_each(std::begin(_luaFilters), std::end(_luaFilters),
			[&](LuaEventFilter& filter) {
			if (!eventTypeName.compare(filter.eventTypeName)) {
				lua.call(filter.callback, event);							
			}
		});
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

	

	int EventProcessor::processEvent_bind(LuaState& lua) {

		std::string eventType = lua["typeName"];

		auto it = EventRegistry::registeredLuaHandlers().find(eventType);

		auto doReturn = false;
		if (it != EventRegistry::registeredLuaHandlers().end()) {

			doReturn = it->second(lua);
		}
		
		return (doReturn) ? 1 : 0;
		
	}





} //end namespace core