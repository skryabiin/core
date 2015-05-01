#ifndef CORE_SHADER_MANAGER_HPP
#define CORE_SHADER_MANAGER_HPP

#include "Templates.hpp"
#include "ShaderProgram.hpp"
#include "LuaState.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <list>

namespace core {


	class ShaderManager : public singleton<ShaderManager>, public initializable<ShaderManager, void, void, void, void> {

	public:

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		ShaderProgram* getShaderProgram(std::string name);

		ShaderProgram* createShaderProgram(std::string name, std::string vertexShaderName, std::string fragmentShaderName);

		VertexShader* getVertexShader(std::string name);

		FragmentShader* getFragmentShader(std::string name);

		VertexShader* ShaderManager::createVertexShader(std::string name, std::string source, std::list<std::string>& attributes, std::list<std::string>& uniforms);

		VertexShader* ShaderManager::declareVertexShader(std::string name);

		FragmentShader* ShaderManager::createFragmentShader(std::string name, std::string source, std::list<std::string>& uniforms);

		FragmentShader* ShaderManager::declareFragmentShader(std::string name);

		static int createShaderProgram_bind(LuaState& lua);

		static int loadVertexShader_bind(LuaState& lua);

		static int loadFragmentShader_bind(LuaState& lua);

	private:

		std::map<std::string, std::unique_ptr<ShaderProgram>> _shaderPrograms;

		std::map<std::string, std::unique_ptr<VertexShader>> _vertexShaders;

		std::map<std::string, std::unique_ptr<FragmentShader>> _fragmentShaders;

	};


} //end namespace core

#endif