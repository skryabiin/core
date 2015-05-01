#ifndef CORE_SHADER_PROGRAM_HPP
#define CORE_SHADER_PROGRAM_HPP

#include "Console.hpp"
#include "FragmentShader.hpp"
#include "VertexShader.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"
#include <map>
#include <stdarg.h>
#include "glm.hpp"

namespace core {

	struct VertexAttribute {

		VertexAttribute() : index{ -1 }, count{ 0 }, dataType{ GL_FLOAT }, normalized{ false }, stride{ 0 } {}

		GLuint index;
		std::string name;
		GLint count;
		GLenum dataType;
		bool normalized;
		GLsizei stride;
	};

	struct UniformVar {

		UniformVar() : location{ -1 } {}

		GLint location;
		std::string name;
	};


	class ShaderProgram : public Resource, public bindable<ShaderProgram> {
		friend class VertexArrayObject;
		
	public:

		ShaderProgram();

		~ShaderProgram();

		virtual bool createImpl() override;
		virtual bool initializeImpl() override;
		virtual bool resetImpl() override;
		virtual bool destroyImpl() override;

		bool link();

		bool isLinked();

		bool isVertexAttributesEnabled();

		bool bindImpl();

		bool setVbo(GLuint& vbo);

		bool setIbo(GLuint& ibo);

		bool unbindImpl();

		bool enableVertexArrayAttributes();
		
		bool disableVertexArrayAttributes();

		bool drawElements(GLuint& vertexBufferObject, GLuint& indexBufferObject, GLenum mode, GLsizei count);

		void setVertexShader(VertexShader* shader);

		void setFragmentShader(FragmentShader* shader);

		GLuint getProgramId();

		std::string getIdentifier();
		
		void setVertexArrayAttribute(std::string name, GLuint buffer, int dimension, GLenum type, GLenum normalized);

		template <typename T>
		bool drawElements(VertexBufferObject<T>& vbo, IndexBufferObject& ibo, GLenum mode) {
			auto glError = glGetError();
			vbo.bind();
			ibo.bind();
			glDrawElements(mode, ibo.getNumVertices(), GL_UNSIGNED_SHORT, NULL);
			ibo.unbind();
			vbo.unbind();
			return true;
		}


		template <typename T>
		void setVertexArrayAttribute(std::string name, VertexBufferObject<T>& vbo) {
			auto& va = _vertexAttributes[name.c_str()];
			vbo.setVertexAttributePointer(va.index);
		}



		template<typename T>
		void setUniformVariable(std::string name, T& value) {
			auto& var = _uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_programId, var.name.c_str());
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
			auto& var = _uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_programId, var.name.c_str());
			}
			setUniformVariableMatrix(var.location, matrix);
		}

		template<typename T>
		void setUniformVariableMatrix(GLint& location, T& matrix) {

		}
		
		void setUniformVariableArray(std::string name, glm::vec4& values) {
			auto& var = _uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_programId, var.name.c_str());
			}
			setUniformVariableArray(var.location, values);
		}

		void setUniformVariableArray(GLint& location, glm::vec4& values) {
			glUniform4fv(location, 4, &values[0]);
		}

		template<>
		void setUniformVariableMatrix<glm::mat4>(GLint& location, glm::mat4& matrix) {
			glUniformMatrix4fv(location, 16, GL_FALSE, &matrix[0][0]);
		}
		template<typename T>
		void setUniformVariableArray(std::string name, std::vector<T>& values) {
			auto& var = _uniformVars[name.c_str()];

			if (var.location == -1) {
				var.location = glGetUniformLocation(_programId, var.name.c_str());
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

	

		




	private:

		FragmentShader* _fragmentShader;
		VertexShader* _vertexShader;
		GLuint _programId;
		bool _isLinked;
		bool _isBound;
		bool _isInit;
		bool _isVAenabled;
		
		GLint _redChannel;

		std::map<std::string, UniformVar> _uniformVars;
		std::map<std::string, VertexAttribute> _vertexAttributes;

	};



} //end namespace core


#endif