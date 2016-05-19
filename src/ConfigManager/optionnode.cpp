#include "optionnode.h"

#include "sectionnode.h"
#include "configuration.h"

namespace ConfigManager
{
	const std::string& OptionNode::Name() const
	{
		return name_;
	}

	void OptionNode::SetSpecified()
	{
		is_specified_ = true;
	}

	void OptionNode::SetProxy(AbstractOptionProxy* proxy)
	{
		proxy_ = proxy;
	}

	void OptionNode::SetRequirement(Requirement requirement)
	{
		// pokud uz byly parametry volby jednou urceny, nemohou byt urceny znovu
		// take nelze vytvorit dve proxy ukazujici na stejnou volbu
		if(is_specified_)
		{
			throw InvalidOperationException();
		}

		if(requirement == Requirement::MANDATORY && !IsLoaded() && Section().IsLoaded())
		{
			throw MandatoryMissingException();
		}
		requirement_ = requirement;
	}

	void OptionNode::SetComment(const std::string& comment)
	{
		comment_ = comment;
	}

	std::string& OptionNode::Value()
	{
		return value_;
	}

	const std::string& OptionNode::Value() const
	{
		return value_;
	}

	void OptionNode::Assign(const std::string& value)
	{
		Value() = value;
		changed_ = true;
	}

	SectionNode& OptionNode::Section()
	{
		return section_;
	}

	const SectionNode& OptionNode::Section() const
	{
		return section_;
	}

	void OptionNode::Load(const std::string& value)
	{
		// pokud byla volba jiz drive nactena, pak ji nyni nacitame podruhe a nejedna se o platny .ini format
		if (loaded_)
		{
			throw MalformedInputException();
		}

		loaded_ = true;
		value_ = value;

		// pokud byla jiz vytvorena proxy, naloadujeme ji daty
		if (proxy_ != nullptr)
		{
			proxy_->AssignValueData(value_);
		}
	}

	bool OptionNode::IsLoaded() const
	{
		return loaded_;
	}
	bool OptionNode::IsSpecified() const
	{
		return is_specified_;
	}
	bool OptionNode::HasChanged() const
	{
		return changed_;
	}

	OptionNode::OptionNode(SectionNode& section, const std::string& name)
		: section_(section), name_(name), loaded_(false), changed_(false),
		proxy_(nullptr), requirement_(Requirement::OPTIONAL), comment_(),
		value_(), is_specified_(false)
	{
	}
}