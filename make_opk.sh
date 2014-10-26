#!/bin/sh

OPK_NAME=wetspot2.opk
echo ${OPK_NAME}

# create default.gcw0.desktop
cat > default.gcw0.desktop <<EOF
[Desktop Entry]
Name=Wetspot 2
Comment=Maze puzzle
Exec=wetspot2
Terminal=false
Type=Application
StartupNotify=true
Icon=icon
X-OD-Manual=README.TXT
Categories=games;
EOF

# create opk
FLIST="data"
FLIST="${FLIST} music"
FLIST="${FLIST} world"
#FLIST="${FLIST} world/chris2"
#FLIST="${FLIST} world/funk"
#FLIST="${FLIST} world/nekro"
#FLIST="${FLIST} world/return"
#FLIST="${FLIST} world/ricland"
#FLIST="${FLIST} world/seav"
#FLIST="${FLIST} world/squinky"
#FLIST="${FLIST} world/surprise"
#FLIST="${FLIST} world/wafn"
#FLIST="${FLIST} world/*.MID"
#FLIST="${FLIST} world/WETSPOT2.WWD"
FLIST="${FLIST} default.gcw0.desktop"
FLIST="${FLIST} icon.png"
FLIST="${FLIST} wetspot2"
FLIST="${FLIST} README.TXT"

rm -f ${OPK_NAME}
mksquashfs ${FLIST} ${OPK_NAME} -all-root -no-xattrs -noappend -no-exports

cat default.gcw0.desktop
rm -f default.gcw0.desktop
