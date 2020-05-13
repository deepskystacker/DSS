<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="cs_CZ">
<context>
    <name>About</name>
    <message>
        <location filename="../ui/About.ui" line="14"/>
        <source>About DeepSkyStacker</source>
        <translation>O programu DeepSkyStacker</translation>
    </message>
    <message>
        <location filename="../ui/About.ui" line="36"/>
        <source>Language</source>
        <translation>Jazyk</translation>
    </message>
    <message>
        <location filename="../ui/About.ui" line="44"/>
        <source>Use the following language at next startup</source>
        <translation>Při dalším spuštění programu bude použit jazyk</translation>
    </message>
    <message>
        <location filename="../ui/About.ui" line="105"/>
        <source>Check if newer version is available at startup</source>
        <translation>Při spuštění ověřit, zda není novější verze</translation>
    </message>
    <message>
        <location filename="../About.cpp" line="23"/>
        <source>DeepSkyStacker version %1</source>
        <translation>DeepSkyStacker verze %1</translation>
    </message>
    <message>
        <location filename="../About.cpp" line="31"/>
        <source>RAW file decoding by LibRaw (version %1)
Copyright © 1997-2019 LibRaw LLC</source>
        <translation>RAW soubory dekódovány pomocí LibRaw (verze %1)
Copyright © 1997-2019 LibRaw LLC</translation>
    </message>
    <message>
        <location filename="../About.cpp" line="38"/>
        <source>TIFF file encoding/decoding by LibTIFF (version %1)
Copyright © 1988-1996 Sam Leffler
Copyright © 1991-1996 Silicon Graphics, Inc.</source>
        <translation>TIFF soubory kódovány/dekódovány pomocí LibTIFF (verze %1)
Copyright © 1988-1996 Sam Leffler
Copyright © 1991-1996 Silicon Graphics, Inc.</translation>
    </message>
    <message>
        <location filename="../About.cpp" line="45"/>
        <source>FITS decoding by CFitsIO (version %1)
Copyright NASA</source>
        <translation>FITS dekódovány pomocí CFitsIO (verze %1)
Copyright NASA</translation>
    </message>
    <message>
        <location filename="../About.cpp" line="53"/>
        <source>Default</source>
        <translation>Automaticky</translation>
    </message>
</context>
<context>
    <name>AlignmentParameters</name>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="14"/>
        <source>Alignment</source>
        <translation>Zarovnání</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="26"/>
        <source>Transformation used during alignment</source>
        <translation>Převod použitý při zarovnávání</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="37"/>
        <source>Automatic</source>
        <translation>Automatický</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="57"/>
        <source>Bilinear</source>
        <translation>Bilineární</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="77"/>
        <source>Bisquared</source>
        <translation>Bisquared</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="97"/>
        <source>Bicubic</source>
        <translation>Bikubický</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="117"/>
        <source>No alignment</source>
        <translation>Nezarovnáno</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="23"/>
        <source>Automatic

The alignment method is automatically selected depending on the number of available stars.</source>
        <translation>Automaticky

Automatické zarovnání vybere samočinně způsob zarovnání podle počtu dostupných hvězd.</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="26"/>
        <source>Bilinear Alignment

The Bilinear Alignment is used in all cases.</source>
        <translation>Bilineární zarovnání

Bilineární (dvojité lineární) zarovnání je použitelné ve všech případech.</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="29"/>
        <source>Bisquared Alignment

The Bisquared Alignment is used when at least 25 stars are available, else the Bilinear Alignment method is used.</source>
        <translation>Bisquared zarovnání

Bisquared (dvojité plošné) zarovnání se používá při dostupnosti nejméně 25 hvězd, jinak se používá bilineární metoda.</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="32"/>
        <source>Bicubic Alignment

The Bicubic method is used when at least 40 stars are available, then the Bisquared method is used if 25 to 39 stars are available, then the Bilinear method is used when less than 25 stars are available.</source>
        <translation>Bikubické zarovnání

Bikubické (dvojité prostorové )zarovnání se používá při dostupnosti nejméně 40 hvězd, pro 25 až 39 dostupných hvězd se používá metoda dvojitá plošná a pro méně jak 25 dostupných hvězd bilineární metoda.</translation>
    </message>
