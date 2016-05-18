#include "configuration.h"

#include <fstream>

namespace ConfigManager
{
	std::string ini_trim(std::string value)
	{
		auto last_char = value.find_last_not_of(' ');
		if(last_char != std::string::npos)
			value.erase(last_char + 1 + (value[last_char] == '\\' ? 1 : 0));
		auto first_char = value.find_first_not_of(' ');
		if(first_char != std::string::npos && first_char > 0)
			value.erase(0, first_char);
		return value;
	}


  Configuration::Configuration()
  {
  }
  
  void Configuration::Open(std::istream& input_stream)
  {
		original_lines_.clear();

		SectionNode* section_ptr = nullptr;

		std::string line;
		while(std::getline(input_stream, line))
		{
			original_lines_.push_back(line);

			auto semilocon_position = line.find_first_of(';');
			if(semilocon_position != std::string::npos)
				line = line.substr(0, semilocon_position); // strip comments away
			line = ini_trim(line); // strip whitespaces away

			if(line.length() == 0)
			{
				continue;
			}

			if(line.front() == '[' && line.back() == ']')
			{
				std::string section_name = line.substr(1, line.length() - 2);
				auto& section = RetrieveSection(section_name);
				if(section.loaded_)
				{
					throw MalformedInputException();
				}
				section.loaded_ = true;
				section_ptr = &section;
			}
			else if(section_ptr == nullptr)
			{
				throw MalformedInputException();
			}
			else
			{
				auto& section = *section_ptr;
				auto assignment_position = line.find_first_of('=');
				if(assignment_position == std::string::npos)
				{
					throw MalformedInputException();
				}
				std::string option_name = ini_trim(line.substr(0, assignment_position));
				std::string option_value = ini_trim(line.substr(assignment_position + 1));
				auto& option = section[option_name];
				option.Load(option_value);
			}
		}
  }

  void Configuration::CheckStrict()
  {
  }
  
  void Configuration::Save(std::ostream& output_stream, OutputMethod output_method)
  {
		bool emit_default = output_method == OutputMethod::EMIT_DEFAULT_VALUES;

		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			bool section_header_emitted = false;
			auto& section = *section_it->second;
			if(emit_default || section.loaded_)
			{
				output_stream << "[" << section.Name() << "]";
				if(emit_default)
				{
					output_stream << ";" << section.comment_;
				}
				output_stream << std::endl;
				section_header_emitted = true;
			}
			for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
			{
				auto& option = *option_it->second;
				if(!option.HasValue())
				{
					if(emit_default)
					{
						if(!option.LoadDefaultValue())
						{
							continue;
						}
					}
					else
					{
						continue;
					}
				}

				if(!section_header_emitted)
				{
					output_stream << "[" << section.Name() << "]";
					if(emit_default)
					{
						output_stream << ";" << section.comment_;
					}
					output_stream << std::endl;

					section_header_emitted = true;
				}

				output_stream << option.Name() << "=" << option.Value();
				if(emit_default)
				{
					output_stream << ";" << option.comment_;
				}
				output_stream << std::endl;
			}
		}
  }
  
  Section Configuration::SpecifySection(const std::string& section_name, Requirement requirement, const std::string& comments)
  {
		auto& section_node = RetrieveSection(section_name);
		if(section_node.is_specified_)
			throw InvalidOperationException();
		section_node.requirement_ = requirement;
		section_node.comment_ = comments;
    return Section(section_node);
  }

	Section Configuration::operator[](const std::string& section_name)
	{
		auto& section_node = RetrieveSection(section_name);
		return Section(section_node);
	}

	SectionNode& Configuration::RetrieveSection(const std::string& section_name)
	{
		auto result = data_.emplace(section_name, std::unique_ptr<SectionNode>(new SectionNode(*this, section_name)));
		return *result.first->second;
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
