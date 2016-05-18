#include "sectionnode.h"

namespace ConfigManager
{
	const std::string& SectionNode::Name() const
	{
		return name_;
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

	SectionNode::SectionNode(Configuration& configuration, const std::string& name)
		: configuration_(configuration), name_(name), loaded_(false), is_specified_(false),
		requirement_(Requirement::OPTIONAL), comment_(""), data_()
	{

	}
}