PROGRAM GamutChk;

USES lcmsdll, Windows;


VAR
        hLabProfile, hDisplayProfile, hProof: cmsHPROFILE;
        ProofTransform : cmsHTRANSFORM;
        Lab: cmsCIELab;
        wRGB: ARRAY [0..2] OF Byte;

BEGIN
     WriteLn('Gamut check test');

     hLabProfile     := cmsCreateLabProfile(NIL);
     hDisplayProfile := cmsCreate_sRGBProfile();
     hProof          := cmsOpenProfileFromFile('test.icm', 'r');


    ProofTransform:=cmsCreateProofingTransform(hLabProfile,
                        TYPE_Lab_DBL,
                        hDisplayProfile,
                        TYPE_RGB_8,
                        hProof,
                        INTENT_RELATIVE_COLORIMETRIC,
                        INTENT_ABSOLUTE_COLORIMETRIC,
			cmsFLAGS_SOFTPROOFING OR cmsFLAGS_GAMUTCHECK OR cmsFLAGS_NOTPRECALC);


    cmsSetAlarmCodes(255,255,255);


    Write('L=?'); ReadLn(Lab.L);
    Write('a=?'); ReadLn(Lab.a);
    Write('b=?'); ReadLn(Lab.b);


    cmsDoTransform(ProofTransform, @Lab, @wRGB, 1);

    WriteLn('R=', wRGB[0], ' G=', wRGB[1], ' B=', wRGB[2]);

    cmsCloseProfile(hLabProfile);
    cmsCloseProfile(hDisplayProfile);
    cmsCloseProfile(hProof);
    cmsDeleteTransform(ProofTransform);

END.



