#include "typespecifiers.h"

namespace ConfigManager
{
	// result: base, skip
	std::pair<int, int> detect_base(const std::string& data)
	{
		std::size_t skip = 0;
		if(data.length() > 0 && (data.at(0) == '+' || data.at(0) == '-'))
		{
			skip = 1;
		}

		if(data.length() == skip)
		{
			throw WrongFormatException();
		}

		switch(data.at(skip))
		{
			case '0':
				if(data.length() > skip + 1)
				{
					switch(data.at(skip + 1))
					{
					case 'x': return std::make_pair(16, 2);
					case 'b': return std::make_pair(2, 2);
					default: break;
					}
					return std::make_pair(8, 1);
				}
				else
					return std::make_pair(10, 0);
			case '1': case '2':	case '3':	case '4':	case '5':
			case '6':	case '7':	case '8':	case '9':
				return std::make_pair(10, 0);
			default:
				throw WrongFormatException();
		}
	}

	BooleanSpecifier::BooleanSpecifier()
	{
	}

	BooleanSpecifier::ValueType ConfigManager::BooleanSpecifier::FromString(const std::string & data)
	{
		if(
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
		else if(
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
		try
		{
			std::size_t processed_count;
			auto base_skip_pair = detect_base(data);
			auto base = base_skip_pair.first;
			auto skip = base_skip_pair.second;
			auto result = std::stoll(data.substr(skip), &processed_count, base);
			if(processed_count != data.length() - skip)
				throw WrongFormatException();
			return CheckConstraint(result);
		}
		catch(...)
		{
			throw WrongFormatException();
		}
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
		try
		{
			std::size_t processed_count;
			auto base_skip_pair = detect_base(data);
			auto base = base_skip_pair.first;
			auto skip = base_skip_pair.second;
			auto result = std::stoull(data.substr(skip), &processed_count, base);
			if(processed_count != data.length() - skip)
				throw WrongFormatException();
			return CheckConstraint(result);
		}
		catch(...)
		{
			throw WrongFormatException();
		}
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
		return CheckConstraint(std::stod(data));
	}

	std::string FloatSpecifier::ToString(ValueType value)
	{
		return std::to_string(CheckConstraint(value));
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
