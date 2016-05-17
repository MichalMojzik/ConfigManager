#include "configuration.h"

#include <fstream>

namespace ConfigManager
{
	std::string trim(std::string value)
	{
		value.erase(0, value.find_first_not_of(' '));
		value.erase(value.find_last_not_of(' ') + 1);
		return value;
	}

  Configuration::Configuration()
  {
  }
  
  void Configuration::Open(std::istream& input_stream)
  {
		decltype(data_.find("")) section_it;
		std::string line;
		while(std::getline(input_stream, line))
		{
			auto semilocon_position = line.find_first_of(';');
			if(semilocon_position != std::string::npos)
				line = line.substr(0, semilocon_position); // strip comments away
			line = trim(line); // strip whitespaces away

			if(line.length() == 0)
			{
				continue;
			}

			if(line.front() == '[' && line.back() == ']')
			{
				//data_.insert(
				std::string section_name = line.substr(1, line.length() - 2);
				auto result = data_.emplace(section_name, SectionData());
				if(!result.second)
				{
					throw MalformedInputException();
				}
				section_it = result.first;
			}
			else if(section_it == data_.end())
			{
				throw MalformedInputException();
			}
			else
			{

			}
		}
  }

  void Configuration::CheckStrict()
  {
  }
  
  void Configuration::Save(std::ostream& output_stream, OutputMethod output_method)
  {
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			auto& section_name = section_it->first;
			auto& section_data = section_end->second;
			output_stream << "[" << section_name << "]" << std::endl;
			for(auto option_it = section_data.begin(), option_end = section_data.end(); option_it != option_end; ++option_it)
			{
				auto& option_name = option_it->first;
				auto& option_value = option_it->second.first;
				auto& option_proxy = option_it->second.second;
				output_stream << option_name << "=" << option_value << std::endl;
			}
		}
  }
  
  Section Configuration::SpecifySection(std::string section_name, Requirement requirement, const std::string comments)
  {
    return Section(section_name);
  }

	Section Configuration::operator[](std::string section_name) const
	{
		return Section(section_name);
	}


	ConfigurationFile::ConfigurationFile(std::string filename, InputFilePolicy policy)
		: filename_(filename), Configuration()
	{
		std::ifstream file_stream(filename_);
		Open(file_stream);
	}
	
	ConfigurationFile::~ConfigurationFile()
	{
		std::ofstream file_stream(filename_);
		Save(file_stream);
	}
};
