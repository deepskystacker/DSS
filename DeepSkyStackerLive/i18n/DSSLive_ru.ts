<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
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
        <translation>Счет</translation>
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
        <translation>#Звёзд</translation>
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
        <translation>Угол</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="111"/>
        <location filename="../ChartTab.cpp" line="85"/>
        <source>Sky Background</source>
        <comment>IDC_SKYBACKGROUND</comment>
        <translation>Фон неба</translation>
    </message>
</context>
<context>
    <name>DSS::EmailSettings</name>
    <message>
        <location filename="../emailsettings.cpp" line="209"/>
        <source>Failed to connect to email server %1 (%2)!</source>
        <translation>Не удалось подключиться к серверу электронной почты %1 (%2)!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="217"/>
        <source>Failed to login to email server as %1!</source>
        <translation>Не удалось войти на сервер электронной почты как %1!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="226"/>
        <source>Failed to send mail!</source>
        <translation>Не удалось отправить письмо!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="231"/>
        <source>Test email sent OK!</source>
        <translation>Тестовое письмо отправлено ОК!</translation>
    </message>
</context>
<context>
    <name>DSS::FileRegistrar</name>
    <message>
        <location filename="../fileregistrar.cpp" line="128"/>
        <source>-&gt; New file: %1
</source>
        <comment>IDS_LOG_NEWFILE</comment>
        <translation>-&gt; Новый файл: %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="152"/>
        <source>Loading %1 bit/ch %2 light frame
%3</source>
        <comment>IDS_LOADRGBLIGHT</comment>
        <translation>Загрука %1 бит/канал %2 кадра изображения
%3</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="154"/>
        <source>Loading %1 bits gray %2 light frame
%3</source>
        <comment>IDS_LOADGRAYLIGHT</comment>
        <translation>Загрузка %1 бит оттенков серого %2 кадра изображения
%3</translation>
    </message>
    <message numerus="yes">
        <location filename="../fileregistrar.cpp" line="183"/>
        <source>Image %1 registered: %n star(s) detected - FWHM = %L2 - Score = %L3
</source>
        <comment>IDS_LOG_REGISTERRESULTS</comment>
        <translation>
            <numerusform>Кадр %1 зарегистрирован: обнаружена %n звезда - FWHM = %L2 - Бал = %L3
</numerusform>
            <numerusform>Кадр %1 зарегистрирован: обнаружено %n звезды - FWHM = %L2 - Бал = %L3
</numerusform>
            <numerusform>Кадр %1 зарегистрирован: обнаружено %n звезд - FWHM = %L2 - Бал = %L3
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="199"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>Предупреждение: Изображение %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="212"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>Кадр %1 не может быть сложен (%2)
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="220"/>
        <source>Error loading file %1
</source>
        <comment>IDS_LOG_ERRORLOADINGFILE</comment>
        <translation>Ошибка загрузки файла %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="237"/>
        <location filename="../fileregistrar.cpp" line="288"/>
        <source>Score (%L1) is less than %L2</source>
        <comment>IDS_NOSTACK_SCORE</comment>
        <translation>Бал (%L1) меньше чем %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="247"/>
        <location filename="../fileregistrar.cpp" line="298"/>
        <source>Star count(%L1) is less than %L2</source>
        <translation>Количество звёзд (%L1) меньше чем %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="257"/>
        <location filename="../fileregistrar.cpp" line="308"/>
        <source>FWHM (%L1 pixels) is greater than %L2 pixels</source>
        <comment>IDS_NOSTACK_FWHM</comment>
        <translation>FWHM (%L1 пикселей) больше чем %L2 пикселей</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="267"/>
        <location filename="../fileregistrar.cpp" line="318"/>
        <source>Sky Background (%L1%) is greater than %L2%</source>
        <comment>IDS_NOSTACK_SKYBACKGROUND</comment>
        <translation>Фон Неба (%L1%) больше чем %L2%</translation>
    </message>
