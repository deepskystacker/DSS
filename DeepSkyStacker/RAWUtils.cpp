#include <stdafx.h>
#include "BitmapExt.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "Registry.h"
#include "RAWUtils.h"
#include <set>
#include <list>
#include <float.h>
#include "Multitask.h"
#include "Workspace.h"


class CRAWSettings
{
public :
	BOOL		m_bRawBayer;
	BOOL		m_bSuperPixel;

private :
	void	CopyFrom(const CRAWSettings & rs)
	{
		m_bRawBayer = rs.m_bRawBayer;
		m_bSuperPixel = rs.m_bSuperPixel;
	};

public :
	CRAWSettings()
	{
		m_bRawBayer		= FALSE;
		m_bSuperPixel	= FALSE;
	};

	CRAWSettings(const CRAWSettings & rs)
	{
		CopyFrom(rs);
	};

	const CRAWSettings & operator = (const CRAWSettings & rs)
	{
		CopyFrom(rs);
		return (*this);
	};
};

typedef std::list<CRAWSettings>		RAWSETTINGSSTACK;

RAWSETTINGSSTACK		g_RawSettingsStack;

/* ------------------------------------------------------------------- */

static BOOL IsRegistrySuperPixels()
{
	DWORD		bResult = FALSE;
	CWorkspace	workspace;

	workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), bResult);

	return bResult;
};

/* ------------------------------------------------------------------- */

static BOOL IsRegistryRawBayer()
{
	DWORD		bResult = FALSE;
	CWorkspace	workspace;

	workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), bResult);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL IsSuperPixels()
{
	DWORD		bResult = FALSE;

	if (g_RawSettingsStack.size())
		bResult = g_RawSettingsStack.back().m_bSuperPixel;
	else
		bResult = IsRegistrySuperPixels();

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL IsRawBayer()
{
	DWORD		bResult = FALSE;

	if (g_RawSettingsStack.size())
		bResult = g_RawSettingsStack.back().m_bRawBayer;
	else
		bResult = IsRegistryRawBayer();
	
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL IsRawBilinear()
{
	CWorkspace	workspace;
	CString		strInterpolation;

	workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), strInterpolation);

	return !strInterpolation.GetLength() || (strInterpolation == _T("Bilinear"));
};

/* ------------------------------------------------------------------- */

BOOL IsRawAHD()
{
	CWorkspace	workspace;
	CString		strInterpolation;

	workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), strInterpolation);

	return !strInterpolation.GetLength() || (strInterpolation == _T("AHD")) || (strInterpolation == _T("VNG"));
};

/* ------------------------------------------------------------------- */

void	PushRAWSettings(BOOL bSuperPixel, BOOL bRawBayer)
{
	CRAWSettings		rs;

	if (IsRegistryRawBayer() || IsRegistrySuperPixels())
	{
		rs.m_bRawBayer		= bRawBayer;
		rs.m_bSuperPixel	= bSuperPixel;

		g_RawSettingsStack.push_back(rs);
	};
};

/* ------------------------------------------------------------------- */

void	PopRAWSettings()
{
	if (g_RawSettingsStack.size())
		g_RawSettingsStack.pop_back();
};

/* ------------------------------------------------------------------- */

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <typeinfo.h>

#define Thread   __declspec( thread )

extern "C" Thread char make[64];
extern "C" Thread char model[64];
extern "C" Thread unsigned short int iheight;
extern "C" Thread unsigned short int iwidth;
extern "C" Thread float iso_speed;
extern "C" Thread unsigned int is_foveon;
extern "C" Thread float shutter;
extern "C" Thread unsigned filters;
extern "C" Thread unsigned mix_green;
extern "C" Thread time_t timestamp;
extern "C" Thread int  colors;

extern "C" int DCRawmain(int argc, char* argv[], FILE * pfout);
extern "C" void InitGlobalVariables();

CFATYPE GetCurrentCFAType()
{
	if (colors==4 && mix_green==0)
	{	// CYMG Bayer pattern
		switch (filters)
		{
		case 0xb4b4b4b4 :
			return CFATYPE_GMYC;
			break;
		case 0xe1e4e1e4 :
			return CFATYPE_GMCYMGCY;
			break;
		case 0x1e4e1e4e :
			return CFATYPE_CYGMCYMG;
			break;
		};

	}
	else
	{	// RGB Bayer Pattern
		switch (filters)
		{
		case 0x1e1e1e1e :
		case 0x16161616 :
			return CFATYPE_BGGR;
			break;
		case 0xe1e1e1e1 :
		case 0x61616161 :
			return CFATYPE_GRBG;
			break;
		case 0x4b4b4b4b :
		case 0x49494949 :
			return CFATYPE_GBRG;
			break;
		case 0xb4b4b4b4 :
		case 0x94949494 :
			return CFATYPE_RGGB;
			break;
		default :
			return CFATYPE_NONE;
		};
	}

	return CFATYPE_NONE;
};

