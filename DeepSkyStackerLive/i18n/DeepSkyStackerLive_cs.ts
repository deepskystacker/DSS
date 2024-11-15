<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="cs_CZ">
<context>
    <name>DSS::ChartTab</name>
    <message>
        <location filename="../ui/ChartTab.ui" line="14"/>
        <source>Form</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="31"/>
        <location filename="../ChartTab.cpp" line="39"/>
        <source>Score</source>
        <comment>IDC_SCORE</comment>
        <translation>Skóre</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="47"/>
        <location filename="../ChartTab.cpp" line="46"/>
        <source>FWHM</source>
        <comment>IDC_FWHM</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="63"/>
        <location filename="../ChartTab.cpp" line="53"/>
        <source>#Stars</source>
        <comment>IDC_STARS</comment>
        <translation>Hvězdy</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="79"/>
        <source>dX/dY</source>
        <comment>IDC_OFFSET</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="95"/>
        <location filename="../ChartTab.cpp" line="77"/>
        <source>Angle</source>
        <comment>IDC_ANGLE</comment>
        <translation>Úhel</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="111"/>
        <location filename="../ChartTab.cpp" line="85"/>
        <source>Sky Background</source>
        <comment>IDC_SKYBACKGROUND</comment>
        <translation>Pozadí oblohy</translation>
    </message>
</context>
<context>
    <name>DSS::EmailSettings</name>
    <message>
        <location filename="../emailsettings.cpp" line="209"/>
        <source>Failed to connect to email server %1 (%2)!</source>
        <translation>Nepodařilo se připojit k e-mailovému serveru %1 (%2)!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="217"/>
        <source>Failed to login to email server as %1!</source>
        <translation>Nepodařilo se přihlásit k e-mailovému serveru jako %1!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="226"/>
        <source>Failed to send mail!</source>
        <translation>Odeslání pošty se nezdařilo!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="231"/>
        <source>Test email sent OK!</source>
        <translation>Zkušební e-mail odeslán OK!</translation>
    </message>
</context>
<context>
    <name>DSS::FileRegistrar</name>
    <message>
        <location filename="../fileregistrar.cpp" line="128"/>
        <source>-&gt; New file: %1
</source>
        <comment>IDS_LOG_NEWFILE</comment>
        <translation>-&gt; Nový soubor: %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="152"/>
        <source>Loading %1 bit/ch %2 light frame
%3</source>
        <comment>IDS_LOADRGBLIGHT</comment>
        <translation>Načítání %1 bit/k %2 aktivního snímku
%3</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="154"/>
        <source>Loading %1 bits gray %2 light frame
%3</source>
        <comment>IDS_LOADGRAYLIGHT</comment>
        <translation>Načítání %1 bitového %2 aktivního šedého snímku
%3</translation>
    </message>
    <message numerus="yes">
        <location filename="../fileregistrar.cpp" line="183"/>
        <source>Image %1 registered: %n star(s) detected - FWHM = %L2 - Score = %L3
</source>
        <comment>IDS_LOG_REGISTERRESULTS</comment>
        <translation>
            <numerusform>Snímky %1 zaregistrovány: zjištěna %n hvězdička - FWHM = %L2 - Kvalita = %L3
</numerusform>
            <numerusform>Snímky %1 zaregistrovány: byly zjištěny %n hvězdičky - FWHM = %L2 - Kvalita = %L3
</numerusform>
            <numerusform>Snímky %1 zaregistrovány: zjištěno %n hvězdiček - FWHM = %L2 - Kvalita = %L3
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="199"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>Upozornění: Obrázek %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="212"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>Obrázek %1 nelze stohovat (%2)
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="220"/>
        <source>Error loading file %1
