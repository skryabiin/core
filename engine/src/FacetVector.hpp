#ifndef CORE_FACET_VECTOR_HPP
#define CORE_FACET_VECTOR_HPP

#include <vector>
#include <stdlib.h>
#include <memory.h>
#include <functional>
#include "Templates.hpp"

namespace core {


	template <typename FACET_TYPE>
	class FacetVector : public initializable<FacetVector<FACET_TYPE>, unsigned short, unsigned short, void, void> {

	public:

		FacetVector() : _size{ 0 }, _reserveIncrement{ 0 }, _reserved{ 0 } {}

		FacetVector(const FacetVector& other) = delete;
		FacetVector(const FacetVector&& other) = delete;

		bool createImpl(unsigned short reserveIncrementSize) {
			if (reserveIncrementSize == 0) return false;
			_reserveIncrement = reserveIncrementSize;
			return true;
		}

		bool initializeImpl(unsigned short reserveIncrements) {
			reserveIncrements = (reserveIncrements == 0) ? 1 : reserveIncrements;
			_reserved = reserveIncrements;
			_facets = (FACET_TYPE*)calloc(_reserved * _reserveIncrement, sizeof(FACET_TYPE));
			return (_facets != NULL) ? true : false;
		}

		bool resetImpl() {
			free(_facets);
			return true;
		}

		bool destroyImpl() {
			return true;
		}

		~FacetVector() {
			reset();
			destroy();
		}

		std::vector<Facet*> getBaseFacets(int entityId) {
			auto out = std::vector<Facet*>{};
			FACET_TYPE* thisFacet;
			for (unsigned short i = 0; i < _size; ++i) {
				thisFacet = &_facets[i];
				if (thisFacet->of() == entityId) {
					out.push_back(static_cast<Facet*>(thisFacet));
				}
			}
			return out;
		}

		std::vector<FACET_TYPE*>* getFacets(int entityId) {
			_facetQueryResults.clear();
			FACET_TYPE* thisFacet;
			for (unsigned short i = 0; i < _size; ++i) {
				thisFacet = &_facets[i];
				if (thisFacet->of() == entityId) {
					_facetQueryResults.push_back(thisFacet);
				}
			}

			return &_facetQueryResults;
		}

		FACET_TYPE* operator[](const int& facetId) {
			return getFacet(facetId);
		}

		FACET_TYPE* getFacet(const int& facetId) {
			auto index = _getFacetIndex(facetId, 0, _size - 1);
			return (index != _size) ? &_facets[index] : nullptr;
		}

		FACET_TYPE* addFacet(FACET_TYPE& facet) {
			_checkReserve(++_size);
			_facets[_size - 1] = facet;
			return &_facets[_size - 1];
		}


		void forEach(std::function<void(FACET_TYPE*)>&& f) {
			for (unsigned short i = 0; i < _size; ++i) {
				f(&_facets[i]);
			}
		}


		std::vector<int>* getFacetIds(const int& entityId) {
			_facetIdQueryResults.clear();
			FACET_TYPE* thisFacet;
			for (unsigned short i = 0; i < _size; ++i) {
				thisFacet = &_facets[i];
				if (thisFacet->of() == entityId) {
					_facetIdQueryResults.push_back(thisFacet->id());
				}
			}
			return &_facetIdQueryResults;
		}

		void removeFacets(const int& entityId) {
			getFacetIds(entityId);
			for (auto facetId : _facetIdQueryResults) {
				removeFacet(facetId);
			}
		}

		void removeFacet(int facetId) {
			auto index = _getFacetIndex(facetId, 0, _size - 1);
			if (index < _size - 1) {
				memcpy(&_facets[index], &_facets[index + 1], (_size - 1 - index) * sizeof(FACET_TYPE));
			}

			--_size;

		}

		unsigned short size() const {
			return _size;
		}

		unsigned short reservedSize() const {
			return _reserved;
		}

		unsigned short reservedIncrement() const {
			return _reservedIncrement;
		}

		

	private:		

		unsigned short _getFacetIndex(const int& facetId, unsigned short a, unsigned short b) {
			auto half = (b - a) / 2 + a;
			auto* facetAtHalf = &_facets[half];
			if (facetAtHalf->id() == facetId) {
				return half;
			}
			else if (b - a == 0) {
				return _size;
			}
			else if (facetId < facetAtHalf->id()) {
				return _getFacetIndex(facetId, a, half - 1);
			}
			else {
				return _getFacetIndex(facetId, half + 1, b);
			}

		}

		void _checkReserve(unsigned short newSize) {
			if (newSize > _reserved * _reserveIncrement) {
				auto temp = (FACET_TYPE*)realloc(_facets, _reserveIncrement * (++_reserved) * sizeof(FACET_TYPE));
				if (temp != NULL) {
					_facets = temp;
				}
			}
		}

		unsigned short _size;
		unsigned short _reserved;
		unsigned short _reserveIncrement;
		std::vector<FACET_TYPE*> _facetQueryResults;
		std::vector<int> _facetIdQueryResults;

		FACET_TYPE* _facets;

	};


} //end namespace core

#endif