/* ------------------------------------------------------------------- */

typedef std::set<LONG>					REDIRECTEDFILES;
typedef REDIRECTEDFILES::iterator		REDIRECTEDFILESIT;

//CComAutoCriticalSection	g_CriticalSection;				
//REDIRECTEDFILES			g_mRedirectedFiles;

/* ------------------------------------------------------------------- */

class CGenericFile
{
public :
	CGenericFile() {};

	virtual ~CGenericFile() {};
};

class COutputFile : public CGenericFile
{
	typedef enum ReadStep
	{
		RS_INIT			= 0,
		RS_WIDTH		= 1,
		RS_HEIGHT		= 2,
		RS_MAXCOLOR		= 3,
		RS_BEFOREROWS	= 4,
		RS_ROWS			= 5
	}ReadStep;

private :
	CDSSProgress *			m_pProgress;
	BOOL					m_bStarted;
	CFATYPE					m_CFAType;
	CMemoryBitmap *			m_pBitmap;
	DWORD					m_dwPos;
	PixelIterator			m_PixelIt;
	DWORD					m_dwCurrentX,
							m_dwCurrentY;
	ReadStep				m_Step;
	BYTE *					m_pBuffer;
	DWORD					m_dwBufferSize;
	DWORD					m_dwBufferReadPos;
	DWORD					m_dwBufferWritePos;
	LONG					m_lWidth,
							m_lHeight,
							m_lMaxColors;
	BOOL					m_lBytePerChannel;
	BOOL					m_bGray;
	double					m_fRedScale,
							m_fGreenScale,
							m_fBlueScale;

private :
	void	AdjustColor(double & fColor, double fAdjust)
	{
		double		fResult = (double)fColor * fAdjust;

		fColor = min(MAXWORD-1, fResult);			
	};

