#ifndef CORE_RESOURCE_HPP
#define CORE_RESOURCE_HPP

#include "Templates.hpp"

namespace core {


	class Resource : public equal_comparable<Resource>, public nameable, public taggable, public initializable<Resource, void, void, void, void> {

	public:

		virtual bool createImpl() = 0;
		virtual bool initializeImpl() = 0;
		virtual bool resetImpl() = 0;
		virtual bool destroyImpl() = 0;

		~Resource() { };

	};



} //end namespace core

#endif