#pragma once


template <class SimdClass>
class SimdFactory
{
public:
	static SimdClass makeSimdObject(auto* pDataClass)
	{
		return SimdClass{ *pDataClass };
	}
};

template <class PrimarySimdClass, class... OtherSimdClasses>
int SimdSelector(auto* pDataClass, auto&& Caller)
{
	const int rv = Caller(PrimarySimdClass::makeSimdObject(pDataClass));

	if constexpr (sizeof...(OtherSimdClasses) == 0)
		return rv;
	else
		return rv == 0 ? 0 : SimdSelector<OtherSimdClasses...>(pDataClass, std::forward<decltype(Caller)>(Caller));
}
