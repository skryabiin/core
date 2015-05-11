#include "ShaderProgram.hpp"

namespace core {

	ShaderProgram::ShaderProgram() : _isLinked{ false }, _programId{ 0 } {}


	ShaderProgram::~ShaderProgram() {
		reset();
		destroy();
	}

	bool ShaderProgram::createImpl() {
		_vertexAttributes = std::map<std::string, VertexAttribute>{};
		return true;
	}

	bool ShaderProgram::initializeImpl() {
		_programId = glCreateProgram();		
		auto glError = glGetError();
		if (glError != GL_NO_ERROR) {
			error("Error creating program: ", glError);
		}
		return true;
	}

	bool ShaderProgram::resetImpl() {		
		glDeleteProgram(_programId);
		auto glError = glGetError();
		if (glError != GL_NO_ERROR) {
			error("Error deleting program: ", glError);
		}
		return true;
	}

	bool ShaderProgram::destroyImpl() {
		return true;
	}

	bool ShaderProgram::link() {
		if (getInitializedStatus() != InitStatus::INIT_TRUE) {
			error("Attempting to link program ", _programId, " before init.");
			return false;
		}
		
		if (_vertexShader->getInitializedStatus() == InitStatus::INIT_TRUE) {
			glAttachShader(_programId, _vertexShader->_shaderId);
		}
		else {
			error("Attempting to link program ", getIdentifier(), " before vertex shader is loaded.");
			_isLinked = false;
			return false;
		}


		if (_fragmentShader->getInitializedStatus() == InitStatus::INIT_TRUE) {
			glAttachShader(_programId, _fragmentShader->_shaderId);
		}
		else {
			error("Attempting to link program ", getIdentifier(), " before fragment shader is loaded.");
			_isLinked = false;
			return false;
		} 

		glLinkProgram(_programId);

		GLint programLinked = GL_TRUE;
		glGetProgramiv(_programId, GL_LINK_STATUS, &programLinked);
		if (programLinked == GL_TRUE)
		{
			_isLinked = true;	

			_vertexAttributes.clear();
			//find the locations of variables 
			for (auto& vertexAttr : _vertexShader->_vertexAttributeNames) {
				auto vertexAttrObj = VertexAttribute{};
				vertexAttrObj.name = vertexAttr;
				vertexAttrObj.index = glGetAttribLocation(_programId, vertexAttr.c_str());
				_vertexAttributes[vertexAttr] = vertexAttrObj;
			}

			_uniformVars.clear();

			for (auto& uniformVar : _vertexShader->_uniformVarNames) {
				auto uniformVarObj = UniformVar{};
				uniformVarObj.name = uniformVar;
				uniformVarObj.location = glGetUniformLocation(_programId, uniformVar.c_str());
				_uniformVars[uniformVar] = uniformVarObj;
			}

			for (auto& uniformVar : _fragmentShader->_uniformVarNames) {
				auto uniformVarObj = UniformVar{};
				uniformVarObj.name = uniformVar;
				uniformVarObj.location = glGetUniformLocation(_programId, uniformVar.c_str());
				_uniformVars[uniformVar] = uniformVarObj;
			}


		}
		else {

			_isLinked = false;
			//Program log length
			int infoLogLength = 0;
			int maxLength = infoLogLength;

			//Get info string length
			glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &maxLength);

			//Allocate string
			char* infoLog = new char[maxLength];

			//Get info log
			glGetProgramInfoLog(_programId, maxLength, &infoLogLength, infoLog);

			error("Unable to link program ", getIdentifier(), ": ", infoLog);
			delete[] infoLog;

	
		}

		return _isLinked;
	}


	bool ShaderProgram::bindImpl() {
		if (!_isLinked) {
			error("Attempting to bind program ", getIdentifier(), " that is not linked.");
			return false;
		}

		glUseProgram(_programId);	
		return true;
	}

	
	bool ShaderProgram::unbindImpl() {		
		glUseProgram(NULL);
		return true;
	}

	void ShaderProgram::setVertexShader(VertexShader* shader){
		_isLinked = false;
		_vertexShader = shader;
	}


	void ShaderProgram::setFragmentShader(FragmentShader* shader){
		_isLinked = false;
		_fragmentShader = shader;
	}


	bool ShaderProgram::isLinked() {

		return _isLinked;
	}

	GLuint ShaderProgram::getProgramId() {

		return _programId;
	}

	std::string ShaderProgram::getIdentifier() {
		return _name + " (" + std::to_string(_programId) + ")";
	}

}