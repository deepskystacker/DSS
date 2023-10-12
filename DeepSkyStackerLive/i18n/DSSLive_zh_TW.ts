<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_TW">
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
        <translation>分數</translation>
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
        <translation>星星數</translation>
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
        <translation>旋轉角</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="111"/>
        <location filename="../ChartTab.cpp" line="85"/>
        <source>Sky Background</source>
        <comment>IDC_SKYBACKGROUND</comment>
        <translation>天空背景</translation>
    </message>
</context>
<context>
    <name>DSS::EmailSettings</name>
    <message>
        <location filename="../emailsettings.cpp" line="209"/>
        <source>Failed to connect to email server %1 (%2)!</source>
        <translation>無法連線到電子郵件伺服器 %1 (%2)！</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="217"/>
        <source>Failed to login to email server as %1!</source>
        <translation>無法以 %1 身分登入電子郵件伺服器！</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="226"/>
        <source>Failed to send mail!</source>
        <translation>發送郵件失敗！</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="231"/>
        <source>Test email sent OK!</source>
        <translation>測試郵件發送成功！</translation>
    </message>
</context>
<context>
    <name>DSS::FileRegistrar</name>
    <message>
        <location filename="../fileregistrar.cpp" line="128"/>
        <source>-&gt; New file: %1
</source>
        <comment>IDS_LOG_NEWFILE</comment>
        <translation>-&gt; 新檔案: %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="152"/>
        <source>Loading %1 bit/ch %2 light frame
%3</source>
        <comment>IDS_LOADRGBLIGHT</comment>
        <translation>載入每色 %1 bit %2 之天體影像
%3</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="154"/>
        <source>Loading %1 bits gray %2 light frame
%3</source>
        <comment>IDS_LOADGRAYLIGHT</comment>
        <translation>載入 %1 bit %2 之灰階天體影像
%3</translation>
    </message>
    <message numerus="yes">
        <location filename="../fileregistrar.cpp" line="184"/>
        <source>Image %1 registered: %n star(s) detected - FWHM = %L2 - Score = %L3
</source>
        <comment>IDS_LOG_REGISTERRESULTS</comment>
        <translation>
            <numerusform>影像 %1 已偵測星點: 偵測到 %n 個星星 - FWHM = %L2 - 分數 = %L3
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="200"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>警告: 影像 %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="213"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>影像 %1 不可疊合 (%2)
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="221"/>
        <source>Error loading file %1
</source>
        <comment>IDS_LOG_ERRORLOADINGFILE</comment>
        <translation>檔案載入錯誤 %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="238"/>
        <location filename="../fileregistrar.cpp" line="289"/>
        <source>Score (%L1) is less than %L2</source>
        <comment>IDS_NOSTACK_SCORE</comment>
        <translation>分數 (%L1) 少於 %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="248"/>
        <location filename="../fileregistrar.cpp" line="299"/>
        <source>Star count(%L1) is less than %L2</source>
        <translation>星星數 (%L1) 少於 %L2</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="258"/>
        <location filename="../fileregistrar.cpp" line="309"/>
        <source>FWHM (%L1 pixels) is greater than %L2 pixels</source>
        <comment>IDS_NOSTACK_FWHM</comment>
        <translation>FWHM (%L1 像元) 大於 %L2 像元</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="268"/>
        <location filename="../fileregistrar.cpp" line="319"/>
        <source>Sky Background (%L1%) is greater than %L2%</source>
        <comment>IDS_NOSTACK_SKYBACKGROUND</comment>
        <translation>天空背景 (%L1%) 大於 %L2%</translation>
    </message>
