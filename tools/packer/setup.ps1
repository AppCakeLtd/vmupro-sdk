write-host VMUPacker: checking virtual environment
if ($env:VIRTUAL_ENV) {
    Write-Host "Already inside a virtual environment: $env:VIRTUAL_ENV"
    Write-Host "Type `deactivate` to start a fresh environment"
    exit 0
}

write-host VMUPacker: creating python3 venv
python -m venv .venv
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to create venv"
    exit 1
}

# call .venv\Scripts\activate
. .\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
write-host VMUPacker env created
write-host VMUPacker: you can activate manually via .venv/Scripts/activate
write-host Ready!

