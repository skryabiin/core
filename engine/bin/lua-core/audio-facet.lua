AudioFacet = Facet:new()

function AudioFacet:new(entity, system, soundName, volume, o)
	o = o or {}    
	o = self:create(entity, system, o)    
    if soundName then    
        o:setSoundName(soundName)
    end    
    if volume then
        o:setVolume(volume)
    end
    return o
end

function AudioFacet:type()
	return "AudioFacet"
end

function AudioFacet:setSoundName(soundName)
    self._soundName = soundName
    if(not self:isCoreFacet()) then
        self:createFacetInCore()
    else
        self:updateFacetInCore()
    end
end

function AudioFacet:soundName()
    return self._soundName
end

function AudioFacet:setVolume(volume)
    if self._volume == volume then
        return
    end
    self._volume = volume
    if (not self:isCoreFacet()) and self:soundName() then
        self:createFacetInCore()
    elseif self:isCoreFacet() then
        local volumeChangeEvent = {
            typeName = "VolumeChangeEvent",
            entityId = self:of():id(),
            facetId = self:id(),
            volume = volume}
        EventProcessor.process(volumeChangeEvent)
    end
end

function AudioFacet:volume()
    return self._volume or 0
end

function AudioFacet:createFacetInCore()
    local audioFacet = {
        soundName = self:soundName(),
        volume = self:volume()
    }
    self._id = addAudioFacet_bind(self:system():name(), self:of():id(), audioFacet)
end

function AudioFacet:updateFacetInCore()
    local audioFacet = {       
        facetId = self:id(),        
        soundName = self:soundName()
    }
    updateAudioFacet_bind(self:system():name(), self:of():id(), audioFacet)
end

function AudioFacet:playSound()
    local playSoundEvent = {
        typeName = "PlaySoundEvent",
        entityId = self:of():id(),
        facetId = self:id()
        }
    EventProcessor.process(playSoundEvent)  
end