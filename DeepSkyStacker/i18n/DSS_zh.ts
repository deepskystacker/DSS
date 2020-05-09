<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>About</name>
    <message>
        <location filename="../ui/About.ui" line="14"/>
        <source>About DeepSkyStacker</source>
        <translation>關於DeepSkyStacker</translation>
    </message>
    <message>
        <location filename="../ui/About.ui" line="36"/>
        <source>Language</source>
        <translation>語系</translation>
    </message>
    <message>
        <location filename="../ui/About.ui" line="44"/>
        <source>Use the following language at next startup</source>
        <translation>下次啟動時使用此語系</translation>
    </message>
    <message>
        <location filename="../ui/About.ui" line="105"/>
        <source>Check if newer version is available at startup</source>
        <translation>啟動時檢查是否有新版本</translation>
    </message>
    <message>
        <location filename="../About.cpp" line="23"/>
        <source>DeepSkyStacker version %1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../About.cpp" line="31"/>
        <source>RAW file decoding by LibRaw (version %1)
Copyright © 1997-2019 LibRaw LLC</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../About.cpp" line="38"/>
        <source>TIFF file encoding/decoding by LibTIFF (version %1)
Copyright © 1988-1996 Sam Leffler
Copyright © 1991-1996 Silicon Graphics, Inc.</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../About.cpp" line="45"/>
        <source>FITS decoding by CFitsIO (version %1)
Copyright NASA</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../About.cpp" line="53"/>
        <source>Default</source>
        <translation>預設</translation>
    </message>
</context>
<context>
    <name>AlignmentParameters</name>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="14"/>
        <source>Alignment</source>
        <translation>對齊星點</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="26"/>
        <source>Transformation used during alignment</source>
        <translation>對齊各影像中的星點時所用的轉換法</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="37"/>
        <source>Automatic</source>
        <translation>系統自動決定</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="57"/>
        <source>Bilinear</source>
        <translation>雙線性</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="77"/>
        <source>Bisquared</source>
        <translation>雙二次方</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="97"/>
        <source>Bicubic</source>
        <translation>雙三次方</translation>
    </message>
    <message>
        <location filename="../ui/AlignmentParameters.ui" line="117"/>
        <source>No alignment</source>
        <translation>不執行對齊各幅星點</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="23"/>
        <source>Automatic

The alignment method is automatically selected depending on the number of available stars.</source>
        <translation>自動

視星星數目自動決定星點對齊方式。</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="26"/>
        <source>Bilinear Alignment

The Bilinear Alignment is used in all cases.</source>
        <translation>雙線性

一律使用雙線性法對齊星點。</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="29"/>
        <source>Bisquared Alignment

The Bisquared Alignment is used when at least 25 stars are available, else the Bilinear Alignment method is used.</source>
        <translation>雙平方

在有超過25個星點時使用雙平方法，否則使用雙線性。</translation>
    </message>
    <message>
        <location filename="../AlignmentParameters.cpp" line="32"/>
        <source>Bicubic Alignment

The Bicubic method is used when at least 40 stars are available, then the Bisquared method is used if 25 to 39 stars are available, then the Bilinear method is used when less than 25 stars are available.</source>
        <translation>雙立方

超過40個星點時使用雙立方法，25到39個星點時使用雙平方，25以下使用雙線性。</translation>
    </message>
</context>
<context>
    <name>AskRegistering</name>
    <message>
        <location filename="../ui/AskRegistering.ui" line="14"/>
        <source>Unregistered light frame</source>
        <translation>尚未偵測星點的天體影像</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="37"/>
        <source>This light frame is not registered
(the stars have not been detected).

Do you want...</source>
        <translation>本天體影像尚未通過星點偵測

您要....</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="66"/>
        <source>to register this light frame</source>
        <translation>偵測本幅影像中的星點</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="73"/>
        <source>to register all the light frames</source>
        <translation>偵測所有影像中的星點</translation>
    </message>
    <message>
        <location filename="../ui/AskRegistering.ui" line="80"/>
        <source>to continue without registering the light frame</source>
        <translation>繼續處理但不偵測本幅影像中的星點</translation>
    </message>
