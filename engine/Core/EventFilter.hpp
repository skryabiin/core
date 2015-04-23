#ifndef CORE_EVENT_FILTER_HPP
#define CORE_EVENT_FILTER_HPP

#include <functional>

#include "Templates.hpp"
#include "EventListener.hpp"
#include "Entity.hpp"


namespace core {	
	////
	//EVENT FILTER
	////
	class EventFilterBase : public pausable<EventFilterBase> {

	public:
		EventFilterBase();

		void setEventTypeId(int typeId);

		int getEventTypeId();

		void setOnlyParticipating(bool onlyParticipating);

		bool onlyParticipating();

		void setListener(EventListener* listener);

		void pauseImpl();

		void resumeImpl();

		EventListener* listener();

		~EventFilterBase();

	protected:

		bool _onlyParticipating;
		int _eventTypeId;
		EventListener* _listener;


	};

	template <typename EVENT_TYPE>
	class EventFilter : public EventFilterBase, public equal_comparable<EventFilter<EVENT_TYPE>> {

	public:

		EventFilter() : EventFilterBase{} {
			setEventTypeId(EVENT_TYPE::typeId);
		};



		template <typename LISTENER_TYPE>
		void init(LISTENER_TYPE* listener, std::function<void(LISTENER_TYPE*, EVENT_TYPE&)> callback) {			
			setListener(listener);
			_listenerCallback.reset(new CallbackObject<LISTENER_TYPE>{ listener, callback });
		}


		//fuck you C++ give me concepts
		class CallbackVirtual {
		public:
			virtual void doCallback(EVENT_TYPE&) = 0;

		};

		template <typename LISTENER_TYPE>
		class CallbackObject : public CallbackVirtual {
		public:
			CallbackObject(LISTENER_TYPE* listener, std::function<void(LISTENER_TYPE*, EVENT_TYPE&)> callback) {
				_listener = listener;
				_callback = callback;
			}

			virtual void doCallback(EVENT_TYPE& e) {
				return _callback(_listener, e);
			}

		private:
			LISTENER_TYPE* _listener;
			std::function<void(LISTENER_TYPE*, EVENT_TYPE&)> _callback;

		};

		std::unique_ptr<CallbackVirtual> _listenerCallback;



		void setPredicate(std::function<bool(EVENT_TYPE&)> pred) {
			_predicate = pred;
		}

		void doFilter(EVENT_TYPE& e) {

			//don't filter if it's paused
			if (_paused) return;

			//if we're only listening for events that involve us, and this event doesn't involve us or we are the originator, ignore it
			if (e.originator() != nullptr && *e.originator() == *_listener) return;

			//if the predicate function doesn't exist, or it exists and returns 'true', do the callback
			if (!_predicate || _predicate(e)) {
				_listenerCallback->doCallback(e);
			}

		}

		~EventFilter() {
			
		}
	private:

		std::function<bool(EVENT_TYPE&)> _predicate;

	};




} //end namespace core

#endif