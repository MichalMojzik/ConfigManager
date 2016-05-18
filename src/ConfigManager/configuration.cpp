#include "configuration.h"

#include "utilities.h"
#include <fstream>

namespace ConfigManager
{
	Configuration::Configuration()
		: loaded_(false)
  {
  }
  
  void Configuration::Open(std::istream& input_stream)
  {
		loaded_ = true;
		original_lines_.clear();

		SectionNode* section_ptr = nullptr;

		std::string line;
		while(std::getline(input_stream, line))
		{
			original_lines_.push_back(line);

			auto semilocon_position = find_first_nonespaced(line, ';');
			if(semilocon_position != std::string::npos)
			{
				line = line.substr(0, semilocon_position); // strip comment away
			}
			line = trim_nonescaped(line); // strip whitespaces away

			if(line.length() == 0)
			{
				continue;
			}

			if(line.front() == '[' && line.back() == ']')
			{
				std::string section_name = unescape(line.substr(1, line.length() - 2));
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
				auto assignment_position = find_first_nonespaced(line, '=');
				if(assignment_position == std::string::npos)
				{
					throw MalformedInputException();
				}
				std::string option_name = unescape(trim_nonescaped(line.substr(0, assignment_position)));
				std::string option_value = trim_nonescaped(line.substr(assignment_position + 1));
				auto& option = section[option_name];
				option.Load(option_value);
			}
		}

		// as everything is loaded, we check whether all mandatory sections and options are present
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			auto& section = *section_it->second;
			// only check for requirements if the section was actually present in the stream
			if(section.IsLoaded())
			{
				for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
				{
					auto& option = *option_it->second;
					if(!option.IsLoaded() && option.requirement_ == Requirement::MANDATORY)
					{
						throw MandatoryMissingException();
					}
				}
			}
			// section has to have been loaded if it was specified as mandatory
			else if(section.requirement_ == Requirement::MANDATORY)
			{
				throw MandatoryMissingException();
			}
		}
	}

  void Configuration::CheckStrict()
  {
		// enumerate through all of the known section
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			// extract the SectionNode from the iterator
			auto& section = *section_it->second;

			// if the section wasn't specified, then that is in violation of strict mode
			if(section.IsLoaded() && !section.is_specified_)
			{
				throw StrictException();
			}

			// enumerate through all of the known options of the section
			for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
			{
				// extract the OptionNode from the iterator
				auto& option = *option_it->second;

				// if the option wasn't specified, then that is in vioaltion of strict mode
				if(option.IsLoaded() && option.proxy_ != nullptr)
				{
					throw StrictException();
				}
			}
		}
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
		{
			throw InvalidOperationException();
		}
		if(requirement == Requirement::MANDATORY && IsLoaded() && !section_node.loaded_)
		{
			throw MandatoryMissingException();
		}
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

	bool Configuration::IsLoaded() const
	{
		return loaded_;
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
