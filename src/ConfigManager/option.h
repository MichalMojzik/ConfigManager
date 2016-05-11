#ifndef CONFIGMANAGER_OPTION_H
#define CONFIGMANAGER_OPTION_H

#include <string>
#include <vector>

namespace ConfigManager
{
	/** 
	* Spolecny predek trid Option a ListOption. Virtualni trida. Zastresuje spolecne chovani trid Option a ListOption. 
	* Specialni konstruktory zajistuji pouze "non-copyable" chovani. 
	*/
	class AbstractOption
	{
	protected:
		/** Zakladní konstruktor.
		*
		*/
		AbstractOption() {}
		/** Neni povoleno kopirovani instanci teto tridy. 
		*
		*/
		AbstractOption(const AbstractOption& other) = delete;
		/** Neni povoleno kopirovani instanci teto tridy.
		*
		*/
		AbstractOption& operator=(const AbstractOption& other) = delete;
		/** Je povoleno pouze "movable" chovani. 
		*
		*/
		AbstractOption(AbstractOption&& other) {}
		/** \copydoc AbstractOption(AbstractOption&& other)
		*
		*/
		AbstractOption& operator=(AbstractOption&& other) {}
		/** Zakladni destruktor. 
		*
		*/
		~AbstractOption() {}

		/**
		* Metoda pro komunikaci s Configuration slouzici pro vyvolani prepsani aktualni hodnoty v Configuration retezcovou reprezentaci hodnoty v Optionu (lze tim zapsat default), iniciovane v Configuration.
		* Muze vyhodit WrongFormat vyjimku.
		*/
		virtual void RegenerateValueData() = 0;
		/**
		* Metoda pro komunikaci s Configuration slouzici k aktualizaci hodnoty v Optionu dle retezcove hodnoty v Configuration, iniciovane v Configuration (pripadne Section).
		* Muze vyhodit WrongFormat vyjimku.
		* \param data Text ze ktereho ma byt hodnota prectena.
		*/
		virtual void ProcessValueData(const std::string& data) = 0;
		/**
		* Metoda pro komunikaci s Configuration slouzici k nastaveni hodnoty z Optionu do retezcove hodnoty v Configuration, iniciovane v potomcich Optionu.
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param Textova data ktera maji prepsat retezcove hodnoty v Configuration.
		*/
		void AssignValueData(const std::string& data) {}
		/**
		* Metoda pro komunikaci s Configuration slouzici k nastaveni hodnoty z Optionu do konkretniho mista v retezcove hodnote v Configuration, iniciovane v potomcich Optionu.
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param data nova hodnota
		* \param from_index pocatecni index
		* \param count delka nahrazovaneho useku
		*/
		void AssignValueData(const std::string& data, int from_index, int count) {}
		/**
		* Metoda pro komunikaci s Configuration slouzici k nastaveni reference na jiny Option do retezcove hodnoty v Configuration, iniciovane v potomcich Optionu.
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param data novy Option
		*/
		void AssignLink(const AbstractOption& data) {}
		/**
		* Metoda pro komunikaci s Configuration slouzici k nastaveni reference na jiny Option do konkretniho mista v retezcove hodnote v Configuration, iniciovane v potomcich Optionu.
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param data  novy option
		* \param from_index pocatecni index
		* \param count delka nahrazovaneho useku
		*/
		void AssignLink(const AbstractOption& data, int from_index, int count) {}

	private:
		friend class Section;
		friend class Configuration;
	};

	/** Tato trida je realizuje volbu majici jednu hodnotu. 
	*
	*/
	template<typename TypeSpecifier>
	class Option : public AbstractOption
	{
		typedef typename TypeSpecifier::ValueType ValueType;
	public:
    Option() {}
    
		/** Hlavni kontruktor, specifikujici format dane volby. 
		* \param default_value Prednastavena hodnota.
		* \param type_specifier Trida prirazena pro preklad z retezcove reprezentace.
		* \param comments Komentare. 
		*/
		Option(const ValueType& default_value,
			   TypeSpecifier type_specifier = TypeSpecifier(), 
			   const std::string comments = "" ) { }
		/** \copydoc AbstractOption::operator=(const AbstractOption& other)
		*
		*/
		Option(Option&& other) {}
		/** \copydoc AbstractOption::operator=(const AbstractOption& other)
		*
		*/
		Option& operator=(Option&& other) {}
		/** Metoda vracejici nastavenou hodnotu. 
		*
		*/
		const ValueType& Get() const { return ValueType(); }
		/**
		* Metoda pro nastaveni nove hodnoty volby.
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param value nova hodnota.
		*/
		void Set(ValueType value) { }
		/** Tato metoda realizuje volbu odkazem (ve vstupnim souboru uvedene znakem $).
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param option Volba ke ktere odkaz vede.
		*/
		void Link(const AbstractOption& option) { }
	protected:
		/** \copydoc AbstractOption::RegenerateValueData()
		* 
		*/
		virtual void RegenerateValueData() override {}
		/** \copydoc AbstractOption::ProcessValueData()
		*
		*/
		virtual void ProcessValueData(const std::string& data) override {}
	};