</source>
        <comment>IDS_LOG_ERRORLOADINGFILE</comment>
        <translation>Chyba při načítání souboru %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="237"/>
        <location filename="../fileregistrar.cpp" line="288"/>
        <source>Score (%L1) is less than %L2</source>
        <comment>IDS_NOSTACK_SCORE</comment>
        <translation>Kvalita (%L1) je menší než %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="247"/>
        <location filename="../fileregistrar.cpp" line="298"/>
        <source>Star count(%L1) is less than %L2</source>
        <translation>Počet hvězd (%L1) je menší než %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="257"/>
        <location filename="../fileregistrar.cpp" line="308"/>
        <source>FWHM (%L1 pixels) is greater than %L2 pixels</source>
        <comment>IDS_NOSTACK_FWHM</comment>
        <translation>FWHM (%L1 bodů) je větší než %L2 bodů</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="267"/>
        <location filename="../fileregistrar.cpp" line="318"/>
        <source>Sky Background (%L1%) is greater than %L2%</source>
        <comment>IDS_NOSTACK_SKYBACKGROUND</comment>
        <translation>Pozadí oblohy (%L1%) je větší než %L2%</translation>
    </message>
</context>
<context>
    <name>DSS::FileStacker</name>
    <message>
        <location filename="../filestacker.cpp" line="266"/>
        <source>No transformation found from reference frame</source>
        <comment>IDS_NOSTACK_NOTRANSFORMATION</comment>
        <translation>Žádný převod z referenčního snímku nenalezen</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="237"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>Upozornění: Obrázek %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="272"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>Obrázek %1 nelze stohovat (%2)
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="290"/>
        <location filename="../filestacker.cpp" line="325"/>
        <source>dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels</source>
        <comment>IDS_NOSTACK_OFFSET</comment>
        <translation>dX (%L1 bodů) nebo dY (%L2 bodů) je větší než %L3 bodů</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="305"/>
        <location filename="../filestacker.cpp" line="344"/>
        <source>Angle (%L1°) is greater than %L2°</source>
        <comment>IDS_NOSTACK_ANGLE</comment>
        <translation>Úhel (%L1°) je větší než %L2°</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="373"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>Sloučený obraz byl uložen</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="398"/>
        <source>Image %1 has been added to the stack
</source>
        <translation>Obrázek %1 byl přidán do zásobníku
</translation>
    </message>
</context>
<context>
    <name>DSS::ImageView</name>
    <message>
        <location filename="../../Tools/imageview.cpp" line="58"/>
        <location filename="../../Tools/imageview.cpp" line="78"/>
        <source>Ctrl++ or Ctrl+= to zoom in
Ctrl+- to zoom out
 or use the mouse wheel to zoom
Ctrl+4 to toggle 4-Corners mode</source>
        <translation>Ctrl++ nebo Ctrl+= pro přiblížení
Ctrl+- pro oddálení
nebo použijte kolečko myši pro přiblížení
Ctrl+4 přepne režim 4 rohů</translation>
    </message>
</context>
<context>
    <name>DSS::ImageViewer</name>
    <message>
        <location filename="../imageviewer.cpp" line="208"/>
        <source>The stacked image will be saved as soon as possible</source>
        <comment>IDS_STACKEDIMAGEWILLBESAVED</comment>
        <translation>Sloučený obraz bude uložen co nejdříve</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="220"/>
        <source>You must select an output folder first.
Go to the Settings tab to select the Stacked Image Output Folder.</source>
        <comment>IDS_NOSTACKEDIMAGEFOLDER</comment>
        <translation>Nejprve musíte vybrat výstupní složku.
Jděte na záložku &apos;Nastavení&apos; a vyberte zde výstupní složku sloučeného obrazu.</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="230"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>Sloučený obraz byl uložen</translation>
    </message>
</context>
<context>
    <name>DSS::RestartMonitoring</name>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="14"/>
        <source>Restart Monitoring</source>
        <comment>IDD_RESTARTMONITORING</comment>
        <translation>Obnovení prohlížení</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="20"/>
        <source>Before restarting do you want to</source>
        <translation>Před obnovením chcete</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="44"/>
        <source>Create a new empty stacked image</source>
        <comment>IDC_CREATENEWSTACKEDIMAGE</comment>
        <translation>Vytvořit nový prázdný sloučený obraz</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="74"/>
        <location filename="../ui/RestartMonitoring.ui" line="161"/>
        <source>or</source>
        <translation>nebo</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="101"/>
        <source>Start from the current stacked image</source>
        <comment>IDC_STARTFROMCURRENTSTACKEDIMAGE</comment>
        <translation>Spustit aktuální sloučený obraz</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="131"/>
        <source>Drop all the pending images</source>
        <comment>IDC_DROPPENDINGIMAGES</comment>
        <translation>Vynechat všechny nezpracované snímk</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="188"/>
        <source>Use and process the pending images</source>
        <comment>IDC_USEPENDINGIMAGES</comment>
        <translation>Použít a zpracovat nezpracované snímky</translation>
    </message>
