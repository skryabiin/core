#include "System.hpp"

namespace core {


	InitStatus System::initializeImpl() {

		if (this->getInitializedStatus() == InitStatus::INIT_TRUE) return InitStatus::INIT_TRUE;

		std::function<void(System*, FacetPauseEvent&)> facetPauseHandler = std::mem_fn(&System::handleFacetPauseEvent);
		_facetPauseFilter.init(this, facetPauseHandler);
		single<EventProcessor>().addFilter(&_facetPauseFilter);

		return InitStatus::INIT_TRUE;
	}

	InitStatus System::resetImpl() {

		single<EventProcessor>().removeFilter(&_facetPauseFilter);

		return InitStatus::INIT_FALSE;
	}

} //end namespace core