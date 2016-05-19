#include "configuration.h"
#include "optionnode.h"
#include "sectionnode.h"

#include "utilities.h"
#include <fstream>
#include <stack>

namespace ConfigManager
{
	Configuration::OriginalData::OriginalData(const std::string& line)
		: line_(line), section_(nullptr), option_(nullptr)
	{
	}


	Configuration::Configuration()
		: loaded_(false)
  {
  }
  

	bool Configuration::ResolveLink(const std::string& value, LinkValues& link_values, std::string* result, std::vector<OptionNode*>* resolved_links, Link* unresolved_link)
	{
		std::size_t offset = 0;
		while(offset < value.size())
		{
			auto link_position = find_first_nonespaced(value, '$', offset);
			if(link_position == std::string::npos)
			{
				if(result != nullptr)
				{
					*result += value.substr(offset);
				}
				break;
			}

			if(result != nullptr)
			{
				*result += value.substr(offset, link_position - offset);
			}

			if(link_position + 1 == value.length() || value[link_position + 1] != '{')
			{
				throw MalformedInputException();
			}

			auto colon_position = find_first_nonespaced_unscoped(value, '#', link_position + 2, '{', '}');
			if(colon_position == std::string::npos)
			{
				throw MalformedInputException();
			}
			auto last_brace_position = find_first_nonespaced_unscoped(value, '}', colon_position + 1, '{', '}');
			if(last_brace_position == std::string::npos)
			{
				throw MalformedInputException();
			}

			Link link;
			// resolve section name recursively, report unresolved links
			if(!ResolveLink(value.substr(link_position + 2, colon_position - link_position - 2), link_values, &link.first, resolved_links, unresolved_link))
			{
				return false;
			}
			// resolve option name recursively, report unresolved links
			if(!ResolveLink(value.substr(colon_position + 1, last_brace_position - colon_position - 1), link_values, &link.second, resolved_links, unresolved_link))
			{
				return false;
			}

			auto link_it = link_values.find(link);
			if(link_it == link_values.end())
			{
				if(unresolved_link != nullptr)
				{
					*unresolved_link = link;
				}
				return false;
			}

			if(resolved_links)
			{
				resolved_links->push_back(link_it->second);
			}

			// the option node that is pointed to has to have a valid values by now
			if(result != nullptr)
			{
				*result += link_it->second->Value();
			}

			offset = last_brace_position + 1;
		}

		return true;
	}

	std::string StripComment(std::string line)
	{
		auto semilocon_position = find_first_nonespaced(line, ';');
		if(semilocon_position != std::string::npos)
		{
			line.erase(semilocon_position); // strip comment away
		}
		line = trim_nonescaped(line); // strip whitespaces away
		return line;
	}

	void Configuration::ProcessLines()
	{
		LinkValues link_values;
		PostponedSections postponed_sections;
		PostponedOptions postponed_options;

		bool is_section = false;
		SectionRange section_range;

		for(std::size_t line_index = 0; line_index < original_lines_.size(); ++line_index)
		{
			bool is_last = line_index == original_lines_.size();
			std::string line = StripComment(original_lines_[line_index].line_);
			if(line.empty())
			{
				continue;
			}

			if(line.front() == '[' && line.back() == ']')
			{
				if(is_section)
				{
					ProcessSection(section_range, link_values, postponed_sections, postponed_options);
				}

				is_section = true;
				section_range = { line_index, line_index };
			}
			else if(is_section)
			{
				section_range.second = line_index;
			}
			else
			{
				throw MalformedInputException();
			}
		}

		if(is_section)
		{
			ProcessSection(section_range, link_values, postponed_sections, postponed_options);
		}

		if(!postponed_sections.empty() || !postponed_options.empty())
		{
			throw MalformedInputException();
		}
	}

	void Configuration::ProcessSection(SectionRange range, LinkValues& link_values, PostponedSections& postponed_sections, PostponedOptions& postponed_options)
	{
		std::string line = StripComment(original_lines_[range.first].line_);
		if(line.front() != '[' || line.back() != ']')
		{
			throw InvalidOperationException();
		}

		std::vector<OptionNode*> dependancies;
		Link unresolved_link;
		std::string section_name;
		if(!ResolveLink(line.substr(1, line.length() - 2), link_values, &section_name, &dependancies, &unresolved_link))
		{
			postponed_sections.emplace(unresolved_link, range);
			return;
		}
		section_name = unescape(section_name);

		auto& section = RetrieveSection(section_name);
		if(section.loaded_)
		{
			throw MalformedInputException();
		}
		section.loaded_ = true;
		original_lines_[range.first].section_ = &section;

		for(std::size_t line_index = range.first + 1; line_index <= range.second; ++line_index)
		{
			original_lines_[line_index].section_ = &section;

			ProcessOption(line_index, link_values, postponed_sections, postponed_options);
		}
	}

