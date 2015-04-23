#ifndef CORE_BUFFER_OBJECT_HPP
#define CORE_BUFFER_OBJECT_HPP

#include "BufferObjectBase.hpp"
#include <vector>

namespace core {

	template <typename T, GLenum BUFFER_TYPE>
	class BufferObject : public BufferObjectBase, public bindable<BufferObject<T, BUFFER_TYPE>> {

	public:

		
		bool buffer(std::vector<T>& values) {
			return buffer(values, GL_STATIC_DRAW);
		}

		
		bool bindImpl() {
			glBindBuffer(BUFFER_TYPE, _buffer);
			return true;
		}



		bool unbindImpl() {
			glBindBuffer(BUFFER_TYPE, NULL);

			return true;
		}



		bool buffer(std::vector<T>& values, GLenum usage) {			
			return buffer(values.size() / _vertexDimension, &values[0], usage);
		}

		
		bool buffer(GLushort numVertices, void* data) {			
			return buffer(numVertices * _vertexDimension, data, GL_STATIC_DRAW);
		}

		
		bool buffer(int numVertices, void* data, GLenum usage) {
			if (_isBound) {
				_numVertices = numVertices;
				GLuint numBytes = numVertices * _vertexDimension * sizeof(T);
				glBufferData(BUFFER_TYPE, numBytes, data, usage);
				return true;
			}
			return false;
		}

		template <typename U>
		GLenum getTypeEnum() {
			return GL_FLOAT;
		}

		template<>
		GLenum getTypeEnum<GLfloat>() {
			return GL_FLOAT;
		}
		template<>
		GLenum getTypeEnum<GLint>() {
			return GL_INT;
		}

		template<>
		GLenum getTypeEnum<GLuint>() {
			return GL_UNSIGNED_INT;
		}

		template<>
		GLenum getTypeEnum<GLushort>() {
			return GL_UNSIGNED_SHORT;
		}

		template<>
		GLenum getTypeEnum<GLubyte>() {
			return GL_UNSIGNED_BYTE;
		}


	protected: 



	};

} //end namespace core

#endif