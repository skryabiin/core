
FragmentShaders = {}

function FragmentShaders.loadShader(v)
	
    Console.info("Loading fragment shader '", v.name, "', ", v.path)
	loadFragmentShader_bind(v.name, v.path, v.uniforms)

end


function FragmentShaders.load(shaders) 
	Console.info("Loading fragment shaders...")
	FragmentShaders.shaders = shaders
	for i, v in ipairs(shaders) do
		FragmentShaders.loadShader(v)
	end
end


VertexShaders = {}

function VertexShaders.loadShader(v)
	Console.info("Loading vertex shader '", v.name, "', ", v.path)
	loadVertexShader_bind(v.name, v.path, v.uniforms, v.attributes)

end


function VertexShaders.load(shaders) 
	
    Console.info("Loading vertex shaders...")
	VertexShaders.shaders = shaders
	for i, v in ipairs(shaders) do
		VertexShaders.loadShader(v)
	end
end