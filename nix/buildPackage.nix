{
  lib,
  stdenv,
  clang,
  cmake,
  ninja,
}:

stdenv.mkDerivation {
  pname = "retrograde";
  version = "0.1.0";
  src = ../.;

  buildInputs = [ clang ];

  buildPhase = ''
    clang++ -std=c++20 -I./src src/main.cpp -o retrograde
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp retrograde $out/bin/
  '';

  meta = with lib; {
    description = "A C++ binary decompiler";
    homepage = "https://github.com/playfairs/retrograde";
    license = licenses.unlicense;
    maintainers = [];
    platforms = platforms.all;
  };
}