</context>
<context>
    <name>DSS::FileStacker</name>
    <message>
        <location filename="../filestacker.cpp" line="266"/>
        <source>No transformation found from reference frame</source>
        <comment>IDS_NOSTACK_NOTRANSFORMATION</comment>
        <translation>Не найдено преобразований относительно опорного кадра</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="237"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>Предупреждение: Изображение %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="272"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>Кадр %1 не может быть сложен (%2)
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="290"/>
        <location filename="../filestacker.cpp" line="325"/>
        <source>dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels</source>
        <comment>IDS_NOSTACK_OFFSET</comment>
        <translation>dX (%L1 пикселей) или dY (%L2 пикселей) больше чем %L3 пикселей</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="305"/>
        <location filename="../filestacker.cpp" line="344"/>
        <source>Angle (%L1°) is greater than %L2°</source>
        <comment>IDS_NOSTACK_ANGLE</comment>
        <translation>Угол (%L1°) больше чем %L2°</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="373"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>Сложенное изображение сохранено</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="398"/>
        <source>Image %1 has been added to the stack
</source>
        <translation>Изображение %1 было добавлено в стек
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
        <translation>Ctrl++ или Ctrl+= для увеличения
Ctrl+- для уменьшения масштаба
или используйте колесо мыши для увеличения
Ctrl+4 для переключения режима 4-х углов</translation>
    </message>
</context>
<context>
    <name>DSS::ImageViewer</name>
    <message>
        <location filename="../imageviewer.cpp" line="208"/>
        <source>The stacked image will be saved as soon as possible</source>
        <comment>IDS_STACKEDIMAGEWILLBESAVED</comment>
        <translation>Сложенное изображение будет сохранено как только будет возможно</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="220"/>
        <source>You must select an output folder first.
Go to the Settings tab to select the Stacked Image Output Folder.</source>
        <comment>IDS_NOSTACKEDIMAGEFOLDER</comment>
        <translation>Сначала Вы должны выбрать итоговый каталог.
Перейдите в закладку Настройки и выберите Итоговый Каталог.</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="230"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>Сложенное изображение сохранено</translation>
    </message>
