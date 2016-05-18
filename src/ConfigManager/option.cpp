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
	}

	AbstractOptionProxy::AbstractOptionProxy(AbstractOptionProxy && other)
		: option_node_(other.option_node_)
	{
		if(option_node_ != nullptr)
			option_node_->SetProxy(nullptr);
		other.option_node_ = nullptr;
		option_node_->SetProxy(this);
	}

	AbstractOptionProxy& AbstractOptionProxy::operator=(AbstractOptionProxy && other)
	{
		if(option_node_ != nullptr)
			option_node_->SetProxy(nullptr);
		option_node_ = other.option_node_;
		other.option_node_ = nullptr;
		option_node_->SetProxy(this);
		return *this;
	}

	AbstractOptionProxy::~AbstractOptionProxy()
	{
		if(option_node_ != nullptr)
			option_node_->SetProxy(nullptr);
	}

	void AbstractOptionProxy::AssignValueData(const std::string & data)
	{
		if(option_node_ != nullptr)
			option_node_->Value() = data;
	}

	const std::string& AbstractOptionProxy::GetName()
	{
		if(option_node_ != nullptr)
			return option_node_->Name();
		else
			return NOT_AVAILABLE_STRING;
	}

	const std::string& AbstractOptionProxy::GetSectionName()
	{
		if(option_node_ != nullptr)
			return option_node_->Section().Name();
		else
			return NOT_AVAILABLE_STRING;
	}
}

