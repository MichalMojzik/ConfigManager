#include "configuration.h"
#include "optionnode.h"
#include "sectionnode.h"

#include "utilities.h"
#include <fstream>
#include <stack>

namespace ConfigManager
{
	Configuration::OriginalData::OriginalData(const std::string& line)
		: line_(line), section_(nullptr), option_(nullptr)
	{
	}


	Configuration::Configuration()
		: loaded_(false)
  {
  }
  

	bool Configuration::ResolveLink(const std::string& value, LinkValues& link_values, std::string* result, std::vector<OptionNode*>* resolved_links, Link* unresolved_link)
	{
		std::size_t offset = 0;
		while(offset < value.size())
		{
			// nalezeni dalsiho linku
			auto link_position = find_first_nonespaced(value, '$', offset);
			if(link_position == std::string::npos)
			{
				// zadny link nenalezen, vypsat zbytek vstupu a skoncit
				if(result != nullptr)
				{
					*result += value.substr(offset);
				}
				break;
			}

			// vypiseme vse az do linku
			if(result != nullptr)
			{
				*result += value.substr(offset, link_position - offset);
			}

			// pokud retezec dal nepokracuje, nebo nepokracuje otevrenou slozenou zavorkou, je problem se vstupem
			if(link_position + 1 == value.length() || value[link_position + 1] != '{')
			{
				throw MalformedInputException();
			}

			// najdeme oddelovac jmen sekce a volby, ktery je v aktulnim urovni zanoreni
			auto hash_position = find_first_nonespaced_unscoped(value, '#', link_position + 2, '{', '}');
			if(hash_position == std::string::npos) // pokud neni, chyba
			{
				throw MalformedInputException();
			}
			// najdeme koncovou zavorku, ktera je v aktulnim urovni zanoreni
			auto last_brace_position = find_first_nonespaced_unscoped(value, '}', hash_position + 1, '{', '}');
			if(last_brace_position == std::string::npos) // pokud neni, chyba
			{
				throw MalformedInputException();
			}

			// par jmen {sekce, volba}
			Link link;
			// retezec oznacujici jmeno sekce muze obsahovat linky, pokusime se je rekurzivne vyhodnotit
			if(!ResolveLink(value.substr(link_position + 2, hash_position - link_position - 2), link_values, &link.first, resolved_links, unresolved_link))
			{
				// vyhodnoceni linku se nezdarilo, koncime; nevyhodnoceny link je prirazen do unresolved_link z rekurzivniho volani
				return false;
			}
			// retezec oznacujici jmeno volby muze obsahovat linky, pokusime se je rekurzivne vyhodnotit
			if(!ResolveLink(value.substr(hash_position + 1, last_brace_position - hash_position - 1), link_values, &link.second, resolved_links, unresolved_link))
			{
				// vyhodnoceni linku se nezdarilo, koncime; nevyhodnoceny link je prirazen do unresolved_link z rekurzivniho volani
				return false;
			}

			// pokusime se vyhodnotit link urceny ziskanymi jmeny
			auto link_it = link_values.find(link);
			if(link_it == link_values.end())
			{
				// link nenalezen, ohlasime ho
				if(unresolved_link != nullptr)
				{
					*unresolved_link = link;
				}
				// a selzeme
				return false;
			}

			// link lze vyresit, zapiseme ho tedy do seznamu
			if(resolved_links)
			{
				resolved_links->push_back(link_it->second);
			}

			// hodnotu linku ziskame z OptionNode ziskaneho pres link
			if(result != nullptr)
			{
				*result += link_it->second->Value();
			}

			// skocime za konec specifikace linku a opakujeme
			offset = last_brace_position + 1;
		}

		return true;
	}

	std::string StripComment(std::string line)
	{
		// pokusime se najit zacatek komentare (preskakujeme escapovane stredniky)
		auto semilocon_position = find_first_nonespaced(line, ';');
		if(semilocon_position != std::string::npos)
		{
			line.erase(semilocon_position); // odstranime komentar
		}
		line = trim_nonescaped(line); // odstranime neescapovane mezery
		return line;
	}

