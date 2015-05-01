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
			auto id = glCreateShader(GL_FRAGMENT_SHADER);
			return id;
		}


		~FragmentShader() {
			reset();
			destroy();
		}


	private:


	};


} //end namespace core

#endif