#include "ShaderManager.hpp"
#include "Core.hpp"
#include "LuaList.hpp"


namespace core {

	bool ShaderManager::createImpl() {

		//register lua functions
		auto& lua = single<Core>().lua();
		lua.bindFunction("loadVertexShader_bind", loadVertexShader_bind);
		lua.bindFunction("loadFragmentShader_bind", loadFragmentShader_bind);
		lua.bindFunction("createShaderProgram_bind", createShaderProgram_bind);
		return true;
	}

	bool ShaderManager::initializeImpl() {

		return true;
	}

	bool ShaderManager::resetImpl() {

		for (auto& shader : _vertexShaders) {
			shader.second.get()->reset();
		}		

		for (auto& shader : _fragmentShaders) {
			shader.second.get()->reset();
		}
		
		for (auto& sp : _shaderPrograms) {
			sp.second.get()->reset();
		}

		return true;

	}

	bool ShaderManager::destroyImpl() {
		for (auto& shader : _vertexShaders) {
			shader.second.get()->destroy();
		}
		_vertexShaders.clear();

		for (auto& shader : _fragmentShaders) {
			shader.second.get()->destroy();
		}
		_fragmentShaders.clear();

		for (auto& sp : _shaderPrograms) {
			sp.second.get()->destroy();
		}
		_shaderPrograms.clear();

		return true;
	}

	ShaderProgram* ShaderManager::getShaderProgram(std::string name) {
		auto it = _shaderPrograms.find(name);
		return (it == _shaderPrograms.end()) ? nullptr : it->second.get();
	}

	ShaderProgram* ShaderManager::createShaderProgram(std::string name, std::string vertexShaderName, std::string fragmentShaderName) {

		auto program = new ShaderProgram{};

		program->setVertexShader(getVertexShader(vertexShaderName));
		program->setFragmentShader(getFragmentShader(fragmentShaderName));

		auto it = _shaderPrograms.find(name);
		if (it != _shaderPrograms.end()) {
			_shaderPrograms.erase(it);
		}

		_shaderPrograms[name] = std::unique_ptr<ShaderProgram>(program);
		return _shaderPrograms[name].get();

	}

	VertexShader* ShaderManager::declareVertexShader(std::string name) {

		auto shader = new VertexShader{};

		auto it = _vertexShaders.find(name);
		if (it != _vertexShaders.end()) {
			_vertexShaders.erase(it);
		}

		_vertexShaders[name] = std::unique_ptr<VertexShader>(shader);
		return _vertexShaders[name].get();

	}

	VertexShader* ShaderManager::createVertexShader(std::string name, std::string source, std::list<std::string>& attributes, std::list<std::string>& uniforms) {

		auto shader = declareVertexShader(name);
		shader->setSource(source);
		for (auto& attr : attributes) {
			shader->declareVertexAttribute(attr);
		}
		for (auto& uniform : uniforms) {
			shader->declareUniformVar(uniform);
		}
		shader->create();
		return shader;
	}


	FragmentShader* ShaderManager::declareFragmentShader(std::string name) {

		auto shader = new FragmentShader{};

		auto it = _fragmentShaders.find(name);
		if (it != _fragmentShaders.end()) {
			_fragmentShaders.erase(it);
		}
		_fragmentShaders[name] = std::unique_ptr<FragmentShader>(shader);
		return _fragmentShaders[name].get();		

	}

	FragmentShader* ShaderManager::createFragmentShader(std::string name, std::string source, std::list<std::string>& uniforms) {


		auto shader = declareFragmentShader(name);
		shader->setSource(source);
		for (auto& uniform : uniforms) {
			shader->declareUniformVar(uniform);
		}
		shader->create();
		return shader;


	}

	VertexShader* ShaderManager::getVertexShader(std::string name) {
		auto it = _vertexShaders.find(name);
		if (it == _vertexShaders.end()) return nullptr; 
		auto out = it->second.get();

		out->initialize();
		return out;
	}

	FragmentShader* ShaderManager::getFragmentShader(std::string name) {

		auto it = _fragmentShaders.find(name);		
		if (it == _fragmentShaders.end()) return nullptr;
		auto out = it->second.get();

		out->initialize();
		return out;
	}

	int ShaderManager::createShaderProgram_bind(LuaState& lua) {

		return 0;
	}

	int ShaderManager::loadVertexShader_bind(LuaState& lua) {

		auto name = lua.pullStack<std::string>(1);
		auto file = lua.pullStack<std::string>(2);

		auto attributesList = LuaList<std::string>{};
		attributesList.fromLua(lua);

		lua.popStack();

		auto uniformsList = LuaList<std::string>{};
		uniformsList.fromLua(lua);


		std::string source = "";

		auto fileStream = SDL_RWFromFile(file.c_str(), "r");
		if (fileStream == NULL) {
			error("Shader source file '", file, "' not found.");
			return 0;
		}

		char c;
		int byteRead = 0;
		while (true) {
			byteRead = fileStream->read(fileStream, &c, sizeof(char), 1);
			if (byteRead == 0) break;
			source.push_back(c);
		};
		fileStream->close(fileStream);

		single<ShaderManager>().createVertexShader(name, source, attributesList.values(), uniformsList.values());

		return 0;
	}

	int ShaderManager::loadFragmentShader_bind(LuaState& lua) {


		auto name = lua.pullStack<std::string>(1);
		auto file = lua.pullStack<std::string>(2);

		auto uniformsList = LuaList<std::string>{};

		uniformsList.fromLua(lua);

		std::string source = "";

		auto fileStream = SDL_RWFromFile(file.c_str(), "r");
		if (fileStream == NULL) {
			error("Shader source file '", file, "' not found.");
			return 0;
		}

		char c;
		int byteRead = 0;
		while (true) {
			byteRead = fileStream->read(fileStream, &c, sizeof(char), 1);
			if (byteRead == 0) break;
			source.push_back(c);
		};
		fileStream->close(fileStream);

		single<ShaderManager>().createFragmentShader(name, source, uniformsList.values());

		return 0;
	}



} //end namespace core