</context>
<context>
    <name>DSS::SettingsTab</name>
    <message>
        <location filename="../ui/SettingsTab.ui" line="14"/>
        <source>Form</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="65"/>
        <source>Stacking</source>
        <translation>Slučování</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="71"/>
        <source>Don&apos;t stack until at least</source>
        <comment>IDC_DONTSTACK</comment>
        <translation>Neslučovat, dokud není</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="91"/>
        <source>images are available</source>
        <translation>dostupných snímků</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="127"/>
        <source>Warn  Don&apos;t stack if...</source>
        <translation>Výstr.    Neslučovat, dokud...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="169"/>
        <source>... score is less than</source>
        <comment>IDC_DONTSTACK_SCORE</comment>
        <translation>...je kvalita menší než</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="237"/>
        <source>... star count is less than</source>
        <comment>IDC_DONTSTACK_STARS</comment>
        <translation>...počet hvězd je menší než</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="305"/>
        <source>... sky background is greater than</source>
        <comment>IDC_DONTSTACK_SKYBACKGROUND</comment>
        <translation>...pozadí oblohy je větší než</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="325"/>
        <source>%</source>
        <translation>%</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="380"/>
        <source>... FWHM is greater than</source>
        <comment>IDC_DONTSTACK_FWHM</comment>
        <translation>...FWHM je větší než</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="400"/>
        <location filename="../ui/SettingsTab.ui" line="475"/>
        <source>pixels</source>
        <translation>bodů</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="524"/>
        <source>... angle is greater than</source>
        <translation>...úhel je větší než</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="544"/>
        <source>°</source>
        <translation>°</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="564"/>
        <source>Move non-stackable files to the &apos;NonStackable&apos; sub-folder</source>
        <translation>Přesun nesloučených souborů do složky &apos;NonStackable&apos;</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="577"/>
        <source>(the &apos;NonStackable&apos; sub folder will be created if necessary)</source>
        <translation>(pokud to bude nezbytné, bude složka &apos;NonStackable&apos; vytvořena)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="605"/>
        <source>Warnings</source>
        <translation>Varování</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="641"/>
        <source>Sound</source>
        <comment>IDC_WARN_SOUND</comment>
        <translation>Zvuk</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="648"/>
        <source>Send email to:</source>
        <comment>IDC_WARN_EMAIL</comment>
        <translation>Poslat e-mail:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="835"/>
        <source>Filters</source>
        <comment>IDC_FILTERS</comment>
        <translation>Filtry</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="937"/>
        <source>Apply changes</source>
        <comment>IDC_APPLYCHANGES</comment>
        <translation>Použít změny</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="944"/>
        <source>Cancel changes</source>
        <comment>IDC_CANCELCHANGES</comment>
        <translation>Zrušit změny</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="611"/>
        <source>Flash Application</source>
        <comment>IDC_WARN_FLASH</comment>
        <translation>Použití blesku</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="665"/>
        <source>Create warning file in:</source>
        <translation>Vytvořit soubor varování v:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="759"/>
        <source>Output folder:</source>
        <translation>Výstupní složka:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="739"/>
        <source>images</source>
        <translation>snímcích</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="455"/>
        <source>... dX or dY is greater than</source>
        <comment>IDC_DONTSTACK_OFFSET</comment>
        <translation>...dX nebo dY je větší než</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="685"/>
        <source>Reset email count</source>
        <comment>IDC_RESETEMAILCOUNT</comment>
        <translation>Obnovit seznam e-mailů</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="713"/>
        <source>Options</source>
        <comment>IDC_OPTIONS</comment>
        <translation>Nastavení</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="719"/>
        <source>Save stacked image to file every</source>
        <comment>IDC_SAVESTACKEDIMAGE</comment>
        <translation>Uložit sloučené obrazy do souboru po každých</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="789"/>
        <source>The trace file is normally written to the DeepSkyStacker directory in the users
