Set-Location "C:\Program Files (x86)\Microsoft Visual Studio\Installer\"
$InstallPath = "C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
$componentsToInstall= @(
"Microsoft.VisualStudio.Component.VC.v141.x86.x64"
"Microsoft.VisualStudio.Component.VC.v141.x86.x64.Spectre"
"Microsoft.VisualStudio.Component.VC.v141.MFC"
"Microsoft.VisualStudio.Component.VC.v141.MFC.Spectre"
"Microsoft.VisualStudio.Component.VC.14.39.17.9.x86.x64"
"Microsoft.VisualStudio.Component.VC.14.39.17.9.x86.x64.Spectre"
"Microsoft.VisualStudio.Component.VC.14.39.17.9.MFC"
"Microsoft.VisualStudio.Component.VC.14.39.17.9.ATL"
)
[string]$workloadArgs = $componentsToInstall | ForEach-Object {" --add " +  $_}
$Arguments = ('/c', "vs_installer.exe", 'modify', '--installPath', "`"$InstallPath`"",$workloadArgs, '--quiet', '--norestart', '--nocache')
# should be run twice
$process = Start-Process -FilePath cmd.exe -ArgumentList $Arguments -Wait -PassThru -WindowStyle Hidden
$process = Start-Process -FilePath cmd.exe -ArgumentList $Arguments -Wait -PassThru -WindowStyle Hidden
