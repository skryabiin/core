#ifndef CORE_EVENT_PROCESSOR_HPP
#define CORE_EVENT_PROCESSOR_HPP


#include "lua.hpp"
#include "Templates.hpp"
#include "Event.hpp"
#include "EventListener.hpp"
#include "LuaEventFilter.hpp"
#include "TimedEvent.hpp"
#include "CollisionState.hpp"
#include "EventTopic.hpp"

namespace core {

	////
	//EVENT PROCESSOR
	////
	class EventProcessor : public initializable<EventProcessor, void, void>, public singleton<EventProcessor> {

	public:

		EventProcessor();

		InitStatus initializeImpl();

		InitStatus resetImpl();

		template <typename EVENT_TYPE>
		void process(EVENT_TYPE& e) {	

			for (auto topic : _topics) {

				if (EVENT_TYPE::typeId == topic->eventTypeId) {

					auto eventTopic = static_cast<EventTopic<EVENT_TYPE>*>(topic);

					std::for_each(begin(eventTopic->filters), end(eventTopic->filters),
						[&](EventFilter<EVENT_TYPE>* filter) {
						if (!filter->isPaused()) {

							filter->doFilter(e);
						}

				});
					break;
				}
			}
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
		}

		template <typename EVENT_TYPE>
		void addFilter(EventFilter<EVENT_TYPE>* filter) {
			for (auto topic : _topics) {

				if (EVENT_TYPE::typeId == topic->eventTypeId) {

					auto eventTopic = static_cast<EventTopic<EVENT_TYPE>*>(topic);

					eventTopic->filters.push_back(filter);
					return;
				}
			}

			//no topic exists yet			
			auto newTopic = new EventTopic<EVENT_TYPE>{};
			newTopic->filters.push_back(filter);
			_topics.push_back(newTopic);

		}

		template <typename EVENT_TYPE>
		void removeFilter(EventFilter<EVENT_TYPE>* filter) {

			for (auto topic : _topics) {				
				if (EVENT_TYPE::typeId == topic->eventTypeId) {

					auto eventTopic = static_cast<EventTopic<EVENT_TYPE>*>(topic);

					for (auto it = std::begin(eventTopic->filters); it != std::end(eventTopic->filters); ++it) {
						if (**it == *filter) {
							eventTopic->filters.erase(it);
							return;
						}
					}

				}
			}

		}

		static int removeEventFilter_bind(LuaState& lua);

		static int addEventFilter_bind(LuaState& lua);

		static int processEvent_bind(LuaState& lua);

		

		static int scheduleTimedEvent_bind(LuaState& lua);

		static Uint32 doTimedEventCallback(Uint32 interval, void* timedEvent);

		LuaDummyListener luaDummyListener;

		template <typename Event_type>
		struct EventRegistration {

			EventRegistration() {

				EventProcessor::registeredLuaHandlers()[Event_type::getEventTypeNameImpl()] = &Event_type::createFromLua;
			}

		};

	private:

	
		std::vector<EventTopicBase*> _topics;

		std::vector<void*> _filters;

		std::vector<LuaEventFilter> _luaFilters;

		SDL_SpinLock _processEventLock;

		SDL_SpinLock _timedEventLock;

		std::vector<TimedEvent> _timedEvents;

		static std::map<std::string, std::function<bool(LuaState&)>>& registeredLuaHandlers();
	};


} // end namespace core

#endif