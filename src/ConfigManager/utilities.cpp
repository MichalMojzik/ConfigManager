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

	std::string escape(const std::string& value)
	{
		std::string result;
		result.reserve(value.length());

		for(auto it = value.begin(), end = value.end(); it != end; ++it)
		{
			switch(*it)
			{
			case '=':
			case '$':
			case ':':
			case ',':
			case ' ':
			case '}':
			case '{':
			case '#':
			case ';':
				result += '\\';
				break;
			default:
				break;
			}
			result += *it;
		}

		return result;
	}
	std::string unescape(const std::string& value)
	{
		std::string result;
		result.reserve();

		bool next_escaped = false;
		for(auto it = value.begin(), end = value.end(); it != end; ++it)
		{
			if(*it == '\\' && !next_escaped)
			{
				next_escaped = true;
				continue;
			}

			result += *it;
			next_escaped = false;
		}
		if(next_escaped)
			result += '\\';

		return result;
	}

	std::string trim_nonescaped(std::string value)
	{
		auto last_char = value.find_last_not_of(' ');
		if (last_char != std::string::npos)
		{
			value.erase(last_char + 1 + (value[last_char] == '\\' ? 1 : 0));
		}
		auto first_char = value.find_first_not_of(' ');
		if (first_char != std::string::npos && first_char > 0)
		{
			value.erase(0, first_char);
		}
		return value;
	}

	std::size_t find_first_nonespaced_unscoped(const std::string& s, char character, std::size_t offset, char scope_start, char scope_end)
	{
		std::size_t scope_level = 0;

		bool skip_next = false;
		for(std::size_t char_index = offset; char_index < s.length(); ++char_index)
		{
			char ch = s[char_index];
			if(skip_next)
			{
				skip_next = false;
				continue;
			}

			if(ch == '\\')
			{
				skip_next = true;
				continue;
			}

			if(ch == character && scope_level == 0)
			{
				return char_index;
			}
			else if(ch == scope_start)
			{
				++scope_level;
			}
			else if(ch == scope_end)
			{
				if(scope_level == 0)
				{
					break;
				}
				--scope_level;
			}
		}
		return std::string::npos;
	}

	std::size_t find_first_nonespaced(const std::string& s, char character, std::size_t offset)
	{
		while(offset < s.length())
		{
			std::size_t position = s.find_first_of(character, offset);
			if (position == std::string::npos)
			{
				break;
			}

			if (!is_escaped(s, position))
			{
				return position;
			}
			else
			{
				offset = position + 1;
			}
		}

		return std::string::npos;
	}
	std::size_t find_last_nonespaced(const std::string& s, char character, std::size_t offset)
	{
		while (offset >= 0)
		{
			std::size_t position = s.find_last_of(character, offset);
			if (position == std::string::npos)
			{
				break;
			}

			if (!is_escaped(s, position))
			{
				return position;
			}
			else
			{
				offset = position - 1;
			}
		}

		return std::string::npos;
	}
}