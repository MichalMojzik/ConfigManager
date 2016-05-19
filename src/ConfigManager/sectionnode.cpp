#include "sectionnode.h"

#include "configuration.h"

namespace ConfigManager
{
	const std::string& SectionNode::Name() const
	{
		return name_;
	}

	bool SectionNode::IsLoaded() const
	{
		return loaded_;
	}
	bool SectionNode::IsSpecified() const
	{
		return is_specified_;
	}
	bool SectionNode::HasChanged() const
	{
		for(auto option_it = data_.begin(), option_end = data_.end(); option_it != option_end; ++option_it)
		{
			if(option_it->second->HasChanged())
			{
				return true;
			}
		}
		return false;
	}

	void SectionNode::SetRequirement(Requirement requirement)
	{
		// pokud uz byly parametry sekce jednou urceny, nemohou byt urceny znovu
		if(is_specified_)
		{
			throw InvalidOperationException();
		}
		is_specified_ = true;

		if(requirement == Requirement::MANDATORY && !IsLoaded() && configuration_.IsLoaded())
		{
			throw MandatoryMissingException();
		}
		requirement_ = requirement;
	}

	void SectionNode::SetComment(const std::string& comment)
	{
		comment_ = comment;
	}

	OptionNode& SectionNode::operator[](const std::string& option_name)
	{
		auto result = data_.emplace(option_name, std::unique_ptr<OptionNode>(new OptionNode(*this, option_name)));
		return *result.first->second;
	}

	const OptionNode& SectionNode::operator[](const std::string& option_name) const
	{
		return const_cast<SectionNode&>(*this)[option_name];
	}

	void SectionNode::Load()
	{
		// pokud byla sekce jiz drive nactena, pak ji nyni nacitame podruhe a nejedna se o platny .ini format
		if(loaded_)
		{
			throw MalformedInputException();
		}
		loaded_ = true;
	}

	SectionNode::SectionNode(Configuration& configuration, const std::string& name)
		: configuration_(configuration), name_(name), loaded_(false), is_specified_(false),
		requirement_(Requirement::OPTIONAL), comment_(""), data_()
	{

	}
}