program TestXYZ;
uses lcmsdll;


type
     TRGB8 = Packed record
                r, g, b : Byte
     END;

var

     InputProfile, OutputProfile : cmsHPROFILE;
     Transform: cmsHTRANSFORM;
     RGB8: TRGB8;
     XYZ: cmsCIEXYZ;

begin
     InputProfile   := cmsOpenProfileFromFile('profile.icm','r');
     OutputProfile  := cmsCreateXYZProfile();

     Transform := cmsCreateTransform(InputProfile,
                                           TYPE_RGB_8,
                                           OutputProfile,
                                           TYPE_XYZ_DBL,
                                           INTENT_ABSOLUTE_COLORIMETRIC, cmsFLAGS_NOTPRECALC);

     WriteLn('Enter RGB (0-255) or all 0 to end');
     REPEAT

        Write('R?'); ReadLn(RGB8.r);
        Write('G?'); ReadLn(RGB8.g);
        Write('B?'); ReadLn(RGB8.b);


        cmsDoTransform(Transform,
                        @RGB8,
                        @XYZ,
                        1);


        {Transport to radiance}

        XYZ.X := XYZ.X * 100.;
        XYZ.Y := XYZ.Y * 100.;
        XYZ.Z := XYZ.Z * 100.;


        WriteLn; WriteLn('XYZ (as viewed on D50 box)');

        Write('X='); WriteLn(XYZ.X:3:2);
        Write('Y='); WriteLn(XYZ.Y:3:2);
        Write('Z='); WriteLn(XYZ.Z:3:2);

     UNTIL ((RGB8.r = 0) and (RGB8.g = 0) and (RGB8.b = 0));


     cmsDeleteTransform(Transform);
     cmsCloseProfile(InputProfile);
     cmsCloseProfile(OutputProfile)

END.