</context>
<context>
    <name>BackgroundOptions</name>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="14"/>
        <source>Background Options</source>
        <translation>背景選項</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="20"/>
        <source>Calibration Method</source>
        <translation>校正方法</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="28"/>
        <source>Linear</source>
        <translation>線性</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="35"/>
        <source>Rational</source>
        <translation>有理數</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="60"/>
        <source>RGB Background Calibration Method</source>
        <translation>RGB背景校正方式</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="68"/>
        <source>None</source>
        <translation>無</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="75"/>
        <source>Minimum</source>
        <translation>極小值</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="82"/>
        <source>Middle</source>
        <translation>中間值</translation>
    </message>
    <message>
        <location filename="../ui/BackgroundOptions.ui" line="89"/>
        <source>Maximum</source>
        <translation>最大值</translation>
    </message>
</context>
<context>
    <name>BatchStacking</name>
    <message>
        <location filename="../ui/BatchStacking.ui" line="14"/>
        <source>Batch Stacking</source>
        <translation>批次疊合</translation>
    </message>
    <message>
        <location filename="../ui/BatchStacking.ui" line="20"/>
        <source>This dialog is used to launch the stacking process from existing image lists.

Add the file lists you want to stack and click OK to start the stacking processes.</source>
        <translation>本對話框可用於疊合既有的影像清單

加入您欲疊合的影像清單並按OK鍵以開始疊合</translation>
    </message>
    <message>
        <location filename="../ui/BatchStacking.ui" line="34"/>
        <source>Add File Lists...</source>
        <translation>增加檔案清單</translation>
    </message>
    <message>
        <location filename="../ui/BatchStacking.ui" line="54"/>
        <source>Clear List</source>
        <translation>清除影像清單</translation>
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
        <translation>彗星</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="20"/>
        <source>Comet Stacking Parameters</source>
        <translation>彗星疊合參數</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="50"/>
        <source>Standard Stacking</source>
        <translation>標準疊合</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="60"/>
        <source>Comet position is ignored.
The comet will be fuzzy.</source>
        <translation>不考慮彗星位置。
疊出的彗星會變模糊。</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="87"/>
        <source>Comet Stacking</source>
        <translation>彗星疊合</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="97"/>
        <source>Comet position is used.
The comet will be sharp and the stars will have trails.</source>
        <translation>使用彗星位置。
疊出的彗星會是清析的，恆星則會拖線。</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="124"/>
        <source>Stars + Comet Stacking</source>
        <translation>恆星加彗星疊合</translation>
    </message>
    <message>
        <location filename="../ui/CometStacking.ui" line="134"/>
        <source>Comet position is used.
The comet and the stars will be sharp. This process is twice longer.</source>
        <translation>使用彗星的位置。
