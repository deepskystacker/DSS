// Gradient.cpp: implementation of the CGradient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gradient.h"

IMPLEMENT_SERIAL(CGradient, CObject, 0)

UINT CPeg::uniqueID = 250000;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradient::CGradient()
{
	m_StartPeg.colour = 0x00000000;
	m_StartPeg.position = 0.0f;
	m_EndPeg.colour = 0x00FFFFFF;
	m_EndPeg.position = 1.0f;
	m_Background.colour = 0x00000000; //A default pal
	m_Background.position = 0.0f;
	m_InterpolationMethod = Cosine;
	m_UseBackground = false;
	m_Quantization = -1;
}

CGradient::CGradient(CGradient const& src)
{
	m_StartPeg.colour = src.m_StartPeg.colour;
	m_EndPeg.colour = src.m_EndPeg.colour;
	m_Background.colour = src.m_Background.colour;
	m_InterpolationMethod = src.m_InterpolationMethod;
	m_UseBackground = src.m_UseBackground;
	m_Quantization = src.m_Quantization;

	pegs = src.pegs;
}

CGradient::~CGradient()
{
	pegs.clear();
}

CGradient& CGradient::operator =(CGradient const& src)
{
	pegs = src.pegs;

	m_StartPeg.colour = src.m_StartPeg.colour;
	m_EndPeg.colour = src.m_EndPeg.colour;
	m_Background.colour = src.m_Background.colour;
	m_InterpolationMethod = src.m_InterpolationMethod;
	m_UseBackground = src.m_UseBackground;
	m_Quantization = src.m_Quantization;

	return *this;
}

int CGradient::AddPeg(COLORREF crColour, float fPosition, LONG newid)
{
	CPeg peg;

	if(fPosition < 0)
		fPosition = 0;
	else if(fPosition > 1)
		fPosition = 1;

	peg.colour = crColour;
	peg.position = fPosition;
	if (newid >= 0)
		peg.id = newid;
	pegs.push_back(peg);
	SortPegs();

	return IndexFromId(peg.GetID());
}

int CGradient::AddPeg(CPeg const& peg)
{
	return AddPeg(peg.colour, peg.position);
}

//----- Assorted short functions -----//
void CGradient::RemovePeg([[maybe_unused]] int iIndex) {/*pegs.RemoveAt(iIndex);*/}
void CGradient::SortPegs() {std::sort(pegs.begin(), pegs.end());}
int CGradient::GetPegCount() const {return (int)pegs.size();}

const CPeg CGradient::GetPeg(int iIndex) const
{
	ASSERT(iIndex > -4 && iIndex != -1 && iIndex < GetPegCount());
		//You must pass a valid peg index or STARTPEG, ENDPEG, or BACKGROUND!

	if(iIndex >= 0)
	{
		const CPeg peg(pegs[iIndex]);
		return peg;
	}
	else if(iIndex == STARTPEG)
		return m_StartPeg;
	else if(iIndex == ENDPEG)
		return m_EndPeg;
	else
		return m_Background;
}

int CGradient::SetPeg(int iIndex, COLORREF crColour, float fPosition)
{
	UINT tempid;

	ASSERT(iIndex > -4 && iIndex != -1 && iIndex < GetPegCount());
		//You must pass a valid peg index or STARTPEG, ENDPEG, or BACKGROUND!

	if(fPosition < 0) fPosition = 0;
	else if(fPosition > 1) fPosition = 1;

	if(iIndex == STARTPEG)
		m_StartPeg.colour = crColour;
	else if(iIndex == ENDPEG)
		m_EndPeg.colour = crColour;
	else if(iIndex == BACKGROUND)
		m_Background.colour = crColour;
	else if (iIndex != NONE)
	{
		pegs[iIndex].colour = crColour;
		pegs[iIndex].position = fPosition;
		tempid = pegs[iIndex].GetID();
		SortPegs();
		return IndexFromId(tempid);
	}
	return -1;
}

