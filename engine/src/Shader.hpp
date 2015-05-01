#ifndef CORE_SHADER_HPP
#define CORE_SHADER_HPP


#include "GL/glew.h"
#include "Resource.hpp"

#include <string>
#include <SDL.h>
#include <fstream>

namespace core {

	template <typename SHADER_TYPE>
	class Shader : public Resource {
		friend class ShaderProgram;
	public:
		
		Shader() : _source{ "" }, _fileSource{ "" }{ }


		

		void setSource(std::string source) {
			_source = source;
		}

		void setFileSource(std::string fileName) {
			_fileSource = fileName;
			

		}

		virtual bool createImpl() {
			if (!_source.compare("") && !_fileSource.compare("")) {

				error("Attempting to compile shader ", _shaderId, "  with no source defined.");
				return false;
			}

			if (_fileSource.compare("")) {
				auto source = SDL_RWFromFile(_fileSource.c_str(), "r");
				if (source == NULL) {
					error("Shader source file '", _fileSource, "' not found.");
					return false;
				}

				char c;
				int byteRead = 0;
				while (true) {
					byteRead = source->read(source, &c, sizeof(char), 1);
					if (byteRead == 0) break;
					_source.push_back(c);
				};
				source->close(source);
			}

			return true;

		}



		virtual bool initializeImpl() {

			

			const char *source_cstr = _source.c_str();
			_shaderId = static_cast<SHADER_TYPE*>(this)->createShader(); 
			debug("Shader id is : ", _shaderId);
			glShaderSource(_shaderId, 1, &source_cstr, NULL);
			glCompileShader(_shaderId);

			GLint shaderCompiled = GL_FALSE;
			glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &shaderCompiled);
			if (shaderCompiled != GL_TRUE)
			{
				//Shader log length
				int infoLogLength = 0;
				int maxLength = infoLogLength;

				//Get info string length
				glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &maxLength);

				//Allocate string
				char* infoLog = new char[maxLength];

				//Get info log
				glGetShaderInfoLog(_shaderId, maxLength, &infoLogLength, infoLog);
					
				error("Unable to compile shader ", _shaderId, ": ", infoLog);
				delete[] infoLog;
				return false;
			}

			

			return true;
		}

		virtual bool resetImpl() {
			glDeleteShader(_shaderId);
			return true;
		}

		virtual bool destroyImpl() {
			return true;
		}

		void declareUniformVar(std::string varName) {
			_uniformVarNames.push_back(varName);
		}

	protected:

		bool _sourceChanged;
		GLuint _shaderId;
		GLuint _shaderType;
		std::string _fileSource;
		std::string _source;
		std::vector<std::string> _uniformVarNames;
		


	};


} //end namespace core

#endif