	/** Tato trida realizuje volbu tvorenou seznamem elementu. 
	*
	*/
	template<typename TypeSpecifier>
	class ListOption : public AbstractOption
	{
		typedef typename TypeSpecifier::ValueType ValueType;
	public:
		/** Hlavni kontruktor, specifikujici format dane volby.
		* \param default_value Prednastavena hodnota.
		* \param type_specifier Trida prirazena pro preklad z retezcove reprezentace.
		* \param comments Komentare.
		*/
		ListOption(const std::vector<ValueType>& default_value, 
			       TypeSpecifier type_specifier = TypeSpecifier(),
			       const std::string comments = "") { }
		/** \copydoc AbstractOption::operator=(const AbstractOption& other)
		*
		*/
		ListOption(ListOption&& other) {}
		/** \copydoc AbstractOption::operator=(const AbstractOption& other)
		*
		*/
		ListOption& operator=(ListOption&& other) {}

		/**
		* Trida pro navrat z operatoru [] v pripade konstantniho pristupu, ekvivalent metody get.
		*/
		class ConstItem
		{
		public:
			/** Konstruktor ze seznamu a indexu. 
			* \param parent Seznam ze ktereho prvek pochazi.
			* \param index Pozice v seznamu. 
			*/
			ConstItem(const ListOption<TypeSpecifier>& parent, int index) : parent_(parent), index_(index) {}

			/**
			* Metoda pro pristup k hodnote. 
			*/
			const ValueType& Get() const { return ValueType(); }
		private:
			const ListOption<TypeSpecifier>& parent_;
			int index_;
		};

		/**
		* Trida pro navrat z operatoru [] v pripade pristupu, ekvivalent metody set.
		*/
		class Item
		{
		public:
			/** \copydoc ConstItem::ConstItem(const ListOption<TypeSpecifier>& parent, int index) 
			* 
			*/
			Item(ListOption<TypeSpecifier>& parent, int index) : parent_(parent), index_(index) {}
			/**
			* Metoda pro pristup k hodne.
			*/
			const ValueType& Get() const { return ValueType(); }
			/** Metoda pro nastavovani nove hodnoty.
			* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
			* \param value Nova hodnota.
			*/
			void Set(const ValueType& value) {}
			/** Metoda pro realizaci hodnoty odkazem.
			* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
			* \param option Volba na kterou vede odkaz.
			*/
			void Link(AbstractOption& option) { }
			/**Metoda pro odstraneni volby ze seznamu.
			* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
			*/
			void Remove() {}
		private:
			ListOption<TypeSpecifier>& parent_;
			int index_;
		};
		/** Metoda vracejici pocet voleb v seznamu.
		*
		*/
		int Count() { return 0; }
		/**Operator pro pristup k jednotlivym volbam ze seznamu.
		* Muze vyhodit OutOfBounds vyjimku.
		* \param index Pozice pozadovane volby.
		*/
		Item operator[] (int index) { return Item(*this, index); }
		/**Operator pro konstantni pristup k jednotlivym volbam ze seznamu.
		* Muze vyhodit OutOfBounds vyjimku.
		* \param index Pozice pozadovane volby.
		*/
		ConstItem operator[] (int index) const { return ConstItem(*this, index); }

		/**Metoda pro pridani nove volby do seznamu.
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		* \param value Nova volba.
		*/
		Item Add(const ValueType& value) { return Item(*this, 0); }

	protected:
		
		/** \copydoc AbstractOption::RegenerateValueData()
		* Muze vyhodit WrongFormat vyjimku (kvuli existenci referenci).
		*/
		virtual void RegenerateValueData() override { }
		/** \copydoc AbstractOption::RegenerateValueData()
		*
		*/
		virtual void ProcessValueData(const std::string& data) override { }
	};
};

#endif