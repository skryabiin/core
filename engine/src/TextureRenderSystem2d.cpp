#include "TextureRenderSystem2d.hpp"
#include "Renderer.hpp"
#include "Core.hpp"
#include "Console.hpp"
#include "TextureChangeEvent.hpp"
#include "ColorChangeEvent.hpp"
#include "FacetPauseEvent.hpp"

namespace core {


/*	void TextureRenderSystem::renderImpl() {

		auto d = Drawable{};

		for (auto& texture : _textureFacets) {

			if (texture.isPaused() || (_currentColorId >= 0 && texture.colorId >= 0 && texture.colorId != _currentColorId)) continue;

			d.sourceRect = texture.sourceRect;
			d.targetRect = texture.targetRect;
			d.texture = texture.texture;
			d.zIndex = texture.z;
			//d.scale = texture.scale;

			single<Renderer>().render(d, _camera);

		}

	}*/


	EventProcessor::EventRegistration<ColorChangeEvent> colorChangeEventReg{};

	TextureRenderSystem2d::TextureRenderSystem2d() {

	}


	void TextureRenderSystem2d::handleTextureChange(TextureChangeEvent& e) {

		updateTexture(e);
		
		

	}

	TextureFacet& TextureRenderSystem2d::updateTexture(TextureChangeEvent& e) {
		for (auto& facet : _textureFacets) {
			if (facet.of() == e.entity && facet.id() == e.facetId) {				
				facet.drawable.texture = single<ResourceManager>().getTexture(e.textureName);
				facet.drawable.sourceRect = e.sourceTextureRect.getRect();
				auto targetRect = facet.drawable.targetRect;
				facet.drawable.targetRect = SDL_Rect{ targetRect.x + facet.offset.x, targetRect.y + facet.offset.y, roundFloat(facet.width * facet.scale.x), roundFloat(facet.height * facet.scale.y) };
				return facet;
				single<Renderer>().updateDrawable(facet.drawable);
			}
		}
		return _nullFacet;
	}

