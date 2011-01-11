; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Surfer"
#define MyAppVerName "Surfer 0.1"
#define MyAppPublisher "Mathematisches Forschungsinstitut Oberwolfach"
#define MyAppURL "http://surfer.imaginary-exhibition.com"
#define MyAppExeName "surfer.exe"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppVerName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=gpl-3.0.txt
; InfoBeforeFile=.\PreInstallInformation.txt
; InfoAfterFile=.\PostInstallInformation.txt
OutputDir=.
OutputBaseFilename=surfer-setup
Compression=lzma
SolidCompression=false
PrivilegesRequired=admin

;; use new InnoSetup-Images
;WizardImageFile=compiler:WizModernImage-IS.bmp
;WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp

; special Setup-Images
WizardSmallImageFile=surfer-small.bmp
WizardImageFile=surfer.bmp

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: de; MessagesFile: compiler:Languages\German.isl
Name: es; MessagesFile: compiler:Languages\Spanish.isl

[CustomMessages]
; en
en.RCInstall=Installing %1.
en.VCRedist=Microsoft Visual Studio 2008 SP1 runtime components

; de
de.RCInstall=Installiere %1.
de.VCRedist=Microsoft Visual Studio 2008 SP1 Laufzeitkomponenten

; es
es.RCInstall=Instalar el %1.
es.VCRedist=Paquete redistribuible de Microsoft Visual C++ 2008 SP1

[Components]
Name: main; Description: Surfer; Types: full custom compact; Flags: fixed
Name: vcredist; Description: {cm:VCRedist}; Types: full custom compact; Flags: fixed; Languages: 

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
; Surfer-Files
Source: ..\bin\surfer.exe; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: ..\bin\surfer_de_DE.bat; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: ..\bin\surfer_en_GB.bat; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: ..\bin\surfer_es_ES.bat; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: ..\surfer.ini; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: ..\..\surfer-help.pdf; DestDir: {app}; Flags: ignoreversion; Components: main
Source: ..\..\po\de.gmo; DestDir: {app}\share\locale\de\LC_MESSAGES; DestName: surfer.mo; Flags: ignoreversion; Components: main
Source: ..\..\po\en_GB.gmo; DestDir: {app}\share\locale\en_GB\LC_MESSAGES; DestName: surfer.mo; Flags: ignoreversion; Components: main
Source: ..\..\po\es.gmo; DestDir: {app}\share\locale\es\LC_MESSAGES; DestName: surfer.mo; Flags: ignoreversion; Components: main
Source: ..\..\gallery-de\*; DestDir: {app}\gallery\gallery-de; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main; Excludes: .svn
Source: ..\..\gallery-en\*; DestDir: {app}\gallery\gallery-en; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main; Excludes: .svn
Source: ..\..\gallery-es\*; DestDir: {app}\gallery\gallery-es; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main; Excludes: .svn
; ffmpeg-Files
Source: ..\redist\ffmpeg\ffmpeg.exe; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: ..\redist\ffmpeg\pthreadGC2.dll; DestDir: {app}\bin; Flags: ignoreversion; Components: main
; Surf-Files
Source: surf\pthreadVC2.dll; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: surf\jpeg62.dll; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: surf\libtiff3.dll; DestDir: {app}\bin; Flags: ignoreversion; Components: main
Source: surf\surf.exe; DestDir: {app}\bin; Flags: ignoreversion; Components: main
; Dependencies
Source: ..\redist\vcredist_x86.exe; DestDir: {tmp}; Flags: ignoreversion deleteafterinstall; Components: vcredist
; GTKmm with GTK
Source: ..\redist\gtkmm\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main; Excludes: .svn

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\bin\{#MyAppExeName}; WorkingDir: {app}\bin
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {#MyAppURL}
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}; WorkingDir: {app}
Name: {commondesktop}\{#MyAppName}; Filename: {app}\bin\{#MyAppExeName}; WorkingDir: {app}\bin; Tasks: desktopicon

[Run]
Filename: {tmp}\vcredist_x86.exe; StatusMsg: {cm:RCInstall,{cm:VCRedist}}; Components: vcredist; Parameters: /q
; Launch program after installation
;Filename: "{app}\bin\{#MyAppExeName}"; WorkingDir: {app}; Description: "{cm:LaunchProgram,{#MyAppName}}"; Flags: nowait postinstall skipifsilent
