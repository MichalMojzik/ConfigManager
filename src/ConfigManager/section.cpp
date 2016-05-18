#include "section.h"
#include "configuration.h"

namespace ConfigManager
{
	Section::Section(SectionNode& section_node)
		: section_node_(&section_node)
	{
	}

	const std::string& Section::GetName()
	{
		return section_node_->Name();
	}

	std::string Section::operator[](const std::string& option_name)
	{
		auto& option_node = (*section_node_)[option_name];
		return option_node.Value();
	}
}

