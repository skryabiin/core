#ifndef CORE_MAP_HPP
#define CORE_MAP_HPP

#include "Resource.hpp"

namespace core {
	 

	class Map : public Resource {

	public:

		bool createImpl() override;

		bool initializeImpl() override;

		bool resetImpl() override;

		bool destroyImpl() override;


		void setTmxFilePath(std::string tmxFilePath);

		std::string tmxFilePath();

	private:

		std::string _tmxFilePath;

	};




} //end namespace core

#endif