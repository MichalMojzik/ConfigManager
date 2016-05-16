#include "typespecifiers.h"


namespace ConfigManager
{
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
			data == "disable"
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
	{
	}

	IntegerSpecifier::ValueType IntegerSpecifier::FromString(const std::string& data)
	{
		return std::stoll(data);
	}

	std::string IntegerSpecifier::ToString(IntegerSpecifier::ValueType value)
	{
		return std::to_string(value);
	}



	UnsignedSpecifier::UnsignedSpecifier()
	{
	}

	UnsignedSpecifier::UnsignedSpecifier(uint64_t range_start, uint64_t range_end)
	{
	}

	UnsignedSpecifier::ValueType UnsignedSpecifier::FromString(const std::string& data)
	{
		try
		{
			return std::stoull(data);
		}
		catch(...)
		{
			throw WrongFormatException();
		}
	}

	std::string UnsignedSpecifier::ToString(UnsignedSpecifier::ValueType value)
	{
		return std::to_string(value);
	}



	FloatSpecifier::FloatSpecifier()
	{
	}

	FloatSpecifier::FloatSpecifier(double range_start, double range_end)
	{
	}

	FloatSpecifier::ValueType FloatSpecifier::FromString(const std::string& data)
	{
		return std::stod(data);
	}

	std::string FloatSpecifier::ToString(ValueType value)
	{
		return std::to_string(value);
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
