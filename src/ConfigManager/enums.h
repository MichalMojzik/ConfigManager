#ifndef CONFIGMANAGER_ENUMS_H
#define CONFIGMANAGER_ENUMS_H

namespace ConfigManager
{
	/**
	* Specifikuje zda je sekce/volba povinna.
	*  Povinne volby nesmeji byt doplneny prednastavenou hodnotou, pokud se nejedna o celou nepovinnou sekci. 
	*/
	enum Requirement
	{
		MANDATORY , /**< Sekce a hodnoty voleb s timto nastavenim musi byt vzdy specifikovany, nemohou byt nastaveny implicitni hodnotou.*/
		OPTIONAL    /**< Sekce a hodnoty voleb s timto nastavenim nemusi byt specifikovany a bodou doplneny prednstavenou hodnotou.*/
	};

	/**
	* Urcuje zda budou do vystupniho formatu zapsany prednastavene hodnoty (default).
	*	Pri nastaveni NORMAL se nevypisuji prednastavene hodnoty do vystupniho souboru.
	*/
	enum OutputMethod
	{
		NORMAL, /**< Pri teto volbe se vypisi pouze hodnoty ruzne od prednastavenych.*/
		EMIT_DEFAULT_VALUES  /**< Pri teto volbe se vypisi vsechny hodnoty (vcetne prednastavenych).*/
	};
};

#endif
