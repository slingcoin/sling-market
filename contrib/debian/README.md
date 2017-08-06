
Debian
====================
This directory contains files used to package slingd/sling-qt
for Debian-based Linux systems. If you compile slingd/sling-qt yourself, there are some useful files here.

## sling: URI support ##


sling-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install sling-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your slingqt binary to `/usr/bin`
and the `../../share/pixmaps/sling128.png` to `/usr/share/pixmaps`

sling-qt.protocol (KDE)

