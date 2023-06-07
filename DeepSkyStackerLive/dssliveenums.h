#pragma once
enum class ImageListColumns
{
	Status,
	File,
	Exposure,
	Aperture,
	Score,
	Stars,
	FWHM,
	dX,
	dY,
	Angle,
	DateTime,
	Size,
	CFA,
	Depth,
	Info,
	ISOGain,
	SkyBackground,
	ColumnCount
};

enum class ImageStatus
{
	pending,
	loaded,
	registered,
	stackDelayed,
	nonStackable,
	stacked
};

enum STACKIMAGEINFO : char
{
	II_SETREFERENCE = 0,
	II_DONTSTACK_NONE = 1,
	II_DONTSTACK_SCORE = 2,
	II_DONTSTACK_STARS = 3,
	II_DONTSTACK_FWHM = 4,
	II_DONTSTACK_DX = 5,
	II_DONTSTACK_DY = 6,
	II_DONTSTACK_ANGLE = 7,
	II_DONTSTACK_SKYBACKGROUND = 8,
	II_WARNING_NONE = 101,
	II_WARNING_SCORE = 102,
	II_WARNING_STARS = 103,
	II_WARNING_FWHM = 104,
	II_WARNING_DX = 105,
	II_WARNING_DY = 106,
	II_WARNING_ANGLE = 107,
	II_WARNING_SKYBACKGROUND = 108
};