AudioFacet = Facet:new()

function AudioFacet:new(entity, systemName, soundName, volume, o)
	o = o or {}    
	o = self:create(entity, systemName, o)    
    if soundName then    
        o:setSoundName(soundName)
    end    
    if volume then
        o:setVolume(volume)
    end
    return o
end

function AudioFacet:getType()
	return "AudioFacet"
end

function AudioFacet:setSoundName(soundName)
    self.soundName = soundName
    if(not self:isCoreFacet()) then
        self:createFacetInCore()
    else
        self:updateFacetInCore()
    end
end

function AudioFacet:getSoundName()
    return self.soundName
end

function AudioFacet:setVolume(volume)
    if self.volume == volume then
        return
    end
    self.volume = volume
    if (not self:isCoreFacet()) and self:getSoundName() then
        self:createFacetInCore()
    elseif self:isCoreFacet() then
        local volumeChangeEvent = {
            typeName = "VolumeChangeEvent",
            entityId = self:getOf():getId(),
            facetId = self:getId(),
            volume = volume}
        EventProcessor.process(volumeChangeEvent)
    end
end

function AudioFacet:getVolume()
    return self.volume or 0
end

function AudioFacet:createFacetInCore()
    local audioFacet = {
        soundName = self:getSoundName(),
        volume = self:getVolume()
    }
    self.id = addAudioFacet_bind(self.systemName, self:getOf():getId(), audioFacet)
end

function AudioFacet:updateFacetInCore()
    local audioFacet = {       
        facetId = self:getId(),        
        soundName = self:getSoundName()
    }
    updateAudioFacet_bind(self.systemName, self:getOf():getId(), audioFacet)
end

function AudioFacet:playSound()
    local playSoundEvent = {
        typeName = "PlaySoundEvent",
        entityId = self:getOf():getId(),
        facetId = self:getId()
        }
    EventProcessor.process(playSoundEvent)  
end