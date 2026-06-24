# Builds console_fnaf and packages a Windows zip for GitHub Releases.
# Usage: powershell -ExecutionPolicy Bypass -File scripts/package-windows.ps1

$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$BuildDir = Join-Path $Root "build"
$DistDir = Join-Path $Root "dist"
$PackageDir = Join-Path $DistDir "console-fnaf-windows"
$ZipPath = Join-Path $DistDir "console-fnaf-windows.zip"

$MsysRoot = if ($env:NCURSES_ROOT) { $env:NCURSES_ROOT } else { "C:/msys64/ucrt64" }
$MsysRoot = $MsysRoot -replace "/", "\"

if (-not (Test-Path $MsysRoot)) {
    Write-Error "MSYS2 UCRT64 not found at $MsysRoot. Set NCURSES_ROOT or install MSYS2."
}

Write-Host "==> Configuring and building..."
Push-Location $Root
cmake -S . -B build -DNCURSES_ROOT="$($MsysRoot -replace '\\','/')"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build build --config Release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Pop-Location

$ExePath = Join-Path $BuildDir "console_fnaf.exe"
if (-not (Test-Path $ExePath)) {
    Write-Error "Build failed: $ExePath not found"
}

Write-Host "==> Staging release folder..."
if (Test-Path $PackageDir) { Remove-Item $PackageDir -Recurse -Force }
New-Item -ItemType Directory -Path $PackageDir -Force | Out-Null

Copy-Item $ExePath $PackageDir

$DllNames = @(
    "libgcc_s_seh-1.dll",
    "libncursesw6.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll"
)

$BinDir = Join-Path $MsysRoot "bin"
foreach ($dll in $DllNames) {
    $src = Join-Path $BinDir $dll
    if (-not (Test-Path $src)) {
        Write-Error "Missing runtime DLL: $src"
    }
    Copy-Item $src $PackageDir
}

$TerminfoSrc = Join-Path $MsysRoot "share\terminfo"
$TerminfoDst = Join-Path $PackageDir "share\terminfo"
if (-not (Test-Path $TerminfoSrc)) {
    Write-Error "Missing terminfo at $TerminfoSrc"
}
Copy-Item $TerminfoSrc $TerminfoDst -Recurse

Copy-Item (Join-Path $Root "release\PLAY.bat") $PackageDir
Copy-Item (Join-Path $Root "release\HOW_TO_PLAY.txt") $PackageDir

$AssetsSrc = Join-Path $Root "assets"
if (-not (Test-Path $AssetsSrc)) {
    Write-Error "Missing assets folder: $AssetsSrc"
}
Copy-Item $AssetsSrc (Join-Path $PackageDir "assets") -Recurse

Write-Host "==> Creating zip..."
if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }
New-Item -ItemType Directory -Path $DistDir -Force | Out-Null
Compress-Archive -Path $PackageDir -DestinationPath $ZipPath -Force

Write-Host ""
Write-Host "Done: $ZipPath"
Write-Host "Test locally: extract the zip, double-click PLAY.bat"