</context>
<context>
    <name>AskRegistering</name>
    <message>
        <location filename="../ui/AskRegistering.ui" line="14"/>
        <source>Unregistered light frame</source>
        <translation>Nezarovnaný aktivní snímek</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="37"/>
        <source>This light frame is not registered
(the stars have not been detected).

Do you want...</source>
        <translation>Tento aktivní snímek nebyl zarovnán
(nebyly zjištěny žádné hvězdy).

Chcete...</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="66"/>
        <source>to register this light frame</source>
        <translation>zarovnat tento aktivní snímek</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="73"/>
        <source>to register all the light frames</source>
        <translation>zarovnat všechny aktivní snímky</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="80"/>
        <source>to continue without registering the light frame</source>
        <translation>pokračovat bez zarovnání aktivního snímku</translation>
    </message>
</context>
<context>
    <name>BackgroundOptions</name>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="14"/>
        <source>Background Options</source>
        <translation>Nastavení pozadí</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="20"/>
        <source>Calibration Method</source>
        <translation>Způsob kalibrace</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="28"/>
        <source>Linear</source>
        <translation>Lineární</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="35"/>
        <source>Rational</source>
        <translation>Rozumná</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="60"/>
        <source>RGB Background Calibration Method</source>
        <translation>Způsob kalibrace RGB pozadí</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="68"/>
        <source>None</source>
        <translation>Žádná</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="75"/>
        <source>Minimum</source>
        <translation>Minimální</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="82"/>
        <source>Middle</source>
        <translation>Střední</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="89"/>
        <source>Maximum</source>
        <translation>Maximum</translation>
    </message>
</context>
<context>
    <name>BatchStacking</name>
    <message>
        <location filename="../ui/BatchStacking.ui" line="14"/>
        <source>Batch Stacking</source>
        <translation>Skupinové slučování</translation>
    </message>
    <message>
        <location filename="../ui/BatchStacking.ui" line="20"/>
        <source>This dialog is used to launch the stacking process from existing image lists.

Add the file lists you want to stack and click OK to start the stacking processes.</source>
        <translation>Toto okno se používá pro spuštění slučování z existujícího seznamu snímků.

Seznam můžete přidat a kliknutím na OK spustit slučování.</translation>
    </message>
    <message>
        <location filename="../ui/BatchStacking.ui" line="34"/>
        <source>Add File Lists...</source>
        <translation>Přidat seznam souborů...</translation>
    </message>
    <message>
        <location filename="../ui/BatchStacking.ui" line="54"/>
        <source>Clear List</source>
        <translation>Smazat seznam</translation>
    </message>
</context>
<context>
    <name>CheckAbove</name>
    <message>
        <location filename="../ui/CheckAbove.ui" line="16"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>CometStacking</name>
    <message>
        <location filename="../ui/CometStacking.ui" line="14"/>
        <source>Comet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="20"/>
        <source>Comet Stacking Parameters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="50"/>
        <source>Standard Stacking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="60"/>
        <source>Comet position is ignored.
The comet will be fuzzy.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="87"/>
        <source>Comet Stacking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="97"/>
        <source>Comet position is used.
The comet will be sharp and the stars will have trails.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="124"/>
        <source>Stars + Comet Stacking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="134"/>
        <source>Comet position is used.
