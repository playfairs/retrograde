{
  lib,
  stdenv,
  clang,
  cmake,
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
  nativeBuildInputs = [ qt6.wrapQtAppsHook pkg-config cmake ];
  dontUseCmakeConfigure = true;

  buildPhase = ''
    clang++ -std=c++20 -Isrc src/main.cpp -o retrograde
    
    cmake -B build-gui -S src/gui \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="${qt6.qtbase}"
    cmake --build build-gui
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
