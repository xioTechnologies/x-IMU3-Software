[Setup]
AppName=x-IMU3 GUI
AppPublisher=x-io Technologies
AppVerName=x-IMU3 GUI
AppVersion=1.2.3
DefaultDirName={autopf64}\x-IMU3 GUI
DefaultGroupName=x-IMU3 GUI
DisableProgramGroupPage=yes
OutputDir=cmake-build-release\x-IMU3-GUI\x-IMU3-GUI_artefacts\Release\
OutputBaseFilename=x-IMU3-GUI-Setup
UninstallDisplayIcon={app}\x-IMU3 GUI.exe
WizardStyle=modern

[Files]
Source: "cmake-build-release\x-IMU3-GUI\x-IMU3-GUI_artefacts\Release\x-IMU3 GUI.exe"; destdir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{autodesktop}\x-IMU3 GUI"; Filename: "{app}\x-IMU3 GUI.exe"
Name: "{group}\x-IMU3 GUI"; Filename: "{app}\x-IMU3 GUI.exe"

[Run]
Filename: {app}\x-IMU3 GUI.exe; Flags: postinstall