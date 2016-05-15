#ifndef CONFIGMANAGER_CONFIGURATION_H
#define CONFIGMANAGER_CONFIGURATION_H

#include <string>
#include <memory>

#include "exceptions.h"
#include "enums.h"
#include "section.h"

namespace ConfigManager
{
	/** Trida spravujici soubory s nastavenim.
	* Vzhledem k tomu, ze soubory maji format ".ini" a jejich format tedy neni uplne specifikovany, uchovava tato trida puvodni retezcove hodnoty.
	* Umoznuje pristupovat k sekcim a hodnotam daneho souboru pomoci metody SpecifySection. 
	* V destruktoru jsou pripadne zmeny zapsany zpet do souboru. 
	*/
	class Configuration
	{
	public:
		/** Zakladni kontruktor. 
		*
		*/
		Configuration();
		/** Konstruktor nacitajici nastaveni ze zvoleneho souboru. 
		* Muze vyhodit MalformedInput, IoException vyjimky.
		* \param filename Jmeno souboru. (Ocekavany format ".ini")
		* \param policy Spefikuje pristup k vstupnimu souboru (relaxed/strict).
		*/
		Configuration(std::string filename, InputFilePolicy policy = InputFilePolicy::NONE);

		/* Zakladni dektruktor. Vsechny zmenene volby se program pokusi ulozit na urcene misto (soubor/istream). 
		* Muze selhat obdobne jako Flush(), ale nevyhazuje vyjimky (jelikoz vyjimka z destruktoru by zpusobila pad programu, pokud jiz doslo k jine vyjimce).
		*/
		~Configuration();

		/** Umoznuje menit soubor konfigurace ke kteremu se tato trida vztahuje. 
		* \param filename Jmeno souboru. (Ocekavany format ".ini")
		* \param filename Spefikuje pristup k vstupnimu souboru (relaxed/strict).
		*/
		void SetConfigFile(std::string filename, InputFilePolicy policy = InputFilePolicy::NONE);

		/** Nastavuje jmeno vstupniho souboru. Zakladni pristup je IGNORE_NONEXISTANT(relaxed).
		* Muze vyhodit MalformedInput, IoException, WrongFormat, MandatoryMissing vyjimky.
		* \param filename Jmeno souboru. (Ocekavany format ".ini")
		*/
		void SetInputFile(std::string filename);
		
		/** Metoda pro nastaveni vstupniho streamu. 
		* Muze vyhodit MalformedInput, IoException, WrongFormat, MandatoryMissing vyjimky. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! tady asi zatim trida nevi, ze neco je mandatory - to prijde az u specify, ne?	
		* \param input_stream Vstupni istream.
		*/
		void SetInputStream(std::istream& input_stream);

		/** Metoda pro kontrolu, zda vstup odpovida jiz striktne specifikovanemu formatu. 
		* Muze vyhodit StrictException vyjimku.
		*/
		void CheckStrict();

		/** Meto pro nastaveni vystupniho souboru.
		* Muze vyhodit IoException vyjimku.
		* \param filename Jmeno vystupniho souboru.
		*/
		void SetOutputFile(std::string filename);
		/** Metoda pro nastaveni vystupniho streamu.
		* Muze vyhodit MalformedInput, IoException, WrongFormat, MandatoryMissing vyjimky.
		* \param output_stream Vystupni istream.
		*/
		void SetOutputStream(std::ostream& output_stream);

		/** Metoda vynucijici zapis zmenenych voleb na disk. 
		* Muze vyhodit IoException vyjimku (v pøipadì selhání otevøení souboru pro zápis).
		* \param output_method Volba zapisu prednastavenych hodnot. 
		*/
		void Flush(OutputMethod output_method = OutputMethod::NORMAL);

		/** Metoda specifikujici dalsi sekci. 
		* Muze vyhodit MandatoryMissing vyjimku.
		* \param section_name Jmeno sekce. 
		* \param requirement Povinnost dane sekce.
		* \param comments Komentare k dane sekci. 
		*/
		Section SpecifySection(std::string section_name, Requirement requirement = Requirement::OPTIONAL, const std::string comments = "");
	};
};

#endif