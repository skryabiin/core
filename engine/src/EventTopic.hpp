#ifndef CORE_EVENT_TOPIC_HPP
#define CORE_EVENT_TOPIC_HPP

#include "Templates.hpp"
#include "EventFilter.hpp"

namespace core {


	struct EventTopicBase : public equal_comparable<EventTopicBase> {
		int eventTypeId;

	};

	template <typename EVENT_TYPE>
	struct EventTopic : public EventTopicBase {		

	public:

		EventTopic() {
			eventTypeId = EVENT_TYPE::typeId;

		}

		std::vector<EventFilter<EVENT_TYPE>*> filters;


	};


} //end namespace core

#endif