	void Configuration::ProcessLines()
	{
		// inicializece kontejneru po vazby linku a odlozene sekce a volby
		LinkValues link_values;
		PostponedSections postponed_sections;
		PostponedOptions postponed_options;

		// inicializace: nejsme v zadne sekci
		bool is_section = false;
		SectionRange section_range;
		// prochazime jednotlive radky vstupu
		for(std::size_t line_index = 0; line_index < original_lines_.size(); ++line_index)
		{
			std::string line = StripComment(original_lines_[line_index].line_);
			if(line.empty())
			{
				continue;
			}

			// zacatek nove sekce
			if(line.front() == '[' && line.back() == ']')
			{
				if(is_section)
				{
					// zpracovani prave probehnute sekce
					ProcessSection(section_range, link_values, postponed_sections, postponed_options);
				}

				// oznacime, ze jsme v sekci, a ze jeji rozsah je prozatim jednoradkovy
				is_section = true;
				section_range = { line_index, line_index };
			}
			else if(is_section)
			{
				// rozsireni rozsahu existujici sekce az po aktualni radek
				section_range.second = line_index;
			}
			else
			{
				throw MalformedInputException();
			}
		}
		// posledni sekci zpracujeme take
		if(is_section)
		{
			ProcessSection(section_range, link_values, postponed_sections, postponed_options);
		}

		// pokud zbyvaji nejake odlozene sekce, tak je v sekci nebo volbe link, ktery nelze vyhodnotit
		if(!postponed_sections.empty() || !postponed_options.empty())
		{
			throw MalformedInputException();
		}
	}

	void Configuration::ProcessSection(SectionRange range, LinkValues& link_values, PostponedSections& postponed_sections, PostponedOptions& postponed_options)
	{
		// ziskame prvni radek ze zadaneho rozsahu bez komentaru a prebytecnych mezer
		std::string line = StripComment(original_lines_[range.first].line_);
		if(line.front() != '[' || line.back() != ']')
		{
			throw InvalidOperationException();
		}

		// nazev sekce muze obsahovat linky, pokusime se je vyhodnotit
		std::vector<OptionNode*> dependancies;
		Link unresolved_link;
		std::string section_name;
		if(!ResolveLink(line.substr(1, line.length() - 2), link_values, &section_name, &dependancies, &unresolved_link))
		{
			// nektere linky nebylo mozne vyhodnotit, ulozime si tuto sekci pro zpracovani na dobu, kdy problematicky link jiz vyhodnotit pujde
			postponed_sections.emplace(unresolved_link, range);
			return;
		}
		section_name = unescape(section_name);

		// vytvorime novou sekci, ci ziskame tu, ktera jiz byla vytvorena driv (v ramci nacitani nebo v ramci pristupu k sekci uzivatelem)
		auto& section = RetrieveSection(section_name);
		// oznacime sekci za nactenou (muze vyhodit vyjimku, pokud byla sekce jiz jednou nactena)
		section.Load();
		
		// oznacime uvodni radek sekce tak, ze odpovida aktualni sekci (pro ucely rekonstrukce pri ukladani)
		original_lines_[range.first].section_ = &section;
		for(std::size_t line_index = range.first + 1; line_index <= range.second; ++line_index)
		{
			// kazdy radek ze stanoveneho rozsahu oznacime tak, ze odpovida aktualni sekci
			original_lines_[line_index].section_ = &section;

			ProcessOption(line_index, link_values, postponed_sections, postponed_options);
		}
	}

