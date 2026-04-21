<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja_JP">
<context>
    <name>DSS::ChartTab</name>
    <message>
        <location filename="../ui/ChartTab.ui" line="14"/>
        <source>Form</source>
        <translation>フォーム</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="31"/>
        <location filename="../ChartTab.cpp" line="39"/>
        <source>Quality</source>
        <translation>スコア</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="47"/>
        <location filename="../ChartTab.cpp" line="46"/>
        <source>FWHM</source>
        <comment>IDC_FWHM</comment>
        <translation>FWHM（半値全幅）</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="63"/>
        <location filename="../ChartTab.cpp" line="53"/>
        <source>#Stars</source>
        <comment>IDC_STARS</comment>
        <translation>星の数</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="79"/>
        <source>dX/dY</source>
        <comment>IDC_OFFSET</comment>
        <translation>dX/dY（オフセット）</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="95"/>
        <location filename="../ChartTab.cpp" line="77"/>
        <source>Angle</source>
        <comment>IDC_ANGLE</comment>
        <translation>回転角</translation>
    </message>
    <message>
        <location filename="../ui/ChartTab.ui" line="111"/>
        <location filename="../ChartTab.cpp" line="85"/>
        <source>Sky Background</source>
        <comment>IDC_SKYBACKGROUND</comment>
        <translation>スカイバックグラウンド</translation>
    </message>
</context>
<context>
    <name>DSS::EmailSettings</name>
    <message>
        <location filename="../emailsettings.cpp" line="209"/>
        <source>Failed to connect to email server %1 (%2)!</source>
        <translation>メールサーバー %1 への接続に失敗しました (%2)!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="217"/>
        <source>Failed to login to email server as %1!</source>
        <translation>%1 としてのメールサーバーへのログインに失敗しました!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="226"/>
        <source>Failed to send mail!</source>
        <translation>メールの送信に失敗しました!</translation>
    </message>
    <message>
        <location filename="../emailsettings.cpp" line="231"/>
        <source>Test email sent OK!</source>
        <translation>テストメールを送信しました!</translation>
    </message>
</context>
<context>
    <name>DSS::FileRegistrar</name>
    <message>
        <location filename="../fileregistrar.cpp" line="128"/>
        <source>-&gt; New file: %1
</source>
        <comment>IDS_LOG_NEWFILE</comment>
        <translation>-&gt; 新規ファイル: %1
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="152"/>
        <source>Loading %1 bit/ch %2 light frame
%3</source>
        <comment>IDS_LOADRGBLIGHT</comment>
        <translation>ライトフレームを読み込み中: %1 bit/ch %2
%3</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="154"/>
        <source>Loading %1 bits gray %2 light frame
%3</source>
        <comment>IDS_LOADGRAYLIGHT</comment>
        <translation>ライトフレームを読み込み中: %1 bit グレースケール %2
%3</translation>
    </message>
    <message numerus="yes">
        <location filename="../fileregistrar.cpp" line="183"/>
        <source>Image %1 registered: %n star(s) detected - FWHM = %L2 - Quality = %L3</source>
        <comment>IDS_LOG_REGISTERRESULTS</comment>
        <translation>
            <numerusform>画像 %1 を登録済み: %n 個の星を検出 - FWHM = %L2 - スコア = %L3</numerusform>
        </translation>
<comment>IDS_LOG_REGISTERRESULTS</comment>
        <translation>
            <numerusform>画像 %1 を登録済み: %n 個の星を検出 - FWHM = %L2 - スコア = %L3</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="237"/>
        <location filename="../fileregistrar.cpp" line="288"/>
        <source>Quality (%L1) is less than %L2</source>
        <comment>IDS_NOSTACK_SCORE</comment>
        <translation>スコア (%L1) が %L2 未満です</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="199"/>
        <source>Warning: Image %1 -> %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>警告: 画像 %1 -> %2
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="212"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>画像 %1 はスタッキング不可能です (%2)
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="220"/>
        <source>Error loading file %1
</source>
        <comment>IDS_LOG_ERRORLOADINGFILE</comment>
        <translation>ファイル %1 の読み込みエラー
