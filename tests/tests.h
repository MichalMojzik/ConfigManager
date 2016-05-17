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
	~ConfigurationTestSuite();
private:
	/* 
	*Test proverujici spravne cteni formatu vstupnich dat.
	*
	*/
	void FormatReadingTest();
};

/*
* Testy pro tridy TypeSpecifier.
*/
class TypeSpecifiersTestSuite : public Test::Suite
{
public:
	TypeSpecifiersTestSuite();
	~TypeSpecifiersTestSuite();
private:
	void BooleanSpecTest();
	void IntegerSpecTest();
	void UnsignIntSpecTest();
	void FloatSpecTest();
	void StringSpecTest();
};
