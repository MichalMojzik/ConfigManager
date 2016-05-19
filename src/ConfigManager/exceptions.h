#ifndef CONFIGMANAGER_EXCEPTIONS_H
#define CONFIGMANAGER_EXCEPTIONS_H

namespace ConfigManager
{
	/**
	* Spolecny predek specifickych vyjimek teto knihovny.
	*/
	class ConfigurationException : public std::exception
	{
	public:
		ConfigurationException(const char* message) : message_(message) {}

		const char* what() const throw() override
		{
			return message_;
		}

	private:
		const char* message_;
	};
	/**
	* Vyjimka vyhazovana v pripade, ze doslo k operaci, ktera neni z nejakych duvodu platna a jedna se o chybu programatora, ktery takovou operaci vyvolal.
	*/
	class InvalidOperationException : public ConfigurationException
	{
	public:
		InvalidOperationException() : ConfigurationException("Invalid operation.") {}
		InvalidOperationException(const char* message) : ConfigurationException(message) {}
	};
	/**
	* Vyhazovana v pripade spatneho vstupu (spatne deklarace section ci option).
	*/
	class MalformedInputException : public ConfigurationException
	{
	public:
		MalformedInputException() : ConfigurationException("The input was not the expected configuration file format.") {}
	};

	/**
	* Vyjimka popisujici problem v komunikaci s vnjesim svetem. (Cteni, ci zapis.)
	*/
	class IoException : public ConfigurationException
	{
	public:
		IoException() : ConfigurationException("There was a problem with the I/O operations.") {}
	};

	/**
	* Spatny format vstupnich dat (pro option). 
	*/
	class WrongFormatException : public ConfigurationException
	{
	public:
		WrongFormatException() : ConfigurationException("The format of a value of an option differs from the one specified.") {}
	protected:
		WrongFormatException(const char* message) : ConfigurationException(message) {}
	};

	/**
	* Vstupni hodnota nevyhovuje omezeni (rozsahu) specifikovane hodnoty (Typespecifiers).
	*/
	class WrongRangeException : public WrongFormatException
	{
	public:
		WrongRangeException() : WrongFormatException("The range of a value of an option differs from the one specified.") {}
	};

	/**
	* MANDATORY option, nebo section nebyly nalezeny.
	*/
	class MandatoryMissingException : public ConfigurationException
	{
	public:
		MandatoryMissingException() : ConfigurationException("A mandatory element is missing from the configuration.") {}
	};

	/**
	* Vstupni data neodpovidaji specifikovanemu formatu presne, tedy prebyvaji nejake sekce nebo volby.
	* V pripade, ze nektere chybi a chybet nemaji, se vyhazuje MandatoryMissingException.
	*/
	class StrictException : public ConfigurationException
	{
	public:
		StrictException() : ConfigurationException("The input doesn't match the specified format precisely.") {}
	};

	/**
	* Vyhazovan tridou ListOptionProxy v pripade, ze dochazi k pristupu k prvku, ktery je mimo rozsah seznamu.
	*/
	class OutOfBoundsException : public ConfigurationException
	{
	public:
		OutOfBoundsException() : ConfigurationException("An attempt was made to access an element of a list outside of bounds.") {}
	};
}

#endif
