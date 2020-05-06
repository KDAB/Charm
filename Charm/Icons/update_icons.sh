#!/bin/bash

icon_base_dir="/usr/share/icons/oxygen/base/"
res_1x="22x22"
res_2x="48x48"

48x48/actions/document-new.png

if [ ! -d "$icon_base_dir" ]; then
    echo "ERROR: icon theme not installed? $icon_base_dir does not exist"
    exit 1
fi

cd $(dirname $0)

function copy_if_exists
{
    res="$1"
    name="$2"
    newname="$3"
    i=$(find "$icon_base_dir/$res/" -name "$name" -type f -print -quit)
    if [ ! -z "$i" ]; then
        cp -v "$i" "$newname"
    fi
}

for f in *.png; do
    if [[ "$f" == *"@2x.png" ]]; then
        continue
    fi
    echo $f
    copy_if_exists "$res_1x" "$f" "$f"
    copy_if_exists "$res_2x" "$f" "${f/.png/}@2x.png"
done
