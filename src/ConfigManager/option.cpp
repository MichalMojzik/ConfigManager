#include "option.h"

namespace ConfigManager
{
	AbstractOptionProxy::AbstractOptionProxy()
	{
	}

	AbstractOptionProxy::AbstractOptionProxy(AbstractOptionProxy && other)
	{
	}

	AbstractOptionProxy & ConfigManager::AbstractOptionProxy::operator=(AbstractOptionProxy && other)
	{
		return other;
	}

	AbstractOptionProxy::~AbstractOptionProxy()
	{
	}

	void AbstractOptionProxy::AssignValueData(const std::string & data)
	{
	}

	void AbstractOptionProxy::AssignValueData(const std::string & data, int from_index, int count)
	{
	}

	void AbstractOptionProxy::AssignLink(const AbstractOptionProxy & data)
	{
	}

	void AbstractOptionProxy::AssignLink(const AbstractOptionProxy & data, int from_index, int count)
	{
	}
}