	void	AddToBuffer(const void * buffer, DWORD lSize)
	{
		if (!m_bStarted)
			Start();
		if (!m_pBuffer)
		{
			// Alloc the buffer
			m_dwBufferSize = 2 * lSize;
			m_pBuffer = (BYTE*)malloc(m_dwBufferSize);
			m_dwBufferReadPos  = 0;
			m_dwBufferWritePos = 0;
		};
		if (lSize > m_dwBufferSize - m_dwBufferWritePos)
		{
			// realloc the buffer
			m_pBuffer = (BYTE*)realloc(m_pBuffer, lSize + m_dwBufferWritePos*2);
			m_dwBufferSize = lSize + m_dwBufferWritePos*2;
		};

		BYTE *				pWrite = m_pBuffer;
		BYTE *				pRead  = m_pBuffer;
		LONG				lToRead;
		BOOL				bEnd = FALSE;
		
		pWrite += m_dwBufferWritePos;
		pRead  += m_dwBufferReadPos;
		memcpy(m_pBuffer, buffer, lSize);
		m_dwBufferWritePos += lSize;

		while (!bEnd)
		{
			bEnd = TRUE;
			// Jump whitespace, CR, LF and TAB characters
			lToRead = m_dwBufferWritePos - m_dwBufferReadPos;
			if (m_Step < RS_BEFOREROWS)
			{
				while ((lToRead >= 1) && 
					   (strchr(" \n\t\r", *pRead) != NULL))
				{
					lToRead--;
					m_dwBufferReadPos++;
					pRead++;
				};
			};
			switch (m_Step)
			{
			case RS_INIT :
				// Try to read the magic P5 or P6 number
				if (lToRead >= 2)
				{
					CStringA	strMagic;
					// Read two characters

					strMagic += (char)(*pRead);
					strMagic += (char)(*(pRead+1));

					if (strMagic == "P6")
					{
						m_Step = RS_WIDTH;
						m_dwBufferReadPos+=2;
						pRead	+= 2;
						bEnd	= FALSE;
						m_bGray = FALSE;
					}
					else if (strMagic == "P5")
					{
						m_Step = RS_WIDTH;
						m_dwBufferReadPos+=2;
						pRead	+= 2;
						bEnd	= FALSE;
						m_bGray = TRUE;
					};
				};
				break;
			case RS_WIDTH :
				// Try to find the next white space
				{
					BYTE *		pTemp = pRead;
					LONG		lTemp = lToRead;
					CString		strWidth;

					while (lTemp >= 1 && !strchr(" \n\t\r", *pTemp))
					{
						strWidth += (char)(*pTemp);
						lTemp--;
						pTemp++;
					};

					if (strchr(" \n\t\r", *pTemp))
					{
						m_Step = RS_HEIGHT;
						m_dwBufferReadPos+=strWidth.GetLength();
						pRead += strWidth.GetLength();
						bEnd = FALSE;
						m_lWidth = _ttol(strWidth);
					};
				}
				break;
			case RS_HEIGHT :
				// Try to find the next white space
				{
					BYTE *		pTemp = pRead;
					LONG		lTemp = lToRead;
					CString		strHeight;

					while (lTemp >= 1 && !strchr(" \n\t\r", *pTemp))
					{
						strHeight += (char)(*pTemp);
						lTemp--;
						pTemp++;
					};

					if (strchr(" \n\t\r", *pTemp))
					{
						m_Step = RS_MAXCOLOR;
						m_dwBufferReadPos+=strHeight.GetLength();
						pRead += strHeight.GetLength();
						bEnd = FALSE;
						m_lHeight = _ttol(strHeight);
					};
				}
				break;
			case RS_MAXCOLOR :
				// Try to find the next white space
				{
					BYTE *		pTemp = pRead;
					LONG		lTemp = lToRead;
					CString		strMaxColor;

					while (lTemp >= 1 && !strchr(" \n\t\r", *pTemp))
					{
						strMaxColor += (char)(*pTemp);
						lTemp--;
						pTemp++;
					};

					if (strchr(" \n\t\r", *pTemp))
					{
						m_Step = RS_BEFOREROWS;
						m_dwBufferReadPos+=strMaxColor.GetLength();
						pRead += strMaxColor.GetLength();
						bEnd = FALSE;
						m_lMaxColors = _ttol(strMaxColor);

						if (m_lMaxColors > 255)
							m_lBytePerChannel = 2;
						else
							m_lBytePerChannel = 1;
					};
				}
				break;
			case RS_BEFOREROWS :
				if (lToRead >= 1)
				{
					// Jump a single character
					m_Step = RS_ROWS;
					pRead++;
					m_dwBufferReadPos ++;
					bEnd = FALSE;
				};
				break;
			case RS_ROWS :
				// Read pixels
				{
					if (m_bGray)
					{
						if (lToRead >= m_lBytePerChannel)
						{
							// Read gray
							double		fGray;
							if (m_lBytePerChannel == 2)
							{
								fGray = ((*(pRead+0)) << 8) + (*(pRead+1));
								m_dwBufferReadPos += 2;
								pRead += 2;
							}
							else
							{
								fGray = ((*(pRead+0)) << 8);
								m_dwBufferReadPos++;
								pRead++;
							};

							if (m_dwCurrentX >= m_lWidth)
							{
								m_dwCurrentY ++;
								m_dwCurrentX = 0;
								m_PixelIt->Reset(0, m_dwCurrentY);
								m_dwPos = m_dwCurrentY+1;
							};
							
							switch (GetBayerColor(m_dwCurrentX, m_dwCurrentY, m_CFAType))
							{
							case BAYER_RED :
								AdjustColor(fGray, m_fRedScale);
								break;
							case BAYER_GREEN :
								AdjustColor(fGray, m_fGreenScale);
								break;
							case BAYER_BLUE :
								AdjustColor(fGray, m_fBlueScale);
								break;
							};

							m_PixelIt->SetPixel((double)fGray/256.0);
							(*m_PixelIt)++;
							m_dwCurrentX++;

							bEnd = FALSE;
						};
					}
					else
					{
						if (lToRead >= m_lBytePerChannel * 3)
						{
							// Read RGB
							double				fRed, fGreen, fBlue;

							if (m_lBytePerChannel == 2)
							{
								fRed		= ((*(pRead+0)) << 8) + (*(pRead+1));
								fGreen		= ((*(pRead+2)) << 8) + (*(pRead+3));
								fBlue		= ((*(pRead+4)) << 8) + (*(pRead+5));
								m_dwBufferReadPos += 6;
								pRead += 6;
							}
							else
							{
								fRed		= ((*(pRead+0)) << 8);
								fGreen		= ((*(pRead+1)) << 8);
								fBlue		= ((*(pRead+2)) << 8);
								m_dwBufferReadPos += 3;
								pRead += 3;
							};

							if (m_dwCurrentX >= m_lWidth)
							{
								m_dwCurrentY ++;
								m_dwCurrentX = 0;
								m_PixelIt->Reset(0, m_dwCurrentY);
								m_dwPos = m_dwCurrentY+1;
							};

							AdjustColor(fRed, m_fRedScale);
							AdjustColor(fGreen, m_fGreenScale);
							AdjustColor(fBlue, m_fBlueScale);

							m_PixelIt->SetPixel(fRed/256.0, fGreen/256.0, fBlue/256.0);
							m_dwCurrentX++;
							(*m_PixelIt)++;

							bEnd = FALSE;
						};
					};
				}
				break;
			};
		};

		// Adjust Buffer
		LONG			lToMove = m_dwBufferWritePos - m_dwBufferReadPos;

		if (lToMove)
		{
			memmove(m_pBuffer, pRead, lToMove);
			m_dwBufferWritePos -= m_dwBufferReadPos;
			m_dwBufferReadPos = 0;
		}
		else
			m_dwBufferReadPos = m_dwBufferWritePos = 0;
	};
private:

public :
	COutputFile(CMemoryBitmap * pBitmap, CDSSProgress * pProgress) 
	{
		m_pBitmap	= pBitmap;
		m_pProgress = pProgress;
		m_bStarted  = FALSE;
		m_fRedScale = 1.0;
		m_fGreenScale = 1.0;
		m_fBlueScale  = 1.0;
		m_CFAType	= CFATYPE_NONE;
	};

