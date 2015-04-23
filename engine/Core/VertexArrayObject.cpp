#include "VertexArrayObject.hpp"

namespace core {


	InitStatus VertexArrayObject::initializeImpl(bool autoBindMode) {
		_autoBindMode = autoBindMode;
		glGenVertexArrays(1, &_id);
		return InitStatus::INIT_TRUE;
	}

	InitStatus VertexArrayObject::resetImpl() {
		glDeleteVertexArrays(1, &_id);
		return InitStatus::INIT_FALSE;

	}


	bool VertexArrayObject::bindImpl() {		
		glBindVertexArray(_id);
		auto glError = glGetError();
		if (glError != GL_NO_ERROR) {
			error("In VertexArrayobject::bind1, error code ", glError);
		}
		_program->bind();
		glError = glGetError();
		if (glError != GL_NO_ERROR) {
			error("In VertexArrayobject::bind2, error code ", glError);
		}
		return true;

	}

	void VertexArrayObject::draw(GLenum mode) {
		if (_autoBindMode) bind();
		glDrawElements(mode, _indices->getNumVertices(), GL_UNSIGNED_SHORT, NULL);
		//glDrawArrays(mode, 0, 12 * 3);
		auto glError = glGetError();
		if (glError != GL_NO_ERROR) {
			error("In VertexArrayobject::draw, error code ", glError);
		}
		if (_autoBindMode) unbind();
	}


	bool VertexArrayObject::setProgram(ShaderProgram* program) {
		_program = program;
		return true;
	}
	bool VertexArrayObject::enableSetAttributes() {
		if (_autoBindMode) bind();
		_program->enableVertexArrayAttributes();
		return true;
	}

	bool VertexArrayObject::disableSetAttributes() {
		_program->disableVertexArrayAttributes();
		if (_autoBindMode) unbind();
		return true;
	}
	bool VertexArrayObject::unbindImpl() {		
		_program->unbind();
		auto glError = glGetError();
		if (glError != GL_NO_ERROR) {
			error("In VertexArrayobject::unbind1, error code ", glError);
		}
		glBindVertexArray(NULL);

		return true;
	}

}