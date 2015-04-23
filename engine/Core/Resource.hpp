#ifndef CORE_RESOURCE_HPP
#define CORE_RESOURCE_HPP

#include "Templates.hpp"

namespace core {


	class Resource : public equal_comparable<Resource>, public nameable, public taggable, public initializable<Resource, void, void> {

	public:

		virtual InitStatus initializeImpl() = 0;
		virtual InitStatus resetImpl() = 0;

		~Resource() { };

	};



} //end namespace core

#endif