</context>
<context>
    <name>DSS::RestartMonitoring</name>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="14"/>
        <source>Restart Monitoring</source>
        <comment>IDD_RESTARTMONITORING</comment>
        <translation>Перезапуск мониторинга</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="20"/>
        <source>Before restarting do you want to</source>
        <translation>До пересложения вы хотели бы</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="44"/>
        <source>Create a new empty stacked image</source>
        <comment>IDC_CREATENEWSTACKEDIMAGE</comment>
        <translation>Складывать изображения заново</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="74"/>
        <location filename="../ui/RestartMonitoring.ui" line="161"/>
        <source>or</source>
        <translation>или</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="101"/>
        <source>Start from the current stacked image</source>
        <comment>IDC_STARTFROMCURRENTSTACKEDIMAGE</comment>
        <translation>Продолжить складывать изображения</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="131"/>
        <source>Drop all the pending images</source>
        <comment>IDC_DROPPENDINGIMAGES</comment>
        <translation>Отбросить все ожидающие кадры</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="188"/>
        <source>Use and process the pending images</source>
        <comment>IDC_USEPENDINGIMAGES</comment>
        <translation>Обработать все ожидающие кадры</translation>
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
        <translation>Сложение</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="71"/>
        <source>Don&apos;t stack until at least</source>
        <comment>IDC_DONTSTACK</comment>
        <translation>Не складывать, пока кадров, хотябы</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="91"/>
        <source>images are available</source>
        <translation>изображений доступны</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="127"/>
        <source>Warn  Don&apos;t stack if...</source>
        <translation>Предупр.     Не складывать, если...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="169"/>
        <source>... score is less than</source>
        <comment>IDC_DONTSTACK_SCORE</comment>
        <translation>...бал меньше чем</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="237"/>
        <source>... star count is less than</source>
        <comment>IDC_DONTSTACK_STARS</comment>
        <translation>...кол-во звёзд меньше чем</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="305"/>
        <source>... sky background is greater than</source>
        <comment>IDC_DONTSTACK_SKYBACKGROUND</comment>
        <translation>...Фон неба больше чем</translation>
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
        <translation>...FWHM больше чем</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="400"/>
        <location filename="../ui/SettingsTab.ui" line="475"/>
        <source>pixels</source>
        <translation>пикселей</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="524"/>
        <source>... angle is greater than</source>
        <translation>...Угол больше чем</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="544"/>
        <source>°</source>
        <translation>°</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="564"/>
        <source>Move non-stackable files to the &apos;NonStackable&apos; sub-folder</source>
        <translation>Перемещать нескладываемые в каталог &apos;NonStackable&apos;</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="577"/>
        <source>(the &apos;NonStackable&apos; sub folder will be created if necessary)</source>
        <translation>(каталог &apos;NonStackable&apos; будет создан, если необходимо)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="605"/>
        <source>Warnings</source>
        <translation>Установки</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="641"/>
        <source>Sound</source>
        <comment>IDC_WARN_SOUND</comment>
        <translation>Звук</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="648"/>
        <source>Send email to:</source>
        <comment>IDC_WARN_EMAIL</comment>
        <translation>Послать email:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="835"/>
        <source>Filters</source>
        <comment>IDC_FILTERS</comment>
        <translation>Фильтры</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="937"/>
        <source>Apply changes</source>
        <comment>IDC_APPLYCHANGES</comment>
        <translation>Применить изменения</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="944"/>
        <source>Cancel changes</source>
        <comment>IDC_CANCELCHANGES</comment>
        <translation>Отменить изменения</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="611"/>
        <source>Flash Application</source>
        <comment>IDC_WARN_FLASH</comment>
        <translation>Моргающее приложение</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="665"/>
        <source>Create warning file in:</source>
        <translation>Создать файл предупреждений в:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="759"/>
        <source>Output folder:</source>
        <translation>Каталог для записи:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="739"/>
        <source>images</source>
        <translation>изображение(я)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="455"/>
        <source>... dX or dY is greater than</source>
        <comment>IDC_DONTSTACK_OFFSET</comment>
        <translation>...dX или dY больше чем</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="685"/>
        <source>Reset email count</source>
        <comment>IDC_RESETEMAILCOUNT</comment>
        <translation>Сбросить счётчик email</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="713"/>
        <source>Options</source>
        <comment>IDC_OPTIONS</comment>
        <translation>Опции</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="719"/>
        <source>Save stacked image to file every</source>
        <comment>IDC_SAVESTACKEDIMAGE</comment>
        <translation>Сохранить сложенные изображения для каждого файла</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="789"/>
        <source>The trace file is normally written to the DeepSkyStacker directory in the users
&quot;Documents&quot; directory, and is deleted on exit (unless the application crashes).
Tick this box to keep the trace file.</source>
        <translation>Файл трассировки обычно записывается в каталог DeepSkyStacker в папке пользователей.
Каталог «Документы» и удаляется при выходе (если приложение не аварийно завершает работу).
Установите этот флажок, чтобы сохранить файл трассировки.</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="794"/>
        <source>Retain the trace file on exit</source>
        <translation>Сохранять файл трассировки при выходе</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="804"/>
        <source>Reset</source>
        <translation>Сброс</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="841"/>
        <source>Process only ...</source>
        <translation>Обрабатывать только...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="848"/>
        <source>RAW images (CR2, NEF, ORF, DNG...)</source>
        <comment>IDC_PROCESS_RAW</comment>
        <translation>RAW файлы (CR2, NEF, ORF, DNG...)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="855"/>
        <source>TIFF images</source>
        <comment>IDC_PROCESS_TIFF</comment>
        <translation>TIFF файлы</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="875"/>
        <source>FITS images</source>
        <comment>IDC_PROCESS_FITS</comment>
        <translation>FITS файлы</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="882"/>
        <source>Other images (JPEG, PNG)</source>
        <comment>IDC_PROCESS_OTHERS</comment>
        <translation>Другие файлы (JPEG, PNG)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="634"/>
        <location filename="../settingstab.cpp" line="87"/>
        <location filename="../settingstab.cpp" line="251"/>
        <location filename="../settingstab.cpp" line="405"/>
        <location filename="../settingstab.cpp" line="449"/>
        <source>Click here to set or change the email address</source>
        <comment>IDC_EMAIL</comment>
        <translation>Нажмите здесь, чтобы установить или изменить адрес электронной почты</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="658"/>
        <location filename="../settingstab.cpp" line="88"/>
        <location filename="../settingstab.cpp" line="262"/>
        <location filename="../settingstab.cpp" line="464"/>
        <source>Click here to select the warning file folder</source>
        <comment>IDC_WARNINGFILEFOLDER</comment>
        <translation>Нажмите здесь, чтобы выбрать папку с файлом предупреждения</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="769"/>
        <location filename="../settingstab.cpp" line="89"/>
        <location filename="../settingstab.cpp" line="270"/>
        <location filename="../settingstab.cpp" line="435"/>
        <source>Click here to select the stacked image output folder</source>
        <comment>IDC_STACKEDOUTPUTFOLDER</comment>
        <translation>Нажмите здесь, чтобы выбрать папку вывода составных изображений</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="356"/>
        <source>Select Warning File Folder</source>
        <comment>IDS_SELECTWARNINGFOLDER</comment>
        <translation>Выберите каталог Файла Предупреждений</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="375"/>
        <source>Select Stacked Image Output Folder</source>
        <comment>IDS_SELECTSTACKEDFOLDER</comment>
        <translation>Выберите итоговый каталог сложенных кадров</translation>
    </message>