	virtual ~COutputFile() 
	{
		if (m_pBuffer)
			free(m_pBuffer);
	};

	void	SetWhiteBalance(double fRedScale, double fGreenScale, double fBlueScale)
	{
		m_fRedScale		= fRedScale;
		m_fGreenScale	= fGreenScale;
		m_fBlueScale	= fBlueScale;
	};

	void	SetCFAType(CFATYPE CFAType)
	{
		m_CFAType = CFAType;
	};

	int	Start()
	{
		m_bStarted   = TRUE;
		m_Step       = RS_INIT;
		m_pBuffer	 = NULL;
		m_dwBufferSize = 0;
		m_dwBufferReadPos  = 0;
		m_dwBufferWritePos = 0;
		m_dwPos		 = 0;
		m_dwCurrentX = 0;
		m_dwCurrentY = 0;
		m_lWidth	 = 0;
		m_lHeight	 = 0;
		if (m_CFAType == CFATYPE_NONE)
			m_CFAType	 = GetCurrentCFAType();

		C16BitGrayBitmap *			pGray16Bitmap = dynamic_cast<C16BitGrayBitmap *>(m_pBitmap);
		if (pGray16Bitmap)
			pGray16Bitmap->SetCFAType(m_CFAType);
		if (m_pProgress)
			m_pProgress->Start2(NULL, m_pBitmap->Height());

		m_pBitmap->GetIterator(&m_PixelIt);

		return 0;
	};

	int	Printf(const char *format, va_list va)
	{
		int				nResult;
		CStringA			strText;

		strText.FormatV(format, va);
		nResult = strText.GetLength();
		
		AddToBuffer(strText.GetBuffer(10000), nResult);

		if (m_pProgress)
			m_pProgress->Progress2(NULL, m_dwPos);

		return nResult;
	};

	size_t Write (const void *buffer, size_t size, size_t count)
	{
		AddToBuffer(buffer, (DWORD)size*(DWORD)count);
		if (m_pProgress)
			m_pProgress->Progress2(NULL, m_dwPos);

		return count;
	};
};

/* ------------------------------------------------------------------- */

//static CString			g_strInputFileName;
static Thread char				g_szInputFileName[1+_MAX_PATH];					
static Thread CDSSProgress *	g_Progress;

class CInputFile : public CGenericFile
{
public :
	FILE *				m_hFile;
	BYTE *				m_pBuffer;
	size_t				m_lFileSize;
	size_t				m_lCurrentPos;

public :
	CInputFile()
	{
		m_hFile		= NULL;
		m_pBuffer	= NULL;
	};

	virtual ~CInputFile()
	{
		if (m_hFile)
			fclose(m_hFile);
		if (m_pBuffer)
			free(m_pBuffer);
	};

	void	UpdatePos()
	{
		if (g_Progress)
			g_Progress->Progress2(NULL, ftell());
	};

	size_t	fread(void * pDstBuff, size_t ElementSize, size_t Count)
	{
		size_t			result = 0;

		if (m_lCurrentPos + ElementSize * Count <= m_lFileSize)
		{
			BYTE *			pSrcBuff = m_pBuffer;

			pSrcBuff += m_lCurrentPos;
			memcpy(pDstBuff, pSrcBuff, ElementSize * Count);
			m_lCurrentPos += ElementSize * (DWORD)Count;

			result = Count;
		}

		return result;
	};

	int		fgetc()
	{
		int				result = 0;

		if (m_lCurrentPos < m_lFileSize)
		{
			BYTE *			pSrcBuff = m_pBuffer;

			pSrcBuff += m_lCurrentPos;

			result = *pSrcBuff;
			m_lCurrentPos++;
		};

		return result;
	};

