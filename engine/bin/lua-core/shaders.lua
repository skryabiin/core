
FragmentShaders = {}

function FragmentShaders.loadShader(v)
	
	loadFragmentShader_bind(v.name, v.path, v.uniforms)

end


function FragmentShaders.load(shaders) 
	
	FragmentShaders.shaders = shaders
	for i, v in ipairs(shaders) do
		FragmentShaders.loadShader(v)
	end
end


VertexShaders = {}

function VertexShaders.loadShader(v)
	
	loadVertexShader_bind(v.name, v.path, v.uniforms, v.attributes)

end


function VertexShaders.load(shaders) 
	
	VertexShaders.shaders = shaders
	for i, v in ipairs(shaders) do
		VertexShaders.loadShader(v)
	end
end