#pragma once

class Multitask
{
public:
	Multitask() = default;

	static int GetNrProcessors(bool bReal = false);
	static int GetNrCurrentOmpThreads();
	static void	setMaxProcessors(uint processors);
	static bool	GetReducedThreadsPriority();
	static void	SetReducedThreadsPriority(bool bReduced);
	static bool GetUseSimd();
	static void SetUseSimd(const bool bUseSimd);
};
