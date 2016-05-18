#ifndef CONFIGMANAGER_OPTION_H
#define CONFIGMANAGER_OPTION_H

#include <string>
#include <vector>

namespace ConfigManager
{
	class OptionNode;

	/**
	* Spolecny predek trid OptionProxy a ListOptionProxy. Virtualni trida. Zastresuje spolecne chovani trid OptionProxy a ListOptionProxy.
	* Specialni konstruktory zajistuji pouze "non-copyable" chovani.
	*/
	class AbstractOptionProxy
	{
	public:
		/**
		* Tato metoda vraci nazev optionu.
		*/
		const std::string& GetName();
		/**
		* Metoda zpristupnujici jmeno sekce do ktere tento option prislusi.
		*/
		const std::string& GetSectionName();

		// /**
		// * Vraci objekt Configuration do ktereho option prislusi. 
		// */
		///Configuration& getConfiguration();

	protected:
		/**
		*Zakladní konstruktor.
		*/
		AbstractOptionProxy();

		AbstractOptionProxy(OptionNode& option_node);
		/**
		*Neni povoleno kopirovani instanci teto tridy.
		*/
		AbstractOptionProxy(const AbstractOptionProxy& other) = delete;
		/**
		*Neni povoleno kopirovani instanci teto tridy.
		*/
		AbstractOptionProxy& operator=(const AbstractOptionProxy& other) = delete;
		/**
		*Je povoleno pouze "movable" chovani.
		*/
		AbstractOptionProxy(AbstractOptionProxy&& other);
		/**
		*\copydoc AbstractOptionProxy(AbstractOptionProxy&& other)
		*/
		AbstractOptionProxy& operator=(AbstractOptionProxy&& other);
		/**
		*Zakladni destruktor.
		*/
		~AbstractOptionProxy();

		/**
		* Metoda pro komunikaci s Configuration slouzici pro vyvolani prepsani aktualni hodnoty v Configuration retezcovou reprezentaci hodnoty v OptionProxyu (lze tim zapsat default), iniciovane v Configuration.
		* Muze vyhodit WrongFormatException vyjimku.
		*/
		virtual void RegenerateValueData() = 0;
		/**
		* Metoda pro komunikaci s Configuration slouzici k aktualizaci hodnoty v OptionProxyu dle retezcove hodnoty v Configuration, iniciovane v Configuration (pripadne Section).
		* Muze vyhodit WrongFormatException vyjimku.
		* \param data Text ze ktereho ma byt hodnota prectena.
		*/
		virtual void ProcessValueData(const std::string& data) = 0;
		/**
		* Metoda pro komunikaci s Configuration slouzici k nastaveni hodnoty z OptionProxyu do retezcove hodnoty v Configuration, iniciovane v potomcich OptionProxyu.
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		* \param Textova data ktera maji prepsat retezcove hodnoty v Configuration.
		*/
		void AssignValueData(const std::string& data);

		OptionNode* const& Node()
		{
			return option_node_;
		}

	private:
		OptionNode* option_node_;

		friend class Section;
		friend class OptionNode;
	};

	/**
	*Tato trida je realizuje volbu majici jednu hodnotu.
	*/
	template<typename TypeSpecifier>
	class OptionProxy : public AbstractOptionProxy
	{
		typedef typename TypeSpecifier::ValueType ValueType;
	public:
		OptionProxy();
		/**
		* \copydoc AbstractOptionProxy::operator=(const AbstractOptionProxy& other)
		*
		*/
		OptionProxy(OptionProxy&& other);
		/**
		* \copydoc AbstractOptionProxy::operator=(const AbstractOptionProxy& other)
		*
		*/
		OptionProxy& operator=(OptionProxy&& other);
		/** Metoda vracejici nastavenou hodnotu.
		*
		*/
		const ValueType& Get() const;
		/**
		* Metoda pro nastaveni nove hodnoty volby.
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		* \param value nova hodnota.
		*/
		void Set(const ValueType& value);

	protected:
		/**
		* \copydoc AbstractOptionProxy::RegenerateValueData()
		*
		*/
		virtual void RegenerateValueData() override;
		/**
		* \copydoc AbstractOptionProxy::ProcessValueData()
		*
		*/
		virtual void ProcessValueData(const std::string& data) override;
	private:
		/**
		* Hlavni kontruktor, specifikujici format dane volby.
		* \param default_value Prednastavena hodnota.
		* \param type_specifier Trida prirazena pro preklad z retezcove reprezentace.
		* \param comments Komentare.
		*/
		OptionProxy(OptionNode& option_node,
			const ValueType& default_value,
			TypeSpecifier type_specifier = TypeSpecifier()
			);

