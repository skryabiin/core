#ifndef CORE_FRAGMENT_SHADER_HPP
#define CORE_FRAGMENT_SHADER_HPP

#include "Shader.hpp"
namespace core {


	class FragmentShader : public Shader<FragmentShader> {

	public:

		FragmentShader() {
			Shader();
		}

		GLuint createShader() {
			return glCreateShader(GL_FRAGMENT_SHADER);			
		}


		~FragmentShader() {
			reset();
		}


	private:


	};


} //end namespace core

#endif