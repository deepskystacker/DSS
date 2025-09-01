#include "pch.h"
#include "Multitask.h"

int Multitask::GetNrCurrentOmpThreads()
{
	return omp_get_num_threads();	// Returns 1 if outside parallel region, else will return threads/cores used!
}

int Multitask::GetNrProcessors(bool bReal)
{
	const auto nrProcessorsSetting = QSettings{}.value("MaxProcessors", uint{ 0 }).toUInt();

	const int nrProcessors = std::max(omp_get_num_procs(), 1);
	if (!bReal && nrProcessorsSetting != 0)
		return std::min(static_cast<int>(nrProcessorsSetting), nrProcessors);
	else
		return nrProcessors;
}

void Multitask::setMaxProcessors(uint processors)
{
	QSettings settings;
 
	// Get the number of available processors.
	const int nrProcessors = std::max(omp_get_num_procs(), 1);
	
	// If processors is 0, we use all available processors.
	if (0 == processors)
		processors = static_cast<uint>(nrProcessors);

	omp_set_num_threads(static_cast<int>(processors));

	if (processors == static_cast<uint>(nrProcessors))
		settings.setValue("MaxProcessors", uint{ 0 });
	else
		settings.setValue("MaxProcessors", processors);
}

bool Multitask::GetReducedThreadsPriority()
{
	return QSettings{}.value("ReducedThreadPriority", true).toBool();
}

void Multitask::SetReducedThreadsPriority(bool bReduced)
{
	QSettings{}.setValue("ReducedThreadPriority", bReduced);
}

bool Multitask::GetUseSimd()
{
	return QSettings{}.value("UseSimd", true).toBool();
}

void Multitask::SetUseSimd(const bool bUseSimd)
{
	QSettings{}.setValue("UseSimd", bUseSimd);
}
