#include "option.h"



ConfigManager::AbstractOptionProxy::AbstractOptionProxy()
{
}

ConfigManager::AbstractOptionProxy::AbstractOptionProxy(AbstractOptionProxy && other)
{
}

ConfigManager::AbstractOptionProxy & ConfigManager::AbstractOptionProxy::operator=(AbstractOptionProxy && other)
{
	return other;
}

ConfigManager::AbstractOptionProxy::~AbstractOptionProxy()
{
}

void ConfigManager::AbstractOptionProxy::AssignValueData(const std::string & data)
{
}

void ConfigManager::AbstractOptionProxy::AssignValueData(const std::string & data, int from_index, int count)
{
}

void ConfigManager::AbstractOptionProxy::AssignLink(const AbstractOptionProxy & data)
{
}

void ConfigManager::AbstractOptionProxy::AssignLink(const AbstractOptionProxy & data, int from_index, int count)
{
}