	int		fseek(long offset, int Origin)
	{
		int				result = 0;

		if (Origin == SEEK_SET)
		{
			if (offset >= 0 && offset < m_lFileSize)
				m_lCurrentPos = offset;
			else
				result = -1;
		}
		else if (Origin == SEEK_CUR)
		{
			if ((m_lCurrentPos + offset >= 0) && (m_lCurrentPos + offset < m_lFileSize))
				m_lCurrentPos += offset;
			else
				result = -1;
		}
		else
		{
			if ((offset <= 0) && (-offset < m_lFileSize))
				m_lCurrentPos = m_lFileSize+offset;
			else
				result = -1;
		};

		return result;
	};

	long		ftell()
	{
		return (long)m_lCurrentPos;
	};

	int		fscanf(const char * Format, va_list marker)
	{
		int				result;
		BYTE *			pSrcBuff = m_pBuffer;
		CStringA		str;

		pSrcBuff += m_lCurrentPos;
		result = sscanf((const char*)pSrcBuff, Format, marker);

		str.Format(Format, marker);
		m_lCurrentPos += str.GetLength();

		return result;
	};

	char *	fgets(char * Buff, int MaxCount)
	{
		BYTE *			pSrcBuff = m_pBuffer;

		pSrcBuff += m_lCurrentPos;
		strncpy(Buff, (const char *)pSrcBuff, MaxCount);

		m_lCurrentPos += min(MaxCount, (DWORD)strlen(Buff));

		return Buff;
	};

	BOOL	Init()
	{
		BOOL			bResult = FALSE;

		if (m_hFile)
		{
			::fseek(m_hFile, 0, SEEK_END);
			m_lFileSize = ::ftell(m_hFile);

			m_pBuffer = (BYTE*)malloc(m_lFileSize);
			if (m_pBuffer)
			{
				::fseek(m_hFile, 0, SEEK_SET);
				bResult = ::fread(m_pBuffer, m_lFileSize, 1, m_hFile) == 1; 
				m_lCurrentPos = 0;
				::fclose(m_hFile);
				m_hFile = NULL;
			};
		};

		if (!bResult && m_pBuffer)
		{
			free(m_pBuffer);
			m_pBuffer = NULL;
		};

		return bResult;
	};
};

/* ------------------------------------------------------------------- */

Thread		CInputFile *		g_pInputFile = NULL;
Thread		COutputFile *		g_pOutputFile = NULL;

/* ------------------------------------------------------------------- */

class CRawDecod
{
private :
	CString			m_strFileName;
	CString			m_strModel;
	CString			m_strMake;
	LONG			m_lISOSpeed;
	double			m_fExposureTime;
	LONG			m_lHeight,
					m_lWidth;
	BOOL			m_bColorRAW;
	CFATYPE			m_CFAType;
	SYSTEMTIME		m_DateTime;

public :
	CRawDecod(LPCTSTR szFile)
	{
		m_strFileName = szFile;
		m_bColorRAW	  = FALSE;
		m_CFAType	  = CFATYPE_NONE;
		m_DateTime.wYear = 0;
	};

	virtual ~CRawDecod() 
	{
	};

	BOOL	IsRawFile();
	BOOL	LoadRawFile(CMemoryBitmap * pBitmap, CDSSProgress * pProgress = NULL, BOOL bThumb = FALSE);

	BOOL	GetModel(CString & strModel)
	{
		strModel = m_strMake + _T(" ") + m_strModel;
		return TRUE;
	};

	LONG	GetISOSpeed()
	{
		return m_lISOSpeed;
	};

	double	GetExposureTime()
	{
		return m_fExposureTime;
	};

	LONG	Width()
	{
		return m_lWidth;
	};

	LONG	Height()
	{
		return m_lHeight;
	};

	BOOL	IsColorRAW()
	{
		return m_bColorRAW;
	};

	CFATYPE	GetCFAType()
	{
		return m_CFAType;
	};

	SYSTEMTIME GetDateTime()
	{
		return m_DateTime;
	};
};

/* ------------------------------------------------------------------- */