&quot;Documents&quot; directory, and is deleted on exit (unless the application crashes).
Tick this box to keep the trace file.</source>
        <translation>Trasovací soubor se normálně zapisuje do adresáře DeepSkyStacker v uživatelích
adresář &quot;Documents&quot; a je odstraněn při ukončení (pokud se aplikace nezhroutí).
Zaškrtnutím tohoto políčka uchováte trasovací soubor.</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="794"/>
        <source>Retain the trace file on exit</source>
        <translation>Ponechejte trasovací soubor při ukončení</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="804"/>
        <source>Reset</source>
        <translation>Resetovat</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="841"/>
        <source>Process only ...</source>
        <translation>Jen zpracování...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="848"/>
        <source>RAW images (CR2, NEF, ORF, DNG...)</source>
        <comment>IDC_PROCESS_RAW</comment>
        <translation>RAW obrazy (CR2, NEF, ORF, DNG...)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="855"/>
        <source>TIFF images</source>
        <comment>IDC_PROCESS_TIFF</comment>
        <translation>TIFF obrazy</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="875"/>
        <source>FITS images</source>
        <comment>IDC_PROCESS_FITS</comment>
        <translation>FITS obrazy</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="882"/>
        <source>Other images (JPEG, PNG)</source>
        <comment>IDC_PROCESS_OTHERS</comment>
        <translation>Jiné obrazy (JPEG, PNG)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="634"/>
        <location filename="../settingstab.cpp" line="87"/>
        <location filename="../settingstab.cpp" line="251"/>
        <location filename="../settingstab.cpp" line="405"/>
        <location filename="../settingstab.cpp" line="449"/>
        <source>Click here to set or change the email address</source>
        <comment>IDC_EMAIL</comment>
        <translation>Kliknutím sem nastavíte nebo změníte e-mailovou adresu</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="658"/>
        <location filename="../settingstab.cpp" line="88"/>
        <location filename="../settingstab.cpp" line="262"/>
        <location filename="../settingstab.cpp" line="464"/>
        <source>Click here to select the warning file folder</source>
        <comment>IDC_WARNINGFILEFOLDER</comment>
        <translation>Kliknutím sem vyberte složku se souborem varování</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="769"/>
        <location filename="../settingstab.cpp" line="89"/>
        <location filename="../settingstab.cpp" line="270"/>
        <location filename="../settingstab.cpp" line="435"/>
        <source>Click here to select the stacked image output folder</source>
        <comment>IDC_STACKEDOUTPUTFOLDER</comment>
        <translation>Kliknutím sem vyberte výstupní složku skládaných obrázků</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="356"/>
        <source>Select Warning File Folder</source>
        <comment>IDS_SELECTWARNINGFOLDER</comment>
        <translation>Výběr složky pro soubor varování</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="375"/>
        <source>Select Stacked Image Output Folder</source>
        <comment>IDS_SELECTSTACKEDFOLDER</comment>
        <translation>Výběr výstupní složky pro sloučený obraz</translation>
    </message>
</context>
<context>
    <name>DeepSkyStackerLive</name>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="159"/>
        <source>This beta version of DeepSkyStacker has expired
You can probably get another one or download the final release from the web site.</source>
        <translation>Platnost této beta verze DeepSkyStacker vypršela
Pravděpodobně můžete získat další nebo stáhnout finální verzi z webových stránek.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="458"/>
        <source>
How to use  DeepSkyStacker Live ?
</source>
        <comment>IDS_LOG_STARTING</comment>
        <translation>
Jak používat DeepSkyStacker Live?
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="461"/>
        <source>Step 1
Check the Settings tabs for all the stacking and warning settings