int	CGradient::SetPeg(int iIndex, float position)
{
	CPeg			peg = GetPeg(iIndex);

	return SetPeg(iIndex, peg.colour, position);
};

int	CGradient::SetPeg(int iIndex, COLORREF crColour)
{
	CPeg			peg = GetPeg(iIndex);

	return SetPeg(iIndex, crColour, peg.position);
};

int CGradient::SetPeg(int iIndex, CPeg peg)
{
	UINT tempid;

	ASSERT(iIndex > -4 && iIndex != -1 && iIndex < GetPegCount());
		//You must pass a valid peg index or STARTPEG, ENDPEG, or BACKGROUND!

	if(peg.position < 0.0f) peg.position = 0.0f;
	else if(peg.position > 1.0f) peg.position = 1.0f;

	if(iIndex == STARTPEG)
		m_StartPeg.colour = peg.colour;
	else if(iIndex == ENDPEG)
		m_EndPeg.colour = peg.colour;
	else if(iIndex == BACKGROUND)
		m_Background.colour = peg.colour;
	else if (iIndex != NONE)
	{
		pegs[iIndex].colour = peg.colour;
		pegs[iIndex].position = peg.position;
		tempid = pegs[iIndex].GetID();
		SortPegs();
		return IndexFromId(tempid);
	}
	return -1;
}

void CGradient::Make8BitPalette(RGBTRIPLE *lpPal)
{
	MakeEntries(lpPal, 256);
}

void CGradient::MakePalette(CPalette *lpPal)
{
	RGBTRIPLE *entries = new RGBTRIPLE[256];
	LOGPALETTE *logpal;
	logpal = (LOGPALETTE*)malloc(2*sizeof(WORD) + 256*sizeof(PALETTEENTRY));

	lpPal->DeleteObject();

	Make8BitPalette(entries);

	logpal->palVersion = 0x300;
	logpal->palNumEntries = 256;

	for(int i = 0; i < 256; i++)
	{
		logpal->palPalEntry[i].peRed = entries[i].rgbtRed;
		logpal->palPalEntry[i].peGreen = entries[i].rgbtGreen;
		logpal->palPalEntry[i].peBlue = entries[i].rgbtBlue;
		logpal->palPalEntry[i].peFlags = PC_RESERVED;
	}

	delete[] entries;

	lpPal->CreatePalette(logpal);
	free(logpal);
}

void CGradient::MakeEntries(RGBTRIPLE *lpPal, int iEntryCount)
{
	float pos;
	COLORREF colour;

	ASSERT(iEntryCount > 1);
	ASSERT(iEntryCount < 65535);

	InterpolateFn Interpolate = GetInterpolationProc();
	ASSERT(Interpolate != nullptr);

	if(pegs.size() > 0)
	{
		//Some things are already constant and so can be found early
		float firstpegpos	= pegs[0].position;
		float lastpegpos	= pegs[pegs.size()-1].position;
		COLORREF lastpegcolour = pegs[pegs.size()-1].colour;
		int curpeg;

		for(int i = 0; i < iEntryCount; i++)
		{
			if(m_Quantization == -1)
				pos = (float)i/iEntryCount;
			else
				pos = ((float)(int)(((float)i/iEntryCount)*m_Quantization))/m_Quantization + 0.5f / m_Quantization;

			if(pos <= firstpegpos)
			{
				colour = Interpolate(m_StartPeg.colour, pegs[0].colour, pos, 0, firstpegpos);
				lpPal[i].rgbtRed = GetRValue(colour);
				lpPal[i].rgbtGreen = GetGValue(colour);
				lpPal[i].rgbtBlue = GetBValue(colour);
			}
			else if(pos > lastpegpos)
			{
				colour = Interpolate(lastpegcolour, m_EndPeg.colour, pos, lastpegpos, 1);
				lpPal[i].rgbtRed = GetRValue(colour);
				lpPal[i].rgbtGreen = GetGValue(colour);
				lpPal[i].rgbtBlue = GetBValue(colour);
			}
			else
			{
				curpeg = IndexFromPos(pos);
				if (curpeg >= 0)
				{
					colour = Interpolate(pegs[curpeg].colour, pegs[curpeg+1].colour, pos, pegs[curpeg].position, pegs[curpeg+1].position);
					lpPal[i].rgbtRed = GetRValue(colour);
					lpPal[i].rgbtGreen = GetGValue(colour);
					lpPal[i].rgbtBlue = GetBValue(colour);
				};
			}
		}
	}
	else
	{
		//When there are no extra peg we can just interpolate the start and end
		for(int i = 0; i < iEntryCount; i++)
		{
			if(m_Quantization == -1)
				pos = (float)i/iEntryCount;
			else
				pos = ((float)(int)(((float)i/iEntryCount)*m_Quantization))/m_Quantization + 0.5f / m_Quantization;

			colour = Interpolate(m_StartPeg.colour, m_EndPeg.colour, pos, 0, 1);
			lpPal[i].rgbtRed = GetRValue(colour);
			lpPal[i].rgbtGreen = GetGValue(colour);
			lpPal[i].rgbtBlue = GetBValue(colour);
		}
	}

	if(m_UseBackground)
	{
		lpPal[0].rgbtRed = GetRValue(m_Background.colour);
		lpPal[0].rgbtGreen = GetGValue(m_Background.colour);
		lpPal[0].rgbtBlue = GetBValue(m_Background.colour);
	}
}

