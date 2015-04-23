#ifndef CORE_FACET_H
#define CORE_FACET_H

#include <typeinfo>
#include "Templates.hpp"
#include "Event.hpp"
#include "Geometry.hpp"
#include "Entity.hpp"

namespace core {


	class Facet : public pausable<Facet>, public equal_comparable<Facet> {

	public:
		Facet() : _of{ -1 } {
			_typeInfo = &typeid(*this);
		}
		void setDeclaredTypeInfo(const std::type_info* info) {
			_typeInfo = info;
		}

		const std::type_info* declaredTypeInfo() {
			return _typeInfo;
		}

		void setOf(Entity of) {
			_of = of;
		}

		Entity of() {
			return _of;
		}

		void pauseImpl() {};

		void resumeImpl() {};

	protected:

		Entity _of;
		const std::type_info* _typeInfo;
	};	



} //end namespace core

#endif