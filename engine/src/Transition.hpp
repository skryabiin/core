#ifndef CORE_TRANSITION_HPP
#define CORE_TRANSITION_HPP

#include "Templates.hpp"
#include "LuaFunction.hpp"
#include "LuaTable.hpp"

namespace core {	

	struct TransitionBase : public equal_comparable<TransitionBase>, public updateable<TransitionBase, bool, float>, public pausable<TransitionBase> {

		virtual ~TransitionBase() {
		};

		void pauseImpl();

		void resumeImpl();

		virtual bool updateImpl(float& dt) = 0;

	protected:



		float duration;
		
	};

	template <typename TRANSITION_TYPE>
	struct Transition : public TransitionBase, public LuaTable {

		Transition() {
			_transitionTypeName = TRANSITION_TYPE::getTransitionTypeNameImpl();
			lua_reg("typeName", &_transitionTypeName);
			lua_reg("duration", &duration);
			lua_reg("finishedCallback", &finishedCallback);
		}

		std::string getTransitionTypeName() {
			return _transitionTypeName;
		}

		LuaFunction finishedCallback;
		

	private:
		std::string _transitionTypeName;

	};


} //end namespace core

#endif