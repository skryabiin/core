#include "Map.hpp"


namespace core {


InitStatus Map::initializeImpl() {

	return InitStatus::INIT_TRUE;
}

InitStatus Map::resetImpl() {

	return InitStatus::INIT_FALSE;
}



void Map::setTmxFilePath(std::string tmxFilePath) {
	_tmxFilePath = tmxFilePath;
}

std::string Map::tmxFilePath() {
	return _tmxFilePath;
}


} //end namespace core