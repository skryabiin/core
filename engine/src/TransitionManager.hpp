#ifndef CORE_TRANSITION_MANAGER_HPP
#define CORE_TRANSITION_MANAGER_HPP

#include "Templates.hpp"
#include "RuntimeContext.hpp"
#include "LuaState.hpp"
#include "Transition.hpp"


namespace core {
	


	class TransitionManager : public initializable<TransitionManager, void, void, void, void>, public singleton<TransitionManager>, public updateable<TransitionManager, void, RuntimeContext>, public pausable<TransitionManager> {

	public:

		void addTransition(TransitionBase* transition);

		//lua bindings
		static int createTransition_bind(LuaState& lua);

		static int deleteTransition_bind(LuaState& lua);		

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		void updateImpl(float dt, RuntimeContext& context);

		void pauseImpl();

		void resumeImpl();

	private:

		std::vector<TransitionBase*> _transitions;

	};




} //end namespace core

#endif