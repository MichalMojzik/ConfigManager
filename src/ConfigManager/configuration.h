#ifndef CONFIGMANAGER_CONFIGURATION_H
#define CONFIGMANAGER_CONFIGURATION_H

#include <string>
#include <memory>
#include <map>

#include "exceptions.h"
#include "enums.h"
#include "section.h"

namespace ConfigManager
{
	/* tridy pro interni reprezentaci sekci a voleb, se kterymi komunikuji Section a Option */
	class OptionNode;

	class SectionNode
	{
	public:
		SectionNode(const SectionNode& other) = delete;
		SectionNode& operator=(const SectionNode& other) = delete;

		OptionNode& operator[](const std::string& option_name);
		const OptionNode& operator[](const std::string& option_name) const;

		const std::string& Name() const;
	private:
		SectionNode(Configuration& configuration);
		Configuration& configuration_;

		std::string name_;
		bool loaded_;
		bool is_specified_;
		Requirement requirement_;
		std::string comment_;

		std::map<std::string, std::unique_ptr<OptionNode>> data_;

		friend Configuration;
	};

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
	private:
		void Load(const std::string& value);

		OptionNode(SectionNode& section);
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

	/**
	* Trida spravujici soubory s nastavenim.
	* Vzhledem k tomu, ze soubory maji format ".ini" a jejich format tedy neni uplne specifikovany, uchovava tato trida puvodni retezcove hodnoty.
	* Umoznuje pristupovat k sekcim a hodnotam daneho souboru pomoci metody SpecifySection. 
	* V destruktoru jsou pripadne zmeny zapsany zpet do souboru. 
	*/
	class Configuration
	{
	public:
		/**
		* Zakladni kontruktor. 
		*/
		Configuration();

		/**
		* Metoda pro nastaveni vstupniho streamu. 
		* Muze vyhodit MalformedInputException, IoException, WrongFormatException, MandatoryMissingException vyjimky. 	
		* \param input_stream Vstupni istream.
		*/
		void Open(std::istream& input_stream);

		/**
		* Metoda pro kontrolu, zda vstup odpovida jiz striktne specifikovanemu formatu. 
		* Muze vyhodit StrictException vyjimku.
		*/
		void CheckStrict();

		/**
		* Metoda vynucijici zapis zmenenych voleb na disk. 
		* Muze vyhodit IoException vyjimku (v pøipadì selhání otevøení souboru pro zápis).
		* \param output_method Volba zapisu prednastavenych hodnot. 
		*/
		void Save(std::ostream& output_stream, OutputMethod output_method = OutputMethod::NORMAL);

		/**
		* Metoda specifikujici dalsi sekci. 
		* Muze vyhodit MandatoryMissingException vyjimku.
		* \param section_name Jmeno sekce. 
		* \param requirement Povinnost dane sekce.
		* \param comments Komentare k dane sekci. 
		*/
		Section SpecifySection(const std::string& section_name, Requirement requirement = Requirement::OPTIONAL, const std::string& comments = "");

		Section operator[](const std::string& section_name);

	private:
		SectionNode& RetrieveSection(const std::string& section_name);

		std::vector<std::string> original_lines_;
		std::map<std::string, std::unique_ptr<SectionNode>> data_;
	};


	class ConfigurationFile : public Configuration
	{
	public:
		/**
		* Konstruktor nacitajici nastaveni ze zvoleneho souboru.
		* Muze vyhodit MalformedInputException, IoException vyjimky.
		* \param filename Jmeno souboru. (Ocekavany format ".ini")
		* \param policy Spefikuje pristup k vstupnimu souboru (relaxed/strict).
		*/
		ConfigurationFile(std::string filename, InputFilePolicy policy = InputFilePolicy::NONE);

		/**
		* Zakladni dektruktor. Vsechny zmenene volby se program pokusi ulozit na urcene misto (soubor/istream).
		* Muze selhat obdobne jako Flush(), ale nevyhazuje vyjimky (jelikoz vyjimka z destruktoru by zpusobila pad programu, pokud jiz doslo k jine vyjimce).
		*/
		~ConfigurationFile();

	private:
		std::string filename_;
	};
};

#endif