</context>
<context>
    <name>DeepSkyStackerLive</name>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="159"/>
        <source>This beta version of DeepSkyStacker has expired
You can probably get another one or download the final release from the web site.</source>
        <translation>Срок действия этой бета-версии DeepSkyStacker истек
Вы, вероятно, можете получить еще один или загрузить финальную версию с веб-сайта.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="458"/>
        <source>
How to use  DeepSkyStacker Live ?
</source>
        <comment>IDS_LOG_STARTING</comment>
        <translation>
Как использовать  DeepSkyStacker Live?
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="461"/>
        <source>Step 1
Check the Settings tabs for all the stacking and warning settings

</source>
        <comment>IDS_LOG_STARTING_1</comment>
        <translation>Шаг 1
В закладке &quot;&quot;Установки&quot;&quot; настройте параметры сложения и предупреждений

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="464"/>
        <source>Step 2
Click on the Monitor button to start monitoring the folder
When monitoring is active incoming images are only registered but not stacked.

</source>
        <comment>IDS_LOG_STARTING_2</comment>
        <translation>Шаг 2
Нажмите кнопку &quot;&quot;Мониторить&quot;&quot; для начала контроля каталога
Как только изображения появятся в каталоге, они регистрируются (не складываются).

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="468"/>
        <source>Step 3
To start stacking the images click on the Stack button
At this point all the incoming (and all previously registered) images will be stacked.
</source>
        <comment>IDS_LOG_STARTING_3</comment>
        <translation>Шаг 3
Для начала сложения нажмите кнопку &quot;Сложение&quot;
Тогда новые и ранее зарегистрированные кадры будут сложены.
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="472"/>
        <source>You can pause/restart the stacking process by clicking on the Stack button.
</source>
        <comment>IDS_LOG_STARTING_4</comment>
        <translation>Вы можете приостановить/начать заново процесс сложения кнопкой Сложение.
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="475"/>
        <source>To stop monitoring and stacking click on the Stop button.

</source>
        <comment>IDS_LOG_STARTING_5</comment>
        <translation>Чтобы перестать мониторить и складывать нажмите кнопку Стоп.

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="528"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1213"/>
        <source>No stacked image</source>
        <comment>IDS_NOSTACKEDIMAGE</comment>
        <translation>Нет сложенных изображений</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="529"/>
        <source>No image loaded</source>
        <comment>IDS_NOIMAGELOADED</comment>
        <translation>Нет загруженных изображений</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="583"/>
        <source>Status</source>
        <translation>Статус</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="584"/>
        <source>File</source>
        <comment>IDS_COLUMN_FILE</comment>
        <translation>Файл</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="585"/>
        <source>Exposure</source>
        <comment>IDS_COLUMN_EXPOSURE</comment>
        <translation>Выдержка</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="586"/>
        <source>Aperture</source>
        <comment>IDS_COLUMN_APERTURE</comment>
        <translation>Диафрагма</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="587"/>
        <source>Score</source>
        <comment>IDS_COLUMN_SCORE</comment>
        <translation>Счет</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="588"/>
        <source>Quality</source>
        <comment>IDS_COLUMN_MEANQUALITY</comment>
        <translation>Качество</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="589"/>
        <source>#Stars</source>
        <comment>IDS_COLUMN_STARS</comment>
        <translation>#Звёзд</translation>
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
        <translation>Угол</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="594"/>
        <source>Date/Time</source>
        <comment>IDS_COLUMN_DATETIME</comment>
        <translation>Дата/Время</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="595"/>
        <source>Size</source>
        <comment>IDS_COLUMN_SIZES</comment>
        <translation>Размер</translation>
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
        <translation>Глубина</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="598"/>
        <source>Info</source>
        <comment>IDS_COLUMN_INFOS</comment>
        <translation>Инфо</translation>
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
        <translation>Фон неба</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="703"/>
        <source>%1 is not a directory. Please select a valid directory.</source>
        <translation>%1 не является каталогом. Пожалуйста, выберите действительный каталог.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="713"/>
        <source>Start monitoring folder %1