</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="247"/>
        <location filename="../fileregistrar.cpp" line="298"/>
        <source>Star count(%L1) is less than %L2</source>
        <translation>星の数 (%L1) が %L2 未満です</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="257"/>
        <location filename="../fileregistrar.cpp" line="308"/>
        <source>FWHM (%L1 pixels) is greater than %L2 pixels</source>
        <comment>IDS_NOSTACK_FWHM</comment>
        <translation>FWHM (%L1 ピクセル) が %L2 ピクセルを超えています</translation>
    </message>
    <message>
        <location filename="../fileregistrar.cpp" line="267"/>
        <location filename="../fileregistrar.cpp" line="318"/>
        <source>Sky Background (%L1%) is greater than %L2%</source>
        <comment>IDS_NOSTACK_SKYBACKGROUND</comment>
        <translation>スカイバックグラウンド (%L1%) が %L2% を超えています</translation>
    </message>
</context>
<context>
    <name>DSS::FileStacker</name>
    <message>
        <location filename="../filestacker.cpp" line="237"/>
        <source>Warning: Image %1 -> %2
</source>
        <comment>IDS_LOG_WARNING</comment>
        <translation>警告: 画像 %1 -> %2
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="266"/>
        <source>No transformation found from reference frame</source>
        <comment>IDS_NOSTACK_NOTRANSFORMATION</comment>
        <translation>基準フレームからの変換係数が見つかりません</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="272"/>
        <source>Image %1 is not stackable (%2)
</source>
        <comment>IDS_LOG_IMAGENOTSTACKABLE1</comment>
        <translation>画像 %1 はスタッキング不可能です (%2)
</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="290"/>
        <location filename="../filestacker.cpp" line="325"/>
        <source>dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels</source>
        <comment>IDS_NOSTACK_OFFSET</comment>
        <translation>dX (%L1 ピクセル) または dY (%L2 ピクセル) が %L3 ピクセルを超えています</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="305"/>
        <location filename="../filestacker.cpp" line="344"/>
        <source>Angle (%L1°) is greater than %L2°</source>
        <comment>IDS_NOSTACK_ANGLE</comment>
        <translation>角度 (%L1°) が %L2° を超えています</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="373"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>スタッキング済み画像を保存しました</translation>
    </message>
    <message>
        <location filename="../filestacker.cpp" line="398"/>
        <source>Image %1 has been added to the stack</source>
<translation>画像 %1 がスタックに追加されました</translation>
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
        <translation>Ctrl++ または Ctrl+= で拡大
Ctrl+- で縮小
 またはマウスホイールでズーム
Ctrl+4 で四隅表示モードの切り替え</translation>
    </message>
</context>
<context>
    <name>DSS::ImageViewer</name>
    <message>
        <location filename="../imageviewer.cpp" line="208"/>
        <source>The stacked image will be saved as soon as possible</source>
        <comment>IDS_STACKEDIMAGEWILLBESAVED</comment>
        <translation>スタッキング済み画像は準備ができ次第保存されます</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="220"/>
        <source>You must select an output folder first.
Go to the Settings tab to select the Stacked Image Output Folder.</source>
        <comment>IDS_NOSTACKEDIMAGEFOLDER</comment>
        <translation>最初に出力フォルダを選択する必要があります。
「設定」タブからスタッキング済み画像の出力フォルダを選択してください。</translation>
    </message>
    <message>
        <location filename="../imageviewer.cpp" line="230"/>
        <source>The stacked image has been saved</source>
        <comment>IDS_STACKEDIMAGESAVED</comment>
        <translation>スタッキング済み画像を保存しました</translation>
    </message>
