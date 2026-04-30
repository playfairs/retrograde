{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/25.11";
    treefmt-nix.url = "github:numtide/treefmt-nix";
  };
  outputs =
    { nixpkgs, ... }@inputs:
    let
      forAllSystems =
        f:
        nixpkgs.lib.genAttrs nixpkgs.lib.systems.flakeExposed (
          system:
          f {
            pkgs = import nixpkgs { inherit system; };
            inherit system;
            inherit (inputs)treefmt-nix;
          }
        );
    in
    {
      devShells = forAllSystems (
        { pkgs, ... }:
        rec {
          retrograde = pkgs.callPackage ./nix/devShell.nix { };
          default = retrograde;
        }
      );

      packages = forAllSystems (
        { pkgs, ... }:
        rec {
          retrograde = pkgs.callPackage ./nix/buildPackage.nix { };
          default = retrograde;
        }
      );

      formatter = forAllSystems (
        { pkgs, treefmt-nix, ... }: pkgs.callPackage ./nix/formatter.nix { inherit treefmt-nix; }
      );
    };
}
