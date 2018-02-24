program TestWhitePoint;
uses lcmsdll;

type
     RGB8 = Packed record
                r, g, b : Byte
     END;

     CMYK8 = Packed record
                c, m, y, k: Byte
             end;
var
     WhitePoint: cmsCIExyY;
     Primaries: cmsCIExyYTRIPLE;
     Gamma: Array [1..3] OF LPGAMMATABLE;
     InputProfile, OutputProfile : cmsHPROFILE;
     Transform: cmsHTRANSFORM;
     InRGB8: RGB8;
     OutCMYK8: CMYK8;


begin

       cmsWhitePointFromTemp(9300, @WhitePoint);

       Primaries.Red.x := 0.67;
       Primaries.Red.y := 0.33;
       Primaries.Red.YY := 1.0;
       Primaries.Green.x := 0.21;
       Primaries.Green.y := 0.71;
       Primaries.Green.YY := 1.0;
       Primaries.Blue.x := 0.14;
       Primaries.Blue.y := 0.08;
       Primaries.Blue.YY := 1.0;



       Gamma[1] := cmsBuildGamma(256, 2.0);
       Gamma[2] := cmsBuildGamma(256, 2.0);
       Gamma[3] := cmsBuildGamma(256, 2.0);

       InputProfile  := cmsCreateRGBProfile(@WhitePoint, @Primaries, Gamma);
       OutputProfile := cmsOpenProfileFromFile('CMYK.icm', 'r');


       WriteLn('Input profile : ',  cmsTakeProductName(InputProfile));
       WriteLn('Output profile : ', cmsTakeProductName(OutputProfile));

       Transform := cmsCreateTransform(InputProfile,
                                           TYPE_RGB_8,
                                           OutputProfile,
                                           TYPE_CMYK_8,
                                           INTENT_PERCEPTUAL, 0);

      REPEAT

        Write('R?'); ReadLn(InRGB8.r);
        Write('G?'); ReadLn(InRGB8.g);
        Write('B?'); ReadLn(InRGB8.b);

        cmsDoTransform(Transform,
                        @InRGB8,
                        @OutCMYK8,
                        1);

        Write('C='); WriteLn(OutCMYK8.c);
        Write('M='); WriteLn(OutCMYK8.m);
        Write('Y='); WriteLn(OutCMYK8.y);
        Write('K='); WriteLn(OutCMYK8.k);

     UNTIL ((InRGB8.r = 0) and (InRGB8.g = 0) and (InRGB8.b = 0));


     cmsDeleteTransform(Transform);
     cmsCloseProfile(InputProfile);
     cmsCloseProfile(OutputProfile)
END.

