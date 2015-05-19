#ifndef CORE_VERTEX_ARRAY_OBJECT_HPP
#define CORE_VERTEX_ARRAY_OBJECT_HPP

#include "ShaderProgram.hpp"
#include "BufferObjectBase.hpp"
#include <map>
#include <cstdarg>

namespace core {




	class VertexArrayObject : public initializable<VertexArrayObject, void, void, void, void>, public bindable<VertexArrayObject> {

	public:

		VertexArrayObject();

		bool createImpl();

		bool initializeImpl(); 

		bool resetImpl();
		
		bool destroyImpl();

		bool bindImpl();

		bool enableVertexArrayAttributes();

		bool disableVertexArrayAttributes();

		bool setIndices(IndexBufferObject& ibo);		

		bool unbindImpl();

		void draw(GLenum mode);

		~VertexArrayObject();

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
			auto& var = _program->_uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_program->_programId, var.name.c_str());
			}
			setUniformVariable(var.location, value);
		}

		template<typename T>
		void setUniformVariable(GLint& location, T& value) {

		}

		template<>
		void setUniformVariable<GLfloat>(GLint& location, GLfloat& value) {
			glUniform1f(location, value);
		}

		template<>
		void setUniformVariable<GLint>(GLint& location, GLint& value) {
			glUniform1i(location, value);
		}


		template<typename T>
		void setUniformVariableMatrix(std::string name, T& matrix) {
			auto& var = _program->_uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_program->_programId, var.name.c_str());
			}
			setUniformVariableMatrix(var.location, matrix);
		}

		template<typename T>
		void setUniformVariableMatrix(GLint& location, T& matrix) {

		}

		void setUniformVariableArray(std::string name, glm::vec4& values) {
			auto& var = _program->_uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_program->_programId, var.name.c_str());
			}
			setUniformVariableArray(var.location, values);
		}

		void setUniformVariableArray(GLint& location, glm::vec4& values) {
			glUniform4fv(location, 4, &values[0]);
		}

		void setUniformVariableArray(std::string name, glm::vec3& values) {
			auto& var = _program->_uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_program->_programId, var.name.c_str());
			}
			setUniformVariableArray(var.location, values);
		}

		void setUniformVariableArray(GLint& location, glm::vec3& values) {
			glUniform3fv(location, 3, &values[0]);
		}

		template<>
		void setUniformVariableMatrix<glm::mat4>(GLint& location, glm::mat4& matrix) {
			glUniformMatrix4fv(location, 16, GL_FALSE, &matrix[0][0]);
		}
		template<typename T>
		void setUniformVariableArray(std::string name, std::vector<T>& values) {
			auto& var = program->_uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(program->_programId, var.name.c_str());
			}
			setUniformVariableArray(var.location, values);
		}

		template<typename T>
		void setUniformVariableArray(GLint& location, std::vector<T>& values) {

		}

		template<>
		void setUniformVariableArray<GLfloat>(GLint& location, std::vector<GLfloat>& values) {
			switch (values.size()) {
			case 1:
				glUniform1fv(location, 1, &values[0]);
				break;
			case 2:
				glUniform2fv(location, 2, &values[0]);
				break;
			case 3:
				glUniform3fv(location, 3, &values[0]);
				break;
			case 4:
				glUniform4fv(location, 4, &values[0]);
				break;
			default:
				error("Attempting to call setUniformVariableArray<GLfloat> with more than 4 values.");
				break;
			}
		}

		template<>
		void setUniformVariableArray<GLint>(GLint& location, std::vector<GLint>& values) {
			switch (values.size()) {
			case 1:
				glUniform1iv(location, 1, &values[0]);
				break;
			case 2:
				glUniform2iv(location, 2, &values[0]);
				break;
			case 3:
				glUniform3iv(location, 3, &values[0]);
				break;
			case 4:
				glUniform4iv(location, 4, &values[0]);
				break;
			default:
				error("Attempting to call setUniformVariableArray<GLint> with more than 4 values.");
				break;
			}
		}


		template<>
		void setUniformVariableArray<GLuint>(GLint& location, std::vector<GLuint>& values) {
			switch (values.size()) {
			case 1:
				glUniform1uiv(location, 1, &values[0]);
				break;
			case 2:
				glUniform2uiv(location, 2, &values[0]);
				break;
			case 3:
				glUniform3uiv(location, 3, &values[0]);
				break;
			case 4:
				glUniform4uiv(location, 4, &values[0]);
				break;
			default:
				error("Attempting to call setUniformVariableArray<GLuint> with more than 4 values.");
				break;
			}

		}


		template<typename T>
		void setUniformVariable(GLint&& location, T& value) {

		}

		template<>
		void setUniformVariable<GLfloat>(GLint&& location, GLfloat& value) {
			glUniform1f(location, value);
		}

		template<>
		void setUniformVariable<GLint>(GLint&& location, GLint& value) {
			glUniform1i(location, value);
		}




		template<typename T>
		void setUniformVariableMatrix(GLint&& location, T& matrix) {

		}


		void setUniformVariableArray(GLint&& location, glm::vec4& values) {
			glUniform4fv(location, 4, &values[0]);
		}

		template<>
		void setUniformVariableMatrix<glm::mat4>(GLint&& location, glm::mat4& matrix) {
			glUniformMatrix4fv(location, 16, GL_FALSE, &matrix[0][0]);
		}

		template<typename T>
		void setUniformVariableArray(GLint&& location, std::vector<T>& values) {

		}

		template<>
		void setUniformVariableArray<GLfloat>(GLint&& location, std::vector<GLfloat>& values) {
			switch (values.size()) {
			case 1:
				glUniform1fv(location, 1, &values[0]);
				break;
			case 2:
				glUniform2fv(location, 2, &values[0]);
				break;
			case 3:
				glUniform3fv(location, 3, &values[0]);
				break;
			case 4:
				glUniform4fv(location, 4, &values[0]);
				break;
			default:
				error("Attempting to call setUniformVariableArray<GLfloat> with more than 4 values.");
				break;
			}
		}

		template<>
		void setUniformVariableArray<GLint>(GLint&& location, std::vector<GLint>& values) {
			switch (values.size()) {
			case 1:
				glUniform1iv(location, 1, &values[0]);
				break;
			case 2:
				glUniform2iv(location, 2, &values[0]);
				break;
			case 3:
				glUniform3iv(location, 3, &values[0]);
				break;
			case 4:
				glUniform4iv(location, 4, &values[0]);
				break;
			default:
				error("Attempting to call setUniformVariableArray<GLint> with more than 4 values.");
				break;
			}
		}


		template<>
		void setUniformVariableArray<GLuint>(GLint&& location, std::vector<GLuint>& values) {
			switch (values.size()) {
			case 1:
				glUniform1uiv(location, 1, &values[0]);
				break;
			case 2:
				glUniform2uiv(location, 2, &values[0]);
				break;
			case 3:
				glUniform3uiv(location, 3, &values[0]);
				break;
			case 4:
				glUniform4uiv(location, 4, &values[0]);
				break;
			default:
				error("Attempting to call setUniformVariableArray<GLuint> with more than 4 values.");
				break;
			}

		}

		IndexBufferObject& indices();


		bool setProgram(ShaderProgram* program);

	private:
		GLuint _id;		
		IndexBufferObject _indices;
		ShaderProgram* _program;
		bool _isVAenabled;
	};


} //end namespace core

#endif