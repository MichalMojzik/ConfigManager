#include "option.h"
#include "configuration.h"

namespace ConfigManager
{
	std::string NOT_AVAILABLE_STRING = "n/a";
	
	AbstractOptionProxy::AbstractOptionProxy()
		: option_node_(nullptr)
	{
	}

	AbstractOptionProxy::AbstractOptionProxy(OptionNode& option_node)
		: option_node_(&option_node)
	{
		// provazani proxy s uzlem
		option_node.SetProxy(this);

		// pokud je v uzlu uz nactena hodnota ze vstupu, oznacime si ho
		// --> ve chvili, kdy dedicove teto tridy poprve nastavi hodnotu, budeme ji povazovat za default
		if(option_node.IsLoaded())
		{
			option_node.SetSpecified();
		}
	}

	AbstractOptionProxy::AbstractOptionProxy(AbstractOptionProxy && other)
		: option_node_(other.option_node_)
	{
		// ukradnuti uzlu
		other.option_node_ = nullptr;
		// informovani uzlu o ukradnuti
		option_node_->SetProxy(this);
	}

	AbstractOptionProxy& AbstractOptionProxy::operator=(AbstractOptionProxy && other)
	{
		// pokud jiz jsme svazany s nejakym uzlem, tak ho informujeme ho odvazani
		if (option_node_ != nullptr)
		{
			option_node_->SetProxy(nullptr);
		}
		// ukradnuti uzlu
		option_node_ = other.option_node_;
		other.option_node_ = nullptr;
		// informovani uzlu o ukradnuti
		option_node_->SetProxy(this);
		return *this;
	}

	AbstractOptionProxy::~AbstractOptionProxy()
	{
		if (option_node_ != nullptr)
		{
			option_node_->SetProxy(nullptr);
		}
	}

	void AbstractOptionProxy::AssignValueData(const std::string & data)
	{
		if (option_node_ != nullptr)
		{
			// je uzel jiz oznacen
			if(option_node_->IsSpecified())
			{
				// nastavime hodnotu a oznacime uzel, ze obsahuje zmenenou hodnotu
				option_node_->Assign(data);
			}
			else
			{
				// uzel oznacime
				option_node_->SetSpecified();
				// a nastavime jeho defaultni hodnotu
				option_node_->Value() = data;
			}
		}
		// pokud uzel neni nastaven, nedelame nic -- proxy by stale melo byt pouzitelne jako promenna
	}

	const std::string& AbstractOptionProxy::GetName()
	{
		if (option_node_ != nullptr)
		{
			return option_node_->Name();
		}
		else
		{
			return NOT_AVAILABLE_STRING;
		}
	}

	const std::string& AbstractOptionProxy::GetSectionName()
	{
		if (option_node_ != nullptr)
		{
			return option_node_->Section().Name();
		}
		else
		{
			return NOT_AVAILABLE_STRING;
		}
	}
}