	void Configuration::ProcessOption(OptionIndex index, LinkValues& link_values, PostponedSections& postponed_sections, PostponedOptions& postponed_options)
	{
		auto& original_line_data = original_lines_[index];

		std::string line = StripComment(original_lines_[index].line_);
		if(line.empty())
		{
			return;
		}

		auto assignment_position = find_first_nonespaced(line, '=');
		if(assignment_position == std::string::npos)
		{
			throw MalformedInputException();
		}

		Link unresolved_link;
		std::vector<OptionNode*> dependancies;
		OptionNode* option = original_line_data.option_;
		if(option == nullptr)
		{
			std::string option_name;
			if(!ResolveLink(trim_nonescaped(line.substr(0, assignment_position)), link_values, &option_name, &dependancies, &unresolved_link))
			{
				postponed_options.emplace(unresolved_link, index);
				return;
			}
			option_name = unescape(option_name);

			option = &(*original_line_data.section_)[option_name];
			original_line_data.option_ = option;
		}

		std::string option_value;
		std::string raw_value = trim_nonescaped(line.substr(assignment_position + 1));
		original_line_data.value_start_ = line.find_first_not_of(' ', assignment_position + 1);
		original_line_data.value_end_ = original_line_data.value_start_ + raw_value.length();
		if(!ResolveLink(raw_value, link_values, &option_value, &dependancies, &unresolved_link))
		{
			postponed_options.emplace(unresolved_link, index);
			return;
		}

		option->Load(option_value);

		Link link = { option->Section().Name(), option->Name() };
		link_values.emplace(link, option);
		for(auto it_range = postponed_sections.equal_range(link); it_range.first != it_range.second; ++it_range.first)
		{
			ProcessSection(it_range.first->second, link_values, postponed_sections, postponed_options);
		}
		for(auto it_range = postponed_options.equal_range(link); it_range.first != it_range.second; ++it_range.first)
		{
			ProcessOption(it_range.first->second, link_values, postponed_sections, postponed_options);
		}
	}


  void Configuration::Open(std::istream& input_stream)
  {
		loaded_ = true;
		original_lines_.clear();

		SectionNode* section_ptr = nullptr;

		std::string line;
		while(std::getline(input_stream, line))
		{
			original_lines_.emplace_back(line);
		}

		ProcessLines();

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
  

	void Configuration::OutputRestOfSections(std::ostream& output_stream, bool emit_default)
	{
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			auto& section = *section_it->second;
			if(section.IsLoaded())
			{
				continue;
			}

			output_stream << "[" << unescape(section.Name()) << "]";
			if(!section.comment_.empty())
			{
				output_stream << ";" << section.comment_;
			}
			output_stream << std::endl;

			OutputRestOfOptions(output_stream, section, emit_default);
		}
	}
	void Configuration::OutputRestOfOptions(std::ostream& output_stream, SectionNode& section, bool emit_default)
	{
		for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
		{
			auto& option = *option_it->second;
			if(option.IsLoaded())
			{
				continue;
			}

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

			output_stream << unescape(option.Name()) << "=" << option.Value();
			if(!option.comment_.empty())
			{
				output_stream << ";" << option.comment_;
			}
			output_stream << std::endl;
		}
	}
  void Configuration::Save(std::ostream& output_stream, OutputMethod output_method)
  {
		bool emit_default = output_method == OutputMethod::EMIT_DEFAULT_VALUES;

		SectionNode* section = nullptr;
		for(auto original_it = original_lines_.begin(), original_end = original_lines_.end(); original_it != original_end; ++original_it)
		{
			auto& original_line_data = *original_it;
			if(section != original_line_data.section_ && section != nullptr)
			{
				OutputRestOfOptions(output_stream, *section, emit_default);
			}

			section = original_line_data.section_;
			if(original_line_data.option_ != nullptr && original_line_data.option_->HasValue())
			{
				output_stream << original_line_data.line_.substr(0, original_line_data.value_start_);
				output_stream << original_line_data.option_->Value();
				output_stream << original_line_data.line_.substr(original_line_data.value_end_);
			}
			else
			{
				output_stream << original_line_data.line_;
			}
			output_stream << std::endl;
			//original_line_data.value_end_
		}

		OutputRestOfSections(output_stream, emit_default);
		/* UNUSED CODE
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
		*/
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