</source>
        <comment>IDS_LOG_STARTING_1</comment>
        <translation>Krok 1
Zkontrolujte nastavení slučování a nastavení výstrah

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="464"/>
        <source>Step 2
Click on the Monitor button to start monitoring the folder
When monitoring is active incoming images are only registered but not stacked.

</source>
        <comment>IDS_LOG_STARTING_2</comment>
        <translation>Krok 2
Klikněte na tlačítko &apos;Prohledat&apos; pro zahájení prohlížení složek
Zjištěné snímky budou zaregistrovány, ale nebudou sloučeny.

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="468"/>
        <source>Step 3
To start stacking the images click on the Stack button
At this point all the incoming (and all previously registered) images will be stacked.
</source>
        <comment>IDS_LOG_STARTING_3</comment>
        <translation>Krok 3
Pro zahájení slučování stlačte tlačítko &apos;Sloučit&apos;
Nově zjištěné (i dříve zaregistrované) snímky budou sloučeny.
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="472"/>
        <source>You can pause/restart the stacking process by clicking on the Stack button.
</source>
        <comment>IDS_LOG_STARTING_4</comment>
        <translation>Stlačením tlačítka &apos;Sloučit&apos; můžete slučování přerušit a opět spustit.
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="475"/>
        <source>To stop monitoring and stacking click on the Stop button.

</source>
        <comment>IDS_LOG_STARTING_5</comment>
        <translation>Kliknutím na tlačítko &apos;Stop&apos; můžete prohledávání a slučování zastavit.

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="528"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1213"/>
        <source>No stacked image</source>
        <comment>IDS_NOSTACKEDIMAGE</comment>
        <translation>Není sloučený obraz</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="529"/>
        <source>No image loaded</source>
        <comment>IDS_NOIMAGELOADED</comment>
        <translation>Není načtený obraz</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="583"/>
        <source>Status</source>
        <translation>Status</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="584"/>
        <source>File</source>
        <comment>IDS_COLUMN_FILE</comment>
        <translation>Soubor</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="585"/>
        <source>Exposure</source>
        <comment>IDS_COLUMN_EXPOSURE</comment>
        <translation>Expozice</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="586"/>
        <source>Aperture</source>
        <comment>IDS_COLUMN_APERTURE</comment>
        <translation>Světelnost</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="587"/>
        <source>Score</source>
        <comment>IDS_COLUMN_SCORE</comment>
        <translation>Skóre</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="588"/>
        <source>Quality</source>
        <comment>IDS_COLUMN_MEANQUALITY</comment>
        <translation>Kvalitní</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="589"/>
        <source>#Stars</source>
        <comment>IDS_COLUMN_STARS</comment>
        <translation>Hvězdy</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="590"/>
        <source>FWHM</source>
        <translation>FWHM</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="591"/>
        <source>dX</source>
        <comment>IDS_COLUMN_DX</comment>
        <translation>dX</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="592"/>
        <source>dY</source>
        <comment>IDS_COLUMN_DY</comment>
        <translation>dY</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="593"/>
        <source>Angle</source>
        <comment>IDS_COLUMN_ANGLE</comment>
        <translation>Úhel</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="594"/>
        <source>Date/Time</source>
        <comment>IDS_COLUMN_DATETIME</comment>
        <translation>Datum/Čas</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="595"/>
        <source>Size</source>
        <comment>IDS_COLUMN_SIZES</comment>
        <translation>Velikost</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="596"/>
        <source>CFA</source>
        <comment>IDS_COLUMN_CFA</comment>
        <translation>CFA</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="597"/>
        <source>Depth</source>
        <comment>IDS_COLUMN_DEPTH</comment>
        <translation>Barevná hloubka</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="598"/>
        <source>Info</source>
        <comment>IDS_COLUMN_INFOS</comment>
        <translation>Informace</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="599"/>
        <source>ISO/Gain</source>
        <comment>IDS_COLUMN_ISO_GAIN</comment>
        <translation>ISO/Gain</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="600"/>
        <source>Sky Background</source>
        <comment>IDS_COLUMN_SKYBACKGROUND</comment>
        <translation>Pozadí oblohy</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="703"/>
        <source>%1 is not a directory. Please select a valid directory.</source>
        <translation>%1 není adresář. Vyberte platný adresář.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="713"/>
        <source>Start monitoring folder %1
