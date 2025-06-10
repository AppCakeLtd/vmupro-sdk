#
# Creates the python venv and passes the build params
# 2025, 8BM
#
param (
    [string]$basedir,   # c:\myprojects\vmusdk\examples\minimal
    [string]$elfname,   # "vmupro_minimal" (matches your .elf filename in basedir/build/<yourfile>.app.elf)
    [string]$icon,      # usually "icon.bmp"
    [string]$meta,      # usually "metadata.json"
    [string]$debug      # set to true for extra spam
)

# Debug
# Write-Host "args: $args"
# Write-Host base=$basedir elfname=$elfname icon=$icon meta=$meta debug=$debug

# Need the parent folder to create abs paths for pip, venv
# e.g. c:\myprojects\vmusdk\examples\minimal\
$parentfolder = Split-Path -Parent $PSCommandPath
Write-Host Packer dir: $parentfolder

if ([string]::IsNullOrEmpty($basedir)){
    # base directory of your project, where it was invoked from
    $basedir = Get-Location 
    Write-Host Inferring basedir
}
Write-Host Base dir: $basedir

write-host VMUPacker: checking virtual environment
if ($env:VIRTUAL_ENV) {
    Write-Host "Already inside a virtual environment: $env:VIRTUAL_ENV"
    Write-Host "Type `deactivate` to start a fresh environment"
    exit 0
}

write-host VMUPacker: creating python3 venv
python -m venv $parentfolder\.venv
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to create venv"
    exit 1
}

# call .venv\Scripts\activate
. $parentfolder\.venv\Scripts\Activate.ps1

# Add crcmod, pillow, etc
pip install -r $parentfolder\requirements.txt

Write-Host VMUPacker env created
Write-Host Ready!

# Running `where.exe python` should point at the ESP IDF's python
# this will be automatic if you've ran the idf export script before building
python $parentfolder/packer.py --basedir $basedir --elfname $elfname --icon $icon --meta $meta --sdkversion 1.0.0 --debug true

Write-Host closing the python environmet
#deactivate