The comet and the stars will be sharp. This process is twice longer.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="169"/>
        <source>:/comet/normal.bmp</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>DeepStackerDlg</name>
    <message>
        <location filename="../DeepStackerDlg.cpp" line="12"/>
        <source>DeepSkyStacker %1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>DropFilesDlg</name>
    <message>
        <location filename="../ui/DropFilesDlg.ui" line="16"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ExplorerBar</name>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="35"/>
        <source>Registering and Stacking</source>
        <translation>Zarovnání a sloučení</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="80"/>
        <source>Open picture files...</source>
        <translation>Otevřít obrazové soubory...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="90"/>
        <source>dark files...</source>
        <translation>temného pozadí...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="103"/>
        <source>flat files...</source>
        <translation>vyrovnání jasu...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="113"/>
        <source>dark flat files...</source>
        <translation>vyrovnání temného pozadí...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="123"/>
        <source>offset/bias files...</source>
        <translation>vyrovnání zkreslení...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="146"/>
        <source>Open a File List...</source>
        <translation>Otevřít seznam...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="153"/>
        <source>Save the FIle List...</source>
        <translation>Uložit seznam...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="176"/>
        <source>Clear List</source>
        <translation>Smazat seznam</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="207"/>
        <source>Check all</source>
        <translation>Zvolit vše</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="214"/>
        <source>Check above a threshold...</source>
        <translation>Volba dosažení prahu...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="221"/>
        <source>Uncheck all</source>
        <translation>Nic nezvolit</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="252"/>
        <source>Register checked pictures...</source>
        <translation>Zarovnat vybrané snímky...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="259"/>
        <source>Compute offsets...</source>
        <translation>Výpočet zkreslení...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="266"/>
        <source>Stack checked pictures...</source>
        <translation>Sloučit zvolené obrazy...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="273"/>
        <source>Batch stacking...</source>
        <translation>Skupinové sloučení...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="286"/>
        <source>Processing</source>
        <translation>Zpracování</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="307"/>
        <source>Open picture file...</source>
        <translation>Otevřít obrazový soubor...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="314"/>
        <source>Copy current picture to clipboard</source>
        <translation>Kopie aktuálního obrazu do schránky</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="321"/>
        <source>Create a Star Mask...</source>
        <translation>Vytvoření masky hvězd...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="328"/>
        <source>Save picture to file...</source>
        <translation>Uložit obraz do souboru...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="338"/>
        <source>Options</source>
        <translation>Nastavení</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="359"/>
        <source>Settings...</source>
        <translation>Nastavení...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="369"/>
        <source>Raw/FITS DDP Settings...</source>
        <translation>Nastavení Raw/FITS DDP...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="379"/>
        <source>Load...</source>
        <translation>Načíst...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="389"/>
        <source>Save...</source>
        <translation>Uložit...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="399"/>
        <source>Recommended...</source>
        <translation>Doporučení...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="406"/>
        <source>About DeepSkyStacker...</source>
        <translation>O programu DeepSkyStacker...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="413"/>
        <source>DeepSkyStacker&apos;s Help...</source>
        <translation>Nápověda programu DeepSkyStacker...</translation>
    </message>
    <message>
        <location filename="../ExplorerBar.cpp" line="61"/>
        <source>Register Settings...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ExplorerBar.cpp" line="62"/>
        <source>Stacking Settings...</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FitsFilesTab</name>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="14"/>
        <source>FITS Files</source>
        <translation>FITS soubory</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="35"/>
        <source>Monochrome 16 bit FITS Files are RAW files created by a DSLR or a color CCD camera</source>
        <translation>Jednobarevný 16ti bitový FITS a RAW soubor vytvořený DSLR nebo barevnou CCD kamerou</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="49"/>
        <source>Camera:</source>
        <translation>Kamera:</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="63"/>
        <source>Color Adjistment</source>
        <translation>Úprava barev</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="102"/>
        <source>Brightness</source>
        <translation>Jas</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="109"/>
        <source>Red scale</source>
        <translation>Červená stupnice</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="116"/>
        <source>Blue scale</source>
        <translation>Modrá stupnice</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="126"/>
        <source>Bayer Pattern Filter used</source>
        <translation>Použít Bayerův filtr</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="150"/>
        <source>Bayer Matrix Transformation</source>
        <translation>Bayerova převodní matice</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="159"/>
        <source>Bilinear Interpolation</source>
        <translation>Bilineární interpolace</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="179"/>
        <source>Adaptive Homogeneity-Directed (AHD) Interpolation</source>
        <translation>Adaptivní Homogeneity-Directed (AHD) interpolace</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="199"/>
        <source>Use Bayer Drizzle algorithm (no interpolation, no debayerization)</source>
        <translation>Použít Bayerův algoritmus (bez interpolace a odmaticování)</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="215"/>
        <source>This option uses the Bayer matrix as is. No interpolation is done and each pixel is given only primary components from the matrix.</source>
        <translation>Tato volba používá Bayerovu matici tak jak je. Neprovádí žádnou interpolaci a každý obrazový bod je součástí matice.</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="238"/>
        <source>Create super-pixels from the raw Bayer matrix (no interpolation)</source>
        <translation>Vytvoří super body přímo z RAW matice (bez interpolace)</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="254"/>
        <source>This option uses the Bayer matrix to create one super-pixel from each group of 4 pixels (RGBG). The sizes of the resulting image are thus divided by two.</source>
        <translation>Tato volba používá Bayerovu matici pro vytvoření jednoho super bodu ze skupiny 4 obrazových bodů (RGBG). Velikost výsledného obrazu je pak poloviční.</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="267"/>
        <source>Force use of unsigned values when signed values are stored</source>
        <translation>Nucené použití neoznačených hodnot, když označené hodnoty jsou uloženy</translation>
    </message>