</context>
<context>
    <name>DSS::RestartMonitoring</name>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="14"/>
        <source>Restart Monitoring</source>
        <comment>IDD_RESTARTMONITORING</comment>
        <translation>監視を再開</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="20"/>
        <source>Before restarting do you want to</source>
        <translation>再開する前に以下のどちらを行いますか？</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="44"/>
        <source>Create a new empty stacked image</source>
        <comment>IDC_CREATENEWSTACKEDIMAGE</comment>
        <translation>新しく空のスタッキング済み画像を作成する</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="74"/>
        <location filename="../ui/RestartMonitoring.ui" line="161"/>
        <source>or</source>
        <translation>または</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="101"/>
        <source>Start from the current stacked image</source>
        <comment>IDC_STARTFROMCURRENTSTACKEDIMAGE</comment>
        <translation>現在のスタッキング済み画像から開始する</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="131"/>
        <source>Drop all the pending images</source>
        <comment>IDC_DROPPENDINGIMAGES</comment>
        <translation>待機中の画像をすべて破棄する</translation>
    </message>
    <message>
        <location filename="../ui/RestartMonitoring.ui" line="188"/>
        <source>Use and process the pending images</source>
        <comment>IDC_USEPENDINGIMAGES</comment>
        <translation>待機中の画像を使用・処理する</translation>
    </message>
</context>
<context>
    <name>DSS::SettingsTab</name>
    <message>
        <location filename="../ui/SettingsTab.ui" line="14"/>
        <source>Form</source>
        <translation>フォーム</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="65"/>
        <source>Stacking</source>
        <translation>スタッキング</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="71"/>
        <source>Don&apos;t stack until at least</source>
        <comment>IDC_DONTSTACK</comment>
        <translation>以下の条件を満たすまでスタッキングしない： 最低</translation>
    </message>
<message>
        <location filename="../ui/SettingsTab.ui" line="91"/>
        <source>images are available</source>
        <translation>枚の画像が利用可能になるまで</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="127"/>
        <source>Warn  Don&apos;t stack if...</source>
        <translation>警告・スタッキング停止条件...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="169"/>
        <source>... quality is less than</source>
        <translation>... スコアが次よりも低い場合：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="237"/>
        <source>... star count is less than</source>
        <comment>IDC_DONTSTACK_STARS</comment>
        <translation>... 星の数が次よりも少ない場合：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="305"/>
        <source>... sky background is greater than</source>
        <comment>IDC_DONTSTACK_SKYBACKGROUND</comment>
        <translation>... スカイバックグラウンドが次よりも大きい場合：</translation>
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
        <translation>... FWHMが次よりも大きい場合：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="400"/>
        <location filename="../ui/SettingsTab.ui" line="475"/>
        <source>pixels</source>
        <translation>ピクセル</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="455"/>
        <source>... dX or dY is greater than</source>
        <comment>IDC_DONTSTACK_OFFSET</comment>
        <translation>... dX または dY が次よりも大きい場合：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="524"/>
        <source>... angle is greater than</source>
        <translation>... 角度が次よりも大きい場合：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="544"/>
        <source>°</source>
        <translation>°</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="564"/>
        <source>Move non-stackable files to the &apos;NonStackable&apos; sub-folder</source>
        <translation>スタッキング不可のファイルを &apos;NonStackable&apos; サブフォルダに移動する</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="577"/>
        <source>(the &apos;NonStackable&apos; sub folder will be created if necessary)</source>
        <translation>（&apos;NonStackable&apos; フォルダは必要に応じて作成されます）</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="605"/>
        <source>Warnings</source>
        <translation>警告方法</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="611"/>
        <source>Flash Application</source>
        <comment>IDC_WARN_FLASH</comment>
        <translation>アプリケーションを点滅させる</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="634"/>
        <location filename="../settingstab.cpp" line="87"/>
        <location filename="../settingstab.cpp" line="251"/>
        <location filename="../settingstab.cpp" line="405"/>
        <location filename="../settingstab.cpp" line="449"/>
        <source>Click here to set or change the email address</source>
        <comment>IDC_EMAIL</comment>
        <translation>ここをクリックしてメールアドレスを設定または変更</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="641"/>
        <source>Sound</source>
        <comment>IDC_WARN_SOUND</comment>
        <translation>音を鳴らす</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="648"/>
        <source>Send email to:</source>
