program TestFormatting;
uses lcmsdll;

type
     RGB16 = Packed record
                r, g, b : Word
     END;

var
     InputProfile, OutputProfile : cmsHPROFILE;
     Transform1, Transform2: cmsHTRANSFORM;
     InRGB16, OutRGB16: RGB16;    
     OutLab: cmsCIELab;


procedure Mult257(var n: Word);
 begin
        n := n * 257
 end;


function Div257(n: Word) : Real;
begin
        Div257 := n / 257.
end;


begin

       InputProfile  := cmsCreate_sRGBProfile();
       OutputProfile := cmsCreateLabProfile(NIL);


       Transform1 := cmsCreateTransform(InputProfile,
                                           TYPE_RGB_16,
                                           OutputProfile,
                                           TYPE_Lab_DBL,
                                           INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC);

       Transform2 := cmsCreateTransform(OutputProfile,
                                           TYPE_Lab_DBL,
                                           InputProfile,
                                           TYPE_RGB_16,
                                           INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC);

      REPEAT

        Write('R?'); ReadLn(InRGB16.r);
        Write('G?'); ReadLn(InRGB16.g);
        Write('B?'); ReadLn(InRGB16.b);


        { Expand to 16 bits }

        Mult257(InRGB16.r);
        Mult257(InRGB16.g);
        Mult257(InRGB16.b);


        { Forward }

        cmsDoTransform(Transform1,
                        @InRGB16,
                        @OutLab,
                        1);
    

        Write('L*='); WriteLn(OutLab.L:3:2);
        Write('a*='); WriteLn(OutLab.a:3:2);
        Write('b*='); WriteLn(OutLab.b:3:2);


        { Backwards }


        cmsDoTransform(Transform2,
                        @OutLab,
                        @OutRGB16,
                        1);

        Write('R='); WriteLn(Div257(OutRGB16.R) :3:2);
        Write('G='); WriteLn(Div257(OutRGB16.G) :3:2);
        Write('B='); WriteLn(Div257(OutRGB16.B) :3:2);



     UNTIL ((InRGB16.r = 0) and (InRGB16.g = 0) and (InRGB16.b = 0));


     cmsDeleteTransform(Transform1);
     cmsDeleteTransform(Transform2);
     cmsCloseProfile(InputProfile);
     cmsCloseProfile(OutputProfile)
END.

