#include "TransitionManager.hpp"
#include "TransitionRegistry.hpp"
#include "Core.hpp"

namespace core {


	bool TransitionManager::createImpl() {
		single<Core>().lua().bindFunction("createTransition_bind", TransitionManager::createTransition_bind);
		return true;
	} 

	bool TransitionManager::initializeImpl() {

		return true;
	}

	bool TransitionManager::resetImpl() {

		for (auto transition : _transitions) {
			delete transition;
		}

		_transitions.clear();
		return true;
	}

	bool TransitionManager::destroyImpl() {

		return true;
	}

	void TransitionManager::updateImpl(float dt, RuntimeContext& context) {
		if (isPaused()) return;

		bool done;		
		auto it = std::begin(_transitions);
		auto endIt = std::end(_transitions);

		while (it != endIt) {
			done = (*it)->update(dt);
			if (done) {
				delete *it;
				it = _transitions.erase(it);	
				endIt = std::end(_transitions);
			}
			else {
				++it;
			}
		}

	}

	void TransitionManager::pauseImpl() {

	}

	void TransitionManager::resumeImpl() {
	}


	void TransitionManager::addTransition(TransitionBase* transition) {
		_transitions.push_back(transition);
	}

	int TransitionManager::createTransition_bind(LuaState& lua) {

		std::string transitionType = lua["typeName"];
	

		auto it = TransitionRegistry::registeredLuaHandlers().find(transitionType);
		
		if (it != TransitionRegistry::registeredLuaHandlers().end()) {

			single<TransitionManager>().addTransition(it->second(lua));

		}

		return 0;

	}

	int TransitionManager::deleteTransition_bind(LuaState& lua) {

		return 0;
	}


} //end namespace core