<comment>IDC_WARN_EMAIL</comment>
        <translation>メールを送信する：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="658"/>
        <location filename="../settingstab.cpp" line="88"/>
        <location filename="../settingstab.cpp" line="262"/>
        <location filename="../settingstab.cpp" line="464"/>
        <source>Click here to select the warning file folder</source>
        <comment>IDC_WARNINGFILEFOLDER</comment>
        <translation>ここをクリックして警告ファイル用フォルダを選択</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="665"/>
        <source>Create warning file in:</source>
        <translation>警告ファイルの作成先：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="685"/>
        <source>Reset email count</source>
        <comment>IDC_RESETEMAILCOUNT</comment>
        <translation>メール送信数をリセット</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="713"/>
        <source>Options</source>
        <comment>IDC_OPTIONS</comment>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="719"/>
        <source>Save stacked image to file every</source>
        <comment>IDC_SAVESTACKEDIMAGE</comment>
        <translation>スタッキング済み画像の保存間隔：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="739"/>
        <source>images</source>
        <translation>枚ごと</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="759"/>
        <source>Output folder:</source>
        <translation>出力フォルダ：</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="769"/>
        <location filename="../settingstab.cpp" line="89"/>
        <location filename="../settingstab.cpp" line="270"/>
        <location filename="../settingstab.cpp" line="435"/>
        <source>Click here to select the stacked image output folder</source>
        <comment>IDC_STACKEDOUTPUTFOLDER</comment>
        <translation>ここをクリックしてスタッキング済み画像の出力フォルダを選択</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="789"/>
        <source>The trace file is normally written to the DeepSkyStacker directory in the users
&quot;Documents&quot; directory, and is deleted on exit (unless the application crashes).
Tick this box to keep the trace file.</source>
        <translation>トレースファイルは通常、ユーザーの「ドキュメント」内にある DeepSkyStacker ディレクトリに書き込まれ、終了時に削除されます（アプリケーションがクラッシュした場合を除く）。
トレースファイルを保持したい場合は、このボックスにチェックを入れてください。</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="794"/>
        <source>Retain the trace file on exit</source>
        <translation>終了時にトレースファイルを保持する</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="804"/>
        <source>Reset</source>
        <translation>リセット</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="835"/>
        <source>Filters</source>
        <comment>IDC_FILTERS</comment>
        <translation>フィルタ</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="841"/>
        <source>Process only ...</source>
        <translation>処理対象の限定...</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="848"/>
        <source>RAW images (CR2, NEF, ORF, DNG...)</source>
        <comment>IDC_PROCESS_RAW</comment>
        <translation>RAW画像 (CR2, NEF, ORF, DNG...)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="855"/>
        <source>TIFF images</source>
        <comment>IDC_PROCESS_TIFF</comment>
        <translation>TIFF画像</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="875"/>
        <source>FITS images</source>
        <comment>IDC_PROCESS_FITS</comment>
        <translation>FITS画像</translation>
    </message>
<message>
        <location filename="../ui/SettingsTab.ui" line="882"/>
        <source>Other images (JPEG, PNG)</source>
        <comment>IDC_PROCESS_OTHERS</comment>
        <translation>その他の画像 (JPEG, PNG)</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="937"/>
        <source>Apply changes</source>
        <comment>IDC_APPLYCHANGES</comment>
        <translation>変更を適用</translation>
    </message>
    <message>
        <location filename="../ui/SettingsTab.ui" line="944"/>
        <source>Cancel changes</source>
        <comment>IDC_CANCELCHANGES</comment>
        <translation>変更をキャンセル</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="356"/>
        <source>Select Warning File Folder</source>
        <comment>IDS_SELECTWARNINGFOLDER</comment>
        <translation>警告ファイルの作成先フォルダを選択</translation>
    </message>
    <message>
        <location filename="../settingstab.cpp" line="375"/>
        <source>Select Stacked Image Output Folder</source>
        <comment>IDS_SELECTSTACKEDFOLDER</comment>
        <translation>スタッキング済み画像の出力先フォルダを選択</translation>
    </message>
