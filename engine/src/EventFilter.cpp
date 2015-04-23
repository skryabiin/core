#include "EventFilter.hpp"
#include "EventProcessor.hpp"

namespace core {




	EventFilterBase::EventFilterBase() : _onlyParticipating{ false } {

	}

	void EventFilterBase::setEventTypeId(int typeId)
	{
		_eventTypeId = typeId;
	}

	int EventFilterBase::getEventTypeId()
	{
		return _eventTypeId;
	}

	void EventFilterBase::setOnlyParticipating(bool onlyParticipating)
	{
		_onlyParticipating = onlyParticipating;
	}

	bool EventFilterBase::onlyParticipating()
	{
		return _onlyParticipating;
	}

	void EventFilterBase::setListener(EventListener* listener)
	{
		_listener = listener;
	}

	void EventFilterBase::pauseImpl()
	{
		//single<EventProcessor>().removeFilter(this);
	}

	void EventFilterBase::resumeImpl()
	{
		//single<EventProcessor>().addFilter(this);
	}

	EventListener* EventFilterBase::listener()
	{
		return _listener;
	}

	EventFilterBase::~EventFilterBase()
	{
	}




} //end namespace core