; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CStackSettings
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "deepskystacker.h"
LastPage=0

ClassCount=21
Class1=CDeepStackerDlg
Class2=CLuminanceTab
Class3=CPictureListCtrl
Class4=CProcessingDlg
Class5=CProcessSettingsSheet
Class6=CRegisteringDlg
Class7=CRGBTab
Class8=CStackingDlg
Class9=CButtonST
Class10=CBubble
Class11=CEditInterval
Class12=CGradientCtrl
Class13=CMultiSlider
Class14=CWndImage

ResourceCount=13
Resource1=IDD_RGB (English (U.S.))
Resource2=IDD_PROCESSING (English (U.S.))
Resource3=IDD_DEEPSTACKER (English (U.S.))
Resource4=IDD_REGISTERING (English (U.S.))
Resource5=IDD_SATURATION (English (U.S.))
Resource6=IDD_REGISTERSETTINGS (English (U.S.))
Class15=CProgressDlg
Resource7=IDD_STACKSETTINGS (English (U.S.))
Class16=CSettingsDlg
Resource8=IDD_CHECKABOVE (English (U.S.))
Class17=CSaturationTab
Resource9=IDD_STACKING (English (U.S.))
Class18=CRegisterSettings
Resource10=IDD_PROGRESS (English (U.S.))
Class19=CStackSettings
Resource11=IDD_SETTINGS (English (U.S.))
Class20=CCheckAbove
Resource12=IDD_LUMINANCE (English (U.S.))
Class21=CRawDDPSettings
Resource13=IDD_RAWSETTINGS (English (U.S.))

[CLS:CDeepStackerDlg]
Type=0
BaseClass=CDialog
HeaderFile=DeepStackerDlg.h
ImplementationFile=DeepStackerDlg.cpp

[CLS:CLuminanceTab]
Type=0
BaseClass=CChildPropertyPage
HeaderFile=LuminanceTab.h
ImplementationFile=LuminanceTab.cpp
Filter=D
VirtualFilter=idWC
LastObject=CLuminanceTab

[CLS:CPictureListCtrl]
Type=0
BaseClass=CListCtrl
HeaderFile=PictureListCtrl.h
ImplementationFile=PictureListCtrl.cpp

[CLS:CProcessingDlg]
Type=0
BaseClass=CDialog
HeaderFile=ProcessingDlg.h
ImplementationFile=ProcessingDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CProcessingDlg