</context>
<context>
    <name>DSS::FileStacker</name>
    <message>
        <location filename="../filestacker.cpp" line="267"/>
        <source>No transformation found from reference frame</source>
        <comment>IDS_NOSTACK_NOTRANSFORMATION</comment>
        <translation>參考影像不具轉換資訊</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="238"/>
        <source>Warning: Image %1 -&gt; %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>警告: 影像 %1 -&gt; %2
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="273"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>影像 %1 不可疊合 (%2)
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="291"/>
        <location filename="../filestacker.cpp" line="326"/>
        <source>dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels</source>
        <comment>IDS_NOSTACK_OFFSET</comment>
        <translation>dX (%L1 像元) 或dY (%L2 像元) 大於 %L3 像元</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="306"/>
        <location filename="../filestacker.cpp" line="345"/>
        <source>Angle (%L1°) is greater than %L2°</source>
        <comment>IDS_NOSTACK_ANGLE</comment>
        <translation>旋轉角 (%L1°) 大於 %L2°</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="374"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>疊合影像已儲存</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="400"/>
        <source>Image %1 has been added to the stack
</source>
        <translation>影像 %1 已加入堆疊中
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
        <translation>Ctrl++ 或 Ctrl+= 放大
Ctrl+- 縮小
或使用鼠標滾輪縮放
Ctrl+4 切換四角模式</translation>
    </message>
</context>
<context>
    <name>DSS::ImageViewer</name>
    <message>
        <location filename="../imageviewer.cpp" line="208"/>
        <source>The stacked image will be saved as soon as possible</source>
        <comment>IDS_STACKEDIMAGEWILLBESAVED</comment>
        <translation>疊合之影像將儘速被儲存</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="220"/>
        <source>You must select an output folder first.
Go to the Settings tab to select the Stacked Image Output Folder.</source>
        <comment>IDS_NOSTACKEDIMAGEFOLDER</comment>
        <translation>需先選擇輸出檔案夾。
請至&quot;&quot;設定&quot;&quot;表單選取已疊合影像輸出檔案夾。</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="230"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>疊合影像已儲存</translation>
    </message>
</context>
<context>
    <name>DSS::RestartMonitoring</name>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="14"/>
        <source>Restart Monitoring</source>
        <comment>IDD_RESTARTMONITORING</comment>
        <translation>重新啟動監視</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="20"/>
        <source>Before restarting do you want to</source>
        <translation>重啟動前, 你要</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="44"/>
        <source>Create a new empty stacked image</source>
        <comment>IDC_CREATENEWSTACKEDIMAGE</comment>
        <translation>產生一個新的空白疊合影像</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="74"/>
        <location filename="../ui/RestartMonitoring.ui" line="161"/>
        <source>or</source>
        <translation>或</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="101"/>
        <source>Start from the current stacked image</source>
        <comment>IDC_STARTFROMCURRENTSTACKEDIMAGE</comment>
        <translation>從現在的已疊合影像開始</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="131"/>
        <source>Drop all the pending images</source>
        <comment>IDC_DROPPENDINGIMAGES</comment>
        <translation>略過所有待處理影像</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="188"/>
        <source>Use and process the pending images</source>
        <comment>IDC_USEPENDINGIMAGES</comment>
        <translation>使用與處理待處理的影像</translation>
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
        <translation>疊合中</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="71"/>
        <source>Don&apos;t stack until at least</source>
        <comment>IDC_DONTSTACK</comment>
        <translation>不要疊合，直到至少有</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="91"/>
        <source>images are available</source>
        <translation>張圖片可用</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="127"/>
        <source>Warn  Don&apos;t stack if...</source>
        <translation>警告  不要疊合, 如果...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="169"/>
        <source>... score is less than</source>
        <comment>IDC_DONTSTACK_SCORE</comment>
        <translation>...分數少於</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="237"/>
        <source>... star count is less than</source>
        <comment>IDC_DONTSTACK_STARS</comment>
        <translation>...星星數少於</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="305"/>
        <source>... sky background is greater than</source>
        <comment>IDC_DONTSTACK_SKYBACKGROUND</comment>
        <translation>...天空背景值高於</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="325"/>
        <source>%</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="380"/>
        <source>... FWHM is greater than</source>
        <comment>IDC_DONTSTACK_FWHM</comment>
        <translation>...FWHM大於</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="400"/>
        <location filename="../ui/SettingsTab.ui" line="475"/>
        <source>pixels</source>
        <translation>像元數</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="524"/>
        <source>... angle is greater than</source>
        <translation>...角度大於</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="544"/>
        <source>°</source>
        <translation>°</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="564"/>
        <source>Move non-stackable files to the &apos;NonStackable&apos; sub-folder</source>
        <translation>將無法疊合的影像移至 &apos;NonStackable&apos; 檔案夾</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="577"/>
        <source>(the &apos;NonStackable&apos; sub folder will be created if necessary)</source>
        <translation>(必要時將產生 &apos;NonStackable&apos; 檔案夾)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="605"/>
        <source>Warnings</source>
        <translation>警告</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="641"/>
        <source>Sound</source>
        <comment>IDC_WARN_SOUND</comment>
        <translation>警告聲</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="648"/>
        <source>Send email to:</source>
        <comment>IDC_WARN_EMAIL</comment>
        <translation>送email到:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="835"/>
        <source>Filters</source>
        <comment>IDC_FILTERS</comment>
        <translation>過濾器</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="937"/>
        <source>Apply changes</source>
        <comment>IDC_APPLYCHANGES</comment>
        <translation>應用程式變更</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="944"/>
        <source>Cancel changes</source>
        <comment>IDC_CANCELCHANGES</comment>
        <translation>取消更改</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="611"/>
        <source>Flash Application</source>
        <comment>IDC_WARN_FLASH</comment>
        <translation>閃爍</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="665"/>
        <source>Create warning file in:</source>
        <translation>產生警告檔於:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="759"/>
        <source>Output folder:</source>
        <translation>輸出的檔案夾:</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="739"/>
        <source>images</source>
        <translation>幅影像</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="455"/>
        <source>... dX or dY is greater than</source>
        <comment>IDC_DONTSTACK_OFFSET</comment>
        <translation>dX或dY大於</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="685"/>
        <source>Reset email count</source>
        <comment>IDC_RESETEMAILCOUNT</comment>
        <translation>重置電子郵件計數</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="713"/>
        <source>Options</source>
        <comment>IDC_OPTIONS</comment>
        <translation>選項</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="719"/>
        <source>Save stacked image to file every</source>
        <comment>IDC_SAVESTACKEDIMAGE</comment>
        <translation>儲存疊好的影像，每當疊入了</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="789"/>
        <source>The trace file is normally written to the DeepSkyStacker directory in the users
