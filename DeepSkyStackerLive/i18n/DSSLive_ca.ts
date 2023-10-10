<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ca_ES">
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
        <translation>Puntuació</translation>
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
        <translation># Estrelles</translation>
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
        <translation>Àngle</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="111"/>
        <location filename="../ChartTab.cpp" line="85"/>
        <source>Sky Background</source>
        <comment>IDC_SKYBACKGROUND</comment>
        <translation>Fons de cel</translation>
    </message>
</context>
<context>
    <name>DSS::EmailSettings</name>
    <message>
        <location filename="../emailsettings.cpp" line="209"/>
        <source>Failed to connect to email server %1 (%2)!</source>
        <translation>No s&apos;ha pogut connectar al servidor de correu electrònic %1 (%2)!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="217"/>
        <source>Failed to login to email server as %1!</source>
        <translation>No s&apos;ha pogut iniciar la sessió al servidor de correu electrònic com a %1!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="226"/>
        <source>Failed to send mail!</source>
        <translation>Error en enviar el correu!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="231"/>
        <source>Test email sent OK!</source>
        <translation>Correu electrònic de prova enviat bé!</translation>
    </message>
</context>
<context>
    <name>DSS::FileRegistrar</name>
    <message>
        <location filename="../fileregistrar.cpp" line="128"/>
        <source>-&gt; New file: %1
</source>
        <comment>IDS_LOG_NEWFILE</comment>
        <translation>-&gt; Nou arxiu: %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="152"/>
        <source>Loading %1 bit/ch %2 light frame
%3</source>
        <comment>IDS_LOADRGBLIGHT</comment>
        <translation>Carregant %1 bit/ch %2 light frame
%3</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="154"/>
        <source>Loading %1 bits gray %2 light frame
%3</source>
        <comment>IDS_LOADGRAYLIGHT</comment>
        <translation>Carregant %1 bits gray %2 light frame
%3</translation>
    </message>
    <message numerus="yes">
        <location filename="../fileregistrar.cpp" line="184"/>
        <source>Image %1 registered: %n star(s) detected - FWHM = %L2 - Score = %L3
</source>
        <comment>IDS_LOG_REGISTERRESULTS</comment>
        <translation>
            <numerusform>Imatge %1 registrada: %n estrella detectada - FWHM = %L2 - Puntuació = %L3
</numerusform>
            <numerusform>Imatge %1 registrada: %n estrelles detectades - FWHM = %L2 - Puntuació = %L3
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="200"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>Avís: Imatge %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="213"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>La imatge %1 no es pot apilar (%2)
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="221"/>
        <source>Error loading file %1
</source>
        <comment>IDS_LOG_ERRORLOADINGFILE</comment>
        <translation>S&apos;ha produït un error en carregar el fitxer %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="238"/>
        <location filename="../fileregistrar.cpp" line="289"/>
        <source>Score (%L1) is less than %L2</source>
        <comment>IDS_NOSTACK_SCORE</comment>
        <translation>La puntuació (%L1) és menys que %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="248"/>
        <location filename="../fileregistrar.cpp" line="299"/>
        <source>Star count(%L1) is less than %L2</source>
        <translation>El recompte d&apos;estrelles (%L1) és menor que %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="258"/>
        <location filename="../fileregistrar.cpp" line="309"/>
        <source>FWHM (%L1 pixels) is greater than %L2 pixels</source>
        <comment>IDS_NOSTACK_FWHM</comment>
        <translation>FWHM (%L1 píxels) és més gran que %L2 píxels</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="268"/>
        <location filename="../fileregistrar.cpp" line="319"/>
        <source>Sky Background (%L1%) is greater than %L2%</source>
        <comment>IDS_NOSTACK_SKYBACKGROUND</comment>
        <translation>El fons de cel (%L1%) és major que %L2%</translation>
    </message>