疊出的彗星與恆星都會是清析的。處理時間會加倍。</translation>
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
        <translation>偵測星點與疊合</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="80"/>
        <source>Open picture files...</source>
        <translation>開啟影像檔...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="90"/>
        <source>dark files...</source>
        <translation>暗電流檔...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="103"/>
        <source>flat files...</source>
        <translation>平場檔案...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="113"/>
        <source>dark flat files...</source>
        <translation>平場暗電流檔...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="123"/>
        <source>offset/bias files...</source>
        <translation>偏壓檔...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="146"/>
        <source>Open a File List...</source>
        <translation>開啟檔案清單...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="153"/>
        <source>Save the FIle List...</source>
        <translation>儲存檔案清單...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="176"/>
        <source>Clear List</source>
        <translation>清除影像清單</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="207"/>
        <source>Check all</source>
        <translation>勾選全部</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="214"/>
        <source>Check above a threshold...</source>
        <translation>勾選高於某臨界值者...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="221"/>
        <source>Uncheck all</source>
        <translation>取消全部的勾選</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="252"/>
        <source>Register checked pictures...</source>
        <translation>偵測勾選影像中的星點...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="259"/>
        <source>Compute offsets...</source>
        <translation>計算位移量...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="266"/>
        <source>Stack checked pictures...</source>
        <translation>疊合勾選之影像...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="273"/>
        <source>Batch stacking...</source>
        <translation>批次疊合...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="286"/>
        <source>Processing</source>
        <translation>影像處理</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="307"/>
        <source>Open picture file...</source>
        <translation>開啟影像檔...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="314"/>
        <source>Copy current picture to clipboard</source>
        <translation>將本照片覆製至暫存區</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="321"/>
        <source>Create a Star Mask...</source>
        <translation>製作星像遮罩...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="328"/>
        <source>Save picture to file...</source>
        <translation>儲存影像...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="338"/>
        <source>Options</source>
        <translation>選項</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="359"/>
        <source>Settings...</source>
        <translation>設定...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="369"/>
        <source>Raw/FITS DDP Settings...</source>
        <translation>Raw/FITS檔數位顯像設定...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="379"/>
        <source>Load...</source>
        <translation>載入...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="389"/>
        <source>Save...</source>
        <translation>儲存...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="399"/>
        <source>Recommended...</source>
        <translation>建議值...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="406"/>
        <source>About DeepSkyStacker...</source>
        <translation>關於DeepSkyStacker...</translation>
    </message>
    <message>
        <location filename="../ui/ExplorerBar.ui" line="413"/>
        <source>DeepSkyStacker&apos;s Help...</source>
        <translation>DeepSkyStacker說明...</translation>
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
        <translation>FITS檔</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="35"/>
        <source>Monochrome 16 bit FITS Files are RAW files created by a DSLR or a color CCD camera</source>
        <translation>單色16 bit FITS檔乃由DSLR或彩色CCD相機產生</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="49"/>
        <source>Camera:</source>
        <translation>相機:</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="63"/>
        <source>Color Adjistment</source>
        <translation>色彩調整</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="102"/>
        <source>Brightness</source>
        <translation>亮度</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="109"/>
        <source>Red scale</source>
        <translation>紅色比率</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="116"/>
        <source>Blue scale</source>
        <translation>藍色比率</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="126"/>
        <source>Bayer Pattern Filter used</source>
        <translation>所使用的Bayer濾鏡配置</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="150"/>
        <source>Bayer Matrix Transformation</source>
        <translation>Bayer矩陣轉換</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="159"/>
        <source>Bilinear Interpolation</source>
        <translation>雙線性內插</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="179"/>
        <source>Adaptive Homogeneity-Directed (AHD) Interpolation</source>
        <translation>自調適均質性導向(AHD)內插法</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="199"/>
        <source>Use Bayer Drizzle algorithm (no interpolation, no debayerization)</source>
        <translation>使用Bayer Drizzle (無內插，不解譯Bayer矩陣)</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="215"/>
        <source>This option uses the Bayer matrix as is. No interpolation is done and each pixel is given only primary components from the matrix.</source>
        <translation>本選項不對Bayer矩陣作任何更動與內插，每個像元只取Bayer矩陣中原有的顏色。</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="238"/>
        <source>Create super-pixels from the raw Bayer matrix (no interpolation)</source>
        <translation>自原始Bayer矩陣產生大像元 (無內插)</translation>
    </message>
    <message>
        <source>RadioButton</source>
        <translation type="vanished">自原始Bayer矩陣產生大像元 (無內插)</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="254"/>
        <source>This option uses the Bayer matrix to create one super-pixel from each group of 4 pixels (RGBG). The sizes of the resulting image are thus divided by two.</source>
        <translation>本選項將以Bayer矩陣中每四個GRGB像元合併成一個大像元，因此產生的影像長寬將各減半。</translation>
    </message>
    <message>
        <location filename="../ui/FitsFilesTab.ui" line="267"/>
        <source>Force use of unsigned values when signed values are stored</source>
        <translation>檔案中的數值有正負號時，強制忽略正負號。</translation>
    </message>
</context>
<context>
    <name>IntermediateFiles</name>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="14"/>
        <source>Intermediate Files</source>
        <translation>中介檔</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="20"/>
        <source>Intermediate files creation settings</source>
        <translation>中介檔儲存設定</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="27"/>
        <source>Create a calibrated file for each light frame</source>
        <translation>每幅天體影像校正後都儲存校正結果</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="37"/>
        <source>Save a debayered image when processing RAW images</source>
        <translation>若為RAW檔則儲存解譯Bayer矩陣後的結果</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="60"/>
        <source>Create a registered/calibrated file for each light frame</source>
        <translation>儲存每幅天體影像校正/星點對齊後的結果</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="80"/>
        <source>Intermediate and Final Image File Format</source>
        <translation>中介檔格式</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="86"/>
        <source>TIFF Files</source>
        <translation>將中介檔存為TIFF格式</translation>
    </message>
    <message>
        <location filename="../ui/IntermediateFiles.ui" line="93"/>
        <source>FITS Files</source>
        <translation>將中介檔存為FITS格式FITS檔</translation>
    </message>
