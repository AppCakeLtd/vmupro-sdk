
python ..\..\tools\packer\send.py --func reset

idf.py build
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

./pack.ps1
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

./send.ps1