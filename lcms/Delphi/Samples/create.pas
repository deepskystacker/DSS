
PROGRAM CreateProfile;
USES lcmsdll, Windows;


VAR
        Primaries:  cmsCIExyYTRIPLE;
        WhitePoint: cmsCIExyY;
        GammaTables: ARRAY [1..3] OF LPGAMMATABLE;
        TheProfile: cmsHPROFILE;


BEGIN

        WriteLn('Creation of profile TEST.ICM...');

        {Fill in primaries (Rec709 as example) }

        Primaries.Red.x := 0.6400;
        Primaries.Red.y := 0.3300;
        Primaries.Red.YY := 1.0;

        Primaries.Green.x := 0.3000;
        Primaries.Green.y := 0.6000;
        Primaries.Green.YY := 1.0;

        Primaries.Blue.x := 0.1500;
        Primaries.Blue.y := 0.0600;
        Primaries.Blue.YY := 1.0;


        {Calculates white point from temperature}
        {That is, 6504§K = D65                  }

        cmsWhitePointFromTemp(6504, @WhitePoint);



        { Compute gamma tables of 2.2 }

        GammaTables[1] := cmsBuildGamma(256, 2.2);
        GammaTables[2] := cmsBuildGamma(256, 2.2);
        GammaTables[3] := cmsBuildGamma(256, 2.2);


        { Does create the profile }

        TheProfile := cmsCreateRGBProfile(@WhitePoint, @Primaries, GammaTables);



        _cmsAddTextTag(TheProfile, $63707274, 'copyright (c) you');
        _cmsAddTextTag(TheProfile, $646D6E64, 'Manufacturer');
        _cmsAddTextTag(TheProfile, $646D6464, 'Model');


        { Save the profile to a file }

        _cmsSaveProfile(TheProfile, 'TEST.ICM');

        WriteLn('Profile saved');

        { Free the gamma tables }

        cmsFreeGamma(GammaTables[1]);
        cmsFreeGamma(GammaTables[2]);
        cmsFreeGamma(GammaTables[3]);

        { And close the handle }

        cmsCloseProfile(TheProfile)
END.

