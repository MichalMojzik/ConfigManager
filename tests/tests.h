#pragma once


#include "../lib/cpptest-1.1.2/src/cpptest.h"


/*
*Tyto testy proveruji fungovani tridy Configuration. Tuto tridu nema smysl pouzivat bez instanci trid Section a OptionProxy, proto se zde testuji jen zakladni vlastnosti.
*
*/
class ConfigurationTestSuite : public Test::Suite
{
public:
	ConfigurationTestSuite();
private:
	/* 
	* Test proverujici spravne cteni formatu vstupnich dat.
	*/
	void FormatReadingTest();
};

class SectionTestSuite : public Test::Suite
{
public:
	SectionTestSuite();
private:
	/*
	* Test ukladani sekci.
	*/
	void SavingTests();
	/*
	* Test metody getName.
	*/
	void BasicTests();
};

class OptionTestSuite : public Test::Suite
{
public:
	OptionTestSuite();
private:
	/*
	* Test ukladani sekci..
	*/
	void SavingTest();
	/*
	* Test metody getName a getSectionName.
	*/
	void BasicTest();

	enum TestEnum
	{
		FIRST_VAL,
		SECOND_VAL,
		THIRD_VAL
	};
};

/*
* Testy pro tridy TypeSpecifier.
*/
class TypeSpecifiersTestSuite : public Test::Suite
{
public:
	TypeSpecifiersTestSuite();
private:
	void BooleanSpecTest();
	void IntegerSpecTest();
	void UnsignIntSpecTest();
	void FloatSpecTest();
	void StringSpecTest();
	void EnumSpecTest();

	enum TestEnum
	{
		FIRST_VAL,
		SECOND_VAL,
		THIRD_VAL
	};
};



