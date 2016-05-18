#ifndef CONFIGMANAGER_SECTION_H
#define CONFIGMANAGER_SECTION_H

#include <string>
#include "exceptions.h"
#include "option.h"
#include "enums.h"

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
		* Muze vyhodit WrongFormatException, MandatoryMissingException vyjimky.
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
			const std::string comments = ""
			)
		{
			return OptionProxy<TypeSpecifier>(default_value, type_specifier);
		}

		/**
		* \copydoc Section::SpecifyOption(std::string name, const TypeSpecifier& type_specifier, const typename TypeSpecifier::ValueType& default_value, Requirement optional = Requirement::OPTIONAL, const std::string comments = "")
		* Muze vyhodit WrongFormatException, MandatoryMissingException vyjimky.
		*/
		template<typename TypeSpecifier>
		ListOptionProxy<TypeSpecifier> SpecifyListOption(
			std::string option_name,
			const TypeSpecifier& type_specifier,
			const std::vector<typename TypeSpecifier::ValueType>& default_value,
			Requirement optional = Requirement::OPTIONAL, 
			const std::string comments = ""
			)
		{
			return ListOptionProxy<TypeSpecifier>(default_value, type_specifier);
		}
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



#endif