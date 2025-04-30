[Setup]
AppName=x-IMU3 GUI
AppPublisher=x-io Technologies Limited
AppVerName=x-IMU3 GUI
AppVersion=1.5.8
DefaultDirName={autopf64}\x-IMU3 GUI
DefaultGroupName=x-IMU3 GUI
DisableProgramGroupPage=yes
OutputDir=cmake-build-release\x-IMU3-GUI\x-IMU3-GUI_artefacts\Release\
OutputBaseFilename=x-IMU3-GUI-Setup
SignTool=signtool
UninstallDisplayIcon={app}\x-IMU3 GUI.exe
WizardStyle=modern

[Files]
Source: "cmake-build-release\x-IMU3-GUI\x-IMU3-GUI_artefacts\Release\x-IMU3 GUI.exe"; destdir: "{app}"; Flags: ignoreversion sign

[Icons]
Name: "{autodesktop}\x-IMU3 GUI"; Filename: "{app}\x-IMU3 GUI.exe"
Name: "{group}\x-IMU3 GUI"; Filename: "{app}\x-IMU3 GUI.exe"

[Run]
Filename: {app}\x-IMU3 GUI.exe; Flags: postinstall