COLORREF CGradient::InterpolateLinear(COLORREF first, COLORREF second, float position, float start, float end)
{
	if(start == end) return first;
	if(end - start == 0) return second;
	if(position == start) return first;
	if(position == end) return second;
	return RGB((BYTE)((GetRValue(second)*(position - start) + GetRValue(first)*(end-position))/(end-start)),
		(BYTE)((GetGValue(second)*(position - start) + GetGValue(first)*(end-position))/(end-start)),
		(BYTE)((GetBValue(second)*(position - start) + GetBValue(first)*(end-position))/(end-start)));
}

COLORREF CGradient::InterpolateReverse(COLORREF first, COLORREF second, float position, float start, float end)
{
	if(start == end) return first;
	if(end - start == 0) return second;
	if(position == start) return second;
	if(position == end) return first;
	return RGB((BYTE)((GetRValue(first)*(position - start) + GetRValue(second)*(end-position))/(end-start)),
		(BYTE)((GetGValue(first)*(position - start) + GetGValue(second)*(end-position))/(end-start)),
		(BYTE)((GetBValue(first)*(position - start) + GetBValue(second)*(end-position))/(end-start)));
}

COLORREF CGradient::InterpolateFlatStart(COLORREF first, COLORREF, float, float, float)
{return first;}

COLORREF CGradient::InterpolateFlatMid(COLORREF first, COLORREF second,	float, float, float)
{
	unsigned short sr, sg, sb, er, eg, eb;
	sr = GetRValue(first);
	sg = GetGValue(first);
	sb = GetBValue(first);
	er = GetRValue(second);
	eg = GetGValue(second);
	eb = GetBValue(second);

	return RGB((sr+er)/2, (sg+eg)/2, (sb+eb)/2);
}

COLORREF CGradient::InterpolateFlatEnd(COLORREF, COLORREF second, float, float, float)
{return second;}

COLORREF CGradient::InterpolateCosine(COLORREF first, COLORREF second, float position, float start, float end)
{
	float theta = (position-start)/(end-start) * 3.1415927f;
	float f = (1 - cosf(theta)) * .5f;

	return RGB((BYTE)(((float)GetRValue(first))*(1-f) + ((float)GetRValue(second))*f),
		(BYTE)(((float)GetGValue(first))*(1-f) + ((float)GetGValue(second))*f),
		(BYTE)(((float)GetBValue(first))*(1-f) + ((float)GetBValue(second))*f));
}

