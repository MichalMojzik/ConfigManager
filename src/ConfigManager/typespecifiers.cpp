#include "typespecifiers.h"


namespace ConfigManager
{
	BooleanSpecifier::BooleanSpecifier()
	{
	}

	BooleanSpecifier::ValueType ConfigManager::BooleanSpecifier::FromString(const std::string & data)
	{
		return false; 
	}

	std::string BooleanSpecifier::ToString(const BooleanSpecifier::ValueType & value)
	{
		return std::string();
	}



	IntegerSpecifier::IntegerSpecifier()
	{
	}

	IntegerSpecifier::IntegerSpecifier(int64 range_start, int64 range_end)
	{
	}

	IntegerSpecifier::ValueType IntegerSpecifier::FromString(const std::string & data)
	{
		return 0;
	}

	std::string IntegerSpecifier::ToString(const IntegerSpecifier::ValueType & value)
	{
		return std::string();
	}



	UnsignedSpecifier::UnsignedSpecifier()
	{
	}

	UnsignedSpecifier::UnsignedSpecifier(uint64 range_start, uint64 range_end)
	{
	}

	UnsignedSpecifier::ValueType UnsignedSpecifier::FromString(const std::string & data)
	{
		return 0;
	}

	std::string UnsignedSpecifier::ToString(const UnsignedSpecifier::ValueType & value)
	{
		return std::string();
	}



	StringSpecifier::StringSpecifier()
	{
	}

	StringSpecifier::ValueType StringSpecifier::FromString(const std::string & data)
	{
		return StringSpecifier::ValueType();
	}

	std::string StringSpecifier::ToString(const StringSpecifier::ValueType & value)
	{
		return std::string();
	}

}
