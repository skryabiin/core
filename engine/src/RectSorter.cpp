#include "RectSorter.hpp"
#include <algorithm>
namespace core {




	void RectSorter::addRect(long id, const SDL_Rect& rect) {
		auto re = RectElement{};
		re.id = id;
		re.rect = rect;
		_rects.push_back(re);
	
	}

	SortResults RectSorter::sort() {
		
		int numRects = _rects.size();

		auto sorted = _rects;

		std::sort(sorted.begin(), sorted.end(), [](RectElement a, RectElement b)->bool {
			return a.rect.h > b.rect.h;
		});

		int totalWidth = 0;
		int totalHeight = 0;
		int sourceArea = 0;
		int maxw = 0;
		int maxh = 0;
		for (auto& re : sorted) {
			totalWidth += re.rect.w;
			totalHeight += re.rect.h;
			sourceArea += re.rect.w * re.rect.h;
			maxw = (maxw > re.rect.w) ? maxw : re.rect.w;
			maxh = (maxh > re.rect.h) ? maxh : re.rect.h;
		}

		float x = 0;
		int dimHalvedDimensions = std::max(totalWidth, totalHeight);
		int dimMaxDimension = std::max(maxw, maxh);		
		int dim = std::max(dimHalvedDimensions, dimMaxDimension);

		auto stacks = std::vector<RectContainer>{};
		auto sr = SortResults{};
		auto stack = RectContainer{};
		stack.vertical = true;
		stack.height = dim;
		stack.width = dim;
		for (auto& re : sorted) {
			if(!stack.addToStack(re)) sr.failed = true;
		}


		stack.getElements(_rects);

		int maxx = 0;
		int maxy = 0;
		for (auto& r : _rects) {
			maxx = (maxx > r.rect.x) ? maxx : r.rect.x;
			maxy = (maxy > r.rect.y) ? maxy : r.rect.y;
		}
		int maxOutputDimension = std::max(maxx, maxy);
		int powerOf2Dimenson = nextPowerOf2(maxOutputDimension);
		sr.maxx = maxx;
		sr.maxy = maxy;
		sr.dimension = powerOf2Dimenson;
		sr.outputArea = powerOf2Dimenson * powerOf2Dimenson;
		sr.sourceArea = sourceArea;
		sr.wastedSpace = sr.outputArea - sourceArea;
		sr.optimizedPercentage = (sr.sourceArea * 1.0f) / (sr.outputArea * 1.0f);
		return sr;

	}




	SDL_Rect RectSorter::getRect(long id) {

		for (auto& re : _rects) {
			if (id == re.id) {
				return re.rect;
			}
		}
	}



} //end namespace core