


PROGRAM LcmsTest;
USES lcmsdll, Windows;

TYPE
        ScanLineType = Array[0..255] OF PACKED RECORD
                                             b, g, r: Byte;
                                           END;

VAR
        hInProfile, hOutProfile: cmsHPROFILE;
        hTransform: cmsHTRANSFORM;

        ScanLine, ScanLineTranslated: ScanLineType;
        r, g, b: Integer;


BEGIN


     hInProfile  := cmsOpenProfileFromFile('HPSJTW.ICM', 'r');
     hOutProfile := cmsOpenProfileFromFile('sRGB Color Space Profile.ICM', 'r');


     WriteLn('Input profile : ',  cmsTakeProductName(hInProfile));
     WriteLn('Output profile : ', cmsTakeProductName(hOutProfile));


     hTransform := cmsCreateTransform(hInProfile,
                                           TYPE_BGR_8,
                                           hOutProfile,
                                           TYPE_BGR_8,
                                           INTENT_PERCEPTUAL, 0);

     WriteLn('As speed test, I will transform all spectrum...');
     WriteLn('This implies an image of 255x255x255x3 = 47 MB!!!');
     WriteLn('Each number represents a chunk 255x255x3 = 190.5 Kb! processed');

     FOR r := 0 TO 255 DO
     BEGIN
        FOR g := 0 TO 255 DO
        BEGIN
                FOR b := 0 TO 255 DO
                BEGIN

                        ScanLine[b].r := r;
                        ScanLine[b].g := g;
                        ScanLine[b].b := b;
                END;

                cmsDoTransform(hTransform,
                                      @ScanLine,
                                      @ScanLineTranslated,
                                      255)

        END;

        Write(r:4, #$0d)
      END;
      WriteLn; WriteLn('Done!!!');



     cmsDeleteTransform(hTransform);
     cmsCloseProfile(hInProfile);
     cmsCloseProfile(hOutProfile);

END.

