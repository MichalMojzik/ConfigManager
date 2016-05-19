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
	*/
	class Configuration
	{
	public:
		/**
		* Zakladni kontruktor.
		*/
		Configuration();

		/**
		* Destruktor odvazujici vsechna privazene OptionProxy.
		*/
		~Configuration();

		/**
		* Metoda pro nastaveni vstupniho streamu. 
		* Muze vyhodit:
		* MalformedInputException (spatny format vstupnich dat - section, ci option), 
		* IoException (problem pri ziskavani vstupu),
		* WrongFormatException (pokud hodnota nejakeho option nesedi do specifikovaneho typu), 
		* MandatoryMissingException  (pokud chybi jiz specifikovana povinna sekce nebo hodnota).
		* \param input_stream Vstupni istream.
		*/
		void Open(std::istream& input_stream);

		/**
		* Metoda pro kontrolu, zda vstup odpovida jiz striktne specifikovanemu formatu. 
		* Muze vyhodit StrictException vyjimku, pokud format nesedi.
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
		* Muze vyhodit MandatoryMissingException vyjimku (v pripade ze nenase MANDATORY sekci).
		* \param section_name Jmeno sekce. 
		* \param requirement Povinnost dane sekce.
		* \param comments Komentare k dane sekci. 
		*/
		Section SpecifySection(const std::string& section_name, Requirement requirement = Requirement::OPTIONAL, const std::string& comments = "");

		/**
		* Metoda zpristupnujici dalsi sekci. Lze pouzit nezavisle na zavolani SpecifySection.
		* \param section_name Jmeno sekce.
		*/
		Section operator[](const std::string& section_name);

		/**
		* Metoda vracejici, zda je v aktualne nactena nejaka konfigurace.
		*/
		bool IsLoaded() const;

	private:
		SectionNode& RetrieveSection(const std::string& section_name);

		typedef std::pair<std::string, std::string> Link;
		typedef std::pair<std::size_t, std::size_t> SectionRange;
		typedef std::size_t OptionIndex;
		typedef std::map<Link, OptionNode*> LinkValues;
		typedef std::multimap<Link, SectionRange> PostponedSections;
		typedef std::multimap<Link, OptionIndex> PostponedOptions;

		// Pomocna rekurzivni metoda pro vyhodnoceni vsech linku v ramci retezce s vyuzitim lookup-table pro linky. Vraci true, kdyz se zdarilo, false naopak.
		// Navic muze vracet i retezec s vyhodnocenymi linky (result, appenduje se), seznam linku, ktere se uspesne vyhodnotily (resolved_links, appenduje se)
		//   nebo link, ktery se nevyhodnotil (unresolved_link)
		bool ResolveLink(const std::string& value, LinkValues& link_values, std::string* result, std::vector<OptionNode*>* resolved_links, Link* unresolved_link);
		// Pomocna metoda zpracovavajici radky nactene v original_lines_.
		void ProcessLines();
		// Pomocna metoda zpracovavajici sekci v urcitem rozsahu radku nactene v original_lines_ s vyuzitim danych kontextu.
		void ProcessSection(SectionRange range, LinkValues& link_values, PostponedSections& postponed_sections, PostponedOptions& postponed_options);
		// Pomocna metoda zpracovavajici volbu na urcitem radku nactenem v original_lines_ s vyuzitim danych kontextu.
		// Muze rekurzivne volat ProcessSection a ProcessOption pro vyhodnoceni odlozenych sekci a voleb.
		void ProcessOption(OptionIndex index, LinkValues& link_values, PostponedSections& postponed_sections, PostponedOptions& postponed_options);

		// Pomocna metoda pro vypis sekci, ktere nebyly soucasti puvodniho vstupu.
		void OutputRestOfSections(std::ostream& output_stream, bool emit_default);
		// Pomocna metoda pro vypis voleb dane sekce, ktere nebyly soucasti puvodniho vstupu.
		void OutputRestOfOptions(std::ostream& output_stream, SectionNode& section, bool emit_default);

		// Privatni pomocna datova struktura pro udrzeni informaci o puvodnim vstupu pro ucely zachovani formatovani.
		struct OriginalData
		{
			std::string line_; // cely radek nacteny ze vstupu
			SectionNode* section_; // sekce, ktera je na danem radku definovana (nebo jsou jeji volby nedefinovany); null pokud zadna takova sekce neni
			OptionNode* option_; // volba, ktera je na danem radku definovana; null pokud zadna takova volba neni

			// pokud je na radku volba, offsety do radku na mista, kde zacina a konci hodnota teto volby
			std::size_t value_start_;
			std::size_t value_end_;

			OriginalData(const std::string& line);
		};

		bool loaded_;
		std::vector<OriginalData> original_lines_;
		std::map<std::string, std::unique_ptr<SectionNode>> data_;
	};
};

#endif