BOOL CRawDecod::LoadRawFile(CMemoryBitmap * pBitmap, CDSSProgress * pProgress, BOOL bThumb)
{
	ZFUNCTRACE_RUNTIME();
	BOOL		bResult = TRUE;

	if (IsRawFile())
	{
		pBitmap->Init(m_lWidth, m_lHeight);
		pBitmap->SetISOSpeed(m_lISOSpeed);
		pBitmap->SetExposure(m_fExposureTime);
		pBitmap->m_DateTime = m_DateTime;
		CString			strDescription;
		GetModel(strDescription);
		pBitmap->SetDescription(strDescription);

		const		int maxargs = 50;
		CWorkspace	workspace;
		CString		strInterpolation;
		double		fBrightness = 1.0;
		double		fRedScale = 1.0;
		double		fBlueScale = 1.0;
		double		fGreenScale = 1.0;

		//DWORD		bSuperPixels;
		//DWORD		bRawBayer;
		//BOOL		bBilinear;
		//BOOL		bAHD;
		DWORD		bBlackPointTo0 = 0;
		DWORD		bValue;
		
		int			argc = 0;
		char *		argv[maxargs];

		
		argv[0] = "MyProgram.exe";
		argc++;

		{
			workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Brighness"), fBrightness);
			workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("RedScale"), fRedScale);
			workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlueScale"), fBlueScale);

			fGreenScale = fBrightness;
			fRedScale   *= fBrightness;
			fBlueScale  *= fBrightness;

			//bSuperPixels = IsSuperPixels();
			//bRawBayer    = IsRawBayer();
			//bBilinear	 = !bSuperPixels && !bRawBayer && IsRawBilinear();
			//bAHD		 = !bSuperPixels && !bRawBayer && IsRawAHD();

			bValue = FALSE;
			workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("AutoWB"), bValue);
			if (bValue)
			{
				// Automatic WB
				argv[argc] = "-a";
				argc++;
			};

			bValue = FALSE;
			workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("CameraWB"), bValue);
			if (bValue)
			{
				// Camera WB (is possible)
				argv[argc] = "-w";
				argc++;
			};

			if (!m_bColorRAW)
			{
				// Document mode
				argv[argc] = "-d";
				argc++;
			};
		};
		
		// No pixel resize
		argv[argc] = "-j";
		argc++;

		// No flip
		argv[argc] = "-t";
		argc++;

		argv[argc] = "0";
		argc++;

		// Output color space : raw-> sRGB (default)
		/*
		argv[argc] = _T("-o");
		argc++;
		argv[argc] = _T("0");
		argc++;*/

		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlackPointTo0"), bBlackPointTo0);
		if (bBlackPointTo0)
		{
			// Set black point to 0
			argv[argc] = "-k";
			argc++;
			argv[argc] = "0";
			argc++;
		};

		// Output is 16 bits
		argv[argc] = "-4";
		argc++;

		strcpy(g_szInputFileName, (LPCSTR)CT2CA(m_strFileName, CP_UTF8));
		g_Progress		   = pProgress;

		argv[argc] = g_szInputFileName;
		argc++;

		COutputFile *		pOutputFile;

		pOutputFile = new COutputFile(pBitmap, pProgress);
		pOutputFile->SetWhiteBalance(fRedScale, fGreenScale, fBlueScale);
		pOutputFile->SetCFAType(m_CFAType);

		g_pOutputFile = pOutputFile;

		InitGlobalVariables();
		bResult = !DCRawmain(argc, argv, (FILE*)(pOutputFile));

		g_szInputFileName[0] = 0;
		g_Progress = NULL;

		if (g_pOutputFile)
			delete g_pOutputFile;
		g_pOutputFile = NULL;
		if (g_pInputFile)
			delete g_pInputFile;
		g_pInputFile = NULL;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CRawDecod::IsRawFile()
{
	ZFUNCTRACE_RUNTIME();
	BOOL		bResult = FALSE;
	int			argc = 3;
	char *		argv[10];

	argv[0] = "MyProgram.exe";
	argv[1] = "-i";
	CStringA asciiFileName = (LPSTR)CT2A(m_strFileName, CP_UTF8);
	argv[2] = asciiFileName.GetBuffer(_MAX_PATH);

	g_szInputFileName[0] = 0;
	g_Progress		= NULL;
	g_pInputFile	= NULL;
	g_pOutputFile	= NULL;

	InitGlobalVariables();
	bResult = !DCRawmain(argc, argv, stdout);

	if (bResult)
	{
		m_strMake	= make;
		m_strModel	= model;
		m_lHeight	= iheight;
		m_lWidth	= iwidth;

		m_lISOSpeed = iso_speed;
		if (_finite(shutter))
			m_fExposureTime = shutter;
		else
			m_fExposureTime = 0;

		// Retrieve the Date/Time
		memset(&m_DateTime, 0, sizeof(m_DateTime));
		tm *		pdatetime;

		if (timestamp)
		{
			pdatetime = localtime(&timestamp);
			if (pdatetime)
			{
				m_DateTime.wDayOfWeek = pdatetime->tm_wday;
				m_DateTime.wDay = pdatetime->tm_mday;
				m_DateTime.wMonth = pdatetime->tm_mon+1;
				m_DateTime.wYear  = pdatetime->tm_year+1900;
				m_DateTime.wHour  = pdatetime->tm_hour;
				m_DateTime.wMinute= pdatetime->tm_min;
				m_DateTime.wSecond= pdatetime->tm_sec;
			};
		};

		m_bColorRAW	= is_foveon || !filters;
		m_CFAType	= GetCurrentCFAType();
	};

	if (g_pInputFile)
		delete g_pInputFile;
	g_pInputFile = NULL;
	if (g_pOutputFile)
		delete g_pOutputFile;
	g_pOutputFile = NULL;

	return bResult;
};

