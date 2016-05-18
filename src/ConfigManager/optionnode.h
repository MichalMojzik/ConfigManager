#ifndef CONFIGMANAGER_OPTIONNODE_H
#define CONFIGMANAGER_OPTIONNODE_H

#include "configuration.h"
#include "sectionnode.h"
#include "option.h"

namespace ConfigManager
{
	class OptionNode
	{
	public:
		void SetProxy(AbstractOptionProxy* proxy);
		void SetRequirement(Requirement requirement);
		void SetComment(const std::string& comment);

		const std::string& Name() const;

		std::string& Value();
		const std::string& Value() const;

		SectionNode& Section();
		const SectionNode& Section() const;

		bool IsLoaded() const;
		bool HasValue() const;
	private:
		void Load(const std::string& value);
		bool LoadDefaultValue();

		OptionNode(SectionNode& section, const std::string& name);
		SectionNode& section_;

		std::string name_;
		bool loaded_;
		bool changed_;
		AbstractOptionProxy* proxy_;
		Requirement requirement_;
		std::string comment_;

		std::string value_;

		friend SectionNode;
		friend Configuration;
	};
}
#endif