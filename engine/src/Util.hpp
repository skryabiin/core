#ifndef CORE_UTIL_H
#define CORE_UTIL_H

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <time.h>
#include <GL/glew.h>

namespace core {


	inline std::string getTimestamp() {

		struct tm newtime;
		__time32_t aclock;
		char buffer[32];		
		_time32(&aclock);   // Get time in seconds.
		_localtime32_s(&newtime, &aclock);   // Convert time to struct tm form.
		asctime_s(buffer, 32, &newtime);
		return std::string{ buffer };
	}

	inline void hexToRgb(std::string hexString, GLfloat& r, GLfloat& g, GLfloat& b) {
		if (hexString[0] == '#') {
			hexString = hexString.substr(1);
		}
		char* p;

		r = strtol(hexString.substr(0, 2).c_str(), &p, 16) / 255.0f;
		g = strtol(hexString.substr(2, 2).c_str(), &p, 16) / 255.0f;
		b = strtol(hexString.substr(4, 2).c_str(), &p, 16) / 255.0f;

	}

	inline bool stob(const std::string& s) {
		return(!s.compare("true"));
	}

	inline std::string ltos(const long& l) {
		std::ostringstream ss;
		ss << l;
		return ss.str();
	}



	inline void split(std::string s, const char* token, std::vector<std::string>& values) {

		if (s.length() == 0) return;

		auto nextIndex = s.find_last_of(token);
		if (nextIndex == std::string::npos) {
			values.push_back(s);
			return;
		}
		else {
			values.push_back(s.substr(nextIndex + 1));
			split(s.substr(0, nextIndex), token, values);
		}
	}

	inline std::vector<std::string> split(std::string s, const char* token) {


		auto out = std::vector<std::string>{};

		split(s, token, out);

		return out;

	}


	inline std::string stripPath(std::string filePath) {

		auto lastIndex = filePath.find_last_of('/');
		return (lastIndex == std::string::npos) ? filePath : filePath.substr(lastIndex + 1);

	}

} //end namespace core

#endif