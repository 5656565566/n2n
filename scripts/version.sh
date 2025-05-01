#!/bin/sh
#
# Output the current version number
#

usage() {
    echo "Usage: $0 [date|short|hash]"
    echo
    echo "Determine the correct version number for the current build"
    exit 0
}

# We assume this script is in the TOPDIR/scripts directory and use that
# to find any other files we need
TOPDIR=$(cd "$(dirname "$0")/.." && pwd)

VER_FILE_SHORT=$(cat "${TOPDIR}/VERSION" 2>/dev/null || echo "v0.0.0")

if [ -d "$TOPDIR/.git" ]; then
    cd "$TOPDIR" || exit 1

    # Get latest tag (annotated or lightweight)
    VER_GIT_SHORT=$(git describe --tags --abbrev=0 2>/dev/null || echo "$VER_FILE_SHORT")

    if [ "$VER_FILE_SHORT" != "$VER_GIT_SHORT" ]; then
        echo "Warning: VERSION file ($VER_FILE_SHORT) does not match latest tag ($VER_GIT_SHORT)"
    fi

    VER_SHORT="$VER_GIT_SHORT"
    VER_HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
    VER=$(git describe --tags --abbrev=7 --dirty 2>/dev/null || echo "$VER_FILE_SHORT-$VER_HASH")
    DATE=$(git log -1 --format=%cd 2>/dev/null || date)
else
    # Fallback for non-Git environments
    VER_SHORT="$VER_FILE_SHORT"
    VER_HASH="HEAD"
    VER="$VER_FILE_SHORT"
    DATE=$(date)
fi

case "$1" in
    date)
        echo "$DATE"
        ;;
    hash)
        echo "$VER_HASH"
        ;;
    short)
        echo "$VER_SHORT"
        ;;
    "")
        echo "$VER"
        ;;
    *)
        usage
        ;;
esac