</context>
<context>
    <name>DeepSkyStackerLive</name>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="49"/>
        <source>Monitoring</source>
        <comment>IDC_MONITORINGRECT</comment>
        <translation>監視設定</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="56"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="62"/>
        <source>Click here to select the Monitored Folder</source>
        <comment>IDC_MONITOREDFOLDER</comment>
        <translation>ここをクリックして監視フォルダを選択</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="205"/>
        <source>Stacked Image</source>
        <comment>IDC_STACKEDIMAGE</comment>
        <translation>スタック済み画像</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="216"/>
        <source>Last Image</source>
        <comment>IDC_LASTIMAGE</comment>
        <translation>最新の画像</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="221"/>
        <source>Graphs</source>
        <comment>IDC_GRAPHS</comment>
        <translation>グラフ</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="235"/>
        <source>Image List</source>
        <comment>IDC_IMAGELIST</comment>
        <translation>画像リスト</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="259"/>
        <source>Log</source>
        <comment>IDC_LOGLIST</comment>
        <translation>ログ</translation>
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
        <translation>監視開始</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="296"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="299"/>
        <source>Stack</source>
        <comment>IDC_STACK_DSSLIVE</comment>
        <translation>スタック開始</translation>
    </message>
    <message>
        <location filename="../ui/DeepSkyStackerLive.ui" line="308"/>
        <location filename="../ui/DeepSkyStackerLive.ui" line="311"/>
        <source>Stop</source>
<comment>IDC_STOP</comment>
        <translation>停止</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="157"/>
        <source>This beta version of DeepSkyStacker has expired
You can probably get another one or download the final release from the web site.</source>
        <translation>このベータ版DeepSkyStackerは有効期限が切れています
ウェブサイトから新しいバージョンまたは正式リリース版をダウンロードしてください。</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="456"/>
        <source>
How to use  DeepSkyStacker Live ?
</source>
        <comment>IDS_LOG_STARTING</comment>
        <translation>
DeepSkyStacker Live の使い方
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="459"/>
        <source>Step 1
Check the Settings tabs for all the stacking and warning settings

</source>
        <comment>IDS_LOG_STARTING_1</comment>
        <translation>ステップ 1
設定タブでスタッキングと警告の全設定を確認します

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="462"/>
        <source>Step 2
Click on the Monitor button to start monitoring the folder
When monitoring is active incoming images are only registered but not stacked.

</source>
        <comment>IDS_LOG_STARTING_2</comment>
        <translation>ステップ 2
「監視開始」ボタンをクリックして、フォルダの監視を開始します
監視中は、新着画像は登録（検出）のみ行われ、スタッキングはされません。

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="466"/>
        <source>Step 3
To start stacking the images click on the Stack button
At this point all the incoming (and all previously registered) images will be stacked.
</source>
        <comment>IDS_LOG_STARTING_3</comment>
        <translation>ステップ 3
画像のスタッキングを開始するには「スタック開始」ボタンをクリックします
この時点で、新着画像（およびそれまでに登録されたすべての画像）がスタッキングされます。
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="470"/>
        <source>You can pause/restart the stacking process by clicking on the Stack button.
</source>
        <comment>IDS_LOG_STARTING_4</comment>
        <translation>「スタック開始」ボタンを再度クリックすることで、スタッキング処理を一時停止/再開できます。
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="473"/>
        <source>To stop monitoring and stacking click on the Stop button.

</source>
        <comment>IDS_LOG_STARTING_5</comment>
        <translation>監視とスタッキングを停止するには「停止」ボタンをクリックしてください。

</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="526"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1211"/>
        <source>No stacked image</source>
        <comment>IDS_NOSTACKEDIMAGE</comment>
        <translation>スタック済み画像なし</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="527"/>
        <source>No image loaded</source>
        <comment>IDS_NOIMAGELOADED</comment>
        <translation>読み込まれた画像なし</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="581"/>
        <source>Status</source>
        <translation>状態</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="582"/>
        <source>File</source>
        <comment>IDS_COLUMN_FILE</comment>
        <translation>ファイル名</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="583"/>
        <source>Exposure</source>
        <comment>IDS_COLUMN_EXPOSURE</comment>
        <translation>露出時間</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="584"/>
        <source>Aperture</source>
        <comment>IDS_COLUMN_APERTURE</comment>
        <translation>絞り値</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="585"/>
        <source>Score</source>
        <comment>IDS_COLUMN_SCORE</comment>
