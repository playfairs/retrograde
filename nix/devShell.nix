{
  mkShell,
  lib,
  stdenv,
  clang,
  clang-tools,
  pkg-config,
  cmake,
  ninja,
  gdb,
  lldb,
}:

mkShell rec {
  meta.license = lib.licenses.unlicense;

  buildInputs = [
    clang
    clang-tools
    pkg-config
    cmake
    ninja
  ] ++ lib.optionals stdenv.isLinux [
    gdb
  ] ++ lib.optionals stdenv.isDarwin [
    lldb
  ];

  nativeBuildInputs = [
    pkg-config
  ];
}
