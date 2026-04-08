#
# Creates the python venv and passes the build params
# 2025, 8BM
#


# Need the tool's folder path to properly locate requirements.txxt
# i.e. the location where THIS script resides
$tooldir = Split-Path -Parent $PSCommandPath
Write-Host "Packer tool's dir: $tooldir"

$projectdir = $(Get-Location)
Write-Host "Project dir: $projectdir"

$venvdir = "$projectdir\vmupacker_venv"
Write-Host "Venv dir: $venvdir"

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


# source cwd\vmupacker_venv\Scripts\activate
. $venvdir\Scripts\Activate.ps1

# Add crcmod, pillow, etc
pip install -r $tooldir\requirements.txt

Write-Host "VMUPacker env created"
Write-Host "Ready!"

# start with required args
$argsList = @(
    "$tooldir/packer.py",
    "--projectdir", $projectdir,
    "--sdkversion", "1.0.0"
) + $args

# Running `where.exe python` should point at the ESP IDF's python
# this will be automatic if you've ran the idf export script before building
python @argsList

Write-Host "closing the python vmupacker venv"
deactivate