<translation>スコア</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="586"/>
        <source>Quality</source>
        <comment>IDS_COLUMN_MEANQUALITY</comment>
        <translation>品質</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="587"/>
        <source>#Stars</source>
        <comment>IDS_COLUMN_STARS</comment>
        <translation>星の数</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="588"/>
        <source>FWHM</source>
        <translation>FWHM（半値全幅）</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="589"/>
        <source>dX</source>
        <comment>IDS_COLUMN_DX</comment>
        <translation>dX</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="590"/>
        <source>dY</source>
        <comment>IDS_COLUMN_DY</comment>
        <translation>dY</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="591"/>
        <source>Angle</source>
        <comment>IDS_COLUMN_ANGLE</comment>
        <translation>角度</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="592"/>
        <source>Date/Time</source>
        <comment>IDS_COLUMN_DATETIME</comment>
        <translation>日時</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="593"/>
        <source>Size</source>
        <comment>IDS_COLUMN_SIZES</comment>
        <translation>サイズ</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="594"/>
        <source>CFA</source>
        <comment>IDS_COLUMN_CFA</comment>
        <translation>CFA</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="595"/>
        <source>Depth</source>
        <comment>IDS_COLUMN_DEPTH</comment>
        <translation>色深度</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="596"/>
        <source>Info</source>
        <comment>IDS_COLUMN_INFOS</comment>
        <translation>情報</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="597"/>
        <source>ISO/Gain</source>
        <comment>IDS_COLUMN_ISO_GAIN</comment>
        <translation>ISO/ゲイン</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="598"/>
        <source>Sky Background</source>
        <comment>IDS_COLUMN_SKYBACKGROUND</comment>
        <translation>スカイバックグラウンド</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="701"/>
        <source>%1 is not a directory. Please select a valid directory.</source>
        <translation>%1 はディレクトリではありません。有効なディレクトリを選択してください。</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="711"/>
        <source>Start monitoring folder %1
</source>
        <comment>IDS_LOG_STARTMONITORING</comment>
        <translation>フォルダ %1 の監視を開始します
</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="724"/>
        <source>Stop monitoring folder %1</source>
        <comment>IDS_LOG_STOPMONITORING</comment>
        <translation>フォルダ %1 の監視を停止しました</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="735"/>
        <source>Stop Stacking files</source>
<comment>IDS_LOG_STOPSTACKING</comment>
        <translation>ファイルのスタッキングを停止しました</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="751"/>
        <source>DeepSkyStacker Help.chm</source>
        <comment>IDS_HELPFILE</comment>
        <translation>DeepSkyStacker Help.chm</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="856"/>
        <source>You cannot change the monitored folder while monitoring.</source>
        <comment>IDS_CANTCHANGEMONITOREDFOLDER</comment>
        <translation>監視中に監視フォルダを変更することはできません。</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="868"/>
        <source>Select the folder to be monitored</source>
        <comment>IDS_SELECTMONITOREDFOLDER</comment>
        <translation>監視するフォルダを選択してください</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="903"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1250"/>
        <source>Stacked</source>
        <comment>IDS_STATUS_STACKED</comment>
        <translation>スタック済み</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="909"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1168"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1235"/>
        <source>Pending</source>
        <translation>保留中</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="910"/>
        <location filename="../DeepSkyStackerLive.cpp" line="1241"/>
        <source>Registered</source>
        <comment>IDS_STATUS_REGISTERED</comment>
        <translation>登録済み</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1019"/>
        <source>Start Stacking files</source>
        <comment>IDS_LOG_STARTSTACKING</comment>
        <translation>ファイルのスタッキングを開始します</translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1084"/>
        <source>You have %n image(s) in the monitored folder.
Do you want to process them?</source>
        <comment>IDS_USEEXISTINGIMAGES</comment>
        <translation>
            <numerusform>監視フォルダ内に %n 枚の画像があります。
これらを処理しますか？</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../DeepSkyStackerLive.cpp" line="1095"/>
        <source>%n new file(s) found
