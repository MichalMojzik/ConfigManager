#include "optionnode.h"

#include "sectionnode.h"
#include "configuration.h"

namespace ConfigManager
{
	const std::string& OptionNode::Name() const
	{
		return name_;
	}

	void OptionNode::SetProxy(AbstractOptionProxy* proxy)
	{
		proxy_ = proxy;
	}

	void OptionNode::SetRequirement(Requirement requirement)
	{
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
		if (loaded_)
			throw MalformedInputException();

		loaded_ = true;
		value_ = value;

		if (proxy_ != nullptr)
		{
			proxy_->AssignValueData(value_);
		}
	}

	bool OptionNode::IsLoaded() const
	{
		return loaded_;
	}

	bool OptionNode::HasValue() const
	{
		return loaded_ || changed_;
	}

	bool OptionNode::LoadDefaultValue()
	{
		if (proxy_ != nullptr)
		{
			proxy_->RegenerateValueData();
			return true;
		}
		else
		{
			return false;
		}
	}

	OptionNode::OptionNode(SectionNode& section, const std::string& name)
		: section_(section), name_(name), loaded_(false), changed_(false),
		proxy_(nullptr), requirement_(Requirement::OPTIONAL), comment_(""),
		value_("")
	{
	}
}