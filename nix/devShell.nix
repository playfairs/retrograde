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

  buildInputs = [
    clang
    clang-tools
    meson
    cmake
    ninja
    qt6.full
  ]
  ++ lib.optionals stdenv.isLinux [
    gdb
  ]
  ++ lib.optionals stdenv.isDarwin [
    lldb
  ];

  nativeBuildInputs = [
    pkg-config
  ];
}
