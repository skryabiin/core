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
				
				facetDimensionQuery.dimensions.setDimension(Dimension( facet.dimensions.w * facet.scale.x, facet.dimensions.h * facet.scale.y ));
				facetDimensionQuery.found = true;
				return false;
			}
		}
		return true;
	}

	TextureFacet& TextureRenderSystem2d::updateTexture(TextureChangeEvent& e) {
		for (auto& facet : _textureFacets) {
			if (facet.of() == e.entity && facet.id() == e.facetId) {
				facet.textureCoordinates = e.sourceTextureRect.getRect();
				facet.texture = single<ResourceManager>().getTexture(e.textureName);
				auto dc = DrawableChange{};
				dc.operation = DrawableChange::Operation::CHANGE_TEXTURE;				
				dc.facetId = facet.id();
				dc.layerId = _drawableLayerId;
				dc.textureCoordinates = facet.textureCoordinates;
				dc.texture = facet.texture;
				single<Renderer>().applyDrawableChange(dc);
				return facet;
				
			}
		}
		return _nullFacet;
	}

	void TextureRenderSystem2d::updateDrawablePosition(VisualFacet* vfacet) {
		auto tfacet = static_cast<TextureFacet*>(vfacet);
		auto& offset = vfacet->offset;
		auto& position = vfacet->position;
		auto& scale = vfacet->scale;
		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CHANGE_TEXTURE_POSITION;
		dc.facetId = vfacet->id();
		dc.layerId = _drawableLayerId;
		dc.zIndex = vfacet->position.z + offset.z;
		dc.targetRect.x = float(position.x + offset.x);
		dc.targetRect.y = float(position.y + offset.y);
		dc.targetRect.w = tfacet->textureCoordinates.w * scale.x;
		dc.targetRect.h = tfacet->textureCoordinates.h * scale.y;

		single<Renderer>().applyDrawableChange(dc);
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

		facet.position = position;
		facet.scale = scale;
		facet.offset = offset;
				
		facet.texture = single<ResourceManager>().getTexture(textureName);
		source = (source.h == 0 || source.w == 0) ? facet.texture->dimensions() : source;
		facet.dimensions.w = source.w;
		facet.dimensions.h = source.h;
		facet.textureCoordinates = source;

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CREATE_TEXTURE_DRAWABLE;
		dc.facetId = facet.id();
		dc.layerId = _drawableLayerId;
		dc.zIndex = position.z + offset.z;

		dc.camera = &_camera;
		dc.texture = facet.texture;
		dc.textureCoordinates = facet.textureCoordinates;				
		dc.targetRect.x = position.x + offset.x;
		dc.targetRect.y = position.y + offset.y;
		dc.targetRect.w = roundFloat(source.w * scale.x);
		dc.targetRect.h = roundFloat(source.h * scale.y);
		dc.shaderProgramName = "textureRender2d";
		single<Renderer>().applyDrawableChange(dc);

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

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
		dc.layerId = _drawableLayerId;
		for (auto& facet : _textureFacets) {
			dc.facetId = facet.id();
			single<Renderer>().applyDrawableChange(dc);
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
				auto dc = DrawableChange{};
				dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
				dc.facetId = it->id();
				dc.layerId = _drawableLayerId;

				single<Renderer>().applyDrawableChange(dc);
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
