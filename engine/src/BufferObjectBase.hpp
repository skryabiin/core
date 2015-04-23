#ifndef CORE_BUFFER_OBJECT_BASE_HPP
#define CORE_BUFFER_OBJECT_BASE_HPP

#include "GL/glew.h"
#include "Templates.hpp"

namespace core {


	class BufferObjectBase : public initializable<BufferObjectBase, GLushort, void> {
	public:

		InitStatus initializeImpl(GLushort vertexDimension) {
			_vertexDimension = vertexDimension;
			glGenBuffers(1, &_buffer);
			return InitStatus::INIT_TRUE;
		}

		GLushort getNumVertices() const {
			return _numVertices;
		}



		InitStatus resetImpl() {
			glDeleteBuffers(1, &_buffer);
			
			return InitStatus::INIT_FALSE;
		}

		GLushort getVertexDimension() const {
			return _vertexDimension;
		}

	protected:

		
		GLuint _buffer;
		GLushort _numVertices;
		GLushort _vertexDimension;
		GLenum _bufferTarget;
	};


} //end namespace core

#endif