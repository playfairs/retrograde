{
  mkShell,
  lib,
  stdenv,
  clang,
  clang-tools,
  pkg-config,
  cmake,
  meson,
  ninja,
  gdb,
  qt6,
  lldb
}:
mkShell {
  meta.license = lib.licenses.unlicense;

  buildInputs = with qt6; [
    clang
    clang-tools
    meson
    cmake
    ninja
    qtbase
    qttools
  ]
  ++ lib.optionals stdenv.isLinux [
    gdb
  ]
  ++ lib.optionals stdenv.isDarwin [
    lldb
  ];

  nativeBuildInputs = with qt6; [
    wrapQtAppsHook
    pkg-config
  ];
}
