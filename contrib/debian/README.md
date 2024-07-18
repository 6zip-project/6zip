
Debian
====================
This directory contains files used to package zipd/zip-qt
for Debian-based Linux systems. If you compile zipd/zip-qt yourself, there are some useful files here.

## zip: URI support ##


zip-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install zip-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your zip-qt binary to `/usr/bin`
and the `../../share/pixmaps/zip128.png` to `/usr/share/pixmaps`

zip-qt.protocol (KDE)

