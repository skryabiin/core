#include "TextureRenderSystem2d.hpp"
#include "Renderer.hpp"
#include "Core.hpp"
#include "Console.hpp"
#include "TextureChangeEvent.hpp"
#include "ColorChangeEvent.hpp"
#include "FacetPauseEvent.hpp"

namespace core {




	TextureRenderSystem2d::TextureRenderSystem2d() {
		int x = 0;
	}


	bool TextureRenderSystem2d::handleEvent(TextureChangeEvent& e) {

		updateTexture(e);
		
		
		return true;
	}

	bool TextureRenderSystem2d::handleEvent(FacetDimensionQuery& facetDimensionQuery) {

		for (auto& facet : _textureFacets) {
			if (facet.of() == facetDimensionQuery.entity && facet.id() == facetDimensionQuery.facetId) {

				SDL_Rect dim = facet.drawable.targetRect;
				facetDimensionQuery.dimensions.setDimension(Dimension{ dim.w, dim.h });
				facetDimensionQuery.found = true;
				return false;
			}
		}
		return true;
	}

	TextureFacet& TextureRenderSystem2d::updateTexture(TextureChangeEvent& e) {
		for (auto& facet : _textureFacets) {
			if (facet.of() == e.entity && facet.id() == e.facetId) {				
				facet.drawable.texture = single<ResourceManager>().getTexture(e.textureName);
				facet.drawable.sourceRect = e.sourceTextureRect.getRect();
				auto targetRect = facet.drawable.targetRect;
				facet.drawable.targetRect = SDL_Rect{ targetRect.x + facet.offset.x, targetRect.y + facet.offset.y, roundFloat(facet.drawable.sourceRect.w * facet.scale.x), roundFloat(facet.drawable.sourceRect.h * facet.scale.y) };
				return facet;
				single<Renderer>().updateDrawable(facet.drawable);
			}
		}
		return _nullFacet;
	}



	std::vector<Facet*> TextureRenderSystem2d::getFacets(Entity& e) {

		auto out = std::vector<Facet*>{};

		for (auto& facet : _textureFacets) {
			if (facet.of() == e) {
				out.push_back(&facet);
			}
		}

		if (_cameraFollow.of() == e) {
			out.push_back(&_cameraFollow);
		}

		return out;

	}

	TextureFacet& TextureRenderSystem2d::createTextureFacet(Entity& e, Pixel position, Pixel offset, Vec2 scale, SDL_Rect source, std::string textureName) {

		auto facet = TextureFacet{};
		facet.setOf(e);

		facet.scale = scale;
		facet.offset = offset;

		auto& drawable = facet.drawable;	
		drawable.drawableType = Drawable::DrawableType::TEXTURE;
		drawable.camera = getCamera();
		drawable.layerId = _drawableLayerId;
		drawable.texture = single<ResourceManager>().getTexture(textureName);
		source = (source.h == 0 || source.w == 0) ? drawable.texture->dimensions() : source;

		drawable.targetRect = SDL_Rect{ position.x + offset.x, position.y + offset.y, roundFloat(source.w * scale.x), roundFloat(source.h * scale.y) };

		

		drawable.sourceRect = source;

		drawable.zIndex = position.z + offset.z;
		drawable.id = single<Renderer>().createDrawable(drawable);

		_textureFacets.push_back(std::move(facet));
		return _textureFacets.back();
	}


	bool TextureRenderSystem2d::createImpl() {
		return RenderableSystem2d::createImpl();
	}

	bool TextureRenderSystem2d::initializeImpl() {


		return RenderableSystem2d::initializeImpl();
	}


	bool TextureRenderSystem2d::resetImpl() {
	
		_movingTextures.clear();

		for (auto& facet : _textureFacets) {
			single<Renderer>().destroyDrawable(facet.drawable);
		}

		_textureFacets.clear();
		_textureFacets.shrink_to_fit();

		return RenderableSystem2d::resetImpl();
	}

	bool TextureRenderSystem2d::destroyImpl() {
		return RenderableSystem2d::destroyImpl();
	}

	void TextureRenderSystem2d::destroyFacets(Entity& entity) {
		
		auto movingIt = _movingTextures.find(entity);
		if (movingIt != std::end(_movingTextures)) {
			_movingTextures.erase(movingIt);
		}

		for (auto it = std::begin(_textureFacets); it != std::end(_textureFacets); ++it) {

			if (it->of() == entity) {
				single<Renderer>().destroyDrawable(it->drawable);
				it = _textureFacets.erase(it);
				return;
			}

		}

	}

	int TextureRenderSystem2d::createFacet_bind(LuaState& lua) {

		auto systemName = lua.pullStack<std::string>(1);
		auto entityId = lua.pullStack<int>(2);		

		auto system = single<Core>().getSystemByName<TextureRenderSystem2d>(systemName);

		if (system != nullptr) {
			
			LuaRect source = lua["source"];			

			LuaPixel position = lua["position"];

			LuaVec2 scale = lua["scale"];

			LuaPixel offset = lua["offset"];
			
			std::string textureName = lua["textureName"];			
			
			auto& newFacet = system->createTextureFacet(entityId, position.getPixel(), offset.getPixel(), scale.getVec2(), source.getRect(), textureName);
			lua.pushStack(newFacet.id());
		}
		else {
			lua.pushStack(-1L);
		}
		return 1;
	}


} //end namespace core