		TypeSpecifier type_specifier_;
		ValueType value_;

		friend class Section;
	};

	/**
	* Tato trida realizuje volbu tvorenou seznamem elementu.
	*
	*/
	template<typename TypeSpecifier>
	class ListOptionProxy : public AbstractOptionProxy
	{
		typedef typename TypeSpecifier::ValueType ValueType;
	public:
		ListOptionProxy();

		/**
		* \copydoc AbstractOptionProxy::operator=(const AbstractOptionProxy& other)
		*
		*/
		ListOptionProxy(ListOptionProxy&& other);
		/**
		* \copydoc AbstractOptionProxy::operator=(const AbstractOptionProxy& other)
		*
		*/
		ListOptionProxy& operator=(ListOptionProxy&& other);

		/**
		* Trida pro navrat z operatoru [] v pripade konstantniho pristupu, ekvivalent metody get.
		*/
		class ConstItem
		{
		public:
			/**
			* Konstruktor ze seznamu a indexu.
			* \param parent Seznam ze ktereho prvek pochazi.
			* \param index Pozice v seznamu.
			*/
			ConstItem(const ListOptionProxy<TypeSpecifier>& parent, int index) : parent_(parent), index_(index) {}

			/**
			* Metoda pro pristup k hodnote.
			*/
			const ValueType& Get() const { return list_[index_]; }
		private:
			const ListOptionProxy<TypeSpecifier>& parent_;
			int index_;
		};

		/**
		* Trida pro navrat z operatoru [] v pripade pristupu, ekvivalent metody set.
		*/
		class Item
		{
		public:
			/**
			* \copydoc ConstItem::ConstItem(const ListOptionProxy<TypeSpecifier>& parent, int index)
			*
			*/
			Item(ListOptionProxy<TypeSpecifier>& parent, int index) : parent_(parent), index_(index) {}
			/**
			* Metoda pro pristup k hodne.
			*/
			const ValueType& Get() const { return list_[index_]; }
			/**
			* Metoda pro nastavovani nove hodnoty.
			* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
			* \param value Nova hodnota.
			*/
			void Set(const ValueType& value) {}
			/**
			* Metoda pro odstraneni volby ze seznamu.
			* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
			*/
			void Remove() {}
		private:
			ListOptionProxy<TypeSpecifier>& parent_;
			int index_;
		};
		/**
		* Metoda vracejici pocet voleb v seznamu.
		*
		*/
		int Count() { return 0; }
		/**
		* Operator pro pristup k jednotlivym volbam ze seznamu.
		* Muze vyhodit OutOfBoundsException vyjimku.
		* \param index Pozice pozadovane volby.
		*/
		Item operator[] (int index) { return Item(*this, index); }
		/**
		* Operator pro konstantni pristup k jednotlivym volbam ze seznamu.
		* Muze vyhodit OutOfBoundsException vyjimku.
		* \param index Pozice pozadovane volby.
		*/
		ConstItem operator[] (int index) const { return ConstItem(*this, index); }

		/**
		* Metoda pro pridani nove volby do seznamu.
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		* \param value Nova volba.
		*/
		Item Add(const ValueType& value) { return Item(*this, 0); }

	protected:
		/**
		* \copydoc AbstractOptionProxy::RegenerateValueData()
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		*/
		virtual void RegenerateValueData() override;
		/**
		* \copydoc AbstractOptionProxy::RegenerateValueData()
		*
		*/
		virtual void ProcessValueData(const std::string& data) override;

	private:
		/**
		* Hlavni kontruktor, specifikujici format dane volby.
		* \param default_value Prednastavena hodnota.
		* \param type_specifier Trida prirazena pro preklad z retezcove reprezentace.
		* \param comments Komentare.
		*/
		ListOptionProxy(OptionNode& option_node,
			const std::vector<ValueType>& default_value,
			TypeSpecifier type_specifier = TypeSpecifier()
			);

		char chosen_delimiter_;
		TypeSpecifier type_specifier_;
		std::vector<ValueType> list_;

		friend class Section;
	};
};


/* ================ OptionProxy */
namespace ConfigManager
{
	template<typename TypeSpecifier>
	OptionProxy<TypeSpecifier>::OptionProxy()
	{
	}

	template<typename TypeSpecifier>
	OptionProxy<TypeSpecifier>::OptionProxy(
		OptionNode& option_node,
		const ValueType& default_value,
		TypeSpecifier type_specifier
		)
		: AbstractOptionProxy(option_node), value_(default_value), type_specifier_(type_specifier)
	{
	}