	void TextureRenderSystem2d::handleEntityLayerQuery(EntityLayerQuery& entityLayerQuery) {
		for (auto& facet : _textureFacets) {
			if (facet.of() == entityLayerQuery.entity) {
				if (_drawableLayerId > entityLayerQuery.layerId) {
					entityLayerQuery.layerId = _drawableLayerId;
					entityLayerQuery.found = true;
				}
			}
		}


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

	void TextureRenderSystem2d::updateDrawPosition(PositionChangeEvent& positionChange) {



		auto moveIt = _movingTextures.find(positionChange.entity);

		TextureFacet* texture = nullptr;

		if (moveIt == std::end(_movingTextures)) {
			for (unsigned i = 0; i < _textureFacets.size(); i++) {
				if (_textureFacets[i].of() == positionChange.entity) {
					texture = &_textureFacets[i];
					break;
				}
			}
			if (texture == nullptr) return;

			_movingTextures.insert(std::pair<Entity, TextureFacet*>(positionChange.entity, texture));

		}
		else {
			texture = moveIt->second;
		}

		auto& d = texture->drawable;

		auto p = positionChange.position.getPixel();
		auto dp = Pixel{ d.targetRect.x, d.targetRect.y, d.zIndex };
		auto outP = (positionChange.relative) ? p + dp : p + texture->offset;

		d.targetRect.x = outP.x;
		d.targetRect.y = outP.y;


		d.zIndex = outP.z;

		single<Renderer>().updateDrawable(d);

		//check if this is the entity the camera is centered on
		if (_cameraFollow.of() == positionChange.entity && !_cameraFollow.isPaused()) {			
			auto x = outP.x + texture->width * 0.5f;
			auto y = outP.y + texture->height * 0.5f;
			snapCameraToCoordinates(x, y);
		}



		
	}

	void TextureRenderSystem2d::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {		

		for (auto& facet : _textureFacets) {
			if (facet.of() == pauseEvent.entity) {
				if (pauseEvent.facetId == -1 || facet.id() == pauseEvent.facetId) {
					if (pauseEvent.paused) {
						facet.pause();
						single<Renderer>().pauseDrawable(facet.drawable);
					}
					else {
						facet.resume();
						single<Renderer>().resumeDrawable(facet.drawable);
					}
					if (facet.id() == pauseEvent.facetId) break;
				}
			}
		}
	}

	void TextureRenderSystem2d::updateColor(ColorChangeEvent& colorChangeEvent) {

		
		for (auto& facet : _textureFacets) {
			if (facet.of() == colorChangeEvent.entity && facet.id() == colorChangeEvent.facetId) {	
				auto& d = facet.drawable;
				d.texture->setBlendMode(colorChangeEvent.blendMode);
				if (colorChangeEvent.doModulateColor) {
					Color newColor = colorChangeEvent.color.getColor();
					if (newColor != Color::get(Color::CommonColor::WHITE)) {
						d.doModulateColor = true;
						d.textureModulateColor = colorChangeEvent.color.getColor();
					}
					else {
						d.doModulateColor = false;
					}
				}
				else {
					d.doModulateColor = false;
				}
				single<Renderer>().updateDrawable(d);
			}
		}		
	}

	TextureFacet& TextureRenderSystem2d::createTextureFacet(Entity& e, Pixel position, Pixel offset, Dimension dimensions, Vec2 scale, SDL_Rect source, std::string textureName) {

		auto facet = TextureFacet{};
		facet.setOf(e);

		facet.scale = scale;
		facet.offset = offset;
		facet.width = dimensions.w;
		facet.height = dimensions.h;

		auto& drawable = facet.drawable;
		drawable.entityId = e;
		drawable.camera = getCamera();
		drawable.layerId = _drawableLayerId;
		
		drawable.targetRect = SDL_Rect{ position.x + offset.x, position.y + offset.y, roundFloat(dimensions.w * scale.x), roundFloat(dimensions.h * scale.y) };

		drawable.texture = single<ResourceManager>().getTexture(textureName);
		
		drawable.sourceRect = (source.h == 0 || source.w == 0) ? drawable.texture->dimensions() : source;

		drawable.zIndex = position.z + offset.z;
		drawable.id = single<Renderer>().createDrawable(drawable);

		_textureFacets.push_back(std::move(facet));
		return _textureFacets.back();
	}

	InitStatus TextureRenderSystem2d::initializeImpl() {

		std::function<void(TextureRenderSystem2d*, TextureChangeEvent&)> callback = std::mem_fn(&TextureRenderSystem2d::handleTextureChange);

		_textureChangeFilter.init(this, callback);
		single<EventProcessor>().addFilter(&_textureChangeFilter);



		return RenderableSystem2d::initializeImpl();
	}


	InitStatus TextureRenderSystem2d::resetImpl() {
		single<EventProcessor>().removeFilter(&_textureChangeFilter);
		_movingTextures.clear();

		for (auto& facet : _textureFacets) {
			single<Renderer>().destroyDrawable(facet.drawable);
		}

		_textureFacets.clear();
		_textureFacets.shrink_to_fit();

		return RenderableSystem2d::resetImpl();
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

			LuaDimension dimensions = lua["dimensions"];

			LuaPixel position = lua["position"];

			LuaVec2 scale = lua["scale"];

			LuaPixel offset = lua["offset"];
			
			std::string textureName = lua["textureName"];			
			
			auto& newFacet = system->createTextureFacet(entityId, position.getPixel(), offset.getPixel(), dimensions.getDimension(), scale.getVec2(), source.getRect(), textureName);
			lua.pushStack(newFacet.id());
		}
		else {
			lua.pushStack(-1L);
		}
		return 1;
	}

	EventProcessor::EventRegistration<TextureChangeEvent> textureChangeEventReg{};
	EventProcessor::EventRegistration<FacetPauseEvent> facetPauseEventReg{};

} //end namespace core
