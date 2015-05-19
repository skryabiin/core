#include "FrameBuffer.hpp"

#include "Console.hpp"

namespace core {



	bool FrameBuffer::createImpl(Dimension frameDimensions) {
		_frameDimensions = frameDimensions;
		return true;
	}


	bool FrameBuffer::initializeImpl() {		
		glGenTextures(1, &_frameTextureId);
		glBindTexture(GL_TEXTURE_2D, _frameTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			_frameDimensions.w, _frameDimensions.h,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			NULL);		

		glGenFramebuffers(1, &_frameBufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _frameBufferId);
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _frameTextureId, 0);

		// define the index array for the outputs
		GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);


		GLenum checkStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (checkStatus != GL_FRAMEBUFFER_COMPLETE) {			
			error("Error initializing frame buffer.");
			return false;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return true;

	}

	bool FrameBuffer::resetImpl() {
		glDeleteFramebuffers(1, &_frameBufferId);
		glDeleteTextures(1, &_frameTextureId);

		return true;
	}

	bool FrameBuffer::destroyImpl() {

		return true;
	}


	bool FrameBuffer::bindImpl() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _frameBufferId);

		return true;
	}

	bool FrameBuffer::unbindImpl() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return true;

	}

	void FrameBuffer::bindTexture() {
		glBindTexture(GL_TEXTURE_2D, _frameTextureId);
	}

	void FrameBuffer::unbindTexture() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	GLuint FrameBuffer::id() const {
		return _frameBufferId;
	}

} //end namespace core