void RGB_to_HSL	(float r, float g, float b, float *h, float *s, float *l)
{
    float v;
    float m;
    float vm;
    float r2, g2, b2;

    v = max(r,g);
    v = max(v,b);
    m = min(r,g);
    m = min(m,b);

    if ((*l = (m + v) / 2.0f) <= 0.0f) return;
    if ((*s = vm = v - m) > 0.0f) {
		*s /= (*l <= 0.5f) ? (v + m ) :
			(2.0f - v - m) ;
    } else
	return;

    r2 = (v - r) / vm;
    g2 = (v - g) / vm;
    b2 = (v - b) / vm;

    if (r == v)
		*h = (g == m ? 5.0f + b2 : 1.0f - g2);
    else if (g == v)
		*h = (b == m ? 1.0f + r2 : 3.0f - b2);
    else
		*h = (r == m ? 3.0f + g2 : 5.0f - r2);

    	*h /= 6.0f;
}

void HSL_to_RGB(float h, float sl, float l, float *r, float *g, float *b)
{
    float v;

    v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);
    if (v <= 0) {
		*r = *g = *b = 0.0f;
    } else {
		float m;
		float sv;
		int sextant;
		float fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0f;
		sextant = (int)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) {
			case 0: *r = v; *g = mid1; *b = m; break;
			case 1: *r = mid2; *g = v; *b = m; break;
			case 2: *r = m; *g = v; *b = mid1; break;
			case 3: *r = m; *g = mid2; *b = v; break;
			case 4: *r = mid1; *g = m; *b = v; break;
			case 5: *r = v; *g = m; *b = mid2; break;
		}
    }
}

COLORREF CGradient::InterpolateHSLClockwise(COLORREF first, COLORREF second, float position, float start, float end)
{
	float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
	RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
		(float)GetBValue(first)/255.0f, &sh, &ss, &sl);
	RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
		(float)GetBValue(second)/255.0f, &eh, &es, &el);

	sh = sh - floorf(sh);
	eh = eh - floorf(eh);

	//Interpolate H clockwise
	if(eh >= sh) h = (eh*(position - start) + sh*(end-position))/(end-start);
	else h = ((eh + 1.0f)*(position - start) + sh*(end-position))/(end-start);
	h = (h>=1.0f)?h-1.0f:h;

	s = ((es*(position - start) + ss*(end-position))/(end-start));
	l = ((el*(position - start) + sl*(end-position))/(end-start));

	HSL_to_RGB(h, s, l, &r, &g, &b);
	return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
}

COLORREF CGradient::InterpolateHSLAntiClockwise(COLORREF first, COLORREF second, float position, float start, float end)
{
	float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
	RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
		(float)GetBValue(first)/255.0f, &sh, &ss, &sl);
	RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
		(float)GetBValue(second)/255.0f, &eh, &es, &el);

	sh = sh - floorf(sh);
	eh = eh - floorf(eh);

	//Interpolate H anticlockwise
	if(eh <= sh) h = (eh*(position - start) + sh*(end-position))/(end-start);
	else h = ((eh + 1.0f)*(position - start) + sh*(end-position))/(end-start);
	h = (h>=1.0f)?h-1.0f:h;

	s = ((es*(position - start) + ss*(end-position))/(end-start));
	l = ((el*(position - start) + sl*(end-position))/(end-start));

	HSL_to_RGB(h, s, l, &r, &g, &b);
	return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
}

COLORREF CGradient::InterpolateHSLLongest(COLORREF first, COLORREF second, float position, float start, float end)
{
	float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
	RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
		(float)GetBValue(first)/255.0f, &sh, &ss, &sl);
	RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
		(float)GetBValue(second)/255.0f, &eh, &es, &el);

	sh = sh - (float)floor(sh);
	eh = eh - (float)floor(eh);

	//Interpolate H short route
	if(((eh-sh)-floor(eh-sh) < 0.5f)?(eh < sh):(eh >= sh)) h = (eh*(position - start) + sh*(end-position))/(end-start);
	else h = ((eh+(sh>eh?1.0f:-1.0f))*(position - start) + sh*(end-position))/(end-start);
	//TRACE3("sh: %f eh: %f h: %f\n", sh, eh, h);

	h = h - floorf(h);

	s = ((es*(position - start) + ss*(end-position))/(end-start));
	l = ((el*(position - start) + sl*(end-position))/(end-start));

	HSL_to_RGB(h, s, l, &r, &g, &b);
	return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
}

