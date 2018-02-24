/*! \file WusDV_Util.cpp
	\brief File d'implementazione di alcune funzioni di utilità generale.*/

#include "stdafx.h"
#include <math.h>
#include "WusDV_Util.h"

float RoundTo(float value, short decimal)
{
	value *= (float)pow(10.0, decimal);
	value += .5f;
	value = (float)floor(value);
	value /= (float)pow(10.0, decimal);
	return value;
}
