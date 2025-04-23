# script to setup a DSS build env and build it

if ($args.count -lt 1) {
    $buildargs = "/property:Configuration=Debug"
}
else {
    $buildargs = "$args"
}
$here=$PSScriptRoot

#Import-Module "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
#Enter-VsDevShell 45e81cd9
set-location $here
$env:QtMsBuild="E:\dev\astronomy\QtMsBuild"
#$env:QT_ROOT_DIR="E:\dev\astronomy\Qt\6.5.1\msvc2019_64"
$env:QT_ROOT_DIR="E:\dev\astronomy\Qt682\6.8.2\msvc2022_64"
$env:QtToolsPath="$env:qt_root_dir\bin"
msbuild  $buildargs -target:DeepSkyStacker
