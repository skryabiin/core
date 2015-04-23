#ifndef CORE_VERTEX_SHADER_HPP
#define CORE_VERTEX_SHADER_HPP

#include "Shader.hpp"

namespace core {


	class VertexShader : public Shader<VertexShader> {
		friend class ShaderProgram;
	public:

		GLuint createShader() {
			return glCreateShader(GL_VERTEX_SHADER);			
		}


		void declareVertexAttribute(std::string vertexAttrName) {
			_vertexAttributeNames.push_back(vertexAttrName);
		}

		~VertexShader() {
			reset();
		}

	private:
		
		std::vector<std::string> _vertexAttributeNames;


	};



} //end namespace core

#endif