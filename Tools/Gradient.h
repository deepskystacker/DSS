// Gradient.h: interface for the CGradient class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CPeg
{
public:
	CPeg()
	{
		id = uniqueID;
		uniqueID++;
		colour = 0x00000000;
		position = 0.0f;
	};
	CPeg(UINT newid)
	{
		id = newid;
		if (newid >= uniqueID)
			uniqueID = newid+1;
		colour = 0x00000000;
		position = 0.0f;
	};
	CPeg(const CPeg &src)
	{
		colour = src.colour;
		position = src.position;
		id = src.id;
	};
	CPeg& operator = (const CPeg &src)
	{
		colour = src.colour;
		position = src.position;
		id = src.id;
		return *this;
	};

	bool operator < (const CPeg &src)
	{
		return position < src.position;
	};

	void Serialize(CArchive &ar);
	const UINT GetID() const {return id;};

	COLORREF colour;
	float position;
	UINT id;
protected:
	static UINT uniqueID;
};

#define BACKGROUND -4
#define STARTPEG -3
#define ENDPEG -2
#define NONE -1

typedef COLORREF (__cdecl* InterpolateFn)(COLORREF first, COLORREF second, float position, float start, float end);

class CGradient : public CObject
{
public:
	CGradient();
	CGradient(CGradient const& gradient);
	virtual ~CGradient();

	CGradient& operator =(CGradient const& src);

	DECLARE_SERIAL(CGradient)

//----- Attributes -----//
	int GetPegCount() const;
	const CPeg GetPeg(int iIndex) const;
	int SetPeg(int iIndex, COLORREF crColour, float fPosition);
	int	SetPeg(int iIndex, float position);
	int SetPeg(int iIndex, CPeg peg);
	int	SetPeg(int iIndex, COLORREF crColour);
	int AddPeg(COLORREF crColour, float fPosition, LONG newid = -1);
	int AddPeg(CPeg const& peg);
	void RemovePeg(int iIndex);
	int IndexFromPos(float pos);

	void SetStartPegColour(const COLORREF crColour){m_StartPeg.colour = crColour;};
	COLORREF GetStartPegColour() const {return m_StartPeg.colour;};
	void SetEndPegColour(const COLORREF crColour) {m_EndPeg.colour = crColour;};
	COLORREF GetEndPegColour() const {return m_EndPeg.colour;};
	void SetBackgroundColour(const COLORREF crColour) {m_Background.colour = crColour;};
	COLORREF GetBackgroundColour() const {return m_Background.colour;};
	void SetUseBackground(const BOOL bUseBackground) {m_UseBackground = bUseBackground;};
	BOOL GetUseBackground() const {return m_UseBackground;};

	enum InterpolationMethod
	{
		Linear,
		FlatStart,
		FlatMid,
		FlatEnd,
		Cosine,
		HSLRedBlue,
		HSLBlueRed,
		HSLShortest,
		HSLLongest,
		Reverse
	};

	InterpolationMethod GetInterpolationMethod() const;
	void SetInterpolationMethod(const InterpolationMethod method);
	int GetQuantization() const;
	void SetQuantization(const int entries);

//----- Operations -----//
	void MakePalette(CPalette *lpPalette);
	void Make8BitPalette(RGBTRIPLE *lpPal);
	void MakeEntries(RGBTRIPLE *lpPal, int iEntryCount);

	COLORREF ColourFromPosition(float pos);

	void Serialize(CArchive &ar);
	int IndexFromId(UINT id);

//----- Internals -----//
protected:
	void SortPegs();

	//----- Interpolation routines -----//
	static COLORREF InterpolateLinear(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateFlatStart(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateFlatMid(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateFlatEnd(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateCosine(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateHSLClockwise(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateHSLAntiClockwise(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateHSLLongest(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateHSLShortest(COLORREF first, COLORREF second,
		float position, float start, float end);
	static COLORREF InterpolateReverse(COLORREF first, COLORREF second,
		float position, float start, float end);
private:
	void InsertSort(int lb, int ub);
	int Partition(int lb, int ub);
	void QuickSort(int lb, int ub);

protected:
	InterpolateFn GetInterpolationProc();
	POSITION GetNextPeg(POSITION current);

	std::vector<CPeg>		pegs;
	CPeg					m_StartPeg,
							m_EndPeg,
							m_Background;
	BOOL m_UseBackground;
	int m_Quantization;
	InterpolationMethod m_InterpolationMethod;
};
