/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FontInfo.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef FONTINFO_H
#define FONTINFO_H

extern bool DoesFontExist( const StringClass &strFontName, BYTE cCharSet );
extern bool IsFontFixed( const StringClass &strFontName, BYTE cCharSet );

#endif //FONTINFO_H