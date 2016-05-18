#ifndef CONFIGMANAGER_UTILITIES_H
#define CONFIGMANAGER_UTILITIES_H

#include <string>

namespace ConfigManager
{
	std::string escape(const std::string& value);
	std::string unescape(const std::string& value);
	std::string trim_nonescaped(std::string value);
	std::size_t find_first_nonespaced(const std::string& s, char character, std::size_t offset = 0);
	std::size_t find_last_nonespaced(const std::string& s, char character, std::size_t offset = 0);
}

#endif