	void Configuration::ProcessOption(OptionIndex index, LinkValues& link_values, PostponedSections& postponed_sections, PostponedOptions& postponed_options)
	{
		// vyextrahujeme data radky jakozto referenci
		auto& original_line_data = original_lines_[index];

		// ziskame radek bez komentaru a prebytecnych mezer
		std::string line = StripComment(original_line_data.line_);
		// prazdny radek neresime
		if(line.empty())
		{
			return;
		}

		// najdeme rovnitko
		auto assignment_position = find_first_nonespaced(line, '=');
		// pokud rovnitko neexistuje, pak se nejedna o volbu, i kdyz jsme ji ocekavali, tedy se nejedna o platny .ini format
		if(assignment_position == std::string::npos)
		{
			throw MalformedInputException();
		}

		Link unresolved_link;
		std::vector<OptionNode*> dependancies;
		// pokusime se ziskat jiz drive prirazenou volbu k tomuto radku
		OptionNode* option = original_line_data.option_;
		// pokud zatim zacna volba prirazena neni, musime zpracovat nazev volby a vytvorit ji
		if(option == nullptr)
		{
			// nazev volby muze obsahovat linky, pokusime se je vyhodnotit
			std::string option_name;
			if(!ResolveLink(trim_nonescaped(line.substr(0, assignment_position)), link_values, &option_name, &dependancies, &unresolved_link))
			{
				// nektere linky nebylo mozne vyhodnotit, ulozime si tuto volby pro zpracovani na dobu, kdy problematicky link jiz vyhodnotit pujde
				postponed_options.emplace(unresolved_link, index);
				return;
			}
			option_name = unescape(option_name);

			// vytvorime novou volbu, ci ziskame tu, ktera jiz byla vytvorena driv (v ramci nacitani nebo v ramci pristupu k sekci uzivatelem)
			option = &(*original_line_data.section_)[option_name];
			// poznacime oznacime radek tak, ze odpovida vytvorene volbe (pro ucely rekonstrukce pri ukladani nebo opetovneho zpracovani pri chybe linku)
			original_line_data.option_ = option;
		}

		// ziskame hodnotu volby, prozatim escapovanou a potencialne obsahujici linky
		std::string raw_value = trim_nonescaped(line.substr(assignment_position + 1));
		// hodnota volby muze obsahovat linky, pokusime se je vyhodnotit
		std::string option_value;
		if(!ResolveLink(raw_value, link_values, &option_value, &dependancies, &unresolved_link))
		{
			// nektere linky nebylo mozne vyhodnotit, ulozime si tuto volby pro zpracovani na dobu, kdy problematicky link jiz vyhodnotit pujde
			postponed_options.emplace(unresolved_link, index);
			return;
		}
		// oznacime rozsah radku, ve kterem je zapsana hodnota (pro ucely rekonstrukce pri ukladani)
		original_line_data.value_start_ = line.find_first_not_of(' ', assignment_position + 1);
		original_line_data.value_end_ = original_line_data.value_start_ + raw_value.length();

		// nactenou hodnotu ulozime do volby (muze vyhodit vyjimku, pokud byla volba jiz jednou nactena)
		option->Load(option_value);

		// ulozime si volbu jakozto hodnotu linku a pokusime se zpracovat vsechny sekce a volby, ktere nebylo mozne zpracovat predtim
		Link link = { option->Section().Name(), option->Name() };
		link_values.emplace(link, option);
		for(auto it_range = postponed_sections.equal_range(link); it_range.first != it_range.second; ++it_range.first)
		{
			ProcessSection(it_range.first->second, link_values, postponed_sections, postponed_options);
		}
		for(auto it_range = postponed_options.equal_range(link); it_range.first != it_range.second; ++it_range.first)
		{
			ProcessOption(it_range.first->second, link_values, postponed_sections, postponed_options);
		}
	}