</source>
        <comment>IDS_LOG_STARTMONITORING</comment>
        <translation>Start prohlížení složky %1
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="726"/>
        <source>Stop monitoring folder %1</source>
        <comment>IDS_LOG_STOPMONITORING</comment>
        <translation>Ukončení prohlížení složky %1</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="737"/>
        <source>Stop Stacking files</source>
        <comment>IDS_LOG_STOPSTACKING</comment>
        <translation>Ukončeno slučování souborů</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="753"/>
        <source>DeepSkyStacker Help.chm</source>
        <comment>IDS_HELPFILE</comment>
        <translation>DeepSkyStacker Help.chm</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="858"/>
        <source>You cannot change the monitored folder while monitoring.</source>
        <comment>IDS_CANTCHANGEMONITOREDFOLDER</comment>
        <translation>Při prohlížení nelze měnit prohlíženou složku.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="870"/>
        <source>Select the folder to be monitored</source>
        <comment>IDS_SELECTMONITOREDFOLDER</comment>
        <translation>Výběr složky pro prohledávání</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="905"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1252"/>
        <source>Stacked</source>
        <comment>IDS_STATUS_STACKED</comment>
        <translation>Sloučeno</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="911"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1170"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1237"/>
        <source>Pending</source>
        <translation>Zbývající</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="912"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1243"/>
        <source>Registered</source>
        <comment>IDS_STATUS_REGISTERED</comment>
        <translation>Zaregistrováno</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1021"/>
        <source>Start Stacking files</source>
        <comment>IDS_LOG_STARTSTACKING</comment>
        <translation>Spuštěno slučování souborů</translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1086"/>
        <source>You have %n image(s) in the monitored folder.
Do you want to process them?</source>
        <comment>IDS_USEEXISTINGIMAGES</comment>
        <translation>
            <numerusform>Ve sledované složce máte %n obrázek.
Chcete to zpracovat?</numerusform>
            <numerusform>Ve sledované složce máte %n obrázky.
Chcete je zpracovat?</numerusform>
            <numerusform>Ve sledované složce máte %n obrázků.
Chcete je zpracovat?</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1097"/>
        <source>%n new file(s) found
</source>
        <comment>IDS_LOG_NEWFILESFOUND</comment>
        <translation>
            <numerusform>Nalezen %n nový soubor
</numerusform>
            <numerusform>Nalezeny %n nové soubory
