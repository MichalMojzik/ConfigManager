#ifndef CONFIGMANAGER_SECTIONNODE_H
#define CONFIGMANAGER_SECTIONNODE_H

#include "enums.h"
#include "optionnode.h"

#include <string>
#include <map>
#include <memory>

namespace ConfigManager
{
	class Configuration;

	/* tridy pro interni reprezentaci sekci a voleb, se kterymi komunikuji Section a Option */ 
	class SectionNode
	{
	public:
		SectionNode(const SectionNode& other) = delete;
		SectionNode& operator=(const SectionNode& other) = delete;

		OptionNode& operator[](const std::string& option_name);
		const OptionNode& operator[](const std::string& option_name) const;

		const std::string& Name() const;

		bool IsLoaded() const;
	private:
		SectionNode(Configuration& configuration, const std::string& name);
		Configuration& configuration_;

		std::string name_;
		bool loaded_;
		bool is_specified_;
		Requirement requirement_;
		std::string comment_;

		std::map<std::string, std::unique_ptr<OptionNode> > data_;

		friend Configuration;
	};
}
#endif