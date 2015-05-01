#include "Map.hpp"


namespace core {


bool Map::createImpl() {
	return true;
}

bool Map::initializeImpl() {

	return true;
}

bool Map::resetImpl() {

	return true;
}

bool Map::destroyImpl() {
	return true;
}


void Map::setTmxFilePath(std::string tmxFilePath) {
	_tmxFilePath = tmxFilePath;
}

std::string Map::tmxFilePath() {
	return _tmxFilePath;
}


} //end namespace core