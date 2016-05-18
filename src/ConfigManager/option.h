#ifndef CONFIGMANAGER_OPTION_H
#define CONFIGMANAGER_OPTION_H

#include <string>
#include <vector>
#include "optionnode.h"
#include "sectionnode.h"

namespace ConfigManager
{

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
			ConstItem(const ListOptionProxy<TypeSpecifier>& parent, std::size_t index);

			/**
			* Metoda pro pristup k hodnote.
			*/
			const ValueType& Get() const;
		protected:
			const ListOptionProxy<TypeSpecifier>& parent_;
			std::size_t index_;
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
			Item(ListOptionProxy<TypeSpecifier>& parent, std::size_t index);

			/**
			* Metoda pro pristup k hodnote.
			*/
			const ValueType& Get() const;
			/**
			* Metoda pro nastavovani nove hodnoty.
			* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
			* \param value Nova hodnota.
			*/
			void Set(const ValueType& value);
		protected:
			ListOptionProxy<TypeSpecifier>& parent_;
			std::size_t index_;
		};

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
		* Metoda vracejici pocet voleb v seznamu.
		*
		*/
		std::size_t Count();
		/**
		* Operator pro pristup k jednotlivym volbam ze seznamu.
		* Muze vyhodit OutOfBoundsException vyjimku.
		* \param index Pozice pozadovane volby.
		*/
		Item operator[] (std::size_t index);
		/**
		* Operator pro konstantni pristup k jednotlivym volbam ze seznamu.
		* Muze vyhodit OutOfBoundsException vyjimku.
		* \param index Pozice pozadovane volby.
		*/
		ConstItem operator[] (std::size_t index) const;

		/**
		* Metoda pro pridani nove volby do seznamu.
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		* \param value Nova volba.
		*/
		Item Add(const ValueType& value);

		/**
		* Metoda pro pridani nove volby do seznamu.
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		* \param value Nova volba.
		*/
		Item Insert(std::size_t index, const ValueType& value);

		/**
		* Metoda pro odstraneni volby ze seznamu.
		* Muze vyhodit WrongFormatException vyjimku (kvuli existenci referenci).
		*/
		void Remove(std::size_t index);

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

#include "utilities.h"

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
		if(option_node.IsLoaded())
		{
			ProcessValueData(option_node.Value());
		}
	}

	template<typename TypeSpecifier>
	OptionProxy<TypeSpecifier>::OptionProxy(OptionProxy&& other)
		: AbstractOptionProxy(std::move(other)), value_(std::move(other.value_)), type_specifier_(std::move(other.type_specifier_))
	{
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
		AssignValueData(escape(type_specifier_.ToString(value)));
		value_ = value;
	}

	template<typename TypeSpecifier>
	void OptionProxy<TypeSpecifier>::RegenerateValueData()
	{
		AssignValueData(escape(type_specifier_.ToString(value_)));
	}

	template<typename TypeSpecifier>
	void OptionProxy<TypeSpecifier>::ProcessValueData(const std::string& data)
	{
		value_ = type_specifier_.FromString(unescape(data));
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
	ListOptionProxy<TypeSpecifier>::ConstItem::ConstItem(
		const ListOptionProxy<TypeSpecifier>& parent,
		std::size_t index
		) : parent_(parent), index_(index)
	{
	}
	template<typename TypeSpecifier>
	ListOptionProxy<TypeSpecifier>::Item::Item(
		ListOptionProxy<TypeSpecifier>& parent,
		std::size_t index
		) : parent_(parent), index_(index)
	{
	}

	template<typename TypeSpecifier>
	auto ListOptionProxy<TypeSpecifier>::ConstItem::Get() const -> const ValueType&
	{
		return parent_.list_[index_];
	}

		template<typename TypeSpecifier>
	auto ListOptionProxy<TypeSpecifier>::Item::Get() const -> const ValueType&
	{
		return parent_.list_[index_];
	}

		template<typename TypeSpecifier>
	void ListOptionProxy<TypeSpecifier>::Item::Set(const ValueType& value)
	{
		parent_.list_[index_] = value;
		parent_.RegenerateValueData();
	}

	template<typename TypeSpecifier>
	std::size_t ListOptionProxy<TypeSpecifier>::Count()
	{
		return list_.size();
	}

	template<typename TypeSpecifier>
	auto ListOptionProxy<TypeSpecifier>::operator[](std::size_t index) -> Item
	{
		if(index < 0 || index >= list_.size())
			throw OutOfBoundsException();

		return Item(*this, index);
	}
	template<typename TypeSpecifier>
	auto ListOptionProxy<TypeSpecifier>::operator[](std::size_t index) const -> ConstItem
	{
		if(index < 0 || index >= list_.size())
			throw OutOfBoundsException();

		return ConstItem(*this, index);
	}

	template<typename TypeSpecifier>
	auto ListOptionProxy<TypeSpecifier>::Add(const ValueType& value) -> Item
	{
		list_.push_back(value);
		RegenerateValueData();

		return Item(*this, list_.size() - 1);
	}
	template<typename TypeSpecifier>
	auto ListOptionProxy<TypeSpecifier>::Insert(std::size_t index, const ValueType& value) -> Item
	{
		if(index < 0 || index >= list_.size())
			throw OutOfBoundsException();
		list_.insert(list_.begin() + index, value);
		RegenerateValueData();

		return Item(*this, index);
	}
	template<typename TypeSpecifier>
	void ListOptionProxy<TypeSpecifier>::Remove(std::size_t index)
	{
		if(index < 0 || index >= list_.size())
			throw OutOfBoundsException();

		list_.erase(list_.begin() + index);
		RegenerateValueData();
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
			std::string data = unescape(type_specifier_.ToString(*it));
			result += data;
		}
		AssignValueData(result);
	}

	template<typename TypeSpecifier>
	void ListOptionProxy<TypeSpecifier>::ProcessValueData(const std::string& data)
	{
		auto first_comma_position = find_first_nonespaced(data, ',');
		auto first_colon_position = find_first_nonespaced(data, ':');
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
			std::size_t next = find_first_nonespaced(data, chosen_delimiter_, offset);
			if(next == std::string::npos)
			{
				next = data.length();
			}
			std::string item_data = data.substr(offset, next - offset);
			ValueType item_value = type_specifier_.FromString(unescape(item_data));
			list_.push_back(item_value);
			offset = next + 1;
		}
	}
}


#endif