#pragma once

#define DSSBETA

#ifdef DSSBETA
#define DSSBETARELEASE 1
constexpr int DSSBETAEXPIREYEAR = 2024;
constexpr int DSSBETAEXPIREMONTH = 12;
#endif

#define DSSVER_MAJOR			5
#define DSSVER_MINOR			1
#define DSSVER_SUB				6

#ifdef DSSBETA
#define DSSVER_BUILD			DSSBETARELEASE
#else
#define DSSVER_BUILD			1
#endif

#define FB(arg)					#arg

#ifdef DSSBETA
#define VERSTRING(M, m, s, b)	FB(M) "." FB(m) "." FB(s) " Beta " FB(b)
#define VERSION_DEEPSKYSTACKER	VERSTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB, DSSVER_BUILD)
#else
#define VERSTRING(M, m, s)		FB(M)"."FB(m)"."FB(s)
#define VERSION_DEEPSKYSTACKER	VERSTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB)
#endif

#define VERFILESTRING1(VER)		FB(VER)
#define VERFILESTRING(M, m, s, b) FB(M)\
								"."\
								FB(m)\
								"."\
								FB(s)\
								"."\
								FB(b)
#define DSSVER_FILEVERSION		VERFILESTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB, DSSVER_BUILD)
#define DSSVER_PRODUCTVERSION	VERFILESTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB, DSSVER_BUILD)

#define DSSVER_COPYRIGHT "Copyright \xc2\xa9 2006-2019 Luc Coiffier;\n\
	Copyright \xc2\xa9 2018-2023 David C. Partridge, Tony Cook, Mat Draper, Simon C. Smith, Vitali Pelenjow, Tomas Tatara, Michal Schulz, Martin Toeltsch,\
	Iustin Amihaesei"
#define DSSVER_PRODUCTNAME		"DeepSkyStacker"

#define DSSLIVEVER_PRODUCTNAME		"DeepSkyStacker Live"
#define DSSLIVEVER_FILEDESCRIPTION	"DeepSkyStacker Live"
#define DSSLIVEVER_INTERNALNAME		"DeepSkyStacker Live"
#define DSSLIVEVER_ORIGINALFILENAME	"DeepSkyStackerLive.exe"

#if defined(DSS_COMMANDLINE)
#define DSSVER_FILEDESCRIPTION	"DeepSkyStacker Command line"
#define DSSVER_INTERNALNAME		"DeepSkyStackerCL"
#define DSSVER_ORIGINALFILENAME	"DeepSkyStackerCL.exe"
#else
#define DSSVER_FILEDESCRIPTION	"DeepSkyStacker"
#define DSSVER_INTERNALNAME		"DeepSkyStacker"
#define DSSVER_ORIGINALFILENAME	"DeepSkyStacker.exe"
#endif
