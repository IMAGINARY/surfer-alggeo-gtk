#!/bin/bash
echo Installs surfer and creates a .deb from the installation
sudo make install
DEB=./deb
rm -rf $DEB/usr/local
mkdir -p $DEB/usr/local/bin/
mkdir -p $DEB/usr/local/share/surf/
mkdir -p $DEB/usr/local/share/surfer/ 
cp /usr/local/bin/surf $DEB/usr/local/bin/
cp /usr/local/bin/surfer $DEB/usr/local/bin/
cp -r /usr/local/share/surf/ $DEB/usr/local/share/surf/
cp -r /usr/local/share/surfer/ $DEB/usr/local/share/surfer/ 
rm -f $DEB/usr/local/share/surfer/gallery/gallery.tar
cp surfer_ui_exhibition.xml $DEB/usr/local/share/surfer/
dpkg -b ./deb surfer.deb
