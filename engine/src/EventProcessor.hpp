#ifndef CORE_EVENT_PROCESSOR_HPP
#define CORE_EVENT_PROCESSOR_HPP


#include "lua.hpp"
#include "Templates.hpp"
#include "LuaEventFilter.hpp"

namespace core {



	class LuaListener {

	};


	struct EventTopicBase : public equal_comparable<EventTopicBase> {
		std::type_index eventType = typeid(this);

	};

	template<typename EVENT_TYPE>
	class EventListener;

	template <typename EVENT_TYPE>
	struct EventTopic : public EventTopicBase {

	public:

		EventTopic() {
			eventType = typeid(EVENT_TYPE);

		}

		std::vector<EventListener<EVENT_TYPE>*> listeners;

		bool process(EVENT_TYPE& e);

		void registerListener(EventListener<EVENT_TYPE>* listener);

		void removeListener(EventListener<EVENT_TYPE>* listener);

		};
	////
	//EVENT PROCESSOR
	////
	class EventProcessor : public initializable<EventProcessor, void, void, void, void>, public singleton<EventProcessor> {

	public:

		EventProcessor();

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		template <typename EVENT_TYPE>
		void process(EVENT_TYPE& e);

		template <typename EVENT_TYPE>
		void registerListener(EventListener<EVENT_TYPE>* listener);

		template <typename EVENT_TYPE>
		void removeListener(EventListener<EVENT_TYPE>* listener);
		

		static int removeEventFilter_bind(LuaState& lua);

		static int addEventFilter_bind(LuaState& lua);

		static int processEvent_bind(LuaState& lua);

	private:

	
		std::vector<EventTopicBase*> _topics;

		std::vector<LuaEventFilter> _luaFilters;

		SDL_SpinLock _processEventLock;

		SDL_SpinLock _timedEventLock;

	};


	template<typename EVENT_TYPE>
	class EventListener : public equal_comparable<EventListener<EVENT_TYPE>> {

	public:

		EventListener()
		{
			_listening = true;
			single<EventProcessor>().registerListener<EVENT_TYPE>(this);
		};

		~EventListener() {
			single<EventProcessor>().removeListener<EVENT_TYPE>(this);
		}

		virtual bool handleEvent(EVENT_TYPE& e) = 0;

		void EventListener::pauseListening() {
			_listening = false;
		}

		void EventListener::resumeListening() {
			_listening = true;
		}

		bool isListening() const {
			return _listening;
		}



	private:

		bool _listening;

	};

	template<typename EVENT_TYPE>
	bool EventTopic<EVENT_TYPE>::process(EVENT_TYPE& e) {
		for (auto listener : listeners) {
			if (listener->isListening()) {
				if (!listener->handleEvent(e)) return false;
			}
		}
		return true;
	}

	template<typename EVENT_TYPE>
	void EventTopic<EVENT_TYPE>::registerListener(EventListener<EVENT_TYPE>* listener) {

		listeners.push_back(listener);
	}

	template<typename EVENT_TYPE>
	void EventTopic<EVENT_TYPE>::removeListener(EventListener<EVENT_TYPE>* listener) {
		for (auto it = std::begin(listeners); it != std::end(listeners); ++it) {
			if (**it == *listener) {
				listeners.erase(it);
				return;
			}
		}
	}


	template <typename EVENT_TYPE>
	void EventProcessor::process(EVENT_TYPE& e) {

		for (auto topic : _topics) {
			if (topic->eventType == typeid(EVENT_TYPE)) {
				if (!static_cast<EventTopic<EVENT_TYPE>*>(topic)->process(e)) break;
			}
		}
		/*
		auto luaEventName = e.getEventTypeName();
		auto& lua = single<Core>().lua();
		std::for_each(begin(_luaFilters), end(_luaFilters),
		[&](LuaEventFilter& filter) {
		if (!luaEventName.compare(filter.eventTypeName)) {
		if (filter.participant == -1 || e.hasParticipant(filter.participant)) {
		lua.call(filter.callback, e);
		info("Doing callback.");
		}
		}
		});
		*/
	}

	template <typename EVENT_TYPE>
	void EventProcessor::registerListener(EventListener<EVENT_TYPE>* listener) {
		for (auto topic : _topics) {
			if (topic->eventType == typeid(EVENT_TYPE)) {
				static_cast<EventTopic<EVENT_TYPE>*>(topic)->registerListener(listener);
				return;
			}
		}

		auto newTopic = new EventTopic<EVENT_TYPE>{};
		newTopic->registerListener(listener);
		_topics.push_back(newTopic);
	}

	template <typename EVENT_TYPE>
	void EventProcessor::removeListener(EventListener<EVENT_TYPE>* listener) {

		for (auto topic : _topics) {
			if (topic->eventType == typeid(EVENT_TYPE)) {
				static_cast<EventTopic<EVENT_TYPE>*>(topic)->removeListener(listener);

			}
		}

	}



} // end namespace core

#endif