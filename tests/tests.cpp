#include "tests.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#ifdef _MSC_VER
#pragma warning (disable: 4290)
#endif

//#include "..\..\..\src\cpptest.h" //"../src/cpptest.h"
#include "../lib/cpptest-1.1.2/src/cpptest.h"
#include "../src/configmanager.h"

using namespace std;
using namespace ConfigManager;



// Main test program
//
int main(int argc, char* argv[])
{
	string welcomeStr = "Starting test of ConfigManager library.\n";
	std::cout << welcomeStr;
	try
	{
		
		Test::TextOutput simpleOutput(Test::TextOutput::Mode::Verbose);
		ConfigurationTestSuite configSuite;
		configSuite.run(simpleOutput);

		SectionTestSuite sectionSuite;
		sectionSuite.run(simpleOutput);

		OptionTestSuite optionSuite;
		optionSuite.run(simpleOutput);

		TypeSpecifiersTestSuite tsSuite;
		tsSuite.run(simpleOutput);
	}
	catch (...)
	{
		cout << "unexpected exception encountered\n";
		return EXIT_FAILURE;
	}
	cin.get();
	/*
	std::cout << "Hi by tests..\n";
	try
	{
		// Demonstrates the ability to use multiple test suites
		//
		Test::Suite ts;
		ts.add(auto_ptr<Test::Suite>(new FailTestSuite));
		ts.add(auto_ptr<Test::Suite>(new CompareTestSuite));
		ts.add(auto_ptr<Test::Suite>(new ThrowTestSuite));

		// Run the tests
		//
		auto_ptr<Test::Output> output(cmdline(argc, argv));
		ts.run(*output, true);

		Test::HtmlOutput* const html = dynamic_cast<Test::HtmlOutput*>(output.get());
		if (html)
			html->generate(cout, true, "MyTest");
	}
	catch (...)
	{
		cout << "unexpected exception encountered\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;*/
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConfigurationTestSuite::ConfigurationTestSuite()
{
	TEST_ADD(ConfigurationTestSuite::CommentsSyntaxTest)
	TEST_ADD(ConfigurationTestSuite::SectionSyntaxTest)
	TEST_ADD(ConfigurationTestSuite::OptionSyntaxTest)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConfigurationTestSuite::CommentsSyntaxTest()
{
	// 
	{// funguji komentare ? (= jsou ignorovany ?  ) 
		ConfigManager::Configuration config;
		stringstream commentFileText;
		commentFileText << "; this is comment.. the following should be ignored: ~@#$%^&*()_+{}:\"|<>?\n";
		commentFileText << ";[commentedSection]\n";
		commentFileText << "[section] ;comment..\n";
		commentFileText << "option=value;comment\n";
		commentFileText << ";commentedOption=what\n";
		// takovy vstup by mel byt korektni. 
		config.Open(commentFileText);
		
		Section dummySection = config.SpecifySection("section", Requirement::MANDATORY, "this section should be there");
			// pokud tam tato sekce neni, nepujde ji specifikovat jako MANDATORY..
		OptionProxy<StringSpecifier> dummyOpt =  dummySection.SpecifyOption<StringSpecifier>
			("option", StringSpecifier() , "default", OPTIONAL,"this option should be there");
		// pokud nebude mit hodnotu "value, tak je to spatne..
		string dummyValue = dummyOpt.Get();
		TEST_ASSERT_EQUALS("value", dummyValue);
		
		TEST_THROWS(Section commentedSectionOut = config.SpecifySection("commentedSection", ConfigManager::MANDATORY, 
			"this should throw, as it was commented out"), MandatoryMissingException)
		TEST_THROWS(OptionProxy<StringSpecifier> commentedOption = dummySection.SpecifyOption
			(";commentedOption", StringSpecifier(), "default", ConfigManager::MANDATORY, 
				"this option should not be in configuration"), MandatoryMissingException )
	}
	// co se stane s nekorektnim vstupem
	{// nesmysl
		ConfigManager::Configuration config;
		stringstream testText;
		testText << "asdf";
		TEST_THROWS(config.Open(testText), MalformedInputException);
	}

	{// spatna sekce 
		ConfigManager::Configuration config;
		stringstream testText;
		testText << "asdf[sectionName]";
		TEST_THROWS(config.Open(testText), MalformedInputException);
	}
	{// spatna sekce 
		ConfigManager::Configuration config;
		stringstream testText;
		testText << "[sectionName";
		TEST_THROWS(config.Open(testText), MalformedInputException);
	}
	{// spatna sekce 
		ConfigManager::Configuration config;
		stringstream testText;
		testText << "sectionName]";
		TEST_THROWS(config.Open(testText), MalformedInputException);
	}
	{
		ConfigManager::Configuration config;
		stringstream testText;
		testText << "[section]\n";
		testText << "asdf";
		TEST_THROWS(config.Open(testText), MalformedInputException);
	}
	{// spatny option
		ConfigManager::Configuration config;
		stringstream testText;
		testText << "[section]\n";
		testText << "asdf";
		TEST_THROWS(config.Open(testText), MalformedInputException);
	}
}

void ConfigurationTestSuite::SectionSyntaxTest()
{
	try
	{// spatne zadane sekce
		{
			ConfigManager::Configuration config;
			stringstream testText;
			testText << "[sectionName]asdf";
			TEST_THROWS(config.Open(testText), MalformedInputException);
		}
		{
			ConfigManager::Configuration config;
			stringstream testText;
			testText << "asdf[sectionName]";
			TEST_THROWS(config.Open(testText), MalformedInputException);
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void ConfigurationTestSuite::OptionSyntaxTest()
{
	try 
	{
		// test ingnoring whitespaces:
		stringstream input;
		input << "[section]\n";
		input << "    option1   =option name with spaces\n";
		input << "option2=      value with spaces     \n";
		input << " \\ option3\\  =with valid spaces in name\n";
		input << "option4= \\ valid spaces in value\\ \n";
		Configuration config;
		config.Open(input);
		Section section = config.SpecifySection("section", MANDATORY);
		TEST_THROWS_NOTHING(OptionProxy<StringSpecifier> option1 = section.SpecifyOption("option1", StringSpecifier(), "default1", MANDATORY))
		OptionProxy<StringSpecifier> option2 = section.SpecifyOption("option2", StringSpecifier(), "default2", MANDATORY);
		TEST_ASSERT_EQUALS("value with spaces", option2.Get())
		TEST_THROWS_NOTHING(OptionProxy<StringSpecifier> option3 = section.SpecifyOption(" option3 ", StringSpecifier(), "default3", MANDATORY))
		OptionProxy<StringSpecifier> option4 = section.SpecifyOption("option4", StringSpecifier(), "default", MANDATORY);
		TEST_ASSERT_EQUALS(" valid spaces in value ", option4.Get())
		// now test the saving:
		
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TypeSpecifiersTestSuite::TypeSpecifiersTestSuite()
{
	TEST_ADD(TypeSpecifiersTestSuite::BooleanSpecTest)
	TEST_ADD(TypeSpecifiersTestSuite::IntegerSpecTest)
	TEST_ADD(TypeSpecifiersTestSuite::UnsignIntSpecTest)
	TEST_ADD(TypeSpecifiersTestSuite::FloatSpecTest)
	TEST_ADD(TypeSpecifiersTestSuite::StringSpecTest)
	TEST_ADD(TypeSpecifiersTestSuite::EnumSpecTest)
}




void TypeSpecifiersTestSuite::BooleanSpecTest()
{
	BooleanSpecifier boolSpec;
	///////////////////////////////////////////////

	// strings to be evaluated to false:
	vector<string> falseStrings;
	falseStrings.push_back("0");
	falseStrings.push_back("f");
	falseStrings.push_back("n");
	falseStrings.push_back("off");
	falseStrings.push_back("disabled");
	for (int i = 0; i < falseStrings.size(); i++)
	{
		bool value = false;
		TEST_THROWS_NOTHING(value = boolSpec.FromString(falseStrings[i]));
		TEST_ASSERT_EQUALS(false, value)
	}
	// strings to be evaluated to false:
	vector<string> trueStrings;
	trueStrings.push_back("1");
	trueStrings.push_back("t");
	trueStrings.push_back("y");
	trueStrings.push_back("on");
	trueStrings.push_back("enabled");

	for (int i = 0; i < trueStrings.size(); i++)
	{
		bool value = true;
		TEST_THROWS_NOTHING(value = boolSpec.FromString(trueStrings[i]));
		TEST_ASSERT_EQUALS(true, value)
	}

	// not booleans:
	vector<string> notBoolStrings;
	notBoolStrings.push_back("asdf");
	notBoolStrings.push_back("3");
	notBoolStrings.push_back("-1");
	notBoolStrings.push_back("$%#");
	for (int i = 0; i < notBoolStrings.size(); i++)
	{
		bool value;
		TEST_THROWS(value = boolSpec.FromString(notBoolStrings[i]), WrongFormatException);
	}
	////////////////////////////////////////////////////////////
	// now test to string:
	std::string resultFalse;
	TEST_THROWS_NOTHING(resultFalse = boolSpec.ToString(false));
	TEST_ASSERT_EQUALS("off", resultFalse);

	std::string resultTrue;
	TEST_THROWS_NOTHING(resultTrue = boolSpec.ToString(true));
	TEST_ASSERT_EQUALS("on", resultTrue);
}
void TypeSpecifiersTestSuite::IntegerSpecTest()
{
	
	try 
	{
		IntegerSpecifier intSpec;
		IntegerSpecifier::ValueType value;
		// test correct from string	
		value = intSpec.FromString("0");
		TEST_ASSERT_EQUALS(0, value);
		value = intSpec.FromString("-1");
		TEST_ASSERT_EQUALS(-1, value);
		value = intSpec.FromString("0x5");
		TEST_ASSERT_EQUALS(0x5, value);
		value = intSpec.FromString("06");
		TEST_ASSERT_EQUALS(06, value);
		value = intSpec.FromString("0b1");
		TEST_ASSERT_EQUALS(1, value);
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
	// test wrong input
	{
		IntegerSpecifier intSpec;
		IntegerSpecifier::ValueType value;
		TEST_THROWS(value = intSpec.FromString("asdf"), WrongFormatException);
		// test wrong range input
		TEST_THROWS(IntegerSpecifier intSpec(5, -5), InvalidOperationException)
	}
	{
		IntegerSpecifier intSpec(2, 7);
		TEST_THROWS(IntegerSpecifier::ValueType value = intSpec.FromString("0"), WrongRangeException)
	}
	/// now to string method:
	{
		IntegerSpecifier intSpec(2, 5);
		std::string three = intSpec.ToString(3);
		TEST_ASSERT_EQUALS("3", three)
		TEST_THROWS(std::string outOfBounds = intSpec.ToString(6), WrongRangeException )
	}

}
void TypeSpecifiersTestSuite::UnsignIntSpecTest()
{ 
	// test correct numbers:
	try
	{
		UnsignedSpecifier uIntSpec;
		UnsignedSpecifier::ValueType value;
		// test correct from string	
		value = uIntSpec.FromString("0");
		TEST_ASSERT_EQUALS(0, value);
		value = uIntSpec.FromString("12");
		TEST_ASSERT_EQUALS(12, value);
		value = uIntSpec.FromString("0x5");
		TEST_ASSERT_EQUALS(0x5, value);
		value = uIntSpec.FromString("016"); /// 8-based: 10 = 8, 16 = 14
		TEST_ASSERT_EQUALS(14, value);
		value = uIntSpec.FromString("0b1");
		TEST_ASSERT_EQUALS(1, value);
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
	{// negative number and numbers outside bounds
		TEST_THROWS(UnsignedSpecifier reversedRange(5, 2), InvalidOperationException)
		TEST_THROWS(UnsignedSpecifier negativeRange(-7, 2), InvalidOperationException)
		UnsignedSpecifier unsignedSpec(7, 15);
		TEST_THROWS(UnsignedSpecifier::ValueType value = unsignedSpec.FromString("-5"), WrongRangeException)
		TEST_THROWS(UnsignedSpecifier::ValueType value = unsignedSpec.FromString("16"), WrongRangeException)
		TEST_THROWS_NOTHING(UnsignedSpecifier::ValueType value = unsignedSpec.FromString("10"))
		// not integer:
		TEST_THROWS(UnsignedSpecifier::ValueType value = unsignedSpec.FromString("3.14"), WrongFormatException)

		TEST_THROWS(UnsignedSpecifier::ValueType value = unsignedSpec.FromString("notAnumber"), WrongFormatException )
	}
	{// test to string feature:
		UnsignedSpecifier unsignedSpec(11, 3025);
		TEST_THROWS(std::string negativeNum = unsignedSpec.ToString(-10), WrongRangeException)
		TEST_THROWS(std::string outsideBounds = unsignedSpec.ToString(6007), WrongRangeException)
		std::string twenty = unsignedSpec.ToString(20);
		TEST_ASSERT_EQUALS("20" , twenty)
	}
}
void TypeSpecifiersTestSuite::FloatSpecTest()
{
	// test correct numbers:
	try
	{
		FloatSpecifier floatSpec;
		FloatSpecifier::ValueType value;
		// test correct from string	
		value = floatSpec.FromString("0");
		TEST_ASSERT_EQUALS(0, value)
		value = floatSpec.FromString("12.05");
		TEST_ASSERT_EQUALS(12.05, value)
		value = floatSpec.FromString("-7.05");
		TEST_ASSERT_EQUALS(-7.05, value)
		value = floatSpec.FromString("150000.5"); 
		TEST_ASSERT_EQUALS(150000.5, value)
		value = floatSpec.FromString("0.00045");
		TEST_ASSERT_EQUALS(0.00045, value)
		value = floatSpec.FromString("1.3E+3");
		TEST_ASSERT_EQUALS(1300, value)
		value = floatSpec.FromString("1.3e+3");
		TEST_ASSERT_EQUALS(1300, value)

		TEST_THROWS(value = floatSpec.FromString("notAnumber"), WrongFormatException)
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}

	{	// wrong bounds
		TEST_THROWS(FloatSpecifier reversedRange(5, 2), InvalidOperationException)
		FloatSpecifier floatSpec(-3.14, 15);
		// outside bounds
		TEST_THROWS(FloatSpecifier::ValueType value = floatSpec.FromString("-5"), WrongRangeException)
		TEST_THROWS(FloatSpecifier::ValueType value = floatSpec.FromString("16"), WrongRangeException)
		// in bounds
		TEST_THROWS_NOTHING(FloatSpecifier::ValueType value = floatSpec.FromString("10"))

		// not a number
		TEST_THROWS(FloatSpecifier::ValueType value = floatSpec.FromString("notAnumber"), WrongFormatException)
	}
	{// test to string feature:
		FloatSpecifier floatSpec(11, 3025);
		TEST_THROWS(std::string negativeNum = floatSpec.ToString(-10), WrongRangeException)
		TEST_THROWS(std::string outsideBounds = floatSpec.ToString(6007), WrongRangeException)
		std::string twenty = floatSpec.ToString(20.5);
		TEST_ASSERT_EQUALS("20.5", twenty)
	}

}
void TypeSpecifiersTestSuite::StringSpecTest()	
{
	// there is not much to test, is it??
	StringSpecifier strSpec;
	std::string data = "hello!@#%^&*()_+}{|\":?><.,;$";
	// will transform into:
	std::string expected = "hello!@#%^&*()_+}{|\":?><.,;$";
	std::string result = strSpec.FromString(data);
	TEST_ASSERT_EQUALS(expected, result)
	std::string backConversion = strSpec.ToString(expected);
	TEST_ASSERT_EQUALS(data, backConversion)
}
void TypeSpecifiersTestSuite::EnumSpecTest()
{
	typedef  EnumSpecifier<TypeSpecifiersTestSuite::TestEnum>::ValueType enumValType;
	{// test right usage of enum
		
		map<std::string, enumValType> rightValMap;
		rightValMap.insert(std::pair <std::string, enumValType>("FIRST_VALUE_STR", enumValType::FIRST_VAL));
		rightValMap.insert(std::pair <std::string, enumValType>("SECOND_VALUE_STR", enumValType::SECOND_VAL));
		rightValMap.insert(std::pair <std::string, enumValType>("THIRD_VALUE_STR", enumValType::THIRD_VAL));

		TEST_THROWS_NOTHING(EnumSpecifier<TypeSpecifiersTestSuite::TestEnum> enumSpec(rightValMap))
			EnumSpecifier<TypeSpecifiersTestSuite::TestEnum> testEnumSpecifier(rightValMap);
		//check values:
		TEST_ASSERT_EQUALS(enumValType::FIRST_VAL, testEnumSpecifier.FromString("FIRST_VALUE_STR"))
		TEST_ASSERT_EQUALS(enumValType::SECOND_VAL, testEnumSpecifier.FromString("SECOND_VALUE_STR"))
		TEST_ASSERT_EQUALS(enumValType::THIRD_VAL, testEnumSpecifier.FromString("THIRD_VALUE_STR"))
		TEST_THROWS(enumValType nonsense = testEnumSpecifier.FromString("gibberish"), WrongFormatException)

		// now to string:
		TEST_ASSERT_EQUALS("FIRST_VALUE_STR", testEnumSpecifier.ToString( enumValType::FIRST_VAL))
		TEST_ASSERT_EQUALS("SECOND_VALUE_STR", testEnumSpecifier.ToString(enumValType::SECOND_VAL))
		TEST_ASSERT_EQUALS("THIRD_VALUE_STR", testEnumSpecifier.ToString(enumValType::THIRD_VAL))
	}
	{
		//what if it is partially specified?
		map<std::string, enumValType> partialValMap;
		partialValMap.insert(std::pair <std::string, enumValType>("FIRST_VALUE_STR", enumValType::FIRST_VAL));
		partialValMap.insert(std::pair <std::string, enumValType>("THIRD_VALUE_STR", enumValType::THIRD_VAL));
		EnumSpecifier<TypeSpecifiersTestSuite::TestEnum> testEnumSpecifier(partialValMap);
		TEST_ASSERT_EQUALS(enumValType::FIRST_VAL, testEnumSpecifier.FromString("FIRST_VALUE_STR"))
		TEST_ASSERT_EQUALS(enumValType::THIRD_VAL, testEnumSpecifier.FromString("THIRD_VALUE_STR"))
		// now to string:
		TEST_ASSERT_EQUALS("FIRST_VALUE_STR", testEnumSpecifier.ToString(enumValType::FIRST_VAL))
		TEST_ASSERT_EQUALS("THIRD_VALUE_STR", testEnumSpecifier.ToString(enumValType::THIRD_VAL))
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SectionTestSuite::SectionTestSuite()
{
	TEST_ADD(SectionTestSuite::BasicTests)
	TEST_ADD(SectionTestSuite::SavingTests)
}

void SectionTestSuite::SavingTests()
{
	try 
	{
		{// saving new section
			ConfigManager::Configuration config;
			Section newSection = config.SpecifySection("newSection", ConfigManager::OPTIONAL, "comments");
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string resultingLine;
			std::getline(output, resultingLine);
			TEST_ASSERT_EQUALS("[newSection];comments", resultingLine)
		}
		{// saving old section:
			ConfigManager::Configuration config;
			stringstream input;
			input << "[section];comment\n";
			config.Open(input);
			Section newSection = config.SpecifySection("section", ConfigManager::MANDATORY, "default");
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string resultingLine;
			std::getline(output, resultingLine);
			TEST_ASSERT_EQUALS("[section];comment", resultingLine)
		}
		{// if section is not needed, it should stay in given format
			ConfigManager::Configuration config;
			stringstream input;
			input << "[section]; untouched  comment\n";
			config.Open(input);
			stringstream output;
			config.Save(output);
			string resultingLine;
			std::getline(output, resultingLine);
			TEST_ASSERT_EQUALS("[section]; untouched  comment", resultingLine)
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void SectionTestSuite::BasicTests()
{
	try 
	{
		{ // try if it gets name correctly from input
			ConfigManager::Configuration homeConfig;
			stringstream testText;
			testText << "[sectionName]\n";

			// takovy vstup by mel byt korektni. 
			homeConfig.Open(testText);
			Section section = homeConfig.SpecifySection("sectionName", ConfigManager::MANDATORY, "comments");
			TEST_ASSERT_EQUALS("sectionName", section.GetName())
		}
		{// is name correct when given through specification
			ConfigManager::Configuration homeConfig;
			stringstream testText;
			homeConfig.Open(testText);
			Section newSection = homeConfig.SpecifySection("newSectionName", ConfigManager::OPTIONAL, "new section cannot be MANDATORY...");
			TEST_ASSERT_EQUALS("newSectionName", newSection.GetName())
		}
		/// move on to list options:
		{ // try if it gets name correctly from input
			ConfigManager::Configuration homeConfig;
			stringstream testText;
			testText << "[sectionName]\n";

			// takovy vstup by mel byt korektni. 
			homeConfig.Open(testText);
			Section section = homeConfig.SpecifySection("sectionName", ConfigManager::MANDATORY, "comments");
			TEST_ASSERT_EQUALS("sectionName", section.GetName())
		}
		{// is name correct when given through specification
			ConfigManager::Configuration homeConfig;
			stringstream testText;
			homeConfig.Open(testText);
			Section newSection = homeConfig.SpecifySection("newSectionName", ConfigManager::OPTIONAL, "new section cannot be MANDATORY...");
			TEST_ASSERT_EQUALS("newSectionName", newSection.GetName())
		}

	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.")
	}
}

OptionTestSuite::OptionTestSuite()
{
	TEST_ADD(OptionTestSuite::InputNameOptionTest)
	TEST_ADD(OptionTestSuite::InputNameListOptionTest)
	TEST_ADD(OptionTestSuite::SpecNameOption)
	TEST_ADD(OptionTestSuite::SavingBoolTest)
	TEST_ADD(OptionTestSuite::SavingIntTest)
	TEST_ADD(OptionTestSuite::SavingUintTest)
	TEST_ADD(OptionTestSuite::SavingFloatTest)
	TEST_ADD(OptionTestSuite::SavingStringTest)
	TEST_ADD(OptionTestSuite::SavingEnumTest)
	//TEST_ADD(OptionTestSuit::SavingListTest) not implemented
	TEST_ADD(OptionTestSuite::PreservingFormatTest)
    TEST_ADD(OptionTestSuite::ListModificationTest)
	TEST_ADD(OptionTestSuite::ListLinkTest )
	TEST_ADD(OptionTestSuite::LinksTest)
	TEST_ADD(OptionTestSuite::CopyingTest)

}

void OptionTestSuite::SavingBoolTest()
{
	try
	{// saving various types:
			ConfigManager::Configuration config;
			Section section = config.SpecifySection("section", ConfigManager::OPTIONAL, "comments");
			OptionProxy<BooleanSpecifier> boolOption = section.SpecifyOption("boolOpt", BooleanSpecifier(), true, OPTIONAL, "commentsBool");
			{
				stringstream output;
				config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
				string oLine;
				std::getline(output, oLine); // throw away the section line
				std::getline(output, oLine);
				TEST_ASSERT_EQUALS("boolOpt=on;commentsBool", oLine)
			}
			// now let us change values:
			boolOption.Set(false);
		    {
				stringstream output;
				config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
				string oLine;
				std::getline(output, oLine); // section line is not needed
				std::getline(output, oLine);
				TEST_ASSERT_EQUALS("boolOpt=off;commentsBool", oLine)
			}	
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void OptionTestSuite::SavingIntTest()
{
	try
	{
		{// saving various types:
			ConfigManager::Configuration config;
			Section section = config.SpecifySection("section", ConfigManager::OPTIONAL, "comments");
			OptionProxy<IntegerSpecifier> intOption = section.SpecifyOption("intOpt", IntegerSpecifier(), -1, OPTIONAL, "commentsInt");
			{
				stringstream output;
				config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
				string oLine;
				std::getline(output, oLine); // throw away the section line
				std::getline(output, oLine);
				TEST_ASSERT_EQUALS("intOpt=-1;commentsInt", oLine)
			}
			// now we change the value
			intOption.Set(-2);
			{
				stringstream output;
				config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
				string oLine;
				std::getline(output, oLine); // section line is not needed
				std::getline(output, oLine);
				TEST_ASSERT_EQUALS("intOpt=-2;commentsInt", oLine)
			}
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void OptionTestSuite::SavingUintTest()
{
	try
	{// saving various types:
		ConfigManager::Configuration config;
		Section section = config.SpecifySection("section", ConfigManager::OPTIONAL, "comments");
		OptionProxy<UnsignedSpecifier> uintOption = section.SpecifyOption("uintOpt",
			UnsignedSpecifier(), 1, OPTIONAL, "commentsUint");
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // throw away the section line
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("uintOpt=1;commentsUint", oLine)
		}
		// now, lets change the value
		uintOption.Set(2);
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not needed
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("uintOpt=2;commentsUint", oLine)
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void OptionTestSuite::SavingFloatTest()
{
	try
	{// saving various types:
		ConfigManager::Configuration config;
		Section section = config.SpecifySection("section", ConfigManager::OPTIONAL, "comments");
		OptionProxy<FloatSpecifier> floatOption = section.SpecifyOption("floatOpt", FloatSpecifier(), 3.14, OPTIONAL, "commentsFloat");
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // throw away the section line
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("floatOpt=3.14;commentsFloat", oLine)
		}
		floatOption.Set(2.72);
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not needed
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("floatOpt=2.72;commentsFloat", oLine)
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void OptionTestSuite::SavingStringTest()
{
	try
	{// saving various types:
		ConfigManager::Configuration config;
		Section section = config.SpecifySection("section", ConfigManager::OPTIONAL, "comments");
		OptionProxy<StringSpecifier> stringOption = section.SpecifyOption("stringOpt", StringSpecifier(),"defaultString", OPTIONAL, "commentsString");
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // throw away the section line
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("stringOpt=defaultString;commentsString", oLine)
		}
		stringOption.Set("anotherString");
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not needed
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("stringOpt=anotherString;commentsString", oLine)
		}
		stringOption.Set(" anotherString  "); // white spaces at beginning and end, need escape characted
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not needed
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("stringOpt=\\ anotherString\\ \\ ;commentsString", oLine)
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void OptionTestSuite::SavingEnumTest()
{
	typedef OptionTestSuite::TestEnum enumType;
	try
	{// saving various types:
		ConfigManager::Configuration config;
		Section section = config.SpecifySection("section", ConfigManager::OPTIONAL, "comments");
		map<std::string, enumType> valueMap;
		valueMap.insert(std::pair <std::string, enumType>("FIRST_VALUE_STR", enumType::FIRST_VAL));
		valueMap.insert(std::pair <std::string, enumType>("SECOND_VALUE_STR", enumType::SECOND_VAL));
		valueMap.insert(std::pair <std::string, enumType>("THIRD_VALUE_STR", enumType::THIRD_VAL));
		OptionProxy<EnumSpecifier<OptionTestSuite::TestEnum> > enumOption = section.SpecifyOption("enumOpt",
			EnumSpecifier<enumType>(valueMap), enumType::THIRD_VAL, OPTIONAL, "commentsEnum");
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // throw away the section line
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("enumOpt=THIRD_VALUE_STR;commentsEnum", oLine)
		}
		enumOption.Set(enumType::FIRST_VAL);
		{
			stringstream output;
			config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not needed
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("enumOpt=FIRST_VALUE_STR;commentsEnum", oLine)
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception")
	}
}

void OptionTestSuite::SavingListTest()
{
	try
	{ // try if it gets name correctly from input
		ConfigManager::Configuration homeConfig;
		// takovy vstup by mel byt korektni. 
		Section section = homeConfig.SpecifySection("sectionName", ConfigManager::OPTIONAL, "commentsSection");
		vector<StringSpecifier::ValueType> defaultValues;
		defaultValues.push_back("defaultValue");
		defaultValues.push_back("valueDefault");
		ListOptionProxy<StringSpecifier> listOption = section.SpecifyListOption("optionList", StringSpecifier(), defaultValues,
			ConfigManager::OPTIONAL, "commentsOption");
		{ // saving 
			stringstream output;
			homeConfig.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not target of test
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("optionList=defaultValue,valueDefault;commentsOption", oLine)
		}
		// change one option:
		listOption[1].Set("changedValue");
		{ // saving 
			stringstream output;
			homeConfig.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
			string oLine;
			std::getline(output, oLine); // section line is not target of test
			std::getline(output, oLine);
			TEST_ASSERT_EQUALS("optionList=defaultValue,changedValue;commentsOption", oLine)
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
}

void OptionTestSuite::PreservingFormatTest()
{
	try
	{// test preserving of formating:
		ConfigManager::Configuration config;
		stringstream inputText;
		string sectionLine = "[section] ; arbitrarily formatted comment \n";
		inputText << sectionLine;
		string boolLine = "boolOpt = enabled; comments Bool\n";
		inputText << boolLine;
		string intLine = "intOpt = -2;comme ntsInt\n";
		inputText << intLine;
		string uintLine = "uintOpt = 2;  comm entsUint\n";
		inputText << uintLine;
		string floatLine = "floatOpt = 2.72;comments Float\n";
		inputText << floatLine;
		string stringLine = "stringOpt = some string; comment string\n";
		inputText << stringLine;
		string enumLine = "enumOpt = FIRST_VALUE_STR;comment sEnum\n";
		inputText << enumLine;
		string listLine1 = "listOpt1 = list,separated,by,comas; com ment list1\n";
		inputText << listLine1;
		string listLine2 = "listOpt2 = list:separated:by:colon; comment list 2\n";
		inputText << listLine2;
		config.Open(inputText);
		Section section = config.SpecifySection("section", ConfigManager::MANDATORY, "comments");
		stringstream output;
		config.Save(output);
		string oLine;
		// problem is that from string we get line without line ending, therefore we need to append it again:
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(sectionLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(boolLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(intLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(uintLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(floatLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(stringLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(enumLine, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(listLine1, oLine + "\n");
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS(listLine2, oLine + "\n");
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.")
	}
}

void OptionTestSuite::CopyingTest()
{
	try 
	{
		Configuration config;
		Section section = config.SpecifySection("section");
		OptionProxy<StringSpecifier> optionOrig = section.SpecifyOption("optionOrig", StringSpecifier(), 
			"defaultValue", OPTIONAL, "comments");
		auto optionMoved = std::move(optionOrig);
		TEST_ASSERT_EQUALS("n/a", optionOrig.GetName())
		TEST_ASSERT_EQUALS("optionOrig", optionMoved.GetName())
		TEST_ASSERT_EQUALS("section", optionMoved.GetSectionName())
		TEST_ASSERT_EQUALS("defaultValue", optionMoved.Get())
		optionMoved.Set("newValue");
		TEST_ASSERT_EQUALS("newValue", optionMoved.Get())
		stringstream output;
		config.Save(output, ConfigManager::EMIT_DEFAULT_VALUES);
		string oLine;
		std::getline(output,oLine); // section line
		std::getline(output, oLine);
		TEST_ASSERT_EQUALS("optionOrig=newValue;comments", oLine);
	}
	catch(...)
	{
		TEST_FAIL("Unexpected value.")
	}

}

void OptionTestSuite::InputNameOptionTest()
{
	try
	{// try if it gets name correctly from input
			ConfigManager::Configuration homeConfig;
			stringstream testText;
			testText << "[sectionName]\n";
			testText << "optionOne=value\n";

			// takovy vstup by mel byt korektni. 
			homeConfig.Open(testText);
			Section section = homeConfig.SpecifySection("sectionName", ConfigManager::MANDATORY, "comments");
			OptionProxy<StringSpecifier> option = section.SpecifyOption("optionOne", StringSpecifier(), "default",
				ConfigManager::MANDATORY, "mandatory testing");
			TEST_ASSERT_EQUALS("optionOne", option.GetName())
			TEST_ASSERT_EQUALS("sectionName", option.GetSectionName())

			OptionProxy<StringSpecifier> newOption = section.SpecifyOption("optionTwo", StringSpecifier(), "defaultValueTwo",
					ConfigManager::OPTIONAL, "testing new option in old section");
			TEST_ASSERT_EQUALS("optionTwo", newOption.GetName())
			TEST_ASSERT_EQUALS("sectionName", newOption.GetSectionName())
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.")
	}
}

void OptionTestSuite::SpecNameOption()
{
	try
	{// is name correct when given through specification
		ConfigManager::Configuration homeConfig;
		stringstream testText;
		homeConfig.Open(testText);
		Section newSection = homeConfig.SpecifySection("newSectionName", ConfigManager::OPTIONAL, 
			"new section cannot be MANDATORY...");
		OptionProxy<StringSpecifier> newOption = newSection.SpecifyOption("optionTwo", StringSpecifier(), 
			"defaultValueTwo", ConfigManager::OPTIONAL, "testing new option in old section");
		TEST_ASSERT_EQUALS("optionTwo", newOption.GetName())
		TEST_ASSERT_EQUALS("newSectionName", newOption.GetSectionName())
	}
	catch(...)
	{
		TEST_FAIL("Unexpected exception.")
	}
}


void OptionTestSuite::ListModificationTest()
{
	try {
		Configuration configuration;
		Section section = configuration.SpecifySection("section", ConfigManager::OPTIONAL, "sectionComments");
		vector<BooleanSpecifier::ValueType> initialList;
		initialList.push_back(true);
		initialList.push_back(true);
		ListOptionProxy<BooleanSpecifier> boolList = section.SpecifyListOption("boolListOption", BooleanSpecifier(), initialList, ConfigManager::OPTIONAL, "optionComments");
		// list size
		TEST_ASSERT_EQUALS(2, boolList.Count())
			// adding to list
			boolList.Add(false);
		TEST_ASSERT_EQUALS(false, boolList[boolList.Count() - 1].Get());
		// removing element
		boolList.Remove(0);
		// checking that list contain (true, false)
		TEST_ASSERT_EQUALS(2, boolList.Count())
			TEST_ASSERT_EQUALS(true, boolList[0].Get())
			TEST_ASSERT_EQUALS(false, boolList[1].Get())
			boolList[0].Set(false);
		TEST_ASSERT_EQUALS(false, boolList[0].Get())
	}
	catch(...)
	{
		TEST_FAIL("Unexpected exception.")
	}
}

void OptionTestSuite::LinksTest()
{
	try 
	{
		{
			stringstream inputText;
			inputText << "[section1]\n";
			inputText << "option=value\n";
			inputText << "[section2]\n";
			inputText << "option=${section1#option}\n";
			Configuration config;
			config.Open(inputText);
			Section section2 = config.SpecifySection("section2", ConfigManager::MANDATORY);
			OptionProxy<StringSpecifier> opt2 = section2.SpecifyOption("option", StringSpecifier(), "defaultVal", OPTIONAL);
			TEST_ASSERT_EQUALS("value", opt2.Get());
		}
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
}

void OptionTestSuite::ListLinkTest()
{
	try
	{
		// test two variants of list links:
		stringstream inputText;
		inputText << "[section1]\n";
		inputText << "option=linkedValue\n";
		inputText << "[section2]\n";
		inputText << "option=inPlaceValue,${section1#option}\n";
		inputText << "secondOption = ${section2#option}\n";
		Configuration config;
		config.Open(inputText);
		Section section2 = config.SpecifySection("section2", ConfigManager::MANDATORY);
		vector<StringSpecifier::ValueType> defValues;
		defValues.push_back("default1");
		ListOptionProxy<StringSpecifier> listOption = section2.SpecifyListOption
			("option", StringSpecifier(), defValues, OPTIONAL);
		TEST_ASSERT_EQUALS("inPlaceValue", listOption[0].Get());
		TEST_ASSERT_EQUALS("linkedValue", listOption[1].Get());
		ListOptionProxy<StringSpecifier> secondListOption = section2.SpecifyListOption
			("secondOption", StringSpecifier(), defValues, OPTIONAL);
		TEST_ASSERT_EQUALS("inPlaceValue", secondListOption[0].Get());
		TEST_ASSERT_EQUALS("linkedValue", secondListOption[1].Get());
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
}

void OptionTestSuite::InputNameListOptionTest()
{
	try
	{ // try if it gets name correctly from input
		ConfigManager::Configuration homeConfig;
		stringstream testText;
		testText << "[sectionName]\n"; //testText << "[sectionName]";
		testText << "optionOne=value,value2\n";

		// takovy vstup by mel byt korektni. 
		homeConfig.Open(testText);
		Section section = homeConfig.SpecifySection("sectionName", ConfigManager::MANDATORY, "comments");
		vector<StringSpecifier::ValueType> defaultValues;
		defaultValues.push_back("defaultValue");
		defaultValues.push_back("valueDefault");
		ListOptionProxy<StringSpecifier> listOption = section.SpecifyListOption("optionOne", StringSpecifier(), defaultValues,
			ConfigManager::MANDATORY, "mandatory testing");
		TEST_ASSERT_EQUALS("optionOne", listOption.GetName())
		TEST_ASSERT_EQUALS("sectionName", listOption.GetSectionName())

		ListOptionProxy<StringSpecifier> newOption = section.SpecifyListOption("optionTwo", StringSpecifier(), defaultValues,
				ConfigManager::OPTIONAL, "testing new option in old section");
		TEST_ASSERT_EQUALS("optionTwo", newOption.GetName())
		TEST_ASSERT_EQUALS("sectionName", newOption.GetSectionName())
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
}

///////////////////////////////////////DEMOSTRATION OF CPPTEST CODE: not used ////////////////////////////////////////////////
/*
// Tests unconditional fail asserts
//
class FailTestSuite : public Test::Suite
{
public:
FailTestSuite()
{
TEST_ADD(FailTestSuite::success)
TEST_ADD(FailTestSuite::always_fail)

}

private:
void success() {}

void always_fail()
{
// This will always fail
//
TEST_FAIL("unconditional fail");
}
};

// Tests compare asserts
//
class CompareTestSuite : public Test::Suite
{
public:
CompareTestSuite()
{
TEST_ADD(CompareTestSuite::success)
TEST_ADD(CompareTestSuite::compare)
TEST_ADD(CompareTestSuite::delta_compare)
}

private:
void success() {}

void compare()
{
// Will succeed since the expression evaluates to true
//
TEST_ASSERT(1 + 1 == 2)

// Will fail since the expression evaluates to false
//
TEST_ASSERT(0 == 1);
}

void delta_compare()
{
// Will succeed since the expression evaluates to true
//
TEST_ASSERT_DELTA(0.5, 0.7, 0.3);

// Will fail since the expression evaluates to false
//
TEST_ASSERT_DELTA(0.5, 0.7, 0.1);
}
};

// Tests throw asserts
//
class ThrowTestSuite : public Test::Suite
{
public:
ThrowTestSuite()
{
TEST_ADD(ThrowTestSuite::success)
TEST_ADD(ThrowTestSuite::test_throw)
}

private:
void success() {}

void test_throw()
{
// Will fail since the none of the functions throws anything
//
TEST_THROWS_MSG(func(), int, "func() does not throw, expected int exception")
TEST_THROWS_MSG(func_no_throw(), int, "func_no_throw() does not throw, expected int exception")
TEST_THROWS_ANYTHING_MSG(func(), "func() does not throw, expected any exception")
TEST_THROWS_ANYTHING_MSG(func_no_throw(), "func_no_throw() does not throw, expected any exception")

// Will succeed since none of the functions throws anything
//
TEST_THROWS_NOTHING(func())
TEST_THROWS_NOTHING(func_no_throw())

// Will succeed since func_throw_int() throws an int
//
TEST_THROWS(func_throw_int(), int)
TEST_THROWS_ANYTHING(func_throw_int())

// Will fail since func_throw_int() throws an int (not a float)
//
TEST_THROWS_MSG(func_throw_int(), float, "func_throw_int() throws an int, expected a float exception")
TEST_THROWS_NOTHING_MSG(func_throw_int(), "func_throw_int() throws an int, expected no exception at all")
}

void func() {}
void func_no_throw() throw() {}
void func_throw_int() throw(int) { throw 13; }
};

enum OutputType
{
Compiler,
Html,
TextTerse,
TextVerbose
};

static void
usage()
{
cout << "usage: mytest [MODE]\n"
<< "where MODE may be one of:\n"
<< "  --compiler\n"
<< "  --html\n"
<< "  --text-terse (default)\n"
<< "  --text-verbose\n";
exit(0);
}

static auto_ptr<Test::Output>
cmdline(int argc, char* argv[])
{
if (argc > 2)
usage(); // will not return



Test::Output* output = 0;

if (argc == 1)
output = new Test::TextOutput(Test::TextOutput::Verbose);
else
{
const char* arg = argv[1];
if (strcmp(arg, "--compiler") == 0)
output = new Test::CompilerOutput;
else if (strcmp(arg, "--html") == 0)
output = new Test::HtmlOutput;
else if (strcmp(arg, "--text-terse") == 0)
output = new Test::TextOutput(Test::TextOutput::Terse);
else if (strcmp(arg, "--text-verbose") == 0)
output = new Test::TextOutput(Test::TextOutput::Verbose);
else
{
cout << "invalid commandline argument: " << arg << endl;
usage(); // will not return
}
}

return auto_ptr<Test::Output>(output);
} */