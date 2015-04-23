#ifndef CORE_EVENTS_H
#define CORE_EVENTS_H

#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <map>
#include <SDL.h>
#include "lua.hpp"
#include "Templates.hpp"
#include "Config.hpp"
#include "Entity.hpp"
#include "EventListener.hpp"
#include "LuaState.hpp"
#include "LuaTable.hpp"

namespace core {







	class EventBase : public equal_comparable<EventBase> {

		friend class EventFilterBase;
	public:

		EventBase() {
			_originator = nullptr;
		}

		void setOriginator(EventListener* originator) {
			_originator = originator;
		}

		void addParticipant(Entity participant) {
			_participants.push_back(participant);
		}

		bool hasParticipant(Entity& entity) const {
// TODO (jlynem#1#): Add participant code
			//TODO this template
			//return isIn(_participants, listener);
			return true;
		}

		EventListener* originator()  {
			return _originator;
		}

		std::vector<Entity>& participants() {
			return _participants;

		}

		



		static int nextTypeId() {
			return ++_nextTypeId;
		}
	protected:

		EventListener* _originator;

		std::vector<Entity> _participants;

		static int _nextTypeId;
	};



	template <typename T>
	struct Event : public EventBase, public LuaTable {

		Event() {
			_eventTypeName = T::getEventTypeNameImpl();
			lua_reg("typeName", &_eventTypeName);
		}

		std::string getEventTypeName() {
			return _eventTypeName;
		}

		static int typeId;

		std::string _eventTypeName;

	};


	template <typename T>
	int Event<T>::typeId = Event::nextTypeId();



   



} //end namespace core

#endif
