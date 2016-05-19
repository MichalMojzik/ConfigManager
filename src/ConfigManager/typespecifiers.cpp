#include "typespecifiers.h"

namespace ConfigManager
{
	// vraci: ((baze; znamenko); pocet znaku, ktere se maji preskocit
	std::pair<std::pair<int, int>, int> detect_base(const std::string& data)
	{
		// inicializace: nepreskakujeme nic a jedna se o pozitivni cislo
		int sign = 1;
		std::size_t skip = 0;

		// ^[+-]? - preskoceni a poznamenani pripadneho znamenka
		if(data.length() > 0 && (data.at(0) == '+' || data.at(0) == '-'))
		{
			if(data.at(0) == '-')
			{
				sign = -1;
			}
			skip = 1;
		}

		// pokud nezbyvaji zadne znaky, pak se nejedna o cislo (prazdny retezec, "+" nebo "-")
		if(data.length() == skip)
		{
			throw WrongFormatException();
		}

		// rozskok podle baze
		switch(data.at(skip))
		{
			case '0': // ruzne moznosti
				if(data.length() > skip + 1)
				{
					switch(data.at(skip + 1))
					{
						case 'x': // 0x.* - hexadecimalni zapis
							return std::make_pair(std::make_pair(16, sign), skip + 2);
						case 'b': // 0b.* - binarni zapis
							return std::make_pair(std::make_pair(2, sign), skip + 2);
						default:  // 0.*  - oktantovy zapis
							return std::make_pair(std::make_pair(8, sign), skip + 1);
					}
				}
				else // 0 - jen nula
				{
					return std::make_pair(std::make_pair(10, sign), skip);
				}
			case '1': case '2':	case '3':	case '4':	case '5':
			case '6':	case '7':	case '8':	case '9': // [1-9].* - dekadicky zapis
				return std::make_pair(std::make_pair(10, sign), skip);
			default: // prvni znak neni cislice, retezec nereprezentuje cislo
				throw WrongFormatException();
		}
	}

	BooleanSpecifier::BooleanSpecifier()
	{
	}

	BooleanSpecifier::ValueType ConfigManager::BooleanSpecifier::FromString(const std::string & data)
	{
		if( // vsechny mozne retezce vyjadrujici false 
			data == "0" ||
			data == "f" ||
			data == "n" ||
			data == "off" ||
			data == "no" ||
			data == "disabled"
			)
		{
			return false;
		}
		else if( // vsechny mozne retezce vyjadrujici true
			data == "1" ||
			data == "t" ||
			data == "y" ||
			data == "on" ||
			data == "yes" ||
			data == "enabled"
			)
		{
			return true;
		}
		else
		{
			throw WrongFormatException();
		}
	}

	std::string BooleanSpecifier::ToString(BooleanSpecifier::ValueType value)
	{
		if(value)
		{
			return "on";
		}
		else
		{
			return "off";
		}
	}



	IntegerSpecifier::IntegerSpecifier()
	{
	}

	IntegerSpecifier::IntegerSpecifier(int64_t range_start, int64_t range_end)
		: RangeConstraint<ValueType>(range_start, range_end)
	{
	}

	IntegerSpecifier::ValueType IntegerSpecifier::FromString(const std::string& data)
	{
		// manualni detekce baze (stoll neumi binarni format)
		auto base_sign_skip = detect_base(data);
		auto base = base_sign_skip.first.first;
		auto sign = base_sign_skip.first.second;
		auto skip = base_sign_skip.second;

		// prevod z retezce na int64_t pomoci std::stoll
		std::size_t processed_count;
		ValueType result;
		try
		{
			result = sign * std::stoll(data.substr(skip), &processed_count, base);
		}
		catch(...)
		{
			throw WrongFormatException();
		}

		// stoll nemusel precist cely vstup, ale v takovem pripade je vstup chybny
		if (processed_count != data.length() - skip)
		{
			throw WrongFormatException();
		}
		return CheckConstraint(result);
	}

	std::string IntegerSpecifier::ToString(IntegerSpecifier::ValueType value)
	{
		return std::to_string(CheckConstraint(value));
	}



	UnsignedSpecifier::UnsignedSpecifier()
	{
	}

	UnsignedSpecifier::UnsignedSpecifier(uint64_t range_start, uint64_t range_end)
		: RangeConstraint<ValueType>(range_start, range_end)
	{
	}

	UnsignedSpecifier::ValueType UnsignedSpecifier::FromString(const std::string& data)
	{
		// manualni detekce baze (stoull neumi binarni format)
		auto base_sign_skip = detect_base(data);
		auto base = base_sign_skip.first.first;
		auto sign = base_sign_skip.first.second;
		auto skip = base_sign_skip.second;

		// prevod z retezce na int64_t pomoci std::stoull
		ValueType result;
		std::size_t processed_count;
		try
		{
			result = sign * std::stoull(data.substr(skip), &processed_count, base);
		}
		catch(...)
		{
			throw WrongFormatException();
		}

		// stoull nemusel precist cely vstup, ale v takovem pripade je vstup chybny
		if (processed_count != data.length() - skip)
		{
			throw WrongFormatException();
		}
		return CheckConstraint(result);
	}

	std::string UnsignedSpecifier::ToString(UnsignedSpecifier::ValueType value)
	{
		return std::to_string(CheckConstraint(value));
	}



	FloatSpecifier::FloatSpecifier()
	{
	}

	FloatSpecifier::FloatSpecifier(double range_start, double range_end)
		: RangeConstraint<ValueType>(range_start, range_end)
	{
	}

	FloatSpecifier::ValueType FloatSpecifier::FromString(const std::string& data)
	{
		ValueType result;
		try
		{
			result = std::stod(data);
		}
		catch(...)
		{
			throw WrongFormatException();
		}
		return CheckConstraint(result);
	}

	std::string FloatSpecifier::ToString(ValueType value)
	{
		std::string result = std::to_string(CheckConstraint(value));
		// bezny prevod double na retezec vypisuje radu prebytecnych nul na konec; odmazeme je
		auto trailing_zero_start = result.find_last_not_of('0');
		if(trailing_zero_start != std::string::npos)
		{
			result.erase(trailing_zero_start + 1 + (result[trailing_zero_start] == '.' ? 1 : 0));
		}
		return result;
	}



	StringSpecifier::StringSpecifier()
	{
	}

	const StringSpecifier::ValueType& StringSpecifier::FromString(const std::string & data)
	{
		return data;
	}

	const std::string& StringSpecifier::ToString(const StringSpecifier::ValueType & value)
	{
		return value;
	}

}