  void Configuration::Open(std::istream& input_stream)
  {
		// vycisteni predchoziho kontextu
		loaded_ = true;
		original_lines_.clear();

		// nacteni vsech radku vstupniho streamu
		std::string line;
		while(std::getline(input_stream, line))
		{
			original_lines_.emplace_back(line);
		}

		// zpracovani vsech prave nactenych radku, jejich oznaceni a inicializace dat
		ProcessLines();

		// pote, co je vse nacteno, muzeme zkontrolovat, zda jsou vsechny doposavad specifikovane sekce a volby pritomny
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			auto& section = *section_it->second;
			// pouze zkontrolujeme volby v sekci, pokud byla samotna sekce specifikovana
			if(section.IsLoaded())
			{
				for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
				{
					auto& option = *option_it->second;
					if(!option.IsLoaded() && option.requirement_ == Requirement::MANDATORY)
					{
						throw MandatoryMissingException();
					}
				}
			}
			// pokud je sekce povinna, tak musela byt nactena
			else if(section.requirement_ == Requirement::MANDATORY)
			{
				throw MandatoryMissingException();
			}
		}
	}

  void Configuration::CheckStrict()
  {
		// projdeme vsechny zname sekce
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			auto& section = *section_it->second;

			// pokud sekce nebyla specifikovana, ale byla nactena, tak doslo k poruseni podminky strict modu
			if(section.IsLoaded() && !section.is_specified_)
			{
				throw StrictException();
			}

			// projdeme vsechny zname volby probirane sekce
			for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
			{
				auto& option = *option_it->second;

				// pokud volba nebyla specifikovana, ale byla nactena, tak doslo k poruseni podminky strict modu
				if(option.IsLoaded() && option.proxy_ != nullptr)
				{
					throw StrictException();
				}
			}
		}
	}
  

	void Configuration::OutputRestOfSections(std::ostream& output_stream, bool emit_default)
	{
		// projdeme vsechny zname sekce
		for(auto section_it = data_.begin(), section_end = data_.end(); section_it != section_end; ++section_it)
		{
			auto& section = *section_it->second;
			// ignorujeme ty, ktere byly soucasti puvodniho vstupu (a jsou tedy vypsany jindy)
			if(section.IsLoaded())
			{
				continue;
			}

			// pokud se sekce nijak nezmenila a nevypisujeme i vychozi hodnoty, tak take ignorujeme
			if(!emit_default && !section.HasChanged())
			{
				continue;
			}

			// zapsani hlavicky sekce
			output_stream << "[" << unescape(section.Name()) << "]";
			// zapsani komentare, pokud byl specifikovan
			if(!section.comment_.empty())
			{
				output_stream << ";" << section.comment_;
			}
			output_stream << std::endl;

			OutputRestOfOptions(output_stream, section, emit_default);
		}
	}
	void Configuration::OutputRestOfOptions(std::ostream& output_stream, SectionNode& section, bool emit_default)
	{
		// projdeme vsechny zname volby zadane sekce
		for(auto option_it = section.data_.begin(), option_end = section.data_.end(); option_it != option_end; ++option_it)
		{
			auto& option = *option_it->second;
			// ignorujeme ty, ktere byly soucasti puvodniho vstupu (a jsou tedy vypsany jindy)
			if(option.IsLoaded())
			{
				continue;
			}

			// preskoceni volby, pokud nebyla jeji hodnota zadana a pokud nevypisujeme i vychozi hodnoty
			bool value_preserved = !option.HasChanged();
			bool default_available = option.IsSpecified();
			if(value_preserved && !(emit_default && default_available))
			{
				continue;
			}

			// zapsani jmena a hodnoty volby ve standardnim formatovani
			output_stream << unescape(option.Name()) << "=" << option.Value(); // volba jiz obsahuje escape sekvence
			// zapsani komentare, pokud byl specifikovan
			if(!option.comment_.empty())
			{
				output_stream << ";" << option.comment_;
			}
			output_stream << std::endl;
		}
	}
  void Configuration::Save(std::ostream& output_stream, OutputMethod output_method)
  {
		// extrakce konkretnich moznosti z parametru vystupu
		bool emit_default = output_method == OutputMethod::EMIT_DEFAULT_VALUES;

		// odkaz na prave zpracovanou sekci
		SectionNode* section = nullptr;
		// projdeme radky puvodniho vstupu
		for(auto original_it = original_lines_.begin(), original_end = original_lines_.end(); original_it != original_end; ++original_it)
		{
			auto& original_line_data = *original_it;
			// pokud dochazi k prechodu mezi sekcemi (nebo ze sekce na nic), vypiseme nove volby, ktere nebyly soucasti puvodniho vstupu
			if(section != original_line_data.section_ && section != nullptr)
			{
				OutputRestOfOptions(output_stream, *section, emit_default);
			}

			section = original_line_data.section_;
			// pokud je zpracovavany radek spojen s nejakou sekci, u ktere doslo v prubehu fungovani tridy ke zmene hodnoty, zapiseme novou hodnotu
			if(original_line_data.option_ != nullptr && original_line_data.option_->HasChanged())
			{
				output_stream << original_line_data.line_.substr(0, original_line_data.value_start_); // cast puvodniho radku pred hodnotou
				output_stream << original_line_data.option_->Value(); // samotna hodnota
				output_stream << original_line_data.line_.substr(original_line_data.value_end_); // cast puvodniho radku za hodnotou
			}
			else
			{
				// jinak vypiseme puvodni retezec beze zmeny (puvodni volba, prazdny retezec nebo komentar)
				output_stream << original_line_data.line_;
			}
			output_stream << std::endl;
		}
		// pokud nam zbyla nejaka sekce z predchozi smycky, vypiseme nove volby, ktere nebyly soucasti puvodniho vstupu
		if(section != nullptr)
		{
			OutputRestOfOptions(output_stream, *section, emit_default);
		}

		// nakonec vyresime vsechny sekce, ktere nebyly soucasti puvodniho vstupu
		OutputRestOfSections(output_stream, emit_default);
  }
  
  Section Configuration::SpecifySection(const std::string& section_name, Requirement requirement, const std::string& comments)
  {
		auto& section_node = RetrieveSection(section_name);
		section_node.SetRequirement(requirement);
		section_node.SetComment(comments);
    return Section(section_node);
  }

	Section Configuration::operator[](const std::string& section_name)
	{
		auto& section_node = RetrieveSection(section_name);
		return Section(section_node);
	}

	SectionNode& Configuration::RetrieveSection(const std::string& section_name)
	{
		// ziskani, ci pripadne vytvoreni, OptionNode pro dany nazev
		auto result = data_.emplace(section_name, std::unique_ptr<SectionNode>(new SectionNode(*this, section_name)));
		return *result.first->second;
	}

	bool Configuration::IsLoaded() const
	{
		return loaded_;
	}
};
