; basic script template for NullsoftInstallerPackager
;
; Copyright 2016-2018 Hannnah von Reth <hannah.vonreth@kdab.com>
; Copyright 2010 Patrick Spendrin <ps_ml@gmx.de>


!include MUI2.nsh
!include LogicLib.nsh


; registry stuff
!define regkey "Software\${companyName}\${productName}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${productName}"

!define startmenu "$SMPROGRAMS\${productName}"
!define uninstaller "uninstall.exe"

Var StartMenuFolder

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${regkey}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

;--------------------------------


XPStyle on
ShowInstDetails hide
ShowUninstDetails hide

SetCompressor /SOLID lzma

Name "${productName}"
Caption "Installing ${productName}"

OutFile "${setupname}"

!define MUI_ICON ${applicationIcon}

!insertmacro MUI_PAGE_WELCOME

${productLicence}

!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH

;uninstaller
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
;-------

!insertmacro MUI_LANGUAGE "English"

SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal

InstallDir "${programFilesDir}\${productName}"
InstallDirRegKey HKLM "${regkey}" ""


;--------------------------------
AutoCloseWindow false


; beginning (invisible) section
Section "--hidden ${productName}" BASE
   SectionIn RO
   SetOutPath $INSTDIR
   SetShellVarContext all

    WriteRegStr HKLM "${regkey}" "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM "${regkey}" "Version" "${productVersion}"
    WriteRegStr HKLM "${regkey}" "" "$INSTDIR\uninstall.exe"

    WriteRegStr HKLM "${uninstkey}" "DisplayName" "${productName} (remove only)"
    WriteRegStr HKLM "${uninstkey}" "DisplayIcon" "$INSTDIR\${applicationName}"
    WriteRegStr HKLM "${uninstkey}" "DisplayVersion" "${productVersion}"
    WriteRegStr HKLM "${uninstkey}" "UninstallString" '"$INSTDIR\${uninstaller}"'
    WriteRegStr HKLM "${uninstkey}" "Publisher" "${companyName}"

  SetOutPath $INSTDIR


    ; package all files, recursively, preserving attributes
    ; assume files are in the correct places

    File /a /r /x "*.nsi" /x "${setupname}" "${deployDir}\*.*"

    !if "${vcredist}" != "none"
        ExecWait '"$INSTDIR\${vcredist}" /passive'
        Delete "$INSTDIR\${vcredist}"
    !endif


    WriteUninstaller "${uninstaller}"


    ;Create shortcuts
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${productName}.lnk" "$INSTDIR\${applicationName}"
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

; Uninstaller
; All section names prefixed by "Un" will be in the uninstaller

UninstallText "This will uninstall ${productName}."

Section "Uninstall"
    SetShellVarContext all
    SetShellVarContext all

    DeleteRegKey HKLM "${uninstkey}"
    DeleteRegKey HKLM "${regkey}"

    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

    RMDir /r "$SMPROGRAMS\$StartMenuFolder"
    RMDir /r "$INSTDIR"
SectionEnd


Function .onInit
    ReadRegStr $R0 HKLM "${uninstkey}" "UninstallString"
    StrCmp $R0 "" done
    ReadRegStr $INSTDIR HKLM "${regkey}" "Install_Dir"
    ;Run the uninstaller
    ;uninst:
    ClearErrors
    ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
    done:
FunctionEnd