/* ------------------------------------------------------------------- */

inline COutputFile *	GetOutputFile(FILE * stream)
{
	if ((LONG_PTR)stream == (LONG_PTR)g_pOutputFile)
		return g_pOutputFile;
	else
		return NULL;
};

/* ------------------------------------------------------------------- */

inline CInputFile *	GetInputFile(FILE * stream)
{
	if ((LONG_PTR)stream == (LONG_PTR)g_pInputFile)
		return g_pInputFile;
	else
		return NULL;
};

/* ------------------------------------------------------------------- */

extern "C" int PRINTF( const char *format,...)
{
	return 0;
};

/* ------------------------------------------------------------------- */

extern "C" int FPRINTF( FILE *stream, const char *format,...)
{
	va_list					marker;
	va_start(marker, format);

	int						nResult = 0;
	COutputFile *			pOutputFile = GetOutputFile(stream);

	if (pOutputFile)
	{
		nResult = pOutputFile->Printf(format, marker);
	}
	else if ((stream != stdout) && (stream != stderr))
		nResult = vfprintf(stream, format, marker);

	va_end(marker);

	return nResult;
};

/* ------------------------------------------------------------------- */

extern "C" size_t FWRITE( const void *buffer, size_t size, size_t count, FILE *stream )
{
	COutputFile *			pOutputFile = GetOutputFile(stream);;

	if (pOutputFile)
	{
		return pOutputFile->Write(buffer, size, count);
	}
	else
		return fwrite(buffer, size, count, stream);
};

/* ------------------------------------------------------------------- */

extern "C" int FILENO( FILE *stream )
{
	COutputFile *			pOutputFile = GetOutputFile(stream);;
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pOutputFile || pInputFile)
	{
		return 120;
	}
	else
		return _fileno(stream);
};

/* ------------------------------------------------------------------- */

extern "C" FILE * FOPEN( const char *filename, const char *mode )
{
	CStringA			strMode = mode;

	if (strMode == "wb")
	{
		// Create a COutputFile object
		//COutputFile *		pOutputFile;

		//pOutputFile = new COutputFile();

		return (FILE*)NULL; //(pOutputFile);
	}
	else if (strMode == "rb")
	{
		// Create a CInputFile objet if the filename matches
		CStringA			strFileName  = filename;
		if (!strFileName.CompareNoCase(g_szInputFileName))
		{
			CInputFile *	pInputFile;

			pInputFile = new CInputFile;
			pInputFile->m_hFile = fopen(filename, mode);

			if (pInputFile->m_hFile)
			{
				if (pInputFile->Init())
				{
					if (g_Progress)
						g_Progress->Start2(NULL, (LONG)(pInputFile->m_lFileSize));

					g_pInputFile = pInputFile;
					return (FILE*)pInputFile;
				}
				else
					return NULL;
			}
			else
			{
				delete pInputFile;
				return NULL;
			};
		}
		else
			return fopen(filename, mode);
	}
	else
		return fopen(filename, mode);
};

/* ------------------------------------------------------------------- */

extern "C" int FCLOSE( FILE *stream )
{
	COutputFile *			pOutputFile = GetOutputFile(stream);;
	CInputFile *			pInputFile = GetInputFile(stream);;

	if (pOutputFile)
	{
		g_pOutputFile = NULL;
		delete pOutputFile;
		return 0;
	}
	else if (pInputFile)
	{
		if (g_Progress)
			g_Progress->End2();
		g_Progress = NULL;

		g_pInputFile = NULL;
		delete pInputFile;
		return 0;
	}
	else
		return fclose(stream);
};

/* ------------------------------------------------------------------- */

extern "C" size_t FREAD( void *buffer, size_t size, size_t count, FILE *stream )
{
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		size_t			result;

		result = pInputFile->fread(buffer, size, count);

		pInputFile->UpdatePos();
		return result;
	}
	else
		return fread(buffer, size, count, stream);
};

/* ------------------------------------------------------------------- */

extern "C" int FGETC( FILE *stream )
{
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		int				result;

		result = pInputFile->fgetc();
		pInputFile->UpdatePos();

		return result;
	}
	else
		return fgetc(stream);
}

/* ------------------------------------------------------------------- */

extern "C" int FSEEK( FILE *stream, long offset, int origin )
{
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		int				result;

		result = pInputFile->fseek(offset, origin);

		return result;
	}
	else
		return fseek(stream, offset, origin);
};

/* ------------------------------------------------------------------- */