</numerusform>
            <numerusform>Nalezeno %n nových souborů
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1224"/>
        <source>Click here to save the stacked image to file</source>
        <comment>IDS_SAVESTACKEDIMAGE</comment>
        <translation>Klikněte sem pro uložení sloučeného obrazu do souboru</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1240"/>
        <source>Loaded</source>
        <comment>IDS_STATUS_LOADED</comment>
        <translation>Načteno</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1246"/>
        <source>Stack delayed</source>
        <comment>IDS_STATUS_STACKDELAYED</comment>
        <translation>Sloučení odloženo</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1249"/>
        <source>Not stackable</source>
        <comment>IDS_STATUS_NOTSTACKABLE</comment>
        <translation>Nesloučeno</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1332"/>
        <source>No</source>
        <comment>IDS_NO</comment>
        <translation>Ne</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1334"/>
        <source>Yes</source>
        <comment>IDS_YES</comment>
        <translation>Ano</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1337"/>
        <source>RGB %1 bit/ch</source>
        <comment>IDS_FORMAT_RGB</comment>
        <translation>RGB %1 bit/k</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1339"/>
        <source>Gray %1 bit</source>
        <comment>IDS_FORMAT_GRAY</comment>
        <translation>Šedá %1 bit</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1351"/>
        <source>%1%</source>
        <translation>%1%</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1532"/>
        <source>Pending: %1 - Registered: %2 - Stacked: %3 - Total exposure time: %4</source>
        <translation>Zbývající: %1 - Registrováno: %2 - Sloučeno: %3 - Celkový expoziční čas: %4&quot;</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="49"/>
        <source>Monitoring</source>
        <comment>IDC_MONITORINGRECT</comment>
        <translation>Prohledání</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="56"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="62"/>
        <source>Click here to select the Monitored Folder</source>
        <comment>IDC_MONITOREDFOLDER</comment>
        <translation>Při prohledávání klikněte na vybranou složku</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="205"/>
        <source>Stacked Image</source>
        <comment>IDC_STACKEDIMAGE</comment>
        <translation>Sloučené obrazy</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="216"/>
        <source>Last Image</source>
        <comment>IDC_LASTIMAGE</comment>
        <translation>Poslední snímek</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="221"/>
        <source>Graphs</source>
        <comment>IDC_GRAPHS</comment>
        <translation>Zobrazení</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="235"/>
        <source>Image List</source>
        <comment>IDC_IMAGELIST</comment>
        <translation>Seznam snímků</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="259"/>
        <source>Log</source>
        <comment>IDC_LOGLIST</comment>
        <translation>Záznam</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="264"/>
        <source>Settings</source>
        <comment>IDC_WARNINGS</comment>
        <translation>Nastavení</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="280"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="283"/>
        <source>Monitor</source>
        <comment>IDC_MONITOR</comment>
        <translation>Prohledat</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="296"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="299"/>
        <source>Stack</source>
        <comment>IDC_STACK_DSSLIVE</comment>
        <translation>Sloučit</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="308"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="311"/>
        <source>Stop</source>
        <comment>IDC_STOP</comment>
        <translation>Stop</translation>
    </message>
</context>
<context>
    <name>EmailSettings</name>
    <message>
        <location filename="../ui/EmailSettings.ui" line="20"/>
        <source>Sent email settings</source>
        <translation>Nastavení adresáta</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="26"/>
        <source>To:</source>
        <translation>Komu:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="33"/>
        <source>Send an Email only for the first warning</source>
        <comment>IDC_SENDONCE</comment>
        <translation>Poslat e-mail jen po prvním varování</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="43"/>
        <source>Subject:</source>
        <translation>Předmět:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="66"/>
        <source>Email Account settings</source>
        <translation>Nastavení účtu</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="14"/>
        <source>Email Settings</source>
        <translation>Nastavení e-mail</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="72"/>
        <source>SMTP server:</source>
        <translation>SMTP server:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="82"/>
        <source>Port:</source>
        <translation>Účet:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="126"/>
        <source>Encryption:</source>
        <translation>Šifrování:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="134"/>
        <source>None</source>
        <translation>Žádný</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="139"/>
        <source>SSL/TLS</source>
        <translation>SSL/TLS</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="144"/>
        <source>STARTTLS</source>
        <translation>STARTTLS</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="152"/>
        <source>Account (From):</source>
        <translation>Účet (od):</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="165"/>
        <source>Password:</source>
        <translation>Heslo:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="179"/>
        <source>Show password</source>
        <translation>Zobrazit heslo</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="189"/>
        <source>Test</source>
        <comment>IDC_TEST</comment>
        <translation>Test</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../ui/ImageViewer.ui" line="89"/>
        <source>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Copy current picture to clipboard&lt;/a&gt;</source>
        <comment>IDC_COPYTOCLIPBOARD</comment>
        <translation>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Kopie aktuálního obrazu do schránky&lt;/a&gt;</translation>
    </message>
</context>
<context>
    <name>QLinearGradientCtrl</name>
    <message>
        <location filename="../../Tools/QLinearGradientCtrl.cpp" line="44"/>
        <source>After selecting a slider with the mouse, you can move it using the Up Arrow, Right Arrow, Down Arrow, Left Arrow, Page Up, and Page Down keys.</source>
        <translation>Po výběru posuvníku pomocí myši jej můžete přesunout pomocí kláves Šipka nahoru, Šipka doprava, Šipka dolů, Šipka doleva, Page Up a Page Down.</translation>
    </message>
</context>
</TS>