</context>
<context>
    <name>DSS::FileStacker</name>
    <message>
        <location filename="../filestacker.cpp" line="267"/>
        <source>No transformation found from reference frame</source>
        <comment>IDS_NOSTACK_NOTRANSFORMATION</comment>
        <translation>No s&apos;ha trobat cap transformació del marc de referència</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="238"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>Avís: Imatge %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="273"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>La imatge %1 no es pot apilar (%2)
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="291"/>
        <location filename="../filestacker.cpp" line="326"/>
        <source>dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels</source>
        <comment>IDS_NOSTACK_OFFSET</comment>
        <translation>dX (%L1 píxels) o dY (%L2) és més gran que %L3 píxels</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="306"/>
        <location filename="../filestacker.cpp" line="345"/>
        <source>Angle (%L1°) is greater than %L2°</source>
        <comment>IDS_NOSTACK_ANGLE</comment>
        <translation>L&apos;angle (%L1°) és més gran que %L2°</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="374"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>La imatge apilada ha estat guardada</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="400"/>
        <source>Image %1 has been added to the stack
</source>
        <translation>La imatge %1 s&apos;ha afegit a la pila
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
        <translation>Ctrl++ o Ctrl+= per ampliar
Ctrl+- per reduir la imatge
 o utilitzeu la roda del ratolí per fer zoom
Ctrl+4 per canviar el mode de 4 cantonades</translation>
    </message>
</context>
<context>
    <name>DSS::ImageViewer</name>
    <message>
        <location filename="../imageviewer.cpp" line="208"/>
        <source>The stacked image will be saved as soon as possible</source>
        <comment>IDS_STACKEDIMAGEWILLBESAVED</comment>
        <translation>La imatge apilada serà guardada tan aviat com sigui possible</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="220"/>
        <source>You must select an output folder first.
Go to the Settings tab to select the Stacked Image Output Folder.</source>
        <comment>IDS_NOSTACKEDIMAGEFOLDER</comment>
        <translation>Vostè ha de seleccionar un directori de sortida primer.
Vagi a les opcions d&apos;Ajustaments per a seleccionar el Directori de Sortida d&apos;Imatges Apilades.</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="230"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>La imatge apilada ha estat guardada</translation>
    </message>
