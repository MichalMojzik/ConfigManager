#ifndef CONFIGMANAGER_CONFIGURATION_H
#define CONFIGMANAGER_CONFIGURATION_H

#include <string>
#include <memory>
#include <map>
#include "exceptions.h"
#include "enums.h"
#include "section.h"
#include "sectionnode.h"
#include "optionnode.h"

namespace ConfigManager
{	
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