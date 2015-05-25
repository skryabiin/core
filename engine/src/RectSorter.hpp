#ifndef CORE_RECT_SORTER_HPP
#define CORE_RECT_SORTER_HPP

#include <vector>
#include <SDL.h>
#include "Math.hpp"

namespace core {



	struct RectElement {

		long id;
		SDL_Rect rect;
		bool sorted;
	};

	struct RectContainer {

		RectContainer() : xbase{ 0 }, ybase{ 0 }, width{ 0 }, height{ 0 }, span{ 0 }, vertical{ false }, filled{ false } {}

		std::vector<RectContainer> containers;
		RectElement element;
		bool vertical;
		bool filled;
		int span;
		int height;
		int width;
		int xbase;
		int ybase;


		bool addToStack(RectElement& re) {
			if (filled) return false;
			if (re.rect.w == width && re.rect.h == height) {
				element = re;
				element.rect.x = xbase;
				element.rect.y = ybase;
				filled = true;
				return true;
			}
			for (auto& c : containers) {
				if(c.addToStack(re)) return true;				
			}
			if (vertical) {
				if ((re.rect.w <= width) && (re.rect.h + span <= height)) {
					auto newRc = RectContainer{};
					newRc.vertical = false;
					newRc.width = width;
					newRc.height = re.rect.h;
					newRc.xbase = xbase;
					newRc.ybase = ybase + span;
					span += re.rect.h;
					if (newRc.addToStack(re)) {
						containers.push_back(newRc);
						return true;
					}
				}
			}
			else {
				if ((re.rect.h <= height) && (re.rect.w + span <= width)) {

					auto newRc = RectContainer{};
					newRc.vertical = true;
					newRc.width = re.rect.w;
					newRc.height = height;
					newRc.xbase = xbase + span;
					newRc.ybase = ybase;
					span += re.rect.w;
					if (newRc.addToStack(re)) {
						containers.push_back(newRc);
						return true;
					}
				}
			}
			return false;
		}

		void getElements(std::vector<RectElement>& elements) {
			if (filled) {
				for (auto& e : elements) {
					if (e.id == element.id) {
						e.rect = element.rect;
						return;
					}
				}
			}

			for (auto& c : containers) {
				c.getElements(elements);
			}
		}


	};

	struct SortResults {
		
		int sourceArea;
		int outputArea;
		int wastedSpace;
		float optimizedPercentage;
		bool failed;
		int dimension;
		int maxx;
		int maxy;

	};

	class RectSorter {

	public:

		RectSorter() : _currentIndex{ 0 } {}

		void addRect(long id, const SDL_Rect& re);

		SortResults sort();

		SDL_Rect getRect(long id);

	private:

		unsigned short _currentIndex;
		std::vector<RectElement> _rects;

		std::vector<RectElement> _sortedRects;




	};


} //end namespace core

#endif