</context>
<context>
    <name>OutputTab</name>
    <message>
        <location filename="../ui/OutputTab.ui" line="14"/>
        <source>Output</source>
        <translation>輸出</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="20"/>
        <source>Output Files Settings</source>
        <translation>輸出影像設定</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="29"/>
        <source>Create Output file</source>
        <translation>產生輸出檔</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="36"/>
        <source>Create HTML Description file</source>
        <translation>產生HTML敘述檔</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="45"/>
        <source>Output File Name</source>
        <translation>輸出檔名</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="53"/>
        <source>Autosave.tif/fits</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="60"/>
        <source>&lt;file list name&gt;.tif/fits</source>
        <translation>&lt;檔案清單名稱&gt;.tif</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="71"/>
        <source>Append a number to avoid file overwrite (001, 002, ...)</source>
        <translation>添附數字以避免檔案蓋寫 (001, 002, ...)</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="83"/>
        <source>Output Location</source>
        <translation>輸出位置</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="94"/>
        <source>Create Output file in the folder of the reference frame</source>
        <translation>於參考影像所在的檔案夾產生輸出檔</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="108"/>
        <source>Create Output file in the folder of the file list</source>
        <translation>於檔案清單所在的檔案夾產生輸出檔</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="122"/>
        <source>Create Output file in</source>
        <translation>產生輸出檔於</translation>
    </message>
    <message>
        <location filename="../ui/OutputTab.ui" line="138"/>
        <source>&lt;Output Folder&gt;</source>
        <translation>&lt;輸出檔案夾&gt;</translation>
    </message>
</context>
<context>
    <name>PostCalibration</name>
    <message>
        <location filename="../ui/PostCalibration.ui" line="14"/>
        <source>Cosmetic</source>
        <translation>外觀修飾</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="20"/>
        <source>Post Calibration Cosmetic settings</source>
        <translation>影像校正後的外觀修飾設定</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="27"/>
        <source>Detect and Clean remaining Hot Pixels</source>
        <translation>偵測並移除殘餘的熱像元</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="41"/>
        <location filename="../ui/PostCalibration.ui" line="114"/>
        <location filename="../ui/PostCalibration.ui" line="165"/>
        <location filename="../ui/PostCalibration.ui" line="232"/>
        <source>TextLabel</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="65"/>
        <location filename="../ui/PostCalibration.ui" line="183"/>
        <source>&lt;small&gt;Effect weak&lt;/small&gt;</source>
        <translation>&lt;small&gt;效果較弱&lt;/small&gt;</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="75"/>
        <location filename="../ui/PostCalibration.ui" line="193"/>
        <source>&lt;small&gt;Strong&lt;/small&gt;</source>
        <translation>&lt;small&gt;較強&lt;/small&gt;</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="87"/>
        <location filename="../ui/PostCalibration.ui" line="205"/>
        <source>Filter Size</source>
        <translation>濾波器大小</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="107"/>
        <location filename="../ui/PostCalibration.ui" line="225"/>
        <source>Detection Threshold</source>
        <translation>偵測臨界值</translation>
    </message>
    <message>
        <location filename="../ui/PostCalibration.ui" line="151"/>
        <source>Detect and Clean remaining Cold Pixels</source>
        <translation>偵測並移除殘餘的冷像元</translation>
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
        <translation type="unfinished">每幅天體影像都儲存一幅顯示哪些像元被移除的影像</translation>
    </message>
</context>
<context>
    <name>QPlatformTheme</name>
    <message>
        <location filename="../DeepSkyStacker.cpp" line="19"/>
        <source>Apply</source>
        <translation>套用</translation>
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
    <message>
        <location filename="../DeepSkyStacker.cpp" line="18"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
</context>
<context>
    <name>RawDDPSettings</name>
    <message>
        <location filename="../ui/RawDDPSettings.ui" line="14"/>
        <source>RAW/FITS Digital Development Process Settings</source>
        <translation>RAW/FITS 數位顯像設定</translation>
    </message>
