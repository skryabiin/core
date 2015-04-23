#ifndef CORE_MAP_HPP
#define CORE_MAP_HPP

#include "Resource.hpp"

namespace core {
	 

	class Map : public Resource {

	public:

		InitStatus initializeImpl() override;
		InitStatus resetImpl() override;

		void setTmxFilePath(std::string tmxFilePath);

		std::string tmxFilePath();

	private:

		std::string _tmxFilePath;

	};




} //end namespace core

#endif