</context>
<context>
    <name>IntermediateFiles</name>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="14"/>
        <source>Intermediate Files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="20"/>
        <source>Intermediate files creation settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="27"/>
        <source>Create a calibrated file for each light frame</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="37"/>
        <source>Save a debayered image when processing RAW images</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="60"/>
        <source>Create a registered/calibrated file for each light frame</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="80"/>
        <source>Intermediate and Final Image File Format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="86"/>
        <source>TIFF Files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="93"/>
        <source>FITS Files</source>
        <translation type="unfinished">FITS soubory</translation>
    </message>
</context>
<context>
    <name>OutputTab</name>
    <message>
        <location filename="../ui/OutputTab.ui" line="14"/>
        <source>Output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="20"/>
        <source>Output Files Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="29"/>
        <source>Create Output file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="36"/>
        <source>Create HTML Description file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="45"/>
        <source>Output File Name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="53"/>
        <source>Autosave.tif/fits</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="60"/>
        <source>&lt;file list name&gt;.tif/fits</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="71"/>
        <source>Append a number to avoid file overwrite (001, 002, ...)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="83"/>
        <source>Output Location</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="94"/>
        <source>Create Output file in the folder of the reference frame</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="108"/>
        <source>Create Output file in the folder of the file list</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="122"/>
        <source>Create Output file in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="138"/>
        <source>&lt;Output Folder&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>PostCalibration</name>
    <message>
        <location filename="../ui/PostCalibration.ui" line="14"/>
        <source>Cosmetic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="20"/>
        <source>Post Calibration Cosmetic settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="27"/>
        <source>Detect and Clean remaining Hot Pixels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="41"/>
        <location filename="../ui/PostCalibration.ui" line="114"/>
        <location filename="../ui/PostCalibration.ui" line="165"/>
        <location filename="../ui/PostCalibration.ui" line="232"/>
        <source>TextLabel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="65"/>
        <location filename="../ui/PostCalibration.ui" line="183"/>
        <source>&lt;small&gt;Effect weak&lt;/small&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="75"/>
        <location filename="../ui/PostCalibration.ui" line="193"/>
        <source>&lt;small&gt;Strong&lt;/small&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="87"/>
        <location filename="../ui/PostCalibration.ui" line="205"/>
        <source>Filter Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="107"/>
        <location filename="../ui/PostCalibration.ui" line="225"/>
        <source>Detection Threshold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="151"/>
        <source>Detect and Clean remaining Cold Pixels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="268"/>
        <source>Replace pixel value with</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="275"/>
        <source>Test on first frame...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="287"/>
        <source>Save an image showing the cleaned pixels for each light frame</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QPlatformTheme</name>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="18"/>
        <source>Cancel</source>
        <translation>Storno</translation>
    </message>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="19"/>
        <source>Apply</source>
        <translation>Použít</translation>
    </message>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="20"/>
        <source>&amp;Yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="21"/>
        <source>&amp;No</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="22"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
</context>
<context>
    <name>RawDDPSettings</name>
    <message>
        <location filename="../ui/RawDDPSettings.ui" line="14"/>
        <source>RAW/FITS Digital Development Process Settings</source>
        <translation>Nastavení RAW/FITS digitálního zpracování</translation>
    </message>
