# GomokuUIQ

Tutorial Solution for Gomoku game on Symbian / UIQ

Gomoku, v1.10.0
Copyright by Andreas Jakl, 2007-2009
andreas.jakl@live.com


# About

Gomoku (also known as Connect Five or Pente (TM)) is a traditional and very popular game that is a lot of fun and easy to play for two players. All you need is a sheet of checkered paper, a pen and a friend, and you're ready to go. Your task is to get an unbroken row of five stones (horizontal, vertical or diagonal). As you play in turns, you will usually have to think of some nasty traps in order to win.

Gomoku for Symbian lets you play this game everywhere, no matter if you have got a sheet of paper, a pen or a friend near you. It allows playing both in single player mode against a challenging artificial intelligence as well as in two-player mode. Additionally, you can freely define the board size and activate the "pair check" option, which allows to capture two enemy stones by enclosing them.


# Compatibility and Installation

Gomoku is compatible to all S60 3.x, 5.x, Symbian^1 and upcoming Symbian^2 based mobile phones. It supports both the touchscreen and a keyboard and multiple screen sizes. An extra version for UIQ 3 is available.

Please install the .sisx-file on your phone. Please see the manual of your phone for more information on how to install applications.


# License

It is licensed under the Eclipse Public License v1.0, which can be obtained from:
http://www.eclipse.org/legal/epl-v10.html


# Original Tutorial

Exercise / Project

# Goal

A full-blown Gomoku game is the base of this exercise, which allows a variety of tasks. These range from implementing ECom plug-ins over the use of background active objects to TCP/IP or Bluetooth-communication.

# Introduction

The tasks of this exercise all extend a complete game for UIQ 3. Due to its architecture, the game engine and the UI are completely separated from the tasks you have to do, keeping the complexity of your tasks down and making them very focused.

The game concept has got many names, also depending on the exact variant. The most traditional name is _Gomoku_, more well-known names are _connect five_ or _Pente™_. The goal is simple – in turns, two players place &quot;stones&quot; on a grid. The first who achieves to have five (or more) stones directly adjacent in a row, wins the game. This can be in vertical, horizontal and diagonal direction.

A few (non-successive!) impressions of the game implementation:

 ![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/gomoku-step1.png)
 ![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/gomoku-step2.png)
 ![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/gomoku-step3.png)
 ![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/gomoku-step4.png)

A sub-variant of the Gomoku game is called the &quot;Pair Check&quot;-option in this game. If a player manages to enclose two stones of the enemy player with his own, the stones of the enemy are removed (captured) from the playing field. In addition to the usual way of winning a game, this allows an additional scenario: the first player who has captured five or more pairs will also win the game.

The specialty of this game is the way the players (human player, artificial intelligence (AI) or network player) are implemented. They are completely separate from the rest of the game engine and UI and are loaded through the Symbian OS ECom framework. This means that additional player implementations can be installed even while the game is running.

Tasks of this exercise all focus on the player plug-ins, as the game itself would be too complex and is in large parts related to the architecture of the UIQ 3 user interface, which is not covered by these materials.

# The ECom framework

## Plug-Ins

Generally, plug-ins are used to extend the functionality of an application. This mechanism works well with a well-defined interface. This also allows providing multiple implementations – even separately from the main application. Examples are the use of plug-ins to support additional image formats for a gallery application, or – as in this case – to support multiple player types or AI implementations.

If the game would have been designed as a monolithic application, everything would be part of one big .exe file, which does not allow extending the game.

![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/architecture-monolithic.png)

With a different architecture, this issue is solved. The classes related to the player interface are moved into an own DLL. The individual player implementations are also shipped as their own DLLs, which are dynamically loaded by the main application. This allows easy extension of the application, even at run-time. The ECom framework handles resolving and instantiating those implementations.

![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/architecture-ecom.png)

## DLLs and the ECom Framework

On the device, each application runs in its own process. If it loads a DLL (Dynamic Link Library), the instance is loaded into the process of the client application. Plug-ins are usually provided as DLLs.

When using plug-ins, clients always had to provide their own code to find, load, instantiate and unload the plug-in implementations. Symbian OS v7.0 introduced a new mechanism called ECom framework, which provides generic means of loading plug-in code.

![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/ecom-framework.png)

One of the great advantages is that the client is usually not aware that he is using the ECom framework to instantiate an interface – the service remains &quot;hidden&quot; and the client is automatically making use of the correct instantiation function. The application developer only needs to be concerned with the behavior of the plug-in as well as how to interact with it.

## Platform Security

