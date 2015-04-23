#ifndef CORE_RUNTIME_CONTEXT_HPP
#define CORE_RUNTIME_CONTEXT_HPP

#include <vector>

namespace core {



class RuntimeContext {

public:

	//initial dt
	int dt0;
	
	//dt with any modifiers
	int dt;

	//dt converted to float seconds
	float floatDt() {
		return dt / 1000.0f;
	}

};



} //end namespace core


#endif 
