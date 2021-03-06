#ifndef CONFIGMANAGER_TYPES_SPECIFIERS_H
#define CONFIGMANAGER_TYPES_SPECIFIERS_H

#include <string>
#include <map>
#include "exceptions.h"
#include <limits>

namespace ConfigManager
{
	/**
	* Sablonova trida umoznujici sndanou kontrolu rozsahu hodnot. 
	*/
	template<typename TResult>
	class RangeConstraint
	{
	public:
		/**
		* Konstruktor s maximalnim rozsahem.
		*/
		RangeConstraint();
		/**
		* Konstruktor s omezenym rozsahem. (Neostre nerovnosti.)
		* Vyhazuje InvalidOperationException 
		* (v pripade ze je minimalni hodnota vetsi nez maximalni).
		* \param Minimalni povolena hodnota. 
		* \param Maximalni povolenta hodnota. 
		*/
		RangeConstraint(const TResult& range_start, const TResult& range_end);

		/**
		* Metoda provadejici kontrolu, zda je podminka popisovana tridou splnena.
		* Pokud ano, vraci svuj parametr.
		* Jinak vyhazuje vyjimku WrongRangeException.
		*/
		const TResult& CheckConstraint(const TResult& value);

	private:
		TResult range_start_;
		TResult range_end_;
	};


	/**
	* Trida realizujici prevod z textu do nejakoho typu a zpet. Tato trida slouzi pouze jako ukazka rozhrani, ktere musi TypeSpecifiery splnovat.
	*/
	template<typename TValueType>
	class TypeSpecifier
	{
	public:
		/**
		* Tato definice typu urcuje navratovy typ.
		*/
		typedef TValueType ValueType;

		/**
		* Vychozi konstruktor je zapotrebi v pripadech, jelikoz musi byt nektere tridy, ktere TypeSpecifiery obsahuji, default inicializovany.
		*/
		TypeSpecifier();

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
	* Trida realizujici prevod z textu do typu boolean a zpet. 
	*/
	class BooleanSpecifier
	{
	public:
		/**
		*\copydoc TypeSpecifier::ValueType
		*/
		typedef bool ValueType;

		/**
		* Zakladni konstruktor.
		*/
		BooleanSpecifier();

		/**
		*\copydoc TypeSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data);
		/**
		* \copydoc TypeSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(ValueType value);
	};

	/**
	* Trida realizujici prevod z textu do typu integer a zpet.
	*/
	class IntegerSpecifier : private RangeConstraint<int64_t>
	{
	public:
		/**
		*\copydoc BooleanSpecifier::ValueType
		*/
		typedef int64_t ValueType;

		/**
		* Konstruktor ktery neklade omezeni mezeni na rozsah hodnot.
		* Rozsah hodnot je potom dan rozsahem zvoleneho typu. 
		*/
		IntegerSpecifier();
		/**
		* Konstruktor ktery specifikuje rozsah pro vystupni parametry. ?? nema vyhazovat vyjimku kdyz range_start > range_end? 
		* \param range_start dolni mez rozsahu. 
		* \param range_end horni mez rozsahu.
		*/
		IntegerSpecifier(int64_t range_start, int64_t range_end); 

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data);
		/**
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(ValueType value);
	};

	/**
	* Trida realizujici prevod z textu do typu unsigned integer a zpet.
	*/
	class UnsignedSpecifier : private RangeConstraint<uint64_t>
	{
	public:
		/**
		*\copydoc BooleanSpecifier::ValueType
		*/
		typedef uint64_t ValueType;
		/**
		* \copydoc IntegerSpecifier::IntegerSpecifier()
		*/
		UnsignedSpecifier(); 
		/**
		* \copydoc IntegerSpecifier::IntegerSpecifier(int64 range_start, int64 range_end)
		*/
		UnsignedSpecifier(uint64_t range_start, uint64_t range_end); 

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data); 
		/**
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(ValueType value);
	};

	/**
	* Trida realizujici prevod z textu do typu float a zpet.
	*/
	class FloatSpecifier : private RangeConstraint<double>
	{
	public:
		/**
		* \copydoc BooleanSpecifier::ValueType
		*/
		typedef double ValueType;

		/**
		* \copydoc IntegerSpecifier::IntegerSpecifier()
		*/
		FloatSpecifier();
		/**
		* Konstruktor se specifikovanym rozsahem pro parametr.
		* \param range_start Spodni meze rozsahu.
		* \param range_end Horni meze rozsahu.
		*/
		FloatSpecifier(double range_start, double range_end);

		/**
		* \copydoc BooleanSpecifier::FromString(const std::string& data)
		*/
		ValueType FromString(const std::string& data);
		/**
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		std::string ToString(ValueType value);
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

		EnumSpecifier();

		/** Zaladni konstruktor.
		* Meze vyctoveho typu jsou jiz specifikovany.
		*/
		EnumSpecifier(const std::map<std::string, ValueType>& value_mapping);

		/** \copydoc BooleanSpecifier::FromString(const std::string& data)
		*
		*/
		const ValueType& FromString(const std::string& data);
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
		const ValueType& FromString(const std::string& data);
		/** 
		* \copydoc BooleanSpecifier::ToString(const ValueType& value)
		*/
		const std::string& ToString(const ValueType& value);
	};
};


/* ================ RangeConstraint implementation */
namespace ConfigManager
{
	template<typename TResult>
	RangeConstraint<TResult>::RangeConstraint()
		: range_start_(std::numeric_limits<TResult>::lowest()), range_end_(std::numeric_limits<TResult>::max())
	{
	}

	template<typename TResult>
	RangeConstraint<TResult>::RangeConstraint(const TResult& range_start, const TResult& range_end)
		: range_start_(range_start), range_end_(range_end)
	{
		if (range_end < range_start)
		{
			throw InvalidOperationException("Specified values do not form a valid range.");
		}
	}

	template<typename TResult>
	const TResult& RangeConstraint<TResult>::CheckConstraint(const TResult& value)
	{
		if (value < range_start_ || value > range_end_)
		{
			throw WrongRangeException();
		}
		return value;
	}
}


/* ================ EnumSpecifier implementation */
namespace ConfigManager
{
	template<typename TResult>
	EnumSpecifier<TResult>::EnumSpecifier()
		: mapping_()
	{
	}

	template<typename TResult>
	EnumSpecifier<TResult>::EnumSpecifier(const std::map<std::string, TResult>& value_mapping)
		: mapping_(value_mapping)
	{
	}

	template<typename TResult>
	auto EnumSpecifier<TResult>::FromString(const std::string& data) -> const ValueType&
	{
		// pokusime se najit hodnotu, na kterou je namapovany zadany retezec
		auto valueIt = mapping_.find(data);
		if (valueIt == mapping_.end())
		{
			throw WrongFormatException();
		}
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
		throw WrongFormatException();
	}
}

#endif
