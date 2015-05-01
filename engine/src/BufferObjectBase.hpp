#ifndef CORE_BUFFER_OBJECT_BASE_HPP
#define CORE_BUFFER_OBJECT_BASE_HPP

#include "GL/glew.h"
#include "Templates.hpp"

namespace core {


	class BufferObjectBase : public initializable<BufferObjectBase, GLushort, void, void, void> {
	public:

		bool createImpl(GLushort vertexDimension) {
			_vertexDimension = vertexDimension;
			return true;
		}

		bool initializeImpl() {			
			glGenBuffers(1, &_buffer);
			return true;
		}

		bool resetImpl() {
			glDeleteBuffers(1, &_buffer);
			return true;
		}

		bool destroyImpl() {
			return true;
		}

		GLushort getNumVertices() const {
			return _numVertices;
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