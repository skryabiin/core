Renderer = {}

function Renderer.hideWindow()
	hideWindow_bind()
end

function Renderer.showWindow() 
    showWindow_bind()
end

function Renderer.setBackgroundColor(color)
    setBackgroundColor_bind(color)
end


function Renderer.getGlobalColorModulation()
    return getGlobalColorModulation_bind()
end

function Renderer.setGlobalColorModulation(matrix)

    setGlobalColorModulation_bind(matrix)
end

function Renderer.fadeWindow(duration, matrix, callback)
            local fadeTransition = {
                typeName = "RendererFadeTransition",
                duration = duration,                
                endTransform = matrix,
                finishedCallback = callback
            }
            Transitions.createTransition(fadeTransition)
end