Another important reason why to use the ECom framework for your own implementations is the Platform Security concept that was introduced with Symbian OS 9. This caused nearly all system-services used by the OS to be migrated from specific plug-in loading code to using the ECom framework as well.

Searching for available polymorphic DLLs is usually not really possible anymore. All binaries are located in the \sys\bin\ folder and cannot be executed from any place outside this highly secured directory. As only applications with the AllFiles capability can read the contents of the directory, it is very inconvenient for a 3

# rd
 party application to dynamically scan for installed DLLs. The ECom plug-ins circumvent this problem, as the plug-ins register with the framework upon installation. During this process, they have to provide information about their existence, their supported interface and how to load the particular plug-in. This allows the client to get a list of available implementations without manually scanning for installed DLL files.

Another issue related to Platform Security is that polymorphic DLLs now require at least the same capabilities as its loading process, restricting the use and flexibility of DLLs. A solution is to use EXEs for plug-ins as well, which then run in an own process, with only the individually required capabilities. This is not done automatically by the ECom framework, but can be easily implemented using a client / server-side glue code. For implementation details, see the _Symbian OS Platform Security_-book [3].

# Structure of this Exercise

The full reference implementation of this game is split up into three components, each with its own .mmp-file:

- -- **The game implementation** (mmp) is a normal UIQ application with a user interface.
- -- **The player interface** (mmp) defines the interface that is required so that the game engine can communicate with the player implementations. It is realized as a library that is linked by both other components.
- -- **The player implementation** (mmp) is a collection of ECom plug-ins that implement the player interface (CGomokuPlayerInterface). These are compiled as a special kind of DLL (targettype PLUGIN).
 
![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/gomoku-architecture.png)

The user interface of the game is managed by two view-classes (CGomokuViewSettings, CGomokuViewGame). Both views store pointers to the CGomokuGameData-class, which is responsible for storing the settings, both player implementations and information about an active game.

The settings view is the default view and is made of two pages, the first for choosing two player implementations from the ECom plug-ins that were found by the framework. The second page allows to enable the pair check option and to set the grid size.

Afterwards, the application switches to the game view. The grid is implemented as a control (derived from CCoeControl) and is therefore owned by the control stack of the view&#39;s base class (CQikViewBase). This ensures that it automatically gets the chance to draw itself to the screen whenever necessary; it will also receive pointer and key events. Those can then be translated to cell coordinates and are sent to the player implementations (necessary especially for a human player, which has to use the selected cell as the player&#39;s next move – in case this move is legal).

All player implementations have to derive from CGomokuPlayerInterface and are instantiated through their name by the ECom framework. The individual names and the corresponding implementation UIDs are specified in the resource file (GomokuPlayerImplementation.rss) of the ECom plug-in component.

Plug-ins receive the game-relevant events from the game engine through defined methods of the interface (e.g. for initialization, a new turn or a game over-event). When they have finished processing their turn, a call-back method of the MGomokuPlayerObserver-interface will send the event to game engine. The pointer to the listener is set directly after instantiation of the interface, in the case of the game the listener class is the game view.

## ECom Framework – Short Overview

As mentioned before, the ECom framework can transparently instantiate the implementation without requiring the client to call ECom-specific functions – it only has to link to the ECom client library (ecom.lib) and make sure the session is cleaned up when it will not be used anymore. This is done by calling REComSession::FinalClose(). In the Gomoku game, you can find this call in the destructor of the AppUi-class.

ECom itself is built using a client-server architecture. Therefore, the ECom server is executed in a process separate from the client. As it has got the AllFiles capability, it is able to scan the /sys/bin/-folder. ECom uses resource file metadata to construct a registry of all interface implementations currently installed and available on a device.

Plug-ins have to implement an interface class, which is not surprising given their intended use. Additionally, the interface also has to provide one or more factory functions to allow clients to instantiate implementation objects.

These factory functions do not instantiate the objects themselves, though. Instead, they issue a request to the ECom server, which instantiates the appropriate object at runtime. To choose which implementation to use, the interface has to pass the ECom framework a cue, which can either be the implementation UID or some text – in the case of Gomoku, the name of the implementation. It is also possible to fetch a list of all installed implementations, which is necessary to install additional plug-ins later on.

The following figure is based on the _Symbian OS Explained_-book [2] and shows the relationship of the various classes involved in the ECom plug-in resolution:

![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/ecom-plugin-resolution.png)

## UIDs

A UID is a signed 32-bit value, which is used as a globally **u** nique **id** entifier. To make it possible for the ECom framework to correctly associate and handle interfaces as well as their implementations, each is identified by its own UID. This section presents a short overview of all UIDs used in the reference implementation:

