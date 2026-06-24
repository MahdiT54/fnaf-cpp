#!/usr/bin/env bash
# Builds console_fnaf and packages a macOS zip for GitHub Releases.
# Run on a Mac with: brew install cmake ncurses
# Usage: ./scripts/package-macos.sh

set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "This script must be run on macOS (build and bundle dylibs for Mac players)." >&2
  exit 1
fi

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT/build"
DIST_DIR="$ROOT/dist"
PACKAGE_NAME="console-fnaf-macos"
PACKAGE_DIR="$DIST_DIR/$PACKAGE_NAME"
ZIP_PATH="$DIST_DIR/$PACKAGE_NAME.zip"
BIN_NAME="console_fnaf"

echo "==> Configuring and building..."
cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --config Release

BIN_PATH="$BUILD_DIR/$BIN_NAME"
if [[ ! -f "$BIN_PATH" ]]; then
  echo "Build failed: $BIN_PATH not found" >&2
  exit 1
fi

echo "==> Staging release folder..."
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR/lib"

cp "$BIN_PATH" "$PACKAGE_DIR/"
chmod +x "$PACKAGE_DIR/$BIN_NAME"

bundle_deps() {
  local target="$1"
  local dep path base dest_id

  while IFS= read -r dep; do
    [[ -z "$dep" ]] && continue
    case "$dep" in
      /usr/lib/*|/System/*|@executable_path/*)
        continue
        ;;
    esac
    if [[ ! -f "$dep" ]]; then
      continue
    fi

    base="$(basename "$dep")"
    dest_id="@executable_path/lib/$base"

    if [[ ! -f "$PACKAGE_DIR/lib/$base" ]]; then
      cp "$dep" "$PACKAGE_DIR/lib/$base"
      install_name_tool -id "$dest_id" "$PACKAGE_DIR/lib/$base"
      bundle_deps "$PACKAGE_DIR/lib/$base"
    fi

    install_name_tool -change "$dep" "$dest_id" "$target"
  done < <(otool -L "$target" | tail -n +2 | awk '{print $1}' | sed '/:$/d')
}

echo "==> Bundling runtime libraries..."
bundle_deps "$PACKAGE_DIR/$BIN_NAME"

cp "$ROOT/release/Play.command" "$PACKAGE_DIR/"
chmod +x "$PACKAGE_DIR/Play.command"
cp "$ROOT/release/HOW_TO_PLAY_mac.txt" "$PACKAGE_DIR/"

if [[ ! -d "$ROOT/assets" ]]; then
  echo "Missing assets folder: $ROOT/assets" >&2
  exit 1
fi
cp -R "$ROOT/assets" "$PACKAGE_DIR/assets"

echo "==> Creating zip..."
mkdir -p "$DIST_DIR"
rm -f "$ZIP_PATH"
ditto -c -k --sequesterRsrc --keepParent "$PACKAGE_DIR" "$ZIP_PATH"

echo ""
echo "Done: $ZIP_PATH"
echo "Test locally: extract the zip, double-click Play.command"