COLORREF CGradient::InterpolateHSLShortest(COLORREF first, COLORREF second, float position, float start, float end)
{
	float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
	RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
		(float)GetBValue(first)/255.0f, &sh, &ss, &sl);
	RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
		(float)GetBValue(second)/255.0f, &eh, &es, &el);

	sh = sh - (float)floor(sh);
	eh = eh - (float)floor(eh);

	//Interpolate H short route
	if(((eh-sh)-floor(eh-sh) > 0.5f)?(eh < sh):(eh >= sh)) h = (eh*(position - start) + sh*(end-position))/(end-start);
	else h = ((eh+(sh>eh?1.0f:-1.0f))*(position - start) + sh*(end-position))/(end-start);
	//TRACE3("sh: %f eh: %f h: %f\n", sh, eh, h);

	h = h - floorf(h);

	s = ((es*(position - start) + ss*(end-position))/(end-start));
	l = ((el*(position - start) + sl*(end-position))/(end-start));

	HSL_to_RGB(h, s, l, &r, &g, &b);
	return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
}

int CGradient::IndexFromPos(float pos)
{
	ASSERT(pos >= 0.0f && pos <= 1.0f);
		// positions betwen 0 and 1!

	if(pos < pegs[0].position)
		return STARTPEG;

	for(int i = 0; i < pegs.size()-1; i++)
		if(pos >= pegs[i].position && pos <= pegs[i+1].position)
			return i;

	return -1; // Eh? somethings wrong here
}

int CGradient::IndexFromId(UINT id)
{
	for(int i = 0; i < pegs.size(); i++)
		if(id == pegs[i].GetID())
			return i;
	return -1;
}

//----- Fast sorting alogarithm functions -----//
int CGradient::Partition(int lb, int ub)
{
    CPeg temppeg, pivot;
    int i, j, p;

	/* select pivot and exchange with 1st element */
	p = lb + ((ub - lb)>>1);
    pivot = pegs[p];
    pegs[p] = pegs[lb];

    /* sort lb+1..ub based on pivot */
    i = lb+1;
    j = ub;
    while(1)
	{
        while(i < j && pivot.position >= pegs[i].position) i++;
        while(j >= i && pegs[j].position >= pivot.position) j--;
        if(i >= j) break;
        temppeg.colour = pegs[i].colour;
		temppeg.position = pegs[i].position;
        pegs[i].colour = pegs[j].colour;
        pegs[i].position = pegs[j].position;
        pegs[j].colour = temppeg.colour;
        pegs[j].position = temppeg.position;
        j--; i++;
    }

    /* pivot belongs in a[j] */
    pegs[lb] = pegs[j];
    pegs[j] = pivot;

    return j;
}

void CGradient::QuickSort(int lb, int ub)
{
    int m;

   /**************************
    *  sort array pegs[lb..ub]  *
    **************************/

    while (lb < ub)
	{
        /* quickly sort short lists */
        if (ub - lb <= 12)
		{
            InsertSort(lb, ub);
            return;
        }

        m = Partition(lb, ub);

        if (m - lb <= ub - m)
		{
            QuickSort(lb, m - 1);
            lb = m + 1;
        }
		else
		{
            QuickSort(m + 1, ub);
            ub = m - 1;
        }
    }
}

void CGradient::InsertSort(int lb, int ub)
{
    CPeg temppeg;

    for (int i = lb + 1; i <= ub; i++)
	{
		int			j;

        temppeg = pegs[i];
        for(j = i-1; j >= lb && pegs[j].position > temppeg.position; j--)
			pegs[j+1] = pegs[j];
		pegs[j+1] = temppeg;
    }
}

