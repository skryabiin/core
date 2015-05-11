#include "VertexArrayObject.hpp"

namespace core {


	VertexArrayObject::VertexArrayObject() {

	}

	VertexArrayObject::~VertexArrayObject() {
	}

	bool VertexArrayObject::createImpl() {
		return (_indices.create(1) == InitStatus::CREATE_TRUE);
	}

	bool VertexArrayObject::initializeImpl() {		
		glGenVertexArrays(1, &_id);
		return (_indices.initialize() == InitStatus::INIT_TRUE);
	}

	bool VertexArrayObject::resetImpl() {
		if (!unbind()) return false;

		glDeleteVertexArrays(1, &_id);
		return (_indices.reset() == InitStatus::CREATE_TRUE);
	}

	bool VertexArrayObject::destroyImpl() {
		return (_indices.destroy() == InitStatus::CREATE_FALSE);
	}

	bool VertexArrayObject::bindImpl() {	

		glBindVertexArray(_id);
		
		if (!_program->bind()) return false;
		if (!_indices.bind()) return false;

		return enableVertexArrayAttributes();

	}

	void VertexArrayObject::draw(GLenum mode) {		
		glDrawElements(mode, _indices.getNumVertices(), GL_UNSIGNED_SHORT, NULL);		
		//glDrawArrays(mode, 0, 12 * 3);
	}


	bool VertexArrayObject::setIndices(IndexBufferObject& ibo)  {		
		_indices = ibo;
		if (!_indices.isBound() && isBound()) {
			_indices.bind();
		}
		return true;
	}

	bool VertexArrayObject::setProgram(ShaderProgram* program) {
		_program = program;
		return true;
	}

	bool VertexArrayObject::enableVertexArrayAttributes() {

		for (auto& vertexAttr : _program->_vertexAttributes) {
			glEnableVertexAttribArray(vertexAttr.second.index);
		}
		_isVAenabled = true;
		return true;
	}

	bool VertexArrayObject::disableVertexArrayAttributes() {		
		for (auto& vertexAttr : _program->_vertexAttributes) {
			glDisableVertexAttribArray(vertexAttr.second.index);
		}		
		_isVAenabled = false;
		return true;
	}

	bool VertexArrayObject::unbindImpl() {	
		glBindVertexArray(NULL);

		if (!_program->unbind()) return false;
		if (!_indices.unbind()) return false;

		return disableVertexArrayAttributes();

	}

	IndexBufferObject& VertexArrayObject::indices() {
		return _indices;
	}

}