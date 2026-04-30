{
  lib,
  stdenv,
  clang,
  meson,
  ninja,
  qt6,
  pkg-config
}:
stdenv.mkDerivation {
  pname = "retrograde";
  version = "0.1.0";
  src = ../.;

  buildInputs = with qt6; [
    clang
    qtbase
    qttools
  ];
  nativeBuildInputs = with qt6; [
    wrapQtAppsHook
    pkg-config
    meson
    ninja
  ];

  dontUseMesonConfigure = true;

  meta = with lib; {
    description = "A C++ binary decompiler";
    homepage = "https://github.com/playfairs/retrograde";
    license = licenses.unlicense;
    maintainers = [ ];
    platforms = platforms.all;
  };
}
