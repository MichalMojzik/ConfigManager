#ifndef CONFIGMANAGER_OPTIONNODE_H
#define CONFIGMANAGER_OPTIONNODE_H

#include "enums.h"

#include <string>

namespace ConfigManager
{
	class AbstractOptionProxy;
	class SectionNode;
	class Configuration;

	/**
	* Trida reprezentujici data volby v configuration. Muze a nemusi byt spojen s urcitou OptionProxy (ListOptionProxy).
	* Tato trida je urcena pouze pro vnitrni reprezentaci dat v knihovne.
	*/
	class OptionNode
	{
	public:
		/**
		* Nastavi spojeni s danou OptionProxy.
		* \param AbstractOptionProxy ktery odpovida temto datum. 
		*/
		void SetProxy(AbstractOptionProxy* proxy);
		/**
		* Nastavi zda je dana volba v konfiguraci povinna.
		* \param Volab OPTIONAL or MANDATORY.
		*/
		void SetRequirement(Requirement requirement);
		/**
		* Nastavi kommentar.
		* \param Komentar. 
		*/
		void SetComment(const std::string& comment);

		/**
		* Jmeno volby.
		*/
		const std::string& Name() const;

		/**
		* Stringova hodnota volby.
		*/
		std::string& Value();
		/**
		* Stringova hodnota volby (konstantni pristup).
		*/
		const std::string& Value() const;

		/**
		* Odkaz na sekci do ktere patri. 
		*/
		SectionNode& Section();
		/**
		* Konstantni odkaz na sekci do ktere patri.
		*/
		const SectionNode& Section() const;

		/**
		* Dotaz, zda byl nacten z vstupnich dat (nebo je zatim jenom specifikovan).
		*/
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