</context>
<context>
    <name>RawFilesTab</name>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="14"/>
        <source>RAW Files</source>
        <translation>RAW soubory</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="22"/>
        <source>Color Adjustment</source>
        <translation>Úprava barev</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="31"/>
        <source>Brightness</source>
        <translation>Jas</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="48"/>
        <source>Red Scale</source>
        <translation>Červená stupnice</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="65"/>
        <source>Blue Scale</source>
        <translation>Modrá stupnice</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="85"/>
        <source>White Balance</source>
        <translation>Vyvážení bílé</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="91"/>
        <source>No White Balance Processing</source>
        <translation>Žádné zpracování vyvážení bílé</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="98"/>
        <source>Use Camera White Balance</source>
        <translation>Použít vyvážení bílé z kamery</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="110"/>
        <source>Bayer Matrix Transformation</source>
        <translation>Bayerova převodní matice</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="116"/>
        <source>Bilinear Interpolation</source>
        <translation>Bilineární interpolace</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="136"/>
        <source>Adaptive Homogeneity-Directed (AHD) Interpolation</source>
        <translation>Adaptivní Homogeneity-Directed (AHD) interpolace</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="156"/>
        <source>Use Bayer Drizzle algorithm (no interpolation, no debayerization)</source>
        <translation>Použít Bayerův algoritmus (bez interpolace a odmaticování)</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="172"/>
        <source>This option uses the Bayer matrix as is. No interpolation is done and each pixel is given only primary components from the matrix.</source>
        <translation>Tato volba používá Bayerovu matici tak jak je. Neprovádí žádnou interpolaci a každý obrazový bod je součástí matice.</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="198"/>
        <source>Create super- pixels from the raw Bayer matrix (no interpolation)</source>
        <translation>Vytvoří super body přímo z RAW matice (bez interpolace)</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="214"/>
        <source>This option uses the Bayer matrix to create one super-pixel from each group of 4 pixels (RGBG). The sizes of the resulting image are thus divided by two.</source>
        <translation>Tato volba používá Bayerovu matici pro vytvoření jednoho super bodu ze skupiny 4 obrazových bodů (RGBG). Velikost výsledného obrazu je pak poloviční.</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="230"/>
        <source>Set the black point to 0</source>
        <translation>Nastavit černý bod na 0</translation>
    </message>
</context>
<context>
    <name>ResultParameters</name>
    <message>
        <location filename="../ui/ResultParameters.ui" line="14"/>
        <source>Result</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="42"/>
        <source>Standard Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="62"/>
        <source>&quot;Mosaic&quot; Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="82"/>
        <source>Intersection Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="102"/>
        <source>Custom Rectangle</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="132"/>
        <source>Enable 2x Drizzle</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="139"/>
        <source>Enable 3x Drizzle</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="159"/>
        <source>Align RGB Channels in final image</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>StackSettings</name>
    <message>
        <location filename="../ui/StackSettings.ui" line="14"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="41"/>
        <source>Temporary files folder:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="67"/>
        <source>...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="83"/>
        <source>Reduce worker threads priority</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="90"/>
        <source>Use all available processors</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StackSettings.cpp" line="22"/>
        <source>Light</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StackSettings.cpp" line="23"/>
        <source>Dark</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StackSettings.cpp" line="24"/>
        <source>Flat</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StackSettings.cpp" line="25"/>
        <source>Bias/Offset</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>StackingParameters</name>
    <message>
        <location filename="../ui/StackingParameters.ui" line="14"/>
        <source>Stacking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="26"/>
        <source>Stacking Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="36"/>
        <source>Average</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="56"/>
        <source>Median</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="63"/>
        <source>Median Kappa-Sigma clipping</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="70"/>
        <source>Kappa-Sigma clipping</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="77"/>
        <source>Entropy Weighted Average
(High Dynamic Range)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="98"/>
        <source>Maximum</source>
        <translation type="unfinished">Maximum</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="113"/>
        <source>Kappa:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="120"/>
        <source>Number of iterations:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="213"/>
        <source>Auto Adaptive Weighted Average</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>main</name>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="58"/>
        <source>Exception caught:

%1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="62"/>
        <source>Unknown exception caught</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
