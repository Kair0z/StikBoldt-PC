#pragma once

template <class Service>
class ILocator
{
public:
	static void Provide(Service* pService)
	{
		if (m_pService) return;
		else m_pService = pService;
	}

	static void Clear()
	{
		m_pService = nullptr;
	}

	static Service* Locate()
	{
		return m_pService;
	}

private:
	static Service* m_pService;
};

#include "ILocator.inl"