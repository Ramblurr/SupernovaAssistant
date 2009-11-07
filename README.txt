
SuperNova Assistant
-------------------------------------------------------------------------------

SNAssistant is a cross platform GUI client for the play-by-email (PBEM)
game Supernova: Rise of the Empires created and managed by Rolling Thunder
http://www.rollingthunder.com/supernova/index.htm

From the SN Website:
SuperNova is an interactive game of galatic conquest, colonization and exploration. 
SuperNova is an open-ended game so you can join at any time and turns are processed 
simultaneously every two weeks. Download the rules, etc. then join our SuperNova 
community on the Rolling Thunder Forums to get going. Once we have your account 
set up and your SuperNova setup information in hand...your people will enter the 
galaxy on our next cycle and you can begin to build your empire!


SNAssistant is licensed under the GPL 3, and is written in C++ with Qt

Compiling / Running
----------------
Prerequisites for Linux/Mac OS X/Windows:

* Qt >= 4.5
* Poppler >= 0.12.1 (with poppler-qt4)
* SQLite3

SN Assistant uses the cmake build system, which is supported on all major 
operating systems.

On Linux/Mac OS X

From the source directory:
$ mkdir build
$ cd build
$ cmake ../   # This will attempt and find all the dependencies
$ make
$ ./SNAssistant

On Windows

No idea, but you will need the Qt SDK for windows that includes mingw from:
http://qt.nokia.com/downloads/sdk-windows-cpp

Pre-compiled windows binaries of Poppler and its dependencies are available in the
bin/ directory


