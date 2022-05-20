Name "p1xbraten"

OutFile "..\..\p1xbraten_setup_<git-dev>.exe"

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
    Abort "Sauerbraten files not found in specified directory!"
  VanillaFound:
  
  SetOutPath $INSTDIR\bin64
  File "..\..\bin64\p1xbraten_x86_64_client.exe"
  File "..\..\bin64\start_protected_game.exe"
  File "..\..\bin64\EOSSDK-Win64-Shipping.dll"
  File /r "..\..\bin64\EasyAntiCheat"
  SetOutPath $INSTDIR
  File "..\..\p1xbraten.bat"
  File "..\..\anticheat.bat"
  
  WriteRegStr HKLM SOFTWARE\p1xbraten "Install_Dir" "$INSTDIR"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "DisplayName" "p1xbraten"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "UninstallString" '"$INSTDIR\uninstall_p1xbraten.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten" "NoRepair" 1
  WriteUninstaller "uninstall_p1xbraten.exe"

SectionEnd

Section "EasyAntiCheat system service (required)"

  SectionIn RO

  ExecWait '"$INSTDIR\bin64\EasyAntiCheat\EasyAntiCheat_EOS_Setup.exe" install 36e0587a4c3544e8b635f7f55e7ccbfe' $0
  IntCmp $0 0 ZeroReturnCode NegativeReturnCode PositiveReturnCode
  NegativeReturnCode:
  PositiveReturnCode:
    Abort "EasyAntiCheat system service installation failed (code: $0)!"
  ZeroReturnCode:

SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\p1xbraten"
  
  SetOutPath "$INSTDIR"
  
  CreateShortCut "$INSTDIR\p1xbraten.lnk"                "$INSTDIR\p1xbraten.bat" "" "$INSTDIR\bin64\p1xbraten_x86_64_client.exe" 0 SW_SHOWMINIMIZED
  CreateShortCut "$INSTDIR\anticheat.lnk"                "$INSTDIR\anticheat.bat" "" "$INSTDIR\bin64\p1xbraten_x86_64_client.exe" 0 SW_SHOWMINIMIZED
  CreateShortCut "$SMPROGRAMS\p1xbraten\p1xbraten.lnk"   "$INSTDIR\p1xbraten.bat" "" "$INSTDIR\bin64\p1xbraten_x86_64_client.exe" 0 SW_SHOWMINIMIZED
  CreateShortCut "$SMPROGRAMS\p1xbraten\anticheat.lnk"   "$INSTDIR\anticheat.bat" "" "$INSTDIR\bin64\p1xbraten_x86_64_client.exe" 0 SW_SHOWMINIMIZED
  CreateShortCut "$SMPROGRAMS\p1xbraten\Uninstall.lnk"   "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

Section "Uninstall"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\p1xbraten"
  DeleteRegKey HKLM SOFTWARE\p1xbraten

  RMDir /r "$SMPROGRAMS\p1xbraten"
  Delete "$INSTDIR\anticheat.lnk"
  Delete "$INSTDIR\p1xbraten.lnk"
  Delete "$INSTDIR\anticheat.bat"
  Delete "$INSTDIR\p1xbraten.bat"
  RMDir /r "$INSTDIR\bin64\EasyAntiCheat"
  Delete "$INSTDIR\bin64\EOSSDK-Win64-Shipping.dll"
  Delete "$INSTDIR\bin64\start_protected_game.exe"
  Delete "$INSTDIR\bin64\p1xbraten_x86_64_client.exe"

SectionEnd