extern "C" long FTELL( FILE *stream )
{
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		long			result;

		result = pInputFile->ftell();

		return result;
	}
	else
		return ftell(stream);
};

/* ------------------------------------------------------------------- */

extern "C" int FSCANF( FILE *stream, const char *format, ...)
{
	va_list					marker;
	va_start(marker, format);

	int						nResult;
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		nResult = pInputFile->fscanf(format, marker);
		pInputFile->UpdatePos();
	}
	else
		nResult = fscanf(stream, format, marker);

	va_end(marker);

	return nResult;
};

/* ------------------------------------------------------------------- */

extern "C" char *FGETS( char *string, int n, FILE *stream )
{
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		char *				result;

		result = pInputFile->fgets(string, n);
		pInputFile->UpdatePos();
		return result;
	}
	else
		return fgets(string, n, stream);
};

extern "C" int GETC(FILE * stream)
{
	CInputFile *			pInputFile = GetInputFile(stream);

	if (pInputFile)
	{
		int				result;

		result = pInputFile->fgetc();
		pInputFile->UpdatePos();
		return result;
	}
	else
		return getc(stream);
};

/* ------------------------------------------------------------------- */

BOOL	IsRAWPicture(LPCTSTR szFileName, CString & strModel)
{
	BOOL			bResult = FALSE;
	CRawDecod		dcr(szFileName);

	bResult = dcr.IsRawFile();

	if (bResult)
		dcr.GetModel(strModel);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	IsRAWPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	BOOL			bResult = FALSE;
	BOOL			bIsTiff = FALSE;
	TCHAR			szExt[1+_MAX_EXT];
	CString			strExt;

	// Check the extension - a tiff of tif file is not to be
	// considered as a RAW file
	_tsplitpath(szFileName, NULL, NULL, NULL, szExt);
	strExt = szExt;
	strExt.MakeUpper();
	if ((strExt == _T(".TIF")) || (strExt == _T(".TIFF")))
		bIsTiff = TRUE;

	if (!bIsTiff)
	{
		CRawDecod		dcr(szFileName);

		bResult = dcr.IsRawFile();

		if (bResult)
		{
			BitmapInfo.m_strFileName	 = szFileName;
			BitmapInfo.m_strFileType	 = "RAW";
			if (dcr.IsColorRAW())
				BitmapInfo.m_CFAType	 = CFATYPE_NONE;
			else
				BitmapInfo.m_CFAType	 = dcr.GetCFAType();
			BitmapInfo.m_lWidth			 = dcr.Width();
			BitmapInfo.m_lHeight		 = dcr.Height();
			BitmapInfo.m_lBitPerChannel  = 16;
			BitmapInfo.m_lNrChannels	 = dcr.IsColorRAW() ? 3 : 1;
			BitmapInfo.m_bCanLoad		 = TRUE;
			dcr.GetModel(BitmapInfo.m_strModel);
			BitmapInfo.m_lISOSpeed		 = dcr.GetISOSpeed();
			BitmapInfo.m_fExposure		 = dcr.GetExposureTime();
			BitmapInfo.m_DateTime		 = dcr.GetDateTime();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	LoadRAWPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	BOOL			bResult = FALSE;
	CRawDecod		dcr(szFileName);

	if (dcr.IsRawFile() && ppBitmap)
	{
		BOOL						bColorRAW;
		CSmartPtr<CMemoryBitmap>	pBitmap;

		bColorRAW = dcr.IsColorRAW();

		if ((IsSuperPixels() || IsRawBayer() || IsRawBilinear() || IsRawAHD()) && !bColorRAW)
		{
			pBitmap.Attach(new C16BitGrayBitmap);	
			ZTRACE_RUNTIME("Creating 16 bit gray memory bitmap %p (%s)", pBitmap.m_p, szFileName);
		}
		else
		{
			pBitmap.Attach(new C48BitColorBitmap);
			ZTRACE_RUNTIME("Creating 16 bit RGB memory bitmap %p (%s)", pBitmap.m_p, szFileName);
		};

		bResult = dcr.LoadRawFile(pBitmap, pProgress);

		if (bResult)
		{
			C16BitGrayBitmap *	pGrayBitmap;

			pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(pBitmap.m_p);
			if (pGrayBitmap)
			{
				if (IsSuperPixels())
					pGrayBitmap->UseSuperPixels(TRUE);
				else if (IsRawBayer())
					pGrayBitmap->UseRawBayer(TRUE);
				else if (IsRawBilinear())
					pGrayBitmap->UseBilinear(TRUE);
				else if (IsRawAHD())
					pGrayBitmap->UseAHD(TRUE);
			};
			pBitmap.CopyTo(ppBitmap);
		};
	};

	return bResult;
};