</source>
        <comment>IDS_LOG_STARTMONITORING</comment>
        <translation>Начало мониторинга каталога %1
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="726"/>
        <source>Stop monitoring folder %1</source>
        <comment>IDS_LOG_STOPMONITORING</comment>
        <translation>Остановка мониторинга каталога %1</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="737"/>
        <source>Stop Stacking files</source>
        <comment>IDS_LOG_STOPSTACKING</comment>
        <translation>Остановка складывания файлов</translation>
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
        <translation>Нельзя поменять каталог, пока идёт его мониторинг.</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="870"/>
        <source>Select the folder to be monitored</source>
        <comment>IDS_SELECTMONITOREDFOLDER</comment>
        <translation>Выбор каталога мониторинга</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="905"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1252"/>
        <source>Stacked</source>
        <comment>IDS_STATUS_STACKED</comment>
        <translation>Сложено</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="911"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1170"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1237"/>
        <source>Pending</source>
        <translation>В ожидании</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="912"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1243"/>
        <source>Registered</source>
        <comment>IDS_STATUS_REGISTERED</comment>
        <translation>Зарегистрировано</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1021"/>
        <source>Start Stacking files</source>
        <comment>IDS_LOG_STARTSTACKING</comment>
        <translation>Начало складывания файлов</translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1086"/>
        <source>You have %n image(s) in the monitored folder.
Do you want to process them?</source>
        <comment>IDS_USEEXISTINGIMAGES</comment>
        <translation>
            <numerusform>В отслеживаемом каталоге есть %n изображение.
Вы хотите его обработать?</numerusform>
            <numerusform>В отслеживаемом каталоге есть %n изображения.
Вы хотите его обработать?</numerusform>
            <numerusform>В отслеживаемом каталоге есть %n изображений.
Вы хотите его обработать?</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1097"/>
        <source>%n new file(s) found
</source>
        <comment>IDS_LOG_NEWFILESFOUND</comment>
        <translation>
            <numerusform>Найден %n новый файл
</numerusform>
            <numerusform>найдено %n новых файла