</context>
<context>
    <name>DSS::RestartMonitoring</name>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="14"/>
        <source>Restart Monitoring</source>
        <comment>IDD_RESTARTMONITORING</comment>
        <translation>Reiniciar el supervisat</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="20"/>
        <source>Before restarting do you want to</source>
        <translation>Abans de reiniciar vostè vol</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="44"/>
        <source>Create a new empty stacked image</source>
        <comment>IDC_CREATENEWSTACKEDIMAGE</comment>
        <translation>Crear una nova imatge apilada buida</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="74"/>
        <location filename="../ui/RestartMonitoring.ui" line="161"/>
        <source>or</source>
        <translation>o</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="101"/>
        <source>Start from the current stacked image</source>
        <comment>IDC_STARTFROMCURRENTSTACKEDIMAGE</comment>
        <translation>Començar de la imatge apilada actual</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="131"/>
        <source>Drop all the pending images</source>
        <comment>IDC_DROPPENDINGIMAGES</comment>
        <translation>Deixar totes les imatges pendents</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="188"/>
        <source>Use and process the pending images</source>
        <comment>IDC_USEPENDINGIMAGES</comment>
        <translation>Usar i processar les imatges pendents</translation>
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
        <translation>Apilant</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="71"/>
        <source>Don&apos;t stack until at least</source>
        <comment>IDC_DONTSTACK</comment>
        <translation>No apilar fins almenys</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="91"/>
        <source>images are available</source>
        <translation>les imatges estan disponibles</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="127"/>
        <source>Warn  Don&apos;t stack if...</source>
        <translation>Avís     No apilar fins...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="169"/>
        <source>... score is less than</source>
        <comment>IDC_DONTSTACK_SCORE</comment>
        <translation>...el resultat és menys que</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="237"/>
        <source>... star count is less than</source>
        <comment>IDC_DONTSTACK_STARS</comment>
        <translation>...el recompte d&apos;estrelles és menys que</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="305"/>
        <source>... sky background is greater than</source>
        <comment>IDC_DONTSTACK_SKYBACKGROUND</comment>
        <translation>...el fons de cel és major que</translation>
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
        <translation>...FWHM és més gran que</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="400"/>
        <location filename="../ui/SettingsTab.ui" line="475"/>
        <source>pixels</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="524"/>
        <source>... angle is greater than</source>
        <translation>...L&apos;angle és més gran que</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="544"/>
        <source>°</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="564"/>
        <source>Move non-stackable files to the &apos;NonStackable&apos; sub-folder</source>
        <translation>Mou les imatges no apilades al subdirectori &apos;NonStackable&apos;</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="577"/>
        <source>(the &apos;NonStackable&apos; sub folder will be created if necessary)</source>
        <translation>(el subdirectori &apos;NonStackable&apos; serà creat si és necessari)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="605"/>
        <source>Warnings</source>
        <translation>Avisos</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="641"/>
        <source>Sound</source>
        <comment>IDC_WARN_SOUND</comment>
        <translation>So</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="648"/>
        <source>Send email to:</source>
        <comment>IDC_WARN_EMAIL</comment>
        <translation>Envia un correu electrònic a:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="835"/>
        <source>Filters</source>
        <comment>IDC_FILTERS</comment>
        <translation>Filtres</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="937"/>
        <source>Apply changes</source>
        <comment>IDC_APPLYCHANGES</comment>
        <translation>Aplica els canvis</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="944"/>
        <source>Cancel changes</source>
        <comment>IDC_CANCELCHANGES</comment>
        <translation>Cancel·la els canvis</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="611"/>
        <source>Flash Application</source>
        <comment>IDC_WARN_FLASH</comment>
        <translation>Aplicació Flash</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="665"/>
        <source>Create warning file in:</source>
        <translation>Crear fitxer d&apos;avisos a:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="759"/>
        <source>Output folder:</source>
        <translation>Directori de sortida:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="739"/>
        <source>images</source>
        <translation>imatge(s)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="455"/>
        <source>... dX or dY is greater than</source>
        <comment>IDC_DONTSTACK_OFFSET</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="685"/>
        <source>Reset email count</source>
        <comment>IDC_RESETEMAILCOUNT</comment>
        <translation>Resetejar el compte de correu electrònic</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="713"/>
        <source>Options</source>
        <comment>IDC_OPTIONS</comment>
        <translation>Opcions</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="719"/>
        <source>Save stacked image to file every</source>
        <comment>IDC_SAVESTACKEDIMAGE</comment>
        <translation>Guarda la imatge apilada a cada fitxer</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="789"/>
        <source>The trace file is normally written to the DeepSkyStacker directory in the users
&quot;Documents&quot; directory, and is deleted on exit (unless the application crashes).
Tick this box to keep the trace file.</source>
        <translation>El fitxer de traça normalment s&apos;escriu al directori DeepSkyStacker dels usuaris
Directori &quot;Documents&quot; i s&apos;elimina en sortir (tret que l&apos;aplicació es bloquegi).
Marqueu aquesta casella per mantenir el fitxer de traça.</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="794"/>
        <source>Retain the trace file on exit</source>
        <translation>Conserveu el fitxer de traça a la sortida</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="804"/>
        <source>Reset</source>
        <translation>Restablir</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="841"/>
        <source>Process only ...</source>
        <translation>Processa només...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="848"/>
        <source>RAW images (CR2, NEF, ORF, DNG...)</source>
        <comment>IDC_PROCESS_RAW</comment>
        <translation>Imatges RAW (CR2, NEF, ORF, DNG...)&quot;</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="855"/>
        <source>TIFF images</source>
        <comment>IDC_PROCESS_TIFF</comment>
        <translation>Imatges TIFF</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="875"/>
        <source>FITS images</source>
        <comment>IDC_PROCESS_FITS</comment>
        <translation>Imatges FITS</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="882"/>
        <source>Other images (JPEG, PNG)</source>
        <comment>IDC_PROCESS_OTHERS</comment>
        <translation>Altres imatges (JPEG, PNG)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="658"/>
        <location filename="../settingstab.cpp" line="88"/>
        <location filename="../settingstab.cpp" line="262"/>
        <location filename="../settingstab.cpp" line="464"/>
        <source>Click here to select the warning file folder</source>
        <comment>IDC_WARNINGFILEFOLDER</comment>
        <translation>Feu clic aquí per seleccionar el direcotri del fitxer d&apos;advertència</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="634"/>
        <location filename="../settingstab.cpp" line="87"/>
        <location filename="../settingstab.cpp" line="251"/>
        <location filename="../settingstab.cpp" line="405"/>
        <location filename="../settingstab.cpp" line="449"/>
        <source>Click here to set or change the email address</source>
        <comment>IDC_EMAIL</comment>
        <translation>Fes click aquí per a entrar una adreça de correu electrònic</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="769"/>
        <location filename="../settingstab.cpp" line="89"/>
        <location filename="../settingstab.cpp" line="270"/>
        <location filename="../settingstab.cpp" line="435"/>
        <source>Click here to select the stacked image output folder</source>
        <comment>IDC_STACKEDOUTPUTFOLDER</comment>
        <translation>Faci click aquí per a seleccionar el directori de sortida</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="356"/>
        <source>Select Warning File Folder</source>
        <comment>IDS_SELECTWARNINGFOLDER</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="375"/>
        <source>Select Stacked Image Output Folder</source>
        <comment>IDS_SELECTSTACKEDFOLDER</comment>
        <translation></translation>
    </message>
