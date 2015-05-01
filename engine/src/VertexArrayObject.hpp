#ifndef CORE_VERTEX_ARRAY_OBJECT_HPP
#define CORE_VERTEX_ARRAY_OBJECT_HPP

#include "ShaderProgram.hpp"
#include "BufferObjectBase.hpp"
#include <map>
#include <cstdarg>

namespace core {




	class VertexArrayObject : public initializable<VertexArrayObject, void, bool, void, void>, public bindable<VertexArrayObject> {

	public:

		bool createImpl();

		bool initializeImpl(bool autoBindMode); 

		bool resetImpl();
		
		bool destroyImpl();

		bool bindImpl();

		bool enableSetAttributes();

		bool disableSetAttributes();

		bool setIndices(IndexBufferObject& ibo)  {
			if (!ibo.isBound()) {
				error("Attempting to set indices with unbound IBO.");
				return false;
			}
			if (_autoBindMode) bind();
			_indices = &ibo;
			if (_autoBindMode) unbind();
			return true;
		}

		bool unbindImpl();

		void draw(GLenum mode);


		template<typename T>
		void setUniformVariableMatrix(std::string name, T& matrix) {
			if (_autoBindMode) bind();
			_program->setUniformVariableMatrix(name, matrix);
			if (_autoBindMode) unbind();
			
		}

		template <typename T>
		void setVertexArrayAttribute(std::string attributeName, VertexBufferObject<T>& vbo) {
			setVertexArrayAttribute(attributeName, vbo, GL_FALSE);
		}

		template <typename T>
		void setVertexArrayAttribute(std::string attributeName, VertexBufferObject<T>& vbo, GLenum normalized) {
			auto& va = _program->_vertexAttributes[attributeName];
			vbo.setVertexAttributePointer(va.index, normalized, 0);														
		}

		template<typename T>
		void setUniformVariable(std::string name, T& value) {
			if (_autoBindMode) bind();
			_program->setUniformVariable(name, value);
			if (_autoBindMode) unbind();
		}

		template<typename T>
		void setUniformVariableArray(std::string name, std::vector<T>& values) {
			if (_autoBindMode) bind();
			_program->setUniformVariableArray(name, values);
			if (_autoBindMode) unbind();
		}

		bool setProgram(ShaderProgram* program);

	private:
		GLuint _id;
		bool _autoBindMode;
		IndexBufferObject* _indices;
		ShaderProgram* _program;
	};


} //end namespace core

#endif