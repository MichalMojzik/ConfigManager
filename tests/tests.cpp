#include "tests.h"



#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#pragma warning (disable: 4290)
#endif

//#include "..\..\..\src\cpptest.h" //"../src/cpptest.h"
#include "../lib/cpptest-1.1.2/src/cpptest.h"
#include "../src/configmanager.h"

using namespace std;
using namespace ConfigManager;

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
}

// Main test program
//
int main(int argc, char* argv[])
{
	std::cout << "Starting test of ConfigManager library. \n";
	try
	{
		ConfigurationTestSuite configSuite;
		Test::TextOutput simpleOutput(Test::TextOutput::Mode::Verbose);
		configSuite.run(simpleOutput);

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
	TEST_ADD(ConfigurationTestSuite::FormatReadingTest)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConfigurationTestSuite::FormatReadingTest()
{
	// 
	{// funguji komentare ? (= jsou ignorovany ?  ) 
		ConfigManager::Configuration config;
		stringstream commentFileText;
		commentFileText << "; this is comment.. the following should be ignored: ~@#$%^&*()_+{}:\"|<>?\n";
		commentFileText << ";[commentedSection]\n";
		commentFileText << ";commentedOption=what\n";
		commentFileText << "[section] ;comment.. \n";
		commentFileText << "option=value;comment\n";
		// takovy vstup by mel byt korektni. 
		try 
		{
			config.SetInputStream(commentFileText);
		}
		catch(ConfigManager::IoException & e)
		{
			TEST_FAIL("FormatReadingTest: testing comments: IoException.")
		}
		catch (ConfigManager::ConfigurationException & e)
		{	
			TEST_FAIL("FormatReadingTest: testing comments: exception thrown.")
		}
		catch (...)
		{
			TEST_FAIL("FormatReadingTest: testing comments: unexpected exception, while reading input stream.")
		}
		
		try
		{
			Section dummySection = config.SpecifySection("section", Requirement::MANDATORY, "this section should be there");
			// pokud tam tato sekce neni, nepujde ji specifikovat jako MANDATORY..
			OptionProxy<StringSpecifier> dummyOpt =  dummySection.SpecifyOption<StringSpecifier>
								("option", StringSpecifier() , "default", OPTIONAL,"this option should be there");
			// pokud nebude mit hodnotu "value, tak je to spatne..

			string dummyValue = dummyOpt.Get();
			TEST_ASSERT_EQUALS("value", dummyValue);
		}
		catch (MandatoryMissingException & e)
		{
			TEST_FAIL("FormatReadingTest: testing comments: '[section];comment' did not create a section.")
		}
		catch (...)
		{
			TEST_FAIL("FormatReadingTest: testing comments: unexpected exception, specifying section.")
		}	

		// TODO:  test that there are not commented options and sections...
	}

	
	//	config.SetInputStream();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConfigurationTestSuite::~ConfigurationTestSuite()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TypeSpecifiersTestSuite::TypeSpecifiersTestSuite()
{
	TEST_ADD(TypeSpecifiersTestSuite::BooleanSpecTest)
	TEST_ADD(TypeSpecifiersTestSuite::IntegerSpecTest)
}


TypeSpecifiersTestSuite::~TypeSpecifiersTestSuite()
{
}


void TypeSpecifiersTestSuite::BooleanSpecTest()
{
	BooleanSpecifier boolSpec;
	///////////////////////////////////////////////
	try {
		// strings to be evaluated to false:
		vector<string> falseStrings;
		falseStrings.push_back("0");
		falseStrings.push_back("f");
		falseStrings.push_back("n");
		falseStrings.push_back("off");
		falseStrings.push_back("disabled");
		for (int i = 0; i < falseStrings.size(); i++)
		{
			bool value = boolSpec.FromString(falseStrings[i]);
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
			bool value = boolSpec.FromString(trueStrings[i]);
			TEST_ASSERT_EQUALS(true, value)
		}
	}
	catch (...)
	{
		TEST_FAIL("Exception thrown during valid convertion.");
	}
	// not booleans:
	vector<string> notBoolStrings;
	notBoolStrings.push_back("asdf");
	notBoolStrings.push_back("3");
	notBoolStrings.push_back("-1");
	notBoolStrings.push_back("$%#");
	for (int i = 0; i < notBoolStrings.size(); i++)
	{
		try
		{
			bool value = boolSpec.FromString(notBoolStrings[i]);
			// we should throw exception..
			TEST_FAIL("No exception when invalid format.")
		}
		catch(WrongFormatException)
		{
			//this is ok
		}
		catch (...)
		{
			TEST_FAIL("Unexpected exception.")
		}
		
	}
	////////////////////////////////////////////////////////////
	// now test to string:
	try 
	{
		std::string resultFalse = boolSpec.ToString(false);
		TEST_ASSERT_EQUALS("false", resultFalse)
		std::string resultTrue = boolSpec.ToString(true);
		TEST_ASSERT_EQUALS("false", resultTrue)
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.")
	}
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
		TEST_ASSERT_EQUALS(5, value);
		value = intSpec.FromString("06");
		TEST_ASSERT_EQUALS(0x5, value);
		value = intSpec.FromString("0b1");
		TEST_ASSERT_EQUALS(1, value);
	}
	catch (...)
	{
		TEST_FAIL("Unexpected exception.");
	}
	// test wrong input
	try 
	{
		IntegerSpecifier intSpec;
		IntegerSpecifier::ValueType value;
		value = intSpec.FromString("asdf");
		TEST_FAIL("Did not fail the non-numerical string.")
	}
	catch (WrongFormatException)
	{
		// tak to ma byt
	}
	
	// test wrong range input
	try 
	{
		IntegerSpecifier intSpec(5, -5);
		TEST_FAIL("Did allow wrong range.")
	}
	catch (WrongRangeException)
	{
		// this is ok
	}

	try 
	{
		IntegerSpecifier intSpec(2, 7);
		IntegerSpecifier::ValueType value = intSpec.FromString("0");
		TEST_FAIL("Allowed value outside range.")
	}
	catch (WrongRangeException)
	{
		// this is ok
	}
	/// now to string method:
	try 
	{
		IntegerSpecifier intSpec(2, 5);
		std::string three = intSpec.ToString(3);
		TEST_ASSERT_EQUALS("3", three)
		std::string outOfBounds = intSpec.ToString(6);
		TEST_FAIL("Used value outside of range.")
	}
	catch (WrongRangeException)
	{
		// this is ok
	}

	catch (...)
	{
			TEST_FAIL("Unexpected exception.")
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