&quot;Documents&quot; directory, and is deleted on exit (unless the application crashes).
Tick this box to keep the trace file.</source>
        <translation>追蹤檔案通常寫入使用者中的 DeepSkyStacker 目錄中
「Documents」目錄，並在退出時被刪除（除非應用程式崩潰）。
勾選此方塊以保留追蹤檔案。</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="794"/>
        <source>Retain the trace file on exit</source>
        <translation>退出時保留追蹤文件</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="804"/>
        <source>Reset</source>
        <translation>重置</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="841"/>
        <source>Process only ...</source>
        <translation>只有處理 ...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="848"/>
        <source>RAW images (CR2, NEF, ORF, DNG...)</source>
        <comment>IDC_PROCESS_RAW</comment>
        <translation>RAW 影像（CR2、NEF、ORF、DNG...）</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="855"/>
        <source>TIFF images</source>
        <comment>IDC_PROCESS_TIFF</comment>
        <translation>TIFF 影像</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="875"/>
        <source>FITS images</source>
        <comment>IDC_PROCESS_FITS</comment>
        <translation>FITS 影像</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="882"/>
        <source>Other images (JPEG, PNG)</source>
        <comment>IDC_PROCESS_OTHERS</comment>
        <translation>其他影像（JPEG、PNG）</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="634"/>
        <location filename="../settingstab.cpp" line="87"/>
        <location filename="../settingstab.cpp" line="251"/>
        <location filename="../settingstab.cpp" line="405"/>
        <location filename="../settingstab.cpp" line="449"/>
        <source>Click here to set or change the email address</source>
        <comment>IDC_EMAIL</comment>
        <translation>按一下此處設定或變更電子郵件地址</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="658"/>
        <location filename="../settingstab.cpp" line="88"/>
        <location filename="../settingstab.cpp" line="262"/>
        <location filename="../settingstab.cpp" line="464"/>
        <source>Click here to select the warning file folder</source>
        <comment>IDC_WARNINGFILEFOLDER</comment>
        <translation>按一下此處選擇警告資料夾</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="769"/>
        <location filename="../settingstab.cpp" line="89"/>
        <location filename="../settingstab.cpp" line="270"/>
        <location filename="../settingstab.cpp" line="435"/>
        <source>Click here to select the stacked image output folder</source>
        <comment>IDC_STACKEDOUTPUTFOLDER</comment>
        <translation>按一下此處選擇堆疊影像輸出資料夾</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="356"/>
        <source>Select Warning File Folder</source>
        <comment>IDS_SELECTWARNINGFOLDER</comment>
        <translation>選擇警告檔案夾</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="375"/>
        <source>Select Stacked Image Output Folder</source>
        <comment>IDS_SELECTSTACKEDFOLDER</comment>
        <translation>選擇輸出疊合影像之檔案夾</translation>
    </message>
