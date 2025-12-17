## Installation:
Download the relevant installer for you system (Linux, macOS, or Windows) from the Releases section

Only 64 bit versions of Windows 10 and later are supported in this release.

If you need a version of DeepSkyStacker that will run on older versions of Windows or on 32 bit versions, you should use DeepSkyStacker 4.2.6:

   [https://github.com/deepskystacker/DSS/releases/tag/4.2.6](https://github.com/deepskystacker/DSS/releases/tag/4.2.6)

### Linux:

DeepSkyStacker requires that kchmviewer is installed so that the help can be displayed.
To install that on Ubuntu versions of Linux:

    sudo apt-get install kchmviewer

If you are running a different flavour of Linux, then you'll need to install it some other way.

Once you've downloaded the installer on Linux, you will most likely need to make it executable.  For example:

    chmod +x DeepSkyStacker-6.1.2-linux-x64-installer.run

You need to run the installer with root authority, so you should invoke it like this (for example):

    sudo ~/Downloads/DeepSkyStacker-6.1.2-linux-x64-installer.run

If DeepSkyStacker won't start from the Desktop entries then you should try open a terminal window and run:

`/opt/DeepSkyStacker/DeepSkyStacker`

If you see:

```
qt.qpa.plugin: From 6.5.0, xcb-cursor0 or libxcb-cursor0 is needed to load the Qt xcb platform plugin.
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: xcb.
```
then you should install libxcb-cursor0 or xcb-cursor0 (whichever your installer offers)

`sudo apt-get install libxcb-cursor0`

worked for me on Ubuntu and Linux Mint systems.

If you get errors like:
```
/lib/x86_64-linux-gnu/libstdc++.so.6: version `GLIBCXX_3.4.32' not found (required by ./DeepSkyStacker)
/lib/x86_64-linux-gnu/libstdc++.so.6: version `GLIBCXX_3.4.31' not found (required by ./DeepSkyStacker)
```
then you need install a later version of libstdc++ :
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install libstdc++-13-dev
```
should do the trick.

### macOS:

Just open the downloaded .dmg and run the installer that it contains

#### Setting up a symlink for using DeepSkyStackerCL on macOS

DeepSkyStackerCL is delivered in the DeepSkyStacker.app bundle.   To use it you'll need define a symlink to it in e.g. /usr/local/bin
```
#
# If you need to, create the directory /usr/local/bin
#
sudo mkdir -p /usr/local/bin
#
# Add a symlink so you can invoke DeepSkyStackerCL by typing DSSCL
#
sudo ln -sf /Applications/DeepSkyStacker/DeepSkyStacker.app/Contents/MacOS/DeepSkyStackerCL /usr/local/bin/DSSCL
```

You may also need to add /usr/local/bin to your path:  You can do this by creating a file in your home directory
called .zshenv (assuming you're using zsh) this file might contain e.g.

```
# remove duplicate entries from $PATH
# zsh uses $path array along with $PATH 
typeset -U PATH path

pathadd () {
     	  if [ "$2" = "after" ] ; then
              PATH=$PATH:$1
       	  else
              PATH=$1:$PATH
          fi
        }
pathadd /usr/local/bin
pathadd ~/bin
```
### Windows:
On Windows you should right-click on the downloaded installer and select Properties. At the bottom of the first tab you will see a message saying:

![220931439-f74b36b3-9c25-42b0-a5c8-7ac271c98570](https://github.com/deepskystacker/DSS/assets/20211762/e1b9841c-2b34-4b6c-a90e-4133be3adbbb)

Select "Unblock", and click on OK. You should then be able to run the installer.
