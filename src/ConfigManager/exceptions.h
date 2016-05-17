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

	class InvalidOperationException : public ConfigurationException
	{
	public:
		InvalidOperationException(const char* message) : ConfigurationException(message) {}
	};

	class MalformedInputException : public ConfigurationException
	{
	public:
		MalformedInputException() : ConfigurationException("The input was not the expected configuration file format.") {}
	};

	class IoException : public ConfigurationException
	{
	public:
		IoException() : ConfigurationException("There was a problem with the I/O operations.") {}
	};

	class WrongFormatException : public ConfigurationException
	{
	public:
		WrongFormatException() : ConfigurationException("The format of a value of an option differs from the one specified.") {}
	protected:
		WrongFormatException(const char* message) : ConfigurationException(message) {}
	};

	class WrongRangeException : public WrongFormatException
	{
	public:
		WrongRangeException() : WrongFormatException("The range of a value of an option differs from the one specified.") {}
	};

	class MandatoryMissingException : public ConfigurationException
	{
	public:
		MandatoryMissingException() : ConfigurationException("A mandatory element is missing from the configuration.") {}
	};

	class StrictException : public ConfigurationException
	{
	public:
		StrictException() : ConfigurationException("The input doesn't match the specified format precisely.") {}
	};

	class OutOfBoundsException : public ConfigurationException
	{
	public:
		OutOfBoundsException() : ConfigurationException("An attempt was made to access an element of a list outside of bounds.") {}
	};
}

#endif