</context>
<context>
    <name>DeepSkyStackerLive</name>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="161"/>
        <source>This beta version of DeepSkyStacker has expired
You can probably get another one or download the final release from the web site.</source>
        <translation>DeepSkyStacker 測試版已過期
您可能會得到另一個或從網站下載最終版本。</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="464"/>
        <source>
How to use  DeepSkyStacker Live ?
</source>
        <comment>IDS_LOG_STARTING</comment>
        <translation>
如何使用DeepSkyStacker Live?
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="467"/>
        <source>Step 1
Check the Settings tabs for all the stacking and warning settings

</source>
        <comment>IDS_LOG_STARTING_1</comment>
        <translation>第一步
檢查&quot;&quot;設定&quot;&quot;表單裡所有的疊合與警告設定

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="470"/>
        <source>Step 2
Click on the Monitor button to start monitoring the folder
When monitoring is active incoming images are only registered but not stacked.

</source>
        <comment>IDS_LOG_STARTING_2</comment>
        <translation>第二步
點選&quot;&quot;監視&quot;&quot;紐以開始監視檔案夾。
在監視時，存進來的檔案只會被偵測星點，不會被疊合。

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="474"/>
        <source>Step 3
To start stacking the images click on the Stack button
At this point all the incoming (and all previously registered) images will be stacked.
</source>
        <comment>IDS_LOG_STARTING_3</comment>
        <translation>第三步
點選&quot;&quot;疊合&quot;&quot;紐以開始疊合影像。
此時，所有已存以及已偵測星點的影像將被疊起來。
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="478"/>
        <source>You can pause/restart the stacking process by clicking on the Stack button.
</source>
        <comment>IDS_LOG_STARTING_4</comment>
        <translation>您可透過按&quot;&quot;疊合&quot;&quot;鍵以暫停或再開始。
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="481"/>
        <source>To stop monitoring and stacking click on the Stop button.

