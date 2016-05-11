#ifndef CONFIGMANAGER_ENUMS_H
#define CONFIGMANAGER_ENUMS_H

namespace ConfigManager
{
	/**  Specifikuje zda je sekce/volba povinne.
	*  Tento vyctovy typ specifikuje zda je volba, ci sekce povinna, povinne volby nesmeji byt doplneni prednastavenou hodnotou. 
	*/
	enum Requirement
	{
		MANDATORY , /**< Sekce a hodnoty voleb s timto nastavenim musi byt vzdy specifikovany, nemohou byt nastaveny implicitni hodnotou.*/
		OPTIONAL    /**< Sekce a hodnoty voleb s timto nastavenim nemusi byt specifikovany a bodou doplneny prednstavenou hodnotou.*/
	};


	/** Spefikuje pristup k vstupnimu souboru (relaxed/strict).
	*   Volba IGNORE_NONEXISTANT (relaxed) umozni zpracovat vstup obsahujici i sekce mimo zadany format. Volba NONE (strict) zpusobi selhani v pripade neshody se specifikovanym formatem..
	*/
	enum InputFilePolicy
	{
		NONE,				/**< Tato volba vynuti presnou shodu s formatem.*/
		IGNORE_NONEXISTANT  /**< Toto nastaveni umozni ignorovat sekce nespecifikovane v formatu.*/
	};

	/** Urcuje zda budou do vystupniho formatu zapsany prednastavene hodnoty (default).
	*	Pri nastaveni NORMAL se nevypisuji prednastavene hodnoty do vystupniho souboru.
	*/
	enum OutputMethod
	{
		NORMAL, /**< Pri teto volbe se vypisi pouze hodnoty ruzne od prednastavenych.*/
		OUTPUT_DEFAULTS  /**< Pri teto volbe se vypisi vsechny hodnoty (vcetne prednastavenych).*/
	};
};

#endif
