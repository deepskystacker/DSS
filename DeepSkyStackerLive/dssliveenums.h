#pragma once
enum class ImageListColumns
{
	Status,
	File,
	Exposure,
	Aperture,
	Score,
	Quality,
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
	II_DONTSTACK_REGISTER = 1,
	II_DONTSTACK_STACK = 2,
	II_DONTSTACK_SCORE = 11,
	II_DONTSTACK_STARS = 12,
	II_DONTSTACK_FWHM = 13,
	II_DONTSTACK_DX = 14,
	II_DONTSTACK_DY = 15,
	II_DONTSTACK_ANGLE = 16,
	II_DONTSTACK_SKYBACKGROUND = 17,
	II_WARNING_NONE = 101,
	II_WARNING_SCORE = 102,
	II_WARNING_STARS = 103,
	II_WARNING_FWHM = 104,
	II_WARNING_DX = 105,
	II_WARNING_DY = 106,
	II_WARNING_ANGLE = 107,
	II_WARNING_SKYBACKGROUND = 108
};