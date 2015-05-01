#ifndef CORE_VERTEX_BUFFER_OBJECT_HPP
#define CORE_VERTEX_BUFFER_OBJECT_HPP

#include "BufferObject.hpp"

namespace core {

	template<typename T>
	class VertexBufferObject : public BufferObject<T, GL_ARRAY_BUFFER> {

	public:

		bool setVertexAttributePointer(GLuint index) {
			return setVertexAttributePointer(index, GL_FALSE, 0);
		}

		bool setVertexAttributePointer(GLuint index, GLboolean normalized, GLuint stride) {
			if (_isBound) {
				glVertexAttribPointer(index, _vertexDimension, getTypeEnum<T>() , normalized, 0, NULL);		
				return true;
			}
			else {
				error("Error attempting to set vertex attribute pointer: buffer object is not bound.");
				return false;
			}			
		}



	};


} //end namespace core

#endif