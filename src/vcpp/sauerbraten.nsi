Name "p1xbraten"

OutFile "..\..\p1xbraten_<git-dev>_setup.exe"

InstallDir $PROGRAMFILES\Sauerbraten

InstallDirRegKey HKLM "Software\p1xbraten" "Install_Dir"

SetCompressor /SOLID lzma

Page components
DirText "Choose your Sauerbraten installation directory. p1xbraten will be installed in it alongside the original files."
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "p1xbraten (required)"

  SectionIn RO
  
  SetOutPath $INSTDIR
  
  IfFileExists "$INSTDIR\sauerbraten.bat" VanillaFound VanillaNotFound
  VanillaNotFound:
    Abort "Vanilla Sauerbraten files not found in installation directory!"
  VanillaFound:
  
  File "/oname=bin\p1xbraten.exe" "..\..\bin\p1xbraten_x86.exe"
  File "/oname=bin64\p1xbraten.exe" "..\..\bin\p1xbraten_x64.exe"
  File "..\..\p1xbraten.bat"
  
  WriteRegStr HKLM SOFTWARE\p1xbraten "Install_Dir" "$INSTDIR"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "DisplayName" "p1xbraten"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "UninstallString" '"$INSTDIR\uninstall_p1xbraten.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "NoRepair" 1
  WriteUninstaller "uninstall_p1xbraten.exe"

SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\p1xbraten"
  
  SetOutPath "$INSTDIR"
  
  CreateShortCut "$INSTDIR\p1xbraten.lnk"                "$INSTDIR\p1xbraten.bat" "" "$INSTDIR\bin\p1xbraten.exe" 0 SW_SHOWMINIMIZED
  CreateShortCut "$SMPROGRAMS\p1xbraten\p1xbraten.lnk"   "$INSTDIR\p1xbraten.bat" "" "$INSTDIR\bin\p1xbraten.exe" 0 SW_SHOWMINIMIZED
  CreateShortCut "$SMPROGRAMS\p1xbraten\Uninstall.lnk"   "$INSTDIR\uninstall.exe"   "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

Section "Uninstall"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten"
  DeleteRegKey HKLM SOFTWARE\p1xbraten

  RMDir /r "$SMPROGRAMS\p1xbraten"
  Delete "$INSTDIR\p1xbraten.lnk"
  Delete "$INSTDIR\p1xbraten.bat"
  Delete "$INSTDIR\bin\p1xbraten.exe"
  Delete "$INSTDIR\bin64\p1xbraten.exe"

SectionEnd
