{
  lib,
  stdenv,
  clang,
  meson,
  ninja,
  qt6,
  pkg-config,
  src,
}:

stdenv.mkDerivation {
  pname = "retrograde";
  version = "0.1.0";
  inherit src;

  buildInputs = [ clang qt6.qtbase qt6.qttools ];
  nativeBuildInputs = [ qt6.wrapQtAppsHook pkg-config meson ninja ];
  dontUseMesonConfigure = true;

  buildPhase = ''
    clang++ -std=c++20 -Isrc src/main.cpp -o retrograde
    
    meson setup build-gui src/gui \
      --buildtype=release \
      --prefix="${qt6.qtbase}"
    meson compile -C build-gui
    cp build-gui/retrograde-gui .
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp retrograde $out/bin/
    cp retrograde-gui $out/bin/
  '';

  meta = with lib; {
    description = "A C++ binary decompiler";
    homepage = "https://github.com/playfairs/retrograde";
    license = licenses.unlicense;
    maintainers = [];
    platforms = platforms.all;
  };
}