COLORREF CGradient::ColourFromPosition(float pos)
{
	if(pos < 0 || pos > 1)
	{
		ASSERT(0); // Position out of bounds!!
		return 0;
	}

	if(pos == 0 && m_UseBackground)
		return m_Background.colour;

	InterpolateFn Interpolate = GetInterpolationProc();
	ASSERT(Interpolate != nullptr);

	if(m_Quantization != -1)
		pos = ((float)(int)(pos*m_Quantization))/m_Quantization + 0.5f / m_Quantization;

	if(pegs.size() > 0)
	{
		//Some thing are aready constant and so can be found early
		float firstpegpos = pegs[0].position;
		float lastpegpos = pegs[pegs.size()-1].position;

		if(pos <= firstpegpos)
			return Interpolate(m_StartPeg.colour, pegs[0].colour, pos, 0,
				firstpegpos+1);
		else if(pos > lastpegpos)
			return Interpolate(pegs[pegs.size()-1].colour, m_EndPeg.colour,
				pos, lastpegpos, 1);
		else
		{
			int curpeg;
			curpeg = IndexFromPos(pos);
			return Interpolate(pegs[curpeg].colour, pegs[curpeg+1].colour, pos,
				pegs[curpeg].position, pegs[curpeg+1].position);
		}
	}
	else
	{
		//When there are no extra pegs we can just interpolate the start and end
		return Interpolate(m_StartPeg.colour, m_EndPeg.colour, pos, 0, 1);
	}
}

void CGradient::Serialize(CArchive &ar)
{
	if(ar.IsLoading())
	{
		int pegcount, temp;

		ar >> m_Background.colour;
		ar >> m_StartPeg.colour;
		ar >> m_EndPeg.colour;
		ar >> m_UseBackground;
		ar >> m_Quantization;
		ar >> temp;
		m_InterpolationMethod = (CGradient::InterpolationMethod)temp;

		ar >> pegcount;

		pegs.resize(pegcount);
		for(int i = 0; i < pegcount; i++)
			pegs[i].Serialize(ar);
	}
	else
	{
		ar << m_Background.colour;
		ar << m_StartPeg.colour;
		ar << m_EndPeg.colour;
		ar << (unsigned int)m_UseBackground;
		ar << m_Quantization;
		ar << (unsigned int)m_InterpolationMethod;

		ar << pegs.size();

		for(int i = 0; i < pegs.size(); i++)
			pegs[i].Serialize(ar);
	}
}

CGradient::InterpolationMethod CGradient::GetInterpolationMethod() const
{
	return m_InterpolationMethod;
}

void CGradient::SetInterpolationMethod(const InterpolationMethod method)
{
	m_InterpolationMethod = method;
}

int CGradient::GetQuantization() const
{
	return m_Quantization;
}

void CGradient::SetQuantization(const int entries)
{
	m_Quantization = entries;
}

void CPeg::Serialize(CArchive &ar)
{
	if(ar.IsLoading())
	{
		ar >> colour;
		ar >> position;
		id = uniqueID;
		uniqueID++;
	}
	else
	{
		ar << colour;
		ar << position;
	}
}

InterpolateFn CGradient::GetInterpolationProc()
{
	switch(m_InterpolationMethod)
	{
	case Linear: return InterpolateLinear;
	case FlatStart: return InterpolateFlatStart;
	case FlatMid: return InterpolateFlatMid;
	case FlatEnd: return InterpolateFlatEnd;
	case Cosine: return InterpolateCosine;
	case HSLRedBlue: return InterpolateHSLClockwise;
	case HSLBlueRed: return InterpolateHSLAntiClockwise;
	case HSLShortest: return InterpolateHSLShortest;
	case HSLLongest: return InterpolateHSLLongest;
	case Reverse: return InterpolateReverse;
	default: return 0;
	}
}
