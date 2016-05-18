#ifndef CONFIGMANAGER_UTILITIES_H
#define CONFIGMANAGER_UTILITIES_H

#include <string>

namespace ConfigManager
{
	std::string escape_trim(std::string value);
	std::size_t find_first_nonespaced(const std::string& s, char character, std::size_t offset = 0);
	std::size_t find_last_nonespaced(const std::string& s, char character, std::size_t offset = 0);
}

#endif