</source>
        <comment>IDS_LOG_NEWFILESFOUND</comment>
        <translation>
            <numerusform>%n 個の新規ファイルが見つかりました
</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1222"/>
        <source>Click here to save the stacked image to file</source>
        <comment>IDS_SAVESTACKEDIMAGE</comment>
        <translation>ここをクリックしてスタッキング済み画像を保存</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1238"/>
        <source>Loaded</source>
        <comment>IDS_STATUS_LOADED</comment>
        <translation>読み込み済み</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1244"/>
        <source>Stack delayed</source>
        <comment>IDS_STATUS_STACKDELAYED</comment>
        <translation>スタック待機中</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1247"/>
        <source>Not stackable</source>
        <comment>IDS_STATUS_NOTSTACKABLE</comment>
        <translation>スタック不可</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1330"/>
        <source>No</source>
        <comment>IDS_NO</comment>
        <translation>いいえ</translation>
    </message>
<message>
        <location filename="../DeepSkyStackerLive.cpp" line="1332"/>
        <source>Yes</source>
        <comment>IDS_YES</comment>
        <translation>はい</translation>
    </message> <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1335"/>
        <source>RGB %1 bit/ch</source>
        <comment>IDS_FORMAT_RGB</comment>
        <translation>RGB %1 bit/ch</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1337"/>
        <source>Gray %1 bit</source>
        <comment>IDS_FORMAT_GRAY</comment>
        <translation>グレー %1 bit</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1349"/>
        <source>%1%</source>
        <translation>%1%</translation>
    </message>
    <message>
        <location filename="../DeepSkyStackerLive.cpp" line="1530"/>
        <source>Pending: %1 - Registered: %2 - Stacked: %3 - Total exposure time: %4</source>
        <translation>保留中: %1 - 登録済み: %2 - スタック済み: %3 - 総露出時間: %4</translation>
    </message>
</context>
<context>
    <name>EmailSettings</name>
    <message>
        <location filename="../ui/EmailSettings.ui" line="14"/>
        <source>Email Settings</source>
        <translation>メール設定</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="20"/>
        <source>Sent email settings</source>
        <translation>送信メールの設定</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="26"/>
        <source>To:</source>
        <translation>宛先:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="33"/>
        <source>Send an Email only for the first warning</source>
        <comment>IDC_SENDONCE</comment>
        <translation>最初の警告のみメールを送信する</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="43"/>
        <source>Subject:</source>
        <translation>件名:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="66"/>
        <source>Email Account settings</source>
        <translation>メールアカウントの設定</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="72"/>
        <source>SMTP server:</source>
        <translation>SMTPサーバー:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="82"/>
        <source>Port:</source>
        <translation>ポート:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="126"/>
        <source>Encryption:</source>
        <translation>暗号化:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="134"/>
        <source>None</source>
        <translation>なし</translation>
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
        <translation>アカウント (送信元):</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="165"/>
        <source>Password:</source>
        <translation>パスワード:</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="179"/>
        <source>Show password</source>
        <translation>パスワードを表示</translation>
    </message>
    <message>
        <location filename="../ui/EmailSettings.ui" line="189"/>
        <source>Test</source>
        <comment>IDC_TEST</comment>
        <translation>テスト送信</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../ui/ImageViewer.ui" line="89"/>
        <source>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;Copy current picture to clipboard&lt;/a&gt;</source>
        <comment>IDC_COPYTOCLIPBOARD</comment>
        <translation>&lt;a href=&apos;.&apos; style=&apos;text-decoration: none;&apos;&gt;現在の画像をクリップボードにコピー&lt;/a&gt;</translation>
    </message>
</context>
<context>
    <name>QLinearGradientCtrl</name>
    <message>
        <location filename="../../Tools/QLinearGradientCtrl.cpp" line="44"/>
        <source>After selecting a slider with the mouse, you can move it using the Up Arrow, Right Arrow, Down Arrow, Left Arrow, Page Up, and Page Down keys.</source>
        <translation>マウスでスライダーを選択した後、上下左右の矢印キー、Page Up、Page Downキーを使って動かすことができます。</translation>
    </message>
</context>
</TS>