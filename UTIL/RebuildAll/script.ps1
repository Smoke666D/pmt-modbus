#**********INPUT ARGUMENTS*********

#**********CONST***********
$ConfigName = "user_process.h";
$DefLine    = "#define         device          ";

$IarBuild = "C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.5\common\bin\IarBuild.exe";
$ProjName = "rs485.ewp";
$ForkName = "Release";

$RebuildNumber = 4;
$RebuildList   = "BIG","BIC","BKC","IT14";
#**********SYSTEM**********

#**********VAR*************
[string]$NewLine = "";
$i = 0;
#*******************************FUNCTIONS*******************************

#*******************************MAIN********************************
Set-Location ..;	# Подняться выше
Set-Location ..;
$GitDir = Get-Location;
$OutTest = Test-Path $GitDir/OUT;
if($OutTest -eq "True")
{
  Remove-Item $GitDir/OUT -Recurse;
}
New-Item -Path $GitDir/OUT -ItemType "directory";
Set-Location $GitDir/LIB/ModBus/inc;
$ConfigFile = Get-Content $ConfigName;
$ConfLine = select-string -path $ConfigName -pattern $DefLine;
$NewLine = $ConfLine.Line;
$OldLine = $ConfLine.Line;

DO
{
  Set-Location $GitDir/LIB/ModBus/inc;
  $NewLine = $NewLine.Remove($NewLine.LastIndexOfAny($DefLine)-1) + $RebuildList[$i];
  $ConfigFile[$ConfLine.LineNumber-1] = $NewLine;
  $ConfigFile | set-content $ConfigName;
  
  Set-Location $GitDir;
  & $IarBuild $ProjName $ForkName;
  
  Set-Location $GitDir/Release/Exe
  $NewFileName = $RebuildList[$i] + ".hex";
  Rename-Item -path RS485.hex -newname $NewFileName;
  Move-Item -path $NewFileName -destination $GitDir/OUT;
  
  $i = $i + 1;
}while($i -lt $RebuildNumber)

Set-Location $GitDir/LIB/ModBus/inc;
$ConfigFile[$ConfLine.LineNumber-1] = $OldLine;
$ConfigFile | set-content $ConfigName;

Set-Location $GitDir/OUT;
New-Item -Path $GitDir/OUT/README.txt -ItemType "file";
$CurentDate = Get-Date -format ddMMyy;
$LogMess = ">>" + $CurentDate + "      Rebuild success!=)"
Out-File -filepath $GitDir/OUT/README.txt -inputobject $LogMess -encoding ASCII -append;