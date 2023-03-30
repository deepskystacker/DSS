#pragma once

class CMultitask
{
public:
	CMultitask() = default;

	static int GetNrProcessors(bool bReal = false);
	static int GetNrCurrentOmpThreads();
	static void	SetUseAllProcessors(bool bUseAll);
	static bool	GetReducedThreadsPriority();
	static void	SetReducedThreadsPriority(bool bReduced);
	static bool GetUseSimd();
	static void SetUseSimd(const bool bUseSimd);
};