|   | Type | Value | Description | Location |
| --- | --- | --- | --- | --- |
| **Gomoku** | targettype (UID 1) | EXE | Type of the application. | Gomoku.mmp |
| UID 2 | 0x00000000 | Further subdivision of the type specified by UID 1. Not required for EXE files. | Gomoku.mmp |
| UID 3 | 0xE9192D94 | Uniquely identifies the binary. | Gomoku.mmp |
| **PlayerInterface** | targettype (UID 1) | DLL | Type of the application. | PlayerInterface.mmp |
| UID 2 | 0x1000008D | Further subdivision of the type specified by UID 1. This value is common to all static interface DLLs. | PlayerInterface.mmp |
| UID 3 | 0xE00000EA | Uniquely identifies the binary. | PlayerInterface.mmp |
| KCGomokuPlayerInterfaceUid | 0xE0000E00 | UID of the interface that has to be implemented by plug-ins. | GomokuPlayerInterface.h |
| **PlayerImplementation** | targettype (UID 1) | PLUGIN | Type of the application. | PlayerImplementation.mmp |
| UID 2 | 0x10009D8D | Further subdivision of the type specified by UID 1. This value is common for all ECom plug-ins. | PlayerImplementation.mmp |
| UID 3 | 0xE00000EB | Uniquely identifies the binary. | PlayerImplementation.mmp |
| interface\_uid | 0xE0000E00 | UID of the implemented interface (references KCGomokuPlayerInterfaceUid from the player interface definition) | GomokuPlayerImplementation.rss |
| implementation\_uid | 0xE0000E01 | UID of the human player interface implementation. | GomokuPlayerImplementation.rss |
| implementation\_uid | 0xE0000E02 | UID of the random AI interface implementation. | GomokuPlayerImplementation.rss |
| implementation\_uid | 0xE0000E03 | UID of the reference AI interface implementation. | GomokuPlayerImplementation.rss |

Remember that UIDs in the range from 0xE0000000 to 0xEFFFFFFF are reserved for development use only. These UIDs should not to be used when distributing applications.

If different AI implementations are to be created by different students, every binary has to have its own UIDs. A simple way to prevent clashes would be to incorporate the individual registration number of the student into the UID.

## Notes on Building the Game

As the reference implementation is using DLLs, you have to freeze the exports in order to successfully link the executables. In Carbide.c++, this is done by right-clicking on the project and selecting _&quot;Freeze Exports&quot;_. When exporting for the first time, you have to freeze the exports twice. If you need to change the interface (possibly when implementing a network player), it&#39;s safer to clean the project first and to delete the BWINS and EABI-directories in the project folder, which contain the export table before freezing the exports again.
 
![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/freeze-exports.png)

When compiling for the device, the game is configured to be released in two .sis-files. Gomoku.sis only contains the game engine without any player implementations. One or more of those can be installed afterwards through extra .sis-files.

Note that Carbide.c++ is currently only able to associate a single .pkg-file with a build configuration. In order to update both .sis-files, you either have to create two build configurations for the same target or manually instruct the IDE to compile the second .sis-file after you compiled the whole project. Note that this only applies to compiling for the device and not for the emulator.

![](https://raw.githubusercontent.com/andijakl/GomokuUIQ/master/media/build-pkg-file.png)

As the options for compiling the second .pkg-file cannot be specified by this method (e.g. when using an own certificate), a batch-file has been provided (CreateGomokuSis.bat). This requires that the application has already been built and that the .pkg-files have already been resolved by Carbide.c++ – thus, you have to create the .sis-files through Carbide.c++ at least once before using the batch file.



# Recommended Literature

For more information about the ECom framework, take a look at the following resources. The current version of the Symbian OS Explained-book [2] features a good explanation of the ECom framework, but does not yet include Platform Security aspects and the changes required for Symbian OS 9.

[1] Harrison, R. and M. Shackman: _Symbian OS C++ for Mobile Phones (Volume 3)_. Symbian Press, 2007.
[2] Stichbury, J: _Symbian OS Explained_. Symbian Press, 2005.
[3] Heath, C: _Symbian OS Platform Security_. Symbian Press, 2006.
[4] Nokia: _S60 Platform: ECom Plug-in Architecture v2.0_. 2007. Available online at [http://www.forum.nokia.com/](http://www.forum.nokia.com/)
[5] Nokia: _S60 Platform: ECom Plug-In Examples v2.0._ 2007. Available online at [http://www.forum.nokia.com/](http://www.forum.nokia.com/)