	template<typename TypeSpecifier>
	OptionProxy<TypeSpecifier>::OptionProxy(OptionProxy&& other)
		: AbstractOptionProxy(std::move(other)), value_(std::move(other.value_)), type_specifier_(std::move(other.type_specifier_))
	{
		if(option_node.IsLoaded())
		{
			ProcessValueData(option_node.Value());
		}
		else
		{
			RegenerateValueData();
		}
	}

	template<typename TypeSpecifier>
	OptionProxy<TypeSpecifier>& OptionProxy<TypeSpecifier>::operator=(OptionProxy&& other)
	{
		AbstractOptionProxy::operator=(std::move(other));
		value_ = std::move(other.value_);
		type_specifier_ = std::move(other.type_specifier_);
		return *this;
	}

	template<typename TypeSpecifier>
	auto OptionProxy<TypeSpecifier>::Get() const -> const ValueType&
	{
		return value_;
	}

		template<typename TypeSpecifier>
	void OptionProxy<TypeSpecifier>::Set(const ValueType& value)
	{
		AssignValueData(type_specifier_.ToString(value));
		value_ = value;
	}

	template<typename TypeSpecifier>
	void OptionProxy<TypeSpecifier>::RegenerateValueData()
	{
		AssignValueData(type_specifier_.ToString(value_));
	}

	template<typename TypeSpecifier>
	void OptionProxy<TypeSpecifier>::ProcessValueData(const std::string& data)
	{
		value_ = type_specifier_.FromString(data);
	}
}


/* ================ ListOptionProxy */
namespace ConfigManager
{
	template<typename TypeSpecifier>
	ListOptionProxy<TypeSpecifier>::ListOptionProxy()
		: AbstractOptionProxy(), list_(), chosen_delimiter_('\0')
	{
	}

	template<typename TypeSpecifier>
	ListOptionProxy<TypeSpecifier>::ListOptionProxy(
		OptionNode& option_node,
		const std::vector<ValueType>& default_value,
		TypeSpecifier type_specifier
		)
		: AbstractOptionProxy(option_node), list_(default_value), type_specifier_(type_specifier), chosen_delimiter_('\0')
	{
		if(option_node.IsLoaded())
		{
			ProcessValueData(option_node.Value());
		}
		else
		{
			RegenerateValueData();
		}
	}

	template<typename TypeSpecifier>
	ListOptionProxy<TypeSpecifier>::ListOptionProxy(ListOptionProxy&& other)
		: AbstractOptionProxy(std::move(other)), list_(std::move(other.list_)), type_specifier_(std::move(other.type_specifier_)), chosen_delimiter_(other.chosen_delimiter_)
	{
	}

	template<typename TypeSpecifier>
	ListOptionProxy<TypeSpecifier>& ListOptionProxy<TypeSpecifier>::operator=(ListOptionProxy&& other)
	{
		AbstractOptionProxy::operator=(std::move(other));
		list_ = std::move(other.list_);
		chosen_delimiter_ = other.chosen_delimiter_;
		type_specifier_ = std::move(other.type_specifier_);
		return *this;
	}

	template<typename TypeSpecifier>
	void ListOptionProxy<TypeSpecifier>::RegenerateValueData()
	{
		if(chosen_delimiter_ == '\0')
			chosen_delimiter_ = ':';

		std::string result = "";
		for(auto start = list_.begin(), it = start, end = list_.end(); it != end; ++it)
		{
			if(start != it)
			{
				result += chosen_delimiter_;
			}
			std::string data = type_specifier_.ToString(*it);
			result += data;
		}
		AssignValueData(result);
	}

	template<typename TypeSpecifier>
	void ListOptionProxy<TypeSpecifier>::ProcessValueData(const std::string& data)
	{
		auto first_comma_position = data.find_first_of(',');
		auto first_colon_position = data.find_first_of(':');
		if(first_comma_position == std::string::npos)
		{
			chosen_delimiter_ = ':';
		}
		else if(first_colon_position == std::string::npos)
		{
			chosen_delimiter_ = ',';
		}
		else if(first_comma_position < first_colon_position)
		{
			chosen_delimiter_ = ',';
		}
		else
		{
			chosen_delimiter_ = ':';
		}

		list_.clear();

		std::size_t offset = 0;
		while(offset < data.length())
		{
			std::size_t next = data.find_first_of(chosen_delimiter_, offset);
			std::string item_data = data.substr(offset, next - offset);
			ValueType item_value = type_specifier_.FromString(item_data);
			list_.push_back(item_value);
			offset = next + 1;
		}
	}
}


#endif