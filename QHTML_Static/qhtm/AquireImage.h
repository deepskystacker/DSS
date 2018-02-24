/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	AquireImage.h
Owner:	russf@gipsysoft.com
Purpose:	Function for handling default image acquisition
----------------------------------------------------------------------*/
#ifndef AQUIREIMAGE_H
#define AQUIREIMAGE_H

class CImage;

CImage *AquireImage( HINSTANCE hInstance, LPCTSTR pcszFilePath, LPCTSTR pcszFilename, bool bIsTransparent, COLORREF crForceTransparent );


#endif //AQUIREIMAGE_H