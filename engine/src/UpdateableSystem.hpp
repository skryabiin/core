#ifndef CORE_UPDATEABLE_SYSTEM_HPP
#define CORE_UPDATEABLE_SYSTEM_HPP


#include "RuntimeContext.hpp"
#include "System.hpp"

namespace core {


	class UpdateableSystem : public virtual System, public updateable<UpdateableSystem, void, RuntimeContext> {

	public:

		virtual void updateImpl(float dt, RuntimeContext& context) = 0;

		virtual ~UpdateableSystem() {};
	};



} //end namespace core

#endif