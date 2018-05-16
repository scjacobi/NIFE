# NIFE
No-Intro Front-End

This application is designed to be a no-frills WIMP emulation front-end.  It's an attempt to update some of the basic concepts in my original front-end GoodFE, some of the better concepts from Gelide, and mix in some improvements that I thought of along the way.

In contrast to many of the popular modern emulation front-end that provide an entire multimedia experience whttps://github.com/scjacobi/NIFE.githen selecting a game, NIFE is designed to be entirely simple.  At its core, it is a front-end that supposed to help a user with a ton of roms for different systems (such as those who have complete No-Intro rom sets) very quickly choose the system they want, the emulator they want to use, and the game they want to play, in very little time.

I was inspired to write this because I used Gelide on Ubuntu for 7 years.  Then Ubuntu 18.04 was released.  I upgraded to it, and the libraries required to run Gelide were so outdated, I could no longer build it.  The author of Gelide appeared to have abandoned it.  I tried to run Metropolis Launcher (a very impressive project) through WINE, but it didn't work.  So I had two options: switch to one of the fancier front-ends which I didn't want to take the time to setup, or download gigs of artwork and movies for, or write my own.  I wrote one many years ago that hadn't aged well, and wasn't cross-platform compatible, and I was curious about the QT framework, so I decided to learn how to use it.

I am by no means a professional UI developer, I am certain there are better ways to implement much of what I did.  Some improvements that I tried to implement over both GoodFE and Gelide were:
* Support for an arbitrary number of ROM paths per system, which can be selected via browser-like tabs at the top of the ROM list.
* As in Gelide, support for an arbitrary number of emulators per system.
* Decoupling emulators from systems such that you manage emulators independently, and then associate them with the systems afterword.
* Free-form image association for each system.  Images can be screenshots, title screen, box art, whatever you want.  You simply point the front-end to any two paths containing artwork, and it will attempt to display whatever is in them.

All settings are saved in a .nife folder in your home directory.  When you first start the application, you'll have an empty system list in the upper left corner, and an empty emulator list in the lower left.  To get started, right click in the systems list window to begin adding a system.  When you're done with that, right click in the emulator list window to start associating some emulator, which you will have to define in the emulator management window.

In addition to the more obvious user interfaces, there are some shortcuts design to improve the overall experience.  You can launch a game by double clicking the filename, or by keeping the focus on the file list and pressing Enter.  You can edit any system or emulator by double clicking on them.  You can move systems up and down in the systems list by pressing PageUp or PageDown.

This project will likely be very WIP for quite some time, but I welcome feedback either through github, or email me at procyon.sjj (at) gmail.com
