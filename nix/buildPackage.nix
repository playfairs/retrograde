{
  lib,
  stdenv,
  clang,
  qt6,
  pkg-config,
  meson,
  ninja,
}:
stdenv.mkDerivation {
  pname = "retrograde";
  version = "0.1.0";
  src = ../.;

  buildInputs = with qt6; [
    qttools
    qtbase
  ];

  nativeBuildInputs = with qt6; [
    wrapQtAppsHook
    pkg-config
    meson
    ninja
    clang
  ];

  meta = with lib; {
    description = "A C++ binary decompiler";
    homepage = "https://github.com/playfairs/retrograde";
    license = licenses.unlicense;
    maintainers = [ ];
    platforms = platforms.all;
  };
}
