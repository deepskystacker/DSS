#ifndef __MULTITASK_H__
#define __MULTITASK_H__

constexpr UINT WM_MT_PROCESS	= WM_USER + 1;
constexpr UINT WM_MT_STOP		= WM_USER + 2;

class CMultitask
{
public:
	CMultitask() = default;

	static int	GetNrProcessors(bool bReal = false);
	static void	SetUseAllProcessors(bool bUseAll);
	static bool	GetReducedThreadsPriority();
	static void	SetReducedThreadsPriority(bool bReduced);
	static bool GetUseSimd();
	static void SetUseSimd(const bool bUseSimd);
};

#endif // __MULTITASK_H__