World = {}

World.camera = {}


function World.camera.centerAt(position)
    centerCameraAt_bind(position)
end


function World.camera.roll(degrees)
    cameraRoll_bind(degrees)
end


function World.camera.pitch(degrees)
    cameraPitch_bind(degrees)
end


function World.camera.yaw(degrees)
    cameraYaw_bind(degrees)
end


function World.camera.move(delta)
    cameraMove_bind(delta)
end

function World.camera.zoom(scale)
    cameraZoom_bind(scale)
end

function World.runMap(mapName, callbackFunction)
    runMap_bind(mapName, callbackFunction)
end