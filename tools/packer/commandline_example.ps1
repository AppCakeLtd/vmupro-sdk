
write-host VMUPacker: checking virtual environment
if (-not $env:VIRTUAL_ENV) {

    write-host VMUPacker: creating python3 venv
    python -m venv .venv
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to create venv"
        exit 1
    }

} else {
    Write-Host "Already inside a virtual environment: $env:VIRTUAL_ENV"
    Write-Host "Type `deactivate` to start a fresh environment"
}

# call .venv\Scripts\activate
. .\.venv\Scripts\Activate.ps1

pip install -r requirements.txt

# package the minimal example
# accepts relative or absolute paths
py .\packer.py  --basedir ../../examples/minimal --elfname vmupro_minimal --sdkversion "0.0.1" --meta ./metadata.json --icon icon.bmp --debug true


