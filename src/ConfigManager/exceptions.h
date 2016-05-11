#ifndef CONFIGMANAGER_EXCEPTIONS_H
#define CONFIGMANAGER_EXCEPTIONS_H

namespace ConfigManager
{
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

	class MalformedInput : public ConfigurationException
	{
	public:
		MalformedInput() : ConfigurationException("The input was not the expected configuration file format.") {}
	};

	class IoException : public ConfigurationException
	{
	public:
		IoException() : ConfigurationException("There was a problem with the I/O operations.") {}
	};

	class WrongFormat : public ConfigurationException
	{
	public:
		WrongFormat() : ConfigurationException("The format of a value of an option differs from the one specified.") {}
	protected:
		WrongFormat(const char* message) : ConfigurationException(message) {}
	};

	class WrongRange : public WrongFormat
	{
	public:
		WrongRange() : WrongFormat("The range of a value of an option differs from the one specified.") {}
	};

	class MandatoryMissing : public ConfigurationException
	{
	public:
		MandatoryMissing() : ConfigurationException("A mandatory element is missing from the configuration.") {}
	};

	class StrictException : public ConfigurationException
	{
	public:
		StrictException() : ConfigurationException("The input doesn't match the specified format precisely.") {}
	};

	class OutOfBounds : public ConfigurationException
	{
	public:
		OutOfBounds() : ConfigurationException("An attempt was made to access an element of a list outside of bounds.") {}
	};
}

#endif
