# vmupro-sdk



## Setting up your environment

The ESP SDK is called the IDF.
It's a fairly straightforward 2-step proces where you 
- first run an export script to set your current shell/terminal session's environment vars up properly
- run `idf.py build` *

### Download the ESP IDF (SDK)

This is the Espressif SDK for the ESP series.
For this guide we'll be using c:\idfvmusdk\ or /stuff/idfvmusdk/

Start by cloing the repo:
`cd c:\`
`git clone https://github.com/espressif/esp-idf idfvmusdk`

or

`cd /stuff/`
`git clone https://github.com/espressif/esp-idf idfvmusdk`

### Check out the latest 4.5 branch
In a GUI manager like git-fork (strong recommend), this would be something like
`Ctrl + O -> c:\idfvmusdk\`
`Cmd + O -> /stuff/idfvmusdk/`

Then locate `release/v5.4` and `r.click` -> `origin/release/v5.4` -> `Checkout`
It could take 5 minutes to complete; good time for a cup of tea.

### Run the install script

`c:\idfvmusdk\export.ps1`
or
`/stuff/idfvmusdk/install.sh`

On Ubuntu/WSL you may be prompted to install python-venv for 3.8
(It's a good idea to update apt-get while you're here or the download may fail)
`apt-get update`
`sudo apt install python3.8-venv`

Assuming everything went well, this is a one-time process and you're good to go.

## Building your first .app.elf and .vmupack

It's important to CD to your project root
e.g.
`cd c:\idfvmusdk\examples\minimal`
`cd /stuff/idfvmusdk/examples/minimal`

Load in the environment
`c:\idfvmusdk\export.ps1`
`. /stuff/idfvmusdk/export.sh` *

* don't forget the leading dot, it'll warn you though! `.`

And finally:
`idf.py build`

Note: on windows `idf.py` is a frozen .exe file: `idf.py.exe`
You *don't* need to prefix with `py` / `python` / `python3` etc

## packing your app.elf into a .vmupack



# Questions:

### Can I intergrate with VSCode?
Yes, there are at least 2 ways to achieve this:

1: install the vscode ESP IDF extension.

2: Have the VSCode console scrape the terminal's output.
It's not perfect, but it's lightweight.

Assuming your project is `c:\projects\myproject\` or `/projects/myproject`

Create or modify 
`c:\projects\myproject\.vscode\tasks.json`
or
`/projects/myproject/vscode/task.json`

And add the following:

```{
    "tasks": [

        {
          "type": "cppbuild",
          "label": "idf.py build NO FLASH",
          "detail": "detail for build NO FLASH",
          "command": "idf.py",
          "args": [
              "build"
          ],
          "options": {
              "cwd": "${workspaceFolder}"
          },
          "problemMatcher": [
              "$gcc"
          ],
          "group": {
              "kind": "build",
              "isDefault": true
          }
          
        }
    ],
    "version": "2.0.0"
}
```

After running your `export.ps1` `export.sh`
run
`Start-Process code -ArgumentList "yourproject.code-workspace" -windowstyle hidden`
or `code .`
for vscode to inherit the IDF environment.
Build your project via `Ctrl+Shift+B` or via the `F1` key task palette.

Add extra tasks for e.g. `idf.py build flash monitor -p YOURCOMPORT`, `idf.py fullclean`, etc


