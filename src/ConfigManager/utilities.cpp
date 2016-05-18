#include "utilities.h"

namespace ConfigManager
{
	bool is_escaped(const std::string& s, std::size_t index)
	{
		if(index == 0)
		{
			return false;
		}
		else
		{
			std::size_t escape_start = s.find_last_not_of('\\', index - 1);
			std::size_t escape_count = (index - 1 - (escape_start == std::string::npos ? 0 : escape_start));
			return (escape_count % 2) == 1;
		}
	}

	std::string escape_trim(std::string value)
	{
		auto last_char = value.find_last_not_of(' ');
		if(last_char != std::string::npos)
			value.erase(last_char + 1 + (value[last_char] == '\\' ? 1 : 0));
		auto first_char = value.find_first_not_of(' ');
		if(first_char != std::string::npos && first_char > 0)
			value.erase(0, first_char);
		return value;
	}

	std::size_t find_first_nonespaced(const std::string& s, char character, std::size_t offset)
	{
		while(offset < s.length())
		{
			std::size_t position = s.find_first_of(character, offset);
			if(position == std::string::npos)
				break;

			if(!is_escaped(s, position))
				return position;
			else
				offset = position + 1;
		}

		return std::string::npos;
	}
	std::size_t find_last_nonespaced(const std::string& s, char character, std::size_t offset)
	{
		while(offset >= 0)
		{
			std::size_t position = s.find_last_of(character, offset);
			if(position == std::string::npos)
				break;

			if(!is_escaped(s, position))
				return position;
			else
				offset = position - 1;
		}

		return std::string::npos;
	}
}