</numerusform>
            <numerusform>Найдено %n новых файлов
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1224"/>
        <source>Click here to save the stacked image to file</source>
        <comment>IDS_SAVESTACKEDIMAGE</comment>
        <translation>Нажмите здесь для сохранения сложенного изображения в файл</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1240"/>
        <source>Loaded</source>
        <comment>IDS_STATUS_LOADED</comment>
        <translation>Загружено</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1246"/>
        <source>Stack delayed</source>
        <comment>IDS_STATUS_STACKDELAYED</comment>
        <translation>Сложение приостановлено</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1249"/>
        <source>Not stackable</source>
        <comment>IDS_STATUS_NOTSTACKABLE</comment>
        <translation>Не складываемый</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1332"/>
        <source>No</source>
        <comment>IDS_NO</comment>
        <translation>Нет</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1334"/>
        <source>Yes</source>
        <comment>IDS_YES</comment>
        <translation>Да</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1337"/>
        <source>RGB %1 bit/ch</source>
        <comment>IDS_FORMAT_RGB</comment>
        <translation>RGB %1 бит/канал</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1339"/>
        <source>Gray %1 bit</source>
        <comment>IDS_FORMAT_GRAY</comment>
        <translation>Серое %1 бит</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1351"/>
        <source>%1%</source>
        <translation>%1%</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1532"/>
        <source>Pending: %1 - Registered: %2 - Stacked: %3 - Total exposure time: %4</source>
        <translation>Ожидает: %1 - Зарегистрировано: %2 - Сложено: %3 - Время суммарной экспозиции: %4</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="49"/>
        <source>Monitoring</source>
        <comment>IDC_MONITORINGRECT</comment>
        <translation>Мониторинг</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="56"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="62"/>
        <source>Click here to select the Monitored Folder</source>
        <comment>IDC_MONITOREDFOLDER</comment>
        <translation>Нажми, чтобы выбрать каталог мониторинга</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="205"/>
        <source>Stacked Image</source>
        <comment>IDC_STACKEDIMAGE</comment>
        <translation>Сложенное изображение</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="216"/>
        <source>Last Image</source>
        <comment>IDC_LASTIMAGE</comment>
        <translation>Последнее изображение</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="221"/>
        <source>Graphs</source>
        <comment>IDC_GRAPHS</comment>
        <translation>Графики</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="235"/>
        <source>Image List</source>
        <comment>IDC_IMAGELIST</comment>
        <translation>Список изображений</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="259"/>
        <source>Log</source>
        <comment>IDC_LOGLIST</comment>
        <translation>Журнал</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="264"/>
        <source>Settings</source>
        <comment>IDC_WARNINGS</comment>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="280"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="283"/>
        <source>Monitor</source>
        <comment>IDC_MONITOR</comment>
        <translation>Мониторить</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="296"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="299"/>
        <source>Stack</source>
        <comment>IDC_STACK_DSSLIVE</comment>
        <translation>Складывать</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="308"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="311"/>
        <source>Stop</source>
        <comment>IDC_STOP</comment>
        <translation>Остановиться</translation>
    </message>
</context>
<context>
    <name>EmailSettings</name>
    <message>
        <location filename="../ui/EmailSettings.ui" line="20"/>
        <source>Sent email settings</source>
        <translation>Настройки отправки email</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="26"/>
        <source>To:</source>
        <translation>Кому:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="33"/>
        <source>Send an Email only for the first warning</source>
        <comment>IDC_SENDONCE</comment>
        <translation>Послать email только для первого предупреждения</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="43"/>
        <source>Subject:</source>
        <translation>Объект:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="66"/>
        <source>Email Account settings</source>
        <translation>Настройки учётной записи</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="14"/>
        <source>Email Settings</source>
        <translation>Email настройки</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="72"/>
        <source>SMTP server:</source>
        <translation>SMTP сервер:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="82"/>
        <source>Port:</source>
        <translation>Порт:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="126"/>
        <source>Encryption:</source>
        <translation>Шифрование:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="134"/>
        <source>None</source>
        <translation>Никто</translation>
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
        <translation>Учётная запись (От):</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="165"/>
        <source>Password:</source>
        <translation>Пароль:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="179"/>
        <source>Show password</source>
        <translation>Показать пароль</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="189"/>
        <source>Test</source>
        <comment>IDC_TEST</comment>
        <translation>Тест</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../ui/ImageViewer.ui" line="89"/>
        <source>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Copy current picture to clipboard&lt;/a&gt;</source>
        <comment>IDC_COPYTOCLIPBOARD</comment>
        <translation>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Скопировать эту картинку в буфер обмена&lt;/a&gt;</translation>
    </message>
</context>
<context>
    <name>QLinearGradientCtrl</name>
    <message>
        <location filename="../../Tools/QLinearGradientCtrl.cpp" line="44"/>
        <source>After selecting a slider with the mouse, you can move it using the Up Arrow, Right Arrow, Down Arrow, Left Arrow, Page Up, and Page Down keys.</source>
        <translation>Выбрав ползунок с помощью мыши, вы можете перемещать его с помощью клавиш «Стрелка вверх», «Стрелка вправо», «Стрелка вниз», «Стрелка влево», Page Up и Page Down.</translation>
    </message>
</context>
</TS>
