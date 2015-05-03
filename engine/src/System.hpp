#ifndef CORE_SYSTEM_HPP
#define CORE_SYSTEM_HPP


#include "Templates.hpp"
#include "RuntimeContext.hpp"
#include "Camera.hpp"
#include "Facet.hpp"
#include "FacetPauseEvent.hpp"
#include "EventProcessor.hpp"

class Entity;

namespace core {

	class System : public pausable<System>, public nameable, public initializable<System, void, void, void, void>, public EventListener<FacetPauseEvent> {
		friend class SystemManager;
	public:

		virtual bool createImpl() { return true; };
		virtual bool initializeImpl();
		virtual bool resetImpl();
		virtual bool destroyImpl() { return true; };

		virtual void pauseImpl() {};
		virtual void resumeImpl() {};		

		virtual bool handleEvent(FacetPauseEvent& e) = 0;

		virtual std::vector<Facet*> getFacets(Entity& e) {

			auto out = std::vector<Facet*>{};
			return out;
		}

		template <typename Facet_type>
		std::vector<Facet_type*> getFacetsByType(Entity& e) {
			auto out = std::vector<Facet_type*>{};
			
			auto bareFacets = getFacets(e);

			for (auto facet : bareFacets) {
				auto& a = *(facet->declaredTypeInfo());
				auto& b = typeid(Facet_type);
				if (*(facet->declaredTypeInfo()) == typeid(Facet_type)) {
					out.push_back(static_cast<Facet_type*>(facet));
				}

			}

			return out;
		}


		virtual void destroyFacets(Entity& e) = 0;

		virtual ~System() {};

	protected:

		System() : pausable<System>() {};		

	};




	struct ColorStateChange;



} //end namespace core


#endif