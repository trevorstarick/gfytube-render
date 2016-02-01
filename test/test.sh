#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
    /Users/trevorstarick/Library/Caches/CLion12/cmake/generated/1d3c71c2/1d3c71c2/Debug/gfytube_render -g $line
    echo ""
done < "$1"
