#include "VertexArrayObject.hpp"

namespace core {

	bool VertexArrayObject::createImpl() {
		return true;
	}

	bool VertexArrayObject::initializeImpl(bool autoBindMode) {
		_autoBindMode = autoBindMode;
		glGenVertexArrays(1, &_id);

		return true;
	}

	bool VertexArrayObject::resetImpl() {
		if (!unbind()) return false;

		glDeleteVertexArrays(1, &_id);
		return true;

	}

	bool VertexArrayObject::destroyImpl() {
		return true;
	}

	bool VertexArrayObject::bindImpl() {	

		glBindVertexArray(_id);

		return _program->bind();

	}

	void VertexArrayObject::draw(GLenum mode) {
		if (_autoBindMode) bind();
		glDrawElements(mode, _indices->getNumVertices(), GL_UNSIGNED_SHORT, NULL);
		//glDrawArrays(mode, 0, 12 * 3);

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
		bool out = true;
		glBindVertexArray(NULL);

		if (!_program->unbind()) {
			out = false;
		}


		return out;
	}

}