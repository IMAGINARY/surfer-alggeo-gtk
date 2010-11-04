#!/bin/bash
echo -e "\033[1mInstalls surfer and creates a .deb from the installation" && tput sgr0
echo -e "\033[1mbuilding surf" && tput sgr0
cd surf
sudo apt-get install build-essential g++ flex bison zlib1g-dev libjpeg-dev libtiff-dev libgmp3-dev libx11-dev libgtkmm-2.4-dev
chmod +x ./configure ./missing
CFLAGS="-O3 -fopenmp" CXXFLAGS="-O3 -fopenmp" ./configure --disable-gui
make
cd ..

echo -e "\033[1mbuilding and installing surfer" && tput sgr0
chmod +x ./configure ./missing
CFLAGS="-O3 -fopenmp" CXXFLAGS="-O3 -fopenmp" ./configure
sudo make install

DEB=`mktemp -d -p . surfer_deb.XXXXXXXXXX` || exit 1
echo -e "\033[1mcreating .deb content in $DEB" && tput sgr0

cp -r deb/* $DEB/
find $DEB/ -type d -regex '.*/.svn$' | xargs rm -rf

mkdir -p $DEB/usr/local/bin/
mkdir -p $DEB/usr/local/share/surf/
mkdir -p $DEB/usr/local/share/surfer/

echo -e "\033[1mcopying surf and surfer binaries" && tput sgr0
cp ./surf/surf $DEB/usr/local/bin/
cp /usr/local/bin/surfer $DEB/usr/local/bin/

echo -e "\033[1mcopying the gallery" && tput sgr0
cp -r /usr/local/share/surfer/* $DEB/usr/local/share/surfer/ 
rm -rf $DEB/usr/local/share/surfer/gallery.tar

echo -e "\033[1mcopying locales" && tput sgr0
LOCALE_FILES="/usr/local/share/locale/de/LC_MESSAGES/surfer.mo
/usr/local/share/locale/en@quot/LC_MESSAGES/surfer.mo
/usr/local/share/locale/en@boldquot/LC_MESSAGES/surfer.mo
/usr/local/share/locale/en_GB/LC_MESSAGES/surfer.mo"
for lf in $LOCALE_FILES
do
	mkdir -p `dirname $DEB$lf`
	cp $lf $DEB$lf
done

echo -e "\033[1mcopying ui style files" && tput sgr0
cp surfer_ui_exhibition.xml $DEB/usr/local/share/surfer/

echo -e "\033[1mpackaging .deb" && tput sgr0
dpkg -b $DEB surfer.deb

echo -e "\033[1mremoving temporary folder $DEB" && tput sgr0
rm -rf $DEB
