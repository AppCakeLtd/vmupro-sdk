#
# Creates the python venv and passes the build params
# 2025, 8BM
#
param (
    # c:\myprojects\vmusdk\examples\minimal
    [string]$projectdir, 
    # "vmupro_minimal" (matches your .elf filename in projectdir/build/<yourfile>.app.elf)
    [string]$elfname, 
    # usually "icon.bmp"
    [string]$icon, 
    # usually "metadata.json"
    [string]$meta, 
    # set to true for extra spam
    [string]$debug       
)

# Debug
# Write-Host "args: $args"
# Write-Host base=$projectdir elfname=$elfname icon=$icon meta=$meta debug=$debug

# Need the tool's folder path to properly locate requirements.txxt
# i.e. the location where THIS script resides
$tooldir = Split-Path -Parent $PSCommandPath
Write-Host "Packer tool's dir: $tooldir"

if ([string]::IsNullOrEmpty($projectdir)) {
    # base directory of your project, where it was invoked from
    $projectdir = Get-Location 
    Write-Host WARNING: Inferring project dir from current directory
}
Write-Host "Project's dir: $projectdir"

$venvname = "vmupacker_venv"
$venvdir = "$projectdir\$venvname"

write-host "VMUPacker: checking virtual environment"
if (-not $env:VIRTUAL_ENV) {

    if (-not (test-path $venvdir)) {
        write-host "VMUPacker: creating python3 venv @ $venvdir"
        python -m venv $venvdir
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to create venv"
            Write-Error "Hint: VSCode may be using the vmupacker_venv directory"
            exit 1
        }
    }
    else {
        Write-Host "VMUPacker: env already exists at $venvdir"
    }

}
else {
    Write-Host "Already inside a virtual environment: $env:VIRTUAL_ENV"    
}


# source $projectdir\Scripts\activate
. $projectdir\vmupacker_venv\Scripts\Activate.ps1

# Add crcmod, pillow, etc
pip install -r $tooldir\requirements.txt

Write-Host "VMUPacker env created"
Write-Host "Ready!"

# Running `where.exe python` should point at the ESP IDF's python
# this will be automatic if you've ran the idf export script before building
python $tooldir/packer.py --projectdir $projectdir --elfname $elfname --icon $icon --meta $meta --sdkversion 1.0.0 --debug true

Write-Host "closing the python vmupacker venv"
deactivate
