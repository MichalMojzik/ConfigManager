#ifndef CONFIGMANAGER_SECTION_H
#define CONFIGMANAGER_SECTION_H

#include <string>
#include "exceptions.h"
#include "option.h"
#include "enums.h"	
#include "sectionnode.h"

namespace ConfigManager
{
	class SectionNode;

	/**
	* Trida realizujici sekci voleb. Predavana ven z knihovny.
	*/
	class Section
	{
	public:
		/**
		* Metoda urcujici novou volbu v dane sekci.
		* Muze vyhodit MandatoryMissingException (pokud neni odpovidajici 
		* sekce ve vstupnich datech) vyjimky.
		*   \param name Jmeno nove volby.
		*   \param type_specifier Trida pro preklad volby z a  do retezce.
		*   \param default_value Prednastavena hodnota.
		*   \param optional Povinnost volby.
		*   \param comments Komentar.
		*/
		template<typename TypeSpecifier>
		OptionProxy<TypeSpecifier> SpecifyOption(
			std::string option_name,
			const TypeSpecifier& type_specifier,
			const typename TypeSpecifier::ValueType& default_value,
			Requirement optional = Requirement::OPTIONAL,
			const std::string& comment = ""
			);

		/**
		* \copydoc Section::SpecifyOption(std::string name, const TypeSpecifier& type_specifier, const typename TypeSpecifier::ValueType& default_value, Requirement optional = Requirement::OPTIONAL, const std::string comments = "")
		* Muze vyhodit MandatoryMissingException() vyjimky.
		*/
		template<typename TypeSpecifier>
		ListOptionProxy<TypeSpecifier> SpecifyListOption(
			std::string option_name,
			const TypeSpecifier& type_specifier,
			const std::vector<typename TypeSpecifier::ValueType>& default_value,
			Requirement optional = Requirement::OPTIONAL,
			const std::string& comment = ""
			);

		/**
		* Metoda pro pristup k jmenu sekce.
		*/
		const std::string& GetName();

		/**
		* Primy pristup ke retezcove reprezentaci volby.
		*/
		std::string operator[](const std::string& option_name);

	private:
		/**
		* Soukromy konstruktor pouzivany pri specifikaci sekce.
		* \param name Jmeno sekce.
		*/
		Section(SectionNode& section_node);

		SectionNode* section_node_;

		friend class Configuration;
	};
}

/* ================ Section */
namespace ConfigManager
{
	template<typename TypeSpecifier>
	OptionProxy<TypeSpecifier> Section::SpecifyOption(
		std::string option_name,
		const TypeSpecifier& type_specifier,
		const typename TypeSpecifier::ValueType& default_value,
		Requirement optional,
		const std::string& comment
		)
	{
		auto& option_node = (*section_node_)[option_name];
		option_node.SetRequirement(optional);
		option_node.SetComment(comment);
		return OptionProxy<TypeSpecifier>(option_node, default_value, type_specifier);
	}

	template<typename TypeSpecifier>
	ListOptionProxy<TypeSpecifier> Section::SpecifyListOption(
		std::string option_name,
		const TypeSpecifier& type_specifier,
		const std::vector<typename TypeSpecifier::ValueType>& default_value,
		Requirement optional,
		const std::string& comment
		)
	{
		auto& option_node = (*section_node_)[option_name];
		option_node.SetRequirement(optional);
		option_node.SetComment(comment);
		return ListOptionProxy<TypeSpecifier>(option_node, default_value, type_specifier);
	}
}

#endif