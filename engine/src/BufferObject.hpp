#ifndef CORE_BUFFER_OBJECT_HPP
#define CORE_BUFFER_OBJECT_HPP

#include "BufferObjectBase.hpp"
#include <vector>

namespace core {

	template <typename T, GLenum BUFFER_TYPE>
	class BufferObject : public BufferObjectBase, public bindable<BufferObject<T, BUFFER_TYPE>> {

	public:



		
		bool bindImpl() {
			if (getInitializedStatus() == InitStatus::INIT_TRUE) {
				glBindBuffer(BUFFER_TYPE, _buffer);
				return true;
			}
			else {
				return false;
			}
		}



		bool unbindImpl() {
			if (getInitializedStatus() == InitStatus::INIT_TRUE) {
				glBindBuffer(BUFFER_TYPE, NULL);
				return true;
			}
			else {
				return false;
			}
		}

		bool buffer(GLushort numVertices, GLuint offset, std::vector<T>& values) {
			return buffer(numVertices, offset, values, offset);
		}

		bool buffer(GLushort numVertices, GLuint bufferOffset, std::vector<T>& values, GLuint vectorOffset) {
			return buffer(numVertices, bufferOffset, &values[vectorOffset]);
		}

		bool buffer(GLushort numVertices, GLuint offset, void* data) {
			return buffer(numVertices, offset, data, _useType);
		}

		bool buffer(GLushort numVertices, GLuint offset, void* data, GLenum usage) {
			if (_isBound) {
				GLuint numBytes = numVertices * _vertexDimension * sizeof(T);
				glBufferSubData(BUFFER_TYPE, offset * sizeof(T), numBytes, data);
				return true;
			}
			return false;
		}

		bool buffer(std::vector<T>& values) {			
			return buffer(values, _useType);
		}

		bool buffer(std::vector<T>& values, GLenum usage) {		
			if (values.empty()) return false;
			return buffer(values.size() / _vertexDimension, &values[0], usage);
		}

		
		bool buffer(GLushort numVertices, void* data) {			
			return buffer(numVertices, data, _useType);
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