[CLS:CProcessSettingsSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=ProcessSettingsSheet.h
ImplementationFile=ProcessSettingsSheet.cpp

[CLS:CRegisteringDlg]
Type=0
BaseClass=CDialog
HeaderFile=RegisteringDlg.h
ImplementationFile=RegisteringDlg.cpp

[CLS:CRGBTab]
Type=0
BaseClass=CChildPropertyPage
HeaderFile=RGBTab.h
ImplementationFile=RGBTab.cpp
Filter=D
VirtualFilter=idWC
LastObject=CRGBTab

[CLS:CStackingDlg]
Type=0
BaseClass=CDialog
HeaderFile=StackingDlg.h
ImplementationFile=StackingDlg.cpp

[CLS:CButtonST]
Type=0
BaseClass=CButton
HeaderFile=\Astro\Projects\Tools\BtnST.h
ImplementationFile=\Astro\Projects\Tools\BtnST.cpp

[CLS:CBubble]
Type=0
BaseClass=CWnd
HeaderFile=\Astro\Projects\Tools\Bubble.h
ImplementationFile=\Astro\Projects\Tools\Bubble.cpp
LastObject=CBubble

[CLS:CEditInterval]
Type=0
BaseClass=CDialog
HeaderFile=\Astro\Projects\Tools\EditInterval.h
ImplementationFile=\Astro\Projects\Tools\EditInterval.cpp

[CLS:CGradientCtrl]
Type=0
BaseClass=CWnd
HeaderFile=\Astro\Projects\Tools\gradientctrl.h
ImplementationFile=\Astro\Projects\Tools\GradientCtrl.cpp

[CLS:CMultiSlider]
Type=0
BaseClass=CSliderCtrl
HeaderFile=\Astro\Projects\Tools\MultiSlider.h
ImplementationFile=\Astro\Projects\Tools\MultiSlider.cpp

[CLS:CWndImage]
Type=0
BaseClass=CWnd
HeaderFile=\Astro\Projects\Tools\WndImage.h
ImplementationFile=\Astro\Projects\Tools\WndImage.cpp
Filter=W
VirtualFilter=WC
LastObject=CWndImage

[DLG:IDD_DEEPSTACKER]
Type=1
Class=CDeepStackerDlg

[DLG:IDD_LUMINANCE]
Type=1
Class=CLuminanceTab

[DLG:IDD_PROCESSING]
Type=1
Class=CProcessingDlg

[DLG:IDD_REGISTERING]
Type=1
Class=CRegisteringDlg

[DLG:IDD_RGB]
Type=1
Class=CRGBTab

[DLG:IDD_STACKING]
Type=1
Class=CStackingDlg

[DLG:IDD_EDITINTERVAL]
Type=1
Class=CEditInterval

[DLG:IDD_DEEPSTACKER (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_MAINTAB,SysTabControl32,1342177280

[DLG:IDD_REGISTERING (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_PICTURES,SysListView32,1350631437
Control2=IDC_PICTURE,static,1342181902
Control3=IDC_STATIC,static,1342308352
Control4=IDC_ADDPICTURES,button,1342242816
Control5=IDC_ACTIONS,button,1342242816

[DLG:IDD_STACKING (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDC_PICTURES,SysListView32,1350631429
Control2=IDC_PICTURE,static,1342181383
Control3=IDC_DARKS,SysListView32,1350631425
Control4=IDC_ADDPICTURES,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_ADDDARKS,button,1342242816
Control7=IDC_DARKSSTATIC,static,1342308352
Control8=IDC_ACTIONS,button,1342242816

[DLG:IDD_PROCESSING (English (U.S.))]
Type=1
Class=CProcessingDlg
ControlCount=9
Control1=IDC_PICTURE,static,1342181383
Control2=IDC_LOADDSI,button,1342242816
Control3=IDC_ACTIONS,button,1342242816
Control4=IDC_ORIGINAL_HISTOGRAM,static,1342312448
Control5=IDC_LOGHISTO,button,1342242819
Control6=IDC_PROCESSED_HISTOGRAM,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_SETTINGS,static,1073872896
Control9=IDC_INFO,static,1342308352

[DLG:IDD_RGB (English (U.S.))]
Type=1
Class=CRGBTab
ControlCount=12
Control1=IDC_REDGRADIENT,MFCGradientCtrl,1342242816
Control2=IDC_GREENGRADIENT,MFCGradientCtrl,1342242816
Control3=IDC_BLUEGRADIENT,MFCGradientCtrl,1342242816
Control4=IDC_RESET,button,1342242816
Control5=IDC_PROCESS,button,1342242816
Control6=IDC_LINK,button,1342242819
Control7=IDC_UNDO,button,1342242816
Control8=IDC_REDO,button,1342242816
Control9=IDC_SETTINGS,button,1342242816
Control10=IDC_RED_HAT,button,1342242816
Control11=IDC_GREEN_HAT,button,1342242816
Control12=IDC_BLUE_HAT,button,1342242816

[DLG:IDD_LUMINANCE (English (U.S.))]
Type=1
Class=CLuminanceTab
ControlCount=14
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DARKANGLE,msctls_trackbar32,1342242836
Control5=IDC_DARKPOWER,msctls_trackbar32,1342242836
Control6=IDC_MIDTONE,msctls_trackbar32,1342242836
Control7=IDC_MIDANGLE,msctls_trackbar32,1342242836
Control8=IDC_HIGHANGLE,msctls_trackbar32,1342242836
Control9=IDC_HIGHPOWER,msctls_trackbar32,1342242836
Control10=IDC_PROCESS,button,1342242816
Control11=IDC_RESET,button,1342242816
Control12=IDC_UNDO,button,1342242880
Control13=IDC_REDO,button,1342242880
Control14=IDC_SETTINGS,button,1342242880

[DLG:IDD_PROGRESS (English (U.S.))]
Type=1
Class=CProgressDlg
ControlCount=6
Control1=IDC_PROCESS_TEXT1,static,1342308353
Control2=IDC_PROCESS_TEXT2,static,1342308353
Control3=IDC_PROGRESS1,msctls_progress32,1350565888
Control4=IDC_PROGRESS2,msctls_progress32,1350565888
Control5=IDC_STOP,button,1342242816
Control6=IDC_TIMEREMAINING,static,1342308352

[CLS:CProgressDlg]
Type=0
HeaderFile=ProgressDlg.h
ImplementationFile=ProgressDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CProgressDlg

[DLG:IDD_SETTINGS (English (U.S.))]
Type=1
Class=CSettingsDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_DELETE,button,1342242817
Control3=IDC_ADD,button,1342242817
Control4=IDC_LOAD,button,1342242817
Control5=IDC_SETTINGSNAME,edit,1350631552
Control6=IDC_SETTINGSLIST,listbox,1352728835

[CLS:CSettingsDlg]
Type=0
HeaderFile=SettingsDlg.h
ImplementationFile=SettingsDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDOK

[DLG:IDD_SATURATION (English (U.S.))]
Type=1
Class=CSaturationTab
ControlCount=7
Control1=65535,static,1342308352
Control2=IDC_SATURATION,msctls_trackbar32,1342242836
Control3=IDC_PROCESS,button,1342242816
Control4=IDC_RESET,button,1342242816
Control5=IDC_UNDO,button,1342242880
Control6=IDC_REDO,button,1342242880
Control7=IDC_SETTINGS,button,1342242880

[CLS:CSaturationTab]
Type=0
HeaderFile=SaturationTab.h
ImplementationFile=SaturationTab.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CSaturationTab

[DLG:IDD_REGISTERSETTINGS (English (U.S.))]
Type=1
Class=CRegisterSettings
ControlCount=17
Control1=IDC_STATIC,static,1342308352
Control2=IDC_MINLUMINANCY,edit,1350631554
Control3=IDC_FORCEREGISTER,button,1342242819
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STACK,button,1342242819
Control6=IDC_STATIC,static,1342308352
Control7=IDC_PERCENT,edit,1350639746
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_ISOSPEED,combobox,1344339971
Control11=IDC_STATIC,static,1342308352
Control12=IDC_DURATION,edit,1350639746
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342312449
Control15=IDOK,button,1342242817
Control16=IDCANCEL,button,1342242816
Control17=IDC_RAWDDPSETTINGS,button,1342242816

[CLS:CRegisterSettings]
Type=0
HeaderFile=RegisterSettings.h
ImplementationFile=RegisterSettings.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CRegisterSettings

[DLG:IDD_STACKSETTINGS (English (U.S.))]
Type=1
Class=CStackSettings
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_DURATION,edit,1350639746
Control6=IDC_ISOSPEED,combobox,1344339971
Control7=IDC_STATIC,static,1342308352
Control8=IDC_RAWDDPSETTINGS,button,1342242816

[CLS:CStackSettings]
Type=0
HeaderFile=StackSettings.h
ImplementationFile=StackSettings.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CStackSettings

[DLG:IDD_CHECKABOVE (English (U.S.))]
Type=1
Class=CCheckAbove
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_THRESHOLD,edit,1350631554
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816

[CLS:CCheckAbove]
Type=0
HeaderFile=CheckAbove.h
ImplementationFile=CheckAbove.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CCheckAbove

[DLG:IDD_RAWSETTINGS (English (U.S.))]
Type=1
Class=CRawDDPSettings
ControlCount=17
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_BRIGHTNESS,edit,1350631554
Control4=IDC_STATIC,static,1342308352
Control5=IDC_REDSCALE,edit,1350631554
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BLUESCALE,edit,1350631554
Control8=IDC_STATIC,button,1342177287
Control9=IDC_AUTOWB,button,1342242819
Control10=IDC_CAMERAWB,button,1342242819
Control11=IDC_STATIC,button,1342177287
Control12=IDC_BILINEAR,button,1342373897
Control13=IDC_VNG,button,1342373897
Control14=IDC_AHD,button,1342373897
Control15=IDC_RGB4COLORS,button,1342382083
Control16=IDOK,button,1342242817
Control17=IDCANCEL,button,1342242816

[CLS:CRawDDPSettings]
Type=0
HeaderFile=RawDDPSettings.h
ImplementationFile=RawDDPSettings.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

