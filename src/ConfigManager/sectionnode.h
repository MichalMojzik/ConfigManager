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

	/**
	* Trida pro interni reprezentaci sekce, se kterymi komunikuji Section a Option. 
	* Neni urceno pro pouziti uzivatelem.  
	*/ 
	class SectionNode
	{
	public:
		/**
		* Zakazany copy-constructor.
		*/
		SectionNode(const SectionNode& other) = delete;
		/**
		* Zakazany assign-constructor.
		*/
		SectionNode& operator=(const SectionNode& other) = delete;

		/**
		* Operator pro pohodlny pristup k reprezentacim option v datech.
		* \param Jmeno pozadovaneho optionu.
		*/
		OptionNode& operator[](const std::string& option_name);
		/**
		* Operator pro konstantni pristup k reprezentacim option v datech.
		* \param Jmeno pozadovaneho optionu.
		*/
		const OptionNode& operator[](const std::string& option_name) const;

		/**
		* Zpristupnuje jmeno sekce ke cteni.
		*/
		const std::string& Name() const;

		/**
		* Je tato sekce nactena ze vstupnich dat?
		*/
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