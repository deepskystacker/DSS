#include "stdafx.h"
#include "Multitask.h"

int CMultitask::GetNrCurrentOmpThreads()
{
	return omp_get_num_threads();	// Returns 1 if outside parallel region, else will return threads/cores used!
}

int CMultitask::GetNrProcessors(bool bReal)
{
	const auto nrProcessorsSetting = QSettings{}.value("MaxProcessors", uint{ 0 }).toUInt();

	//SYSTEM_INFO SysInfo;
	//GetSystemInfo(&SysInfo);
	//int lResult = SysInfo.dwNumberOfProcessors;
	const int nrProcessors = std::max(omp_get_num_procs(), 1);
	if (!bReal && nrProcessorsSetting != 0)
		return std::min(static_cast<int>(nrProcessorsSetting), nrProcessors);
	else
		return nrProcessors;
}

void CMultitask::SetUseAllProcessors(bool bUseAll)
{
	QSettings settings;
	if (bUseAll)
		settings.setValue("MaxProcessors", uint{ 0 });
	else
		settings.setValue("MaxProcessors", uint{ 1 });
}

bool CMultitask::GetReducedThreadsPriority()
{
	return QSettings{}.value("ReducedThreadPriority", true).toBool();
}

void CMultitask::SetReducedThreadsPriority(bool bReduced)
{
	QSettings{}.setValue("ReducedThreadPriority", bReduced);
}

bool CMultitask::GetUseSimd()
{
	return QSettings{}.value("UseSimd", true).toBool();
}

void CMultitask::SetUseSimd(const bool bUseSimd)
{
	QSettings{}.setValue("UseSimd", bUseSimd);
}