</context>
<context>
    <name>DeepSkyStackerLive</name>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="161"/>
        <source>This beta version of DeepSkyStacker has expired
You can probably get another one or download the final release from the web site.</source>
        <translation>Aquesta versió beta de DeepSkyStacker ha caducat
Probablement en podeu obtenir un altre o descarregar la versió final del lloc web.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="464"/>
        <source>
How to use  DeepSkyStacker Live ?
</source>
        <comment>IDS_LOG_STARTING</comment>
        <translation>
Com usar el DeepSkyStacker Live?
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="467"/>
        <source>Step 1
Check the Settings tabs for all the stacking and warning settings

</source>
        <comment>IDS_LOG_STARTING_1</comment>
        <translation>Pas 1
Comprovi la pestanya de Configuració per a tot el procés d&apos;apilat i els avisos

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="470"/>
        <source>Step 2
Click on the Monitor button to start monitoring the folder
When monitoring is active incoming images are only registered but not stacked.

</source>
        <comment>IDS_LOG_STARTING_2</comment>
        <translation>Pas 2
Faci click en el botó Supervisar per a començar a supervisar el directori\nQuan la supervisió estigui activada, les imatges entrants només seran registrades però no apilades.

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="474"/>
        <source>Step 3
To start stacking the images click on the Stack button
At this point all the incoming (and all previously registered) images will be stacked.
</source>
        <comment>IDS_LOG_STARTING_3</comment>
        <translation>Pas 3
Per a començar a apilar les imatges, faci click en el botó Apilar\nEn aquest moment, totes les imatges entrants (i prèviament registrades) seran apilades.
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="478"/>
        <source>You can pause/restart the stacking process by clicking on the Stack button.
</source>
        <comment>IDS_LOG_STARTING_4</comment>
        <translation>Vostè por pausar o començar de nou el procés d&apos;apilat fent click en el botó d&apos;Apilat.
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="481"/>
        <source>To stop monitoring and stacking click on the Stop button.

