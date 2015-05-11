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

		void setVertexShader(VertexShader* shader);

		void setFragmentShader(FragmentShader* shader);

		GLuint getProgramId();

		std::string getIdentifier();
	


	private:

		FragmentShader* _fragmentShader;
		VertexShader* _vertexShader;
		GLuint _programId;
		bool _isLinked;
		bool _isBound;
		bool _isInit;		
		GLint _redChannel;

		std::map<std::string, UniformVar> _uniformVars;
		std::map<std::string, VertexAttribute> _vertexAttributes;

	};



} //end namespace core


#endif