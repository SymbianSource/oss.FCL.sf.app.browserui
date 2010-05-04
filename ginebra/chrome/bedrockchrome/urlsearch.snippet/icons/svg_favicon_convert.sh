#!/bin/sh


# toolbar icons
ICON_WIDTH=32
ICON_HEIGHT=32

# toolbar icon borders
#ICON_WIDTH=1
#ICON_HEIGHT=30

echo
echo "Usage: $0 <list of svg files to convert>"
echo "This script converts svg icons into png icons with w=$ICON_WIDTH h=$ICON_HEIGHT"
echo "DPI is default, 90dpi"
echo "Edit the script to change the size\n"
echo "... Requires librsvg2-bin package from ubuntu\n"

echo "CONVERTING the files listed below:"

for i in $*; do
    rsvg-convert -w $ICON_WIDTH -h $ICON_HEIGHT $i -o `echo $i | sed -e s/svg$/png/`
    echo $i;
done

