#include "ILocator.h"

template <class Service>
Service* ILocator<Service>::m_pService = nullptr;
