#!/bin/bash
echo Installs surfer and creates a .deb from the installation
# build surf
cd surf
sudo apt-get install build-essential g++ flex bison zlib1g-dev libjpeg-dev libtiff-dev libgmp3-dev libx11-dev libgtkmm-2.4-dev
chmod +x ./configure ./missing
CFLAGS="-O3 -fopenmp" CXXFLAGS="-O3 -fopenmp" ./configure --disable-gui
make
cd ..

# build and install surfer
chmod +x ./configure ./missing
CFLAGS="-O3 -fopenmp" CXXFLAGS="-O3 -fopenmp" ./configure
sudo make install
DEB=./deb
rm -rf $DEB/usr/local
mkdir -p $DEB/usr/local/bin/
mkdir -p $DEB/usr/local/share/surf/
mkdir -p $DEB/usr/local/share/surfer/ 
cp ./surf/surf $DEB/usr/local/bin/
cp /usr/local/bin/surfer $DEB/usr/local/bin/
cp -r /usr/local/share/surfer/ $DEB/usr/local/share/surfer/ 
rm -f $DEB/usr/local/share/surfer/gallery/gallery.tar
cp surfer_ui_exhibition.xml $DEB/usr/local/share/surfer/
dpkg -b ./deb surfer.deb