</source>
        <comment>IDS_LOG_STARTING_5</comment>
        <translation>按&quot;&quot;停止&quot;&quot;鍵可終止監視與疊合。

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="545"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1227"/>
        <source>No stacked image</source>
        <comment>IDS_NOSTACKEDIMAGE</comment>
        <translation>沒有已疊合的影像</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="546"/>
        <source>No image loaded</source>
        <comment>IDS_NOIMAGELOADED</comment>
        <translation>沒有影像被載入</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="600"/>
        <source>Status</source>
        <translation>地位</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="601"/>
        <source>File</source>
        <comment>IDS_COLUMN_FILE</comment>
        <translation>檔案</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="602"/>
        <source>Exposure</source>
        <comment>IDS_COLUMN_EXPOSURE</comment>
        <translation>曝光</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="603"/>
        <source>Aperture</source>
        <comment>IDS_COLUMN_APERTURE</comment>
        <translation>光圈</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="604"/>
        <source>Score</source>
        <comment>IDS_COLUMN_SCORE</comment>
        <translation>分數</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="605"/>
        <source>#Stars</source>
        <comment>IDS_COLUMN_STARS</comment>
        <translation>星星數</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="606"/>
        <source>FWHM</source>
        <translation>FWHM</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="607"/>
        <source>dX</source>
        <comment>IDS_COLUMN_DX</comment>
        <translation>dX</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="608"/>
        <source>dY</source>
        <comment>IDS_COLUMN_DY</comment>
        <translation>dY</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="609"/>
        <source>Angle</source>
        <comment>IDS_COLUMN_ANGLE</comment>
        <translation>旋轉角</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="610"/>
        <source>Date/Time</source>
        <comment>IDS_COLUMN_DATETIME</comment>
        <translation>日期/時間</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="611"/>
        <source>Size</source>
        <comment>IDS_COLUMN_SIZES</comment>
        <translation>大小</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="612"/>
        <source>CFA</source>
        <comment>IDS_COLUMN_CFA</comment>
        <translation>CFA</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="613"/>
        <source>Depth</source>
        <comment>IDS_COLUMN_DEPTH</comment>
        <translation>深度</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="614"/>
        <source>Info</source>
        <comment>IDS_COLUMN_INFOS</comment>
        <translation>資訊</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="615"/>
        <source>ISO/Gain</source>
        <comment>IDS_COLUMN_ISO_GAIN</comment>
        <translation>ISO/Gain</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="616"/>
        <source>Sky Background</source>
        <comment>IDS_COLUMN_SKYBACKGROUND</comment>
        <translation>天空背景</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="718"/>
        <source>%1 is not a directory. Please select a valid directory.</source>
        <translation>%1 不是目錄。 請選擇一個有效的目錄。</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="728"/>
        <source>Start monitoring folder %1
</source>
        <comment>IDS_LOG_STARTMONITORING</comment>
        <translation>開始監視檔案夾 %1
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="741"/>
        <source>Stop monitoring folder %1</source>
        <comment>IDS_LOG_STOPMONITORING</comment>
        <translation>停止監視檔案夾 %1</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="752"/>
        <source>Stop Stacking files</source>
        <comment>IDS_LOG_STOPSTACKING</comment>
        <translation>停止疊合影像</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="768"/>
        <source>DeepSkyStacker Help.chm</source>
        <comment>IDS_HELPFILE</comment>
        <translation>DeepSkyStacker Yardım.chm</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="873"/>
        <source>You cannot change the monitored folder while monitoring.</source>
        <comment>IDS_CANTCHANGEMONITOREDFOLDER</comment>
        <translation>監視中無法改變監視檔案夾。</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="885"/>
        <source>Select the folder to be monitored</source>
        <comment>IDS_SELECTMONITOREDFOLDER</comment>
        <translation>選取欲監視的檔案夾</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="921"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1266"/>
        <source>Stacked</source>
        <comment>IDS_STATUS_STACKED</comment>
        <translation>已疊</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="926"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1184"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1251"/>
        <source>Pending</source>
        <translation>待辦的</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="927"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1257"/>
        <source>Registered</source>
        <comment>IDS_STATUS_REGISTERED</comment>
        <translation>星點偵測完成</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1036"/>
        <source>Start Stacking files</source>
        <comment>IDS_LOG_STARTSTACKING</comment>
        <translation>開始疊合影像</translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1101"/>
        <source>You have %n image(s) in the monitored folder.
Do you want to process them?</source>
        <comment>IDS_USEEXISTINGIMAGES</comment>
        <translation>
            <numerusform>有 %n 幅影像在監視檔案夾中。
要處理它們嗎？</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1112"/>
        <source>%n new file(s) found
