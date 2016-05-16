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

	std::string BooleanSpecifier::ToString(const BooleanSpecifier::ValueType & value)
	{
		if(value)
			return "on";
		else
			return "off";
	}



	IntegerSpecifier::IntegerSpecifier()
	{
	}

	IntegerSpecifier::IntegerSpecifier(int64_t range_start, int64_t range_end)
	{
	}

	IntegerSpecifier::ValueType IntegerSpecifier::FromString(const std::string & data)
	{
		return 0;
	}

	std::string IntegerSpecifier::ToString(const IntegerSpecifier::ValueType & value)
	{
		return std::to_string(value);
	}



	UnsignedSpecifier::UnsignedSpecifier()
	{
	}

	UnsignedSpecifier::UnsignedSpecifier(uint64_t range_start, uint64_t range_end)
	{
	}

	UnsignedSpecifier::ValueType UnsignedSpecifier::FromString(const std::string & data)
	{
		return 0;
	}

	std::string UnsignedSpecifier::ToString(const UnsignedSpecifier::ValueType & value)
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
