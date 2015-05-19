#ifndef CORE_MAP_HPP
#define CORE_MAP_HPP

#include "Resource.hpp"
#include <map>

namespace core {
	 

	class Map : public Resource {

	public:

		bool createImpl() override;

		bool initializeImpl() override;

		bool resetImpl() override;

		bool destroyImpl() override;


		std::string tmxFilePath;

		std::map<std::string, std::string> tilesets;				

	private:


	};




} //end namespace core

#endif