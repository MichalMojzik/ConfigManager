#ifndef CONFIGMANAGER_TYPES_SPECIFIERS_H
#define CONFIGMANAGER_TYPES_SPECIFIERS_H

#include <string>
#include <map>
#include "exceptions.h"

namespace ConfigManager
{


	/** 
	* Trida realizujici prevod z textu do typu boolean a zpet. 
	*/
	class BooleanSpecifier
	{
	public:
		/**
		* Tato definice typu urcuje navratovy typ. 
		*/
		typedef bool ValueType;

		/**
		* Zakladni konstruktor.
		*/
		BooleanSpecifier();

		/**
		* Metoda prevadejici text na vyslednou hodnotu.
		* Muze vyhodit WrongFormatException vyjimku.
		* \param data Vstupni data.
		*/
		ValueType FromString(const std::string& data); 
		
		/**
		* Metoda prevadejici nastavenou/zmenenou hodnotu zpet do textoveho formatu.
		* Muze vyhodit WrongFormatException vyjimku.
		* \param Hodnota preveditelna na string.
		*/
		std::string ToString(const ValueType& value); 
	};

	/**
	* Trida realizujici prevod z textu do typu integer a zpet.
	*/
	class IntegerSpecifier
	{
		typedef long long int64;
	public:
		/**
		* \copydoc BooleanSpecifier::ValueType 
		*/
		typedef int64 ValueType;
		/**
		* Konstruktor ktery neklade omezeni mezeni na rozsah hodnot.
		* Rozsah hodnot je potom dan rozsahem zvoleneho typu. 
		*/
		IntegerSpecifier();
		/**
		* Konstruktor ktery specifikuje rozsah pro vystupni parametry.
		* \param ramge_start dolni mez rozsahu. 
		* \param ramge_end horni mez rozsahu.
		*/
		IntegerSpecifier(int64 range_start, int64 range_end); 

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data);
		/**
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(const ValueType& value); 
	};

	/**
	* Trida realizujici prevod z textu do typu unsigned integer a zpet.
	*/
	class UnsignedSpecifier
	{
		typedef unsigned long long uint64;
	public:
		/**
		*\copydoc BooleanSpecifier::ValueType
		*/
		typedef uint64 ValueType;
		/**
		* \copydoc IntegerSpecifier::IntegerSpecifier()
		*/
		UnsignedSpecifier(); 
		/**
		* \copydoc IntegerSpecifier::IntegerSpecifier(int64 range_start, int64 range_end)
		*/
		UnsignedSpecifier(uint64 range_start, uint64 range_end); 

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data); 
		/**
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(const ValueType& value);
	};

	/**
	* Trida realizujici prevod z textu do typu float a zpet.
	*/
	class FloatSpecifier
	{
	public:
		/**
		* \copydoc BooleanSpecifier::ValueType
		*/
		typedef double ValueType;

		/**
		* \copydoc IntegerSpecifier::IntegerSpecifier()
		*/
		FloatSpecifier() { }
		/**
		* Konstruktor se specifikovanym rozsahem pro parametr.
		* \param range_start Spodni meze rozsahu.
		* \param range_end Horni meze rozsahu.
		*/
		FloatSpecifier(double range_start, double range_end) { }

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data) { return 0; }
		/**
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(const ValueType& value) { return ""; }
	};

	/**  
	* Trida realizujici prevod z textu do typu vyctovy typ a zpet.
	*	Presny vyctovy typ je specifikovan parametrem sablony. 
	*/
	template<typename TResult>
	class EnumSpecifier
	{
	public:
		/** \copydoc BooleanSpecifier::ValueType
		*
		*/
		typedef TResult ValueType;

		/** Zaladni konstruktor. 
		* Meze vyctoveho typu jsou jiz specifikovany.
		*/

		EnumSpecifier(const std::map<std::string, ValueType>& value_mapping);

		/** \copydoc BooleanSpecifier::FromString(const std::string& data)
		*
		*/
		ValueType FromString(const std::string& data);
		/** \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*
		*/
		const std::string& ToString(const ValueType& value);

	private:
		std::map<std::string, ValueType> mapping_;
	};
	/**
	* Trida realizujici prevod z textu do typu string a zpet.	 
	*/
	class StringSpecifier
	{
	public:
		/**
		*\copydoc BooleanSpecifier::ValueType
		*/
		typedef std::string ValueType;
		/**
		* Zakladni konstruktor.
		*/
		StringSpecifier(); 

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data);
		/** 
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(const ValueType& value);
	};
};


/* ================ EnumSpecifier implementation */
namespace ConfigManager
{
	template<typename TResult>
	EnumSpecifier<TResult>::EnumSpecifier(const std::map<std::string, TResult>& value_mapping)
		: mapping_(value_mapping)
	{
	}

	template<typename TResult>
	auto EnumSpecifier<TResult>::FromString(const std::string& data) -> ValueType
	{
		auto valueIt = mapping_.find(data);
		if(valieIt == mapping_.end())
			throw WrongFormat();
		return valueIt->second;
	}

	template<typename TResult>
	const std::string& EnumSpecifier<TResult>::ToString(const ValueType& value)
	{
		for(auto it = mapping_.begin(), end = mapping_.end(); it != end; ++it)
		{
			if(it->second == value)
			{
				return it->first;
			}
		}
		throw WrongFormat();
	}
}

#endif
