#ifndef CORE_FRAME_BUFFER_HPP
#define CORE_FRAME_BUFFER_HPP

#include "Templates.hpp"
#include "Geometry.hpp"
#include "GL/glew.h"

namespace core {

	class FrameBuffer : public initializable<FrameBuffer, Dimension, void, void, void>, public bindable<FrameBuffer> {

	public:


		//initializable
		bool createImpl(Dimension frameDimensions);
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		//bindable
		bool bindImpl();
		bool unbindImpl();

		void bindTexture();
		void unbindTexture();	

		GLuint id() const;

	private:

		Dimension _frameDimensions;
		GLuint _frameBufferId;
		GLuint _frameTextureId;


	};


} //end namespace core

#endif