#pragma once


#include "../lib/cpptest-1.1.2/src/cpptest.h"


/*Tyto testy proveruji fungovani tridy Configuration. Tuto tridu nema smysl pouzivat bez instanci trid Section a OptionProxy, proto se zde testuji jen zakladni vlastnosti.
*
*/
class ConfigurationTestSuite : public Test::Suite
{
public:
	ConfigurationTestSuite();
	~ConfigurationTestSuite();
private:
	/* Test proverujici spravne cteni formatu vstupnich dat.
	*
	*/
	void FormatReadingTest();
};