</source>
        <comment>IDS_LOG_NEWFILESFOUND</comment>
        <translation>
            <numerusform>找到 %n 個新文件
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1238"/>
        <source>Click here to save the stacked image to file</source>
        <comment>IDS_SAVESTACKEDIMAGE</comment>
        <translation>選此以儲存疊合影像至檔案</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1254"/>
        <source>Loaded</source>
        <comment>IDS_STATUS_LOADED</comment>
        <translation>載入完成</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1260"/>
        <source>Stack delayed</source>
        <comment>IDS_STATUS_STACKDELAYED</comment>
        <translation>疊合延遲</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1263"/>
        <source>Not stackable</source>
        <comment>IDS_STATUS_NOTSTACKABLE</comment>
        <translation>不可疊的</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1345"/>
        <source>No</source>
        <comment>IDS_NO</comment>
        <translation>不</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1347"/>
        <source>Yes</source>
        <comment>IDS_YES</comment>
        <translation>是的</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1350"/>
        <source>RGB %1 bit/ch</source>
        <comment>IDS_FORMAT_RGB</comment>
        <translation>RGB每色 %1 bit</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1352"/>
        <source>Gray %1 bit</source>
        <comment>IDS_FORMAT_GRAY</comment>
        <translation>灰階 %1 bit</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1364"/>
        <source>%1%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1545"/>
        <source>Pending: %1 - Registered: %2 - Stacked: %3 - Total exposure time: %4</source>
        <translation>&quot;待處理: %1 - 已偵測星點: %2 - 已疊合: %3 - 總曝光時間: %4</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="49"/>
        <source>Monitoring</source>
        <comment>IDC_MONITORINGRECT</comment>
        <translation>監看</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="56"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="62"/>
        <source>Click here to select the Monitored Folder</source>
        <comment>IDC_MONITOREDFOLDER</comment>
        <translation>點選此處以選擇欲監視的檔案夾</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="205"/>
        <source>Stacked Image</source>
        <comment>IDC_STACKEDIMAGE</comment>
        <translation>疊合後的影像</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="216"/>
        <source>Last Image</source>
        <comment>IDC_LASTIMAGE</comment>
        <translation>最後一幅</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="221"/>
        <source>Graphs</source>
        <comment>IDC_GRAPHS</comment>
        <translation>圖表</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="235"/>
        <source>Image List</source>
        <comment>IDC_IMAGELIST</comment>
        <translation>影像清單</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="259"/>
        <source>Log</source>
        <comment>IDC_LOGLIST</comment>
        <translation>記錄</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="264"/>
        <source>Settings</source>
        <comment>IDC_WARNINGS</comment>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="280"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="283"/>
        <source>Monitor</source>
        <comment>IDC_MONITOR</comment>
        <translation>監視</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="296"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="299"/>
        <source>Stack</source>
        <comment>IDC_STACK_DSSLIVE</comment>
        <translation>疊合</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="308"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="311"/>
        <source>Stop</source>
        <comment>IDC_STOP</comment>
        <translation>停止</translation>
    </message>
</context>
<context>
    <name>EmailSettings</name>
    <message>
        <location filename="../ui/EmailSettings.ui" line="20"/>
        <source>Sent email settings</source>
        <translation>送出email設定</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="26"/>
        <source>To:</source>
        <translation>送到:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="33"/>
        <source>Send an Email only for the first warning</source>
        <comment>IDC_SENDONCE</comment>
        <translation>只在第一次警告時送出email</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="43"/>
        <source>Subject:</source>
        <translation>標題:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="66"/>
        <source>Email Account settings</source>
        <translation>帳號設定</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="14"/>
        <source>Email Settings</source>
        <translation>Email設定</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="72"/>
        <source>SMTP server:</source>
        <translation>SMTP伺服器</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="82"/>
        <source>Port:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="95"/>
        <location filename="../ui/EmailSettings.ui" line="108"/>
        <source>465</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="113"/>
        <source>587</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="118"/>
        <source>2525</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="126"/>
        <source>Encryption:</source>
        <translation>加密：</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="134"/>
        <source>None</source>
        <translation>沒有任何</translation>
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
        <translation>帳號 (來自):</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="165"/>
        <source>Password:</source>
        <translation>密碼：</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="179"/>
        <source>Show password</source>
        <translation>顯示密碼</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="189"/>
        <source>Test</source>
        <comment>IDC_TEST</comment>
        <translation>測試</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../ui/ImageViewer.ui" line="89"/>
        <source>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Copy current picture to clipboard&lt;/a&gt;</source>
        <comment>IDC_COPYTOCLIPBOARD</comment>
        <translation>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;將本照片覆製至暫存區&lt;/a&gt;</translation>
    </message>
</context>
</TS>