</context>
<context>
    <name>RawFilesTab</name>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="14"/>
        <source>RAW Files</source>
        <translation>RAW檔</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="22"/>
        <source>Color Adjustment</source>
        <translation>色彩調整</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="31"/>
        <source>Brightness</source>
        <translation>亮度</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="48"/>
        <source>Red Scale</source>
        <translation>紅色比率</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="65"/>
        <source>Blue Scale</source>
        <translation>藍色比率</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="85"/>
        <source>White Balance</source>
        <translation>白平衡</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="91"/>
        <source>No White Balance Processing</source>
        <translation>沒有白平衡處理</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="98"/>
        <source>Use Camera White Balance</source>
        <translation>使用相機白平衡</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="110"/>
        <source>Bayer Matrix Transformation</source>
        <translation>Bayer矩陣轉換</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="116"/>
        <source>Bilinear Interpolation</source>
        <translation>雙線性內插</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="136"/>
        <source>Adaptive Homogeneity-Directed (AHD) Interpolation</source>
        <translation>自調適均質性導向(AHD)內插法</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="156"/>
        <source>Use Bayer Drizzle algorithm (no interpolation, no debayerization)</source>
        <translation>使用Bayer Drizzle (無內插，不解譯Bayer矩陣)</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="172"/>
        <source>This option uses the Bayer matrix as is. No interpolation is done and each pixel is given only primary components from the matrix.</source>
        <translation>本選項不對Bayer矩陣作任何更動與內插，每個像元只取Bayer矩陣中原有的顏色。</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="198"/>
        <source>Create super- pixels from the raw Bayer matrix (no interpolation)</source>
        <translation>自原始Bayer矩陣產生大像元 (無內插)</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="214"/>
        <source>This option uses the Bayer matrix to create one super-pixel from each group of 4 pixels (RGBG). The sizes of the resulting image are thus divided by two.</source>
        <translation>本選項將以Bayer矩陣中每四個GRGB像元合併成一個大像元，因此產生的影像長寬將各減半。</translation>
    </message>
    <message>
        <location filename="../ui/RawFilesTab.ui" line="230"/>
        <source>Set the black point to 0</source>
        <translation>設定黑色值為0</translation>
    </message>
</context>
<context>
    <name>ResultParameters</name>
    <message>
        <location filename="../ui/ResultParameters.ui" line="14"/>
        <source>Result</source>
        <translation>結果</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="42"/>
        <source>Standard Mode</source>
        <translation>標準模式</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="62"/>
        <source>&quot;Mosaic&quot; Mode</source>
        <translation>馬賽克模式</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="82"/>
        <source>Intersection Mode</source>
        <translation>交疊模式</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="102"/>
        <source>Custom Rectangle</source>
        <translation>自訂方型區域</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="132"/>
        <source>Enable 2x Drizzle</source>
        <translation>使用二倍Drizzle</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="139"/>
        <source>Enable 3x Drizzle</source>
        <translation>使用三倍Drizzle</translation>
    </message>
    <message>
        <location filename="../ui/ResultParameters.ui" line="159"/>
        <source>Align RGB Channels in final image</source>
        <translation>在最終影像中對齊RGB三色</translation>
    </message>
</context>
<context>
    <name>StackSettings</name>
    <message>
        <location filename="../ui/StackSettings.ui" line="14"/>
        <source>Dialog</source>
        <translation>疊合參數</translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="41"/>
        <source>Temporary files folder:</source>
        <translation>暫存檔檔案夾:</translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="67"/>
        <source>...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="83"/>
        <source>Reduce worker threads priority</source>
        <translation>降低執行緒的優先性</translation>
    </message>
    <message>
        <location filename="../ui/StackSettings.ui" line="90"/>
        <source>Use all available processors</source>
        <translation>使用所有可用的處理器</translation>
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
        <translation>疊合中</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="26"/>
        <source>Stacking Mode</source>
        <translation>疊合方式</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="36"/>
        <source>Average</source>
        <translation>平均</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="56"/>
        <source>Median</source>
        <translation>中位數</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="63"/>
        <source>Median Kappa-Sigma clipping</source>
        <translation>中位數Kappa-Sigma剪去法</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="70"/>
        <source>Kappa-Sigma clipping</source>
        <translation>Kappa-Sigma剪去法</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="77"/>
        <source>Entropy Weighted Average
(High Dynamic Range)</source>
        <translation>熵加權平均 (高動態範圍)</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="98"/>
        <source>Maximum</source>
        <translation>最大值</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="113"/>
        <source>Kappa:</source>
        <translation>Kappa值:</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="120"/>
        <source>Number of iterations:</source>
        <translation>重覆次數:</translation>
    </message>
    <message>
        <location filename="../ui/StackingParameters.ui" line="213"/>
        <source>Auto Adaptive Weighted Average</source>
        <translation>自動可調適式加權平均</translation>
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