</source>
        <comment>IDS_LOG_STARTING_5</comment>
        <translation>Per aturar el procés de supervisió i apilat, faci click en el botó Atura.

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="545"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1227"/>
        <source>No stacked image</source>
        <comment>IDS_NOSTACKEDIMAGE</comment>
        <translation>No hi ha cap imatge apilada</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="546"/>
        <source>No image loaded</source>
        <comment>IDS_NOIMAGELOADED</comment>
        <translation>No hi ha cap imatge carregada</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="600"/>
        <source>Status</source>
        <translation>Condició</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="601"/>
        <source>File</source>
        <comment>IDS_COLUMN_FILE</comment>
        <translation>Arxiu</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="602"/>
        <source>Exposure</source>
        <comment>IDS_COLUMN_EXPOSURE</comment>
        <translation>Exposició</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="603"/>
        <source>Aperture</source>
        <comment>IDS_COLUMN_APERTURE</comment>
        <translation>Obertura</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="604"/>
        <source>Score</source>
        <comment>IDS_COLUMN_SCORE</comment>
        <translation>Puntuació</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="605"/>
        <source>#Stars</source>
        <comment>IDS_COLUMN_STARS</comment>
        <translation># Estrelles</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="606"/>
        <source>FWHM</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="607"/>
        <source>dX</source>
        <comment>IDS_COLUMN_DX</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="608"/>
        <source>dY</source>
        <comment>IDS_COLUMN_DY</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="609"/>
        <source>Angle</source>
        <comment>IDS_COLUMN_ANGLE</comment>
        <translation>Àngle</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="610"/>
        <source>Date/Time</source>
        <comment>IDS_COLUMN_DATETIME</comment>
        <translation>Data/Hora</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="611"/>
        <source>Size</source>
        <comment>IDS_COLUMN_SIZES</comment>
        <translation>Grandària</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="612"/>
        <source>CFA</source>
        <comment>IDS_COLUMN_CFA</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="613"/>
        <source>Depth</source>
        <comment>IDS_COLUMN_DEPTH</comment>
        <translation>Profunditat</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="614"/>
        <source>Info</source>
        <comment>IDS_COLUMN_INFOS</comment>
        <translation>Informació</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="615"/>
        <source>ISO/Gain</source>
        <comment>IDS_COLUMN_ISO_GAIN</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="616"/>
        <source>Sky Background</source>
        <comment>IDS_COLUMN_SKYBACKGROUND</comment>
        <translation>Fons de cel</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="718"/>
        <source>%1 is not a directory. Please select a valid directory.</source>
        <translation>55 / 5,000
Translation results
Translation result
%1 no és un directori. Seleccioneu un directori vàlid.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="728"/>
        <source>Start monitoring folder %1
</source>
        <comment>IDS_LOG_STARTMONITORING</comment>
        <translation>Començar a supervisar el directori %1
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="741"/>
        <source>Stop monitoring folder %1</source>
        <comment>IDS_LOG_STOPMONITORING</comment>
        <translation>Aturar la supervisió del directori %1</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="752"/>
        <source>Stop Stacking files</source>
        <comment>IDS_LOG_STOPSTACKING</comment>
        <translation>Parar d&apos;Apilar Arxius</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="768"/>
        <source>DeepSkyStacker Help.chm</source>
        <comment>IDS_HELPFILE</comment>
        <translation>Ayuda DeepSkyStacker.chm</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="873"/>
        <source>You cannot change the monitored folder while monitoring.</source>
        <comment>IDS_CANTCHANGEMONITOREDFOLDER</comment>
        <translation>Vostè no pot canviar el directori supervisat mentre s&apos;està supervisant.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="885"/>
        <source>Select the folder to be monitored</source>
        <comment>IDS_SELECTMONITOREDFOLDER</comment>
        <translation>Seleccioni la carpeta supervisada</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="921"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1266"/>
        <source>Stacked</source>
        <comment>IDS_STATUS_STACKED</comment>
        <translation>Apilat</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="926"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1184"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1251"/>
        <source>Pending</source>
        <translation>Pendents</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="927"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1257"/>
        <source>Registered</source>
        <comment>IDS_STATUS_REGISTERED</comment>
        <translation>Registrat</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1036"/>
        <source>Start Stacking files</source>
        <comment>IDS_LOG_STARTSTACKING</comment>
        <translation>Començar a Apilar Arxius</translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1101"/>
        <source>You have %n image(s) in the monitored folder.
Do you want to process them?</source>
        <comment>IDS_USEEXISTINGIMAGES</comment>
        <translation>
            <numerusform>Vostè té %n imatge en el directori supervisat.
Vol processar-ho?</numerusform>
            <numerusform>Vostè té %n imatges en el directori supervisat.
Vol processar-les?</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1112"/>
        <source>%n new file(s) found
</source>
        <comment>IDS_LOG_NEWFILESFOUND</comment>
        <translation>
            <numerusform>%n nous arxiu trobats
