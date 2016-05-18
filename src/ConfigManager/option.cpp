#include "option.h"
#include "configuration.h"

namespace ConfigManager
{
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

	AbstractOptionProxy & AbstractOptionProxy::operator=(AbstractOptionProxy && other)
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
		if(option_node_ == nullptr)
			throw InvalidOperationException();
		option_node_->Value() = data;
	}

	const std::string& AbstractOptionProxy::GetName()
	{
		return option_node_->Name();
	}

	const std::string& AbstractOptionProxy::GetSectionName()
	{
		return option_node_->Section().Name();
	}
	
	/*Configuration& AbstractOptionProxy::getConfiguration()
	{
		return parentConfiguration_;
	}*/
}