</numerusform>
            <numerusform>%n nous arxius trobats
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1238"/>
        <source>Click here to save the stacked image to file</source>
        <comment>IDS_SAVESTACKEDIMAGE</comment>
        <translation>Faci click aquí per a guardar la imatge apilada a un arxiu</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1254"/>
        <source>Loaded</source>
        <comment>IDS_STATUS_LOADED</comment>
        <translation>Carregat</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1260"/>
        <source>Stack delayed</source>
        <comment>IDS_STATUS_STACKDELAYED</comment>
        <translation>Apilat ajornat</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1263"/>
        <source>Not stackable</source>
        <comment>IDS_STATUS_NOTSTACKABLE</comment>
        <translation>No apilable</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1345"/>
        <source>No</source>
        <comment>IDS_NO</comment>
        <translation>No</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1347"/>
        <source>Yes</source>
        <comment>IDS_YES</comment>
        <translation>Si</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1350"/>
        <source>RGB %1 bit/ch</source>
        <comment>IDS_FORMAT_RGB</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1352"/>
        <source>Gray %1 bit</source>
        <comment>IDS_FORMAT_GRAY</comment>
        <translation>Gris %1 bit</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1364"/>
        <source>%1%</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1545"/>
        <source>Pending: %1 - Registered: %2 - Stacked: %3 - Total exposure time: %4</source>
        <translation>Pendent: %1 - Registrat: %2 - Apilat: %3 - Temps d&apos;exposició total: %4</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="49"/>
        <source>Monitoring</source>
        <comment>IDC_MONITORINGRECT</comment>
        <translation>Supervisant</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="56"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="62"/>
        <source>Click here to select the Monitored Folder</source>
        <comment>IDC_MONITOREDFOLDER</comment>
        <translation>Fes click aquí per a seleccionar la Carpeta Supervisada</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="205"/>
        <source>Stacked Image</source>
        <comment>IDC_STACKEDIMAGE</comment>
        <translation>Imatge Apilada</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="216"/>
        <source>Last Image</source>
        <comment>IDC_LASTIMAGE</comment>
        <translation>Última imatge</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="221"/>
        <source>Graphs</source>
        <comment>IDC_GRAPHS</comment>
        <translation>Gràfics</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="235"/>
        <source>Image List</source>
        <comment>IDC_IMAGELIST</comment>
        <translation>Llista d&apos;imatges</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="259"/>
        <source>Log</source>
        <comment>IDC_LOGLIST</comment>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="264"/>
        <source>Settings</source>
        <comment>IDC_WARNINGS</comment>
        <translation>Configuració</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="280"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="283"/>
        <source>Monitor</source>
        <comment>IDC_MONITOR</comment>
        <translation>Supervisar</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="296"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="299"/>
        <source>Stack</source>
        <comment>IDC_STACK_DSSLIVE</comment>
        <translation>Apilar</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="308"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="311"/>
        <source>Stop</source>
        <comment>IDC_STOP</comment>
        <translation>Atura</translation>
    </message>
</context>
<context>
    <name>EmailSettings</name>
    <message>
        <location filename="../ui/EmailSettings.ui" line="20"/>
        <source>Sent email settings</source>
        <translation>Envia els paràmetres del correu electrònic</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="26"/>
        <source>To:</source>
        <translation>Envia a:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="33"/>
        <source>Send an Email only for the first warning</source>
        <comment>IDC_SENDONCE</comment>
        <translation>Envia un correu electrònic només pel primer avís</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="43"/>
        <source>Subject:</source>
        <translation>Assumpte:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="66"/>
        <source>Email Account settings</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="14"/>
        <source>Email Settings</source>
        <translation>Paràmetres de correu electrònic</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="72"/>
        <source>SMTP server:</source>
        <translation>Servidor SMTP:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="82"/>
        <source>Port:</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="95"/>
        <location filename="../ui/EmailSettings.ui" line="108"/>
        <source>465</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="113"/>
        <source>587</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="118"/>
        <source>2525</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="126"/>
        <source>Encryption:</source>
        <translation>Xifratge:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="134"/>
        <source>None</source>
        <translation>Cap</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="139"/>
        <source>SSL/TLS</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="144"/>
        <source>STARTTLS</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="152"/>
        <source>Account (From):</source>
        <translation>Compte (De):</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="165"/>
        <source>Password:</source>
        <translation>Contrasenya:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="179"/>
        <source>Show password</source>
        <translation>Mostra contrasenya</translation>
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
        <translation>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Copiar la imatge al portapapers&lt;/a&gt;</translation>
    </message>
</context>
</TS>
