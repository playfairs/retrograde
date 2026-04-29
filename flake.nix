{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  inputs.treefmt-nix.url = "github:numtide/treefmt-nix";

  outputs = { self, nixpkgs, treefmt-nix, ... }@inputs:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = nixpkgs.legacyPackages.${system};
        inherit system;
        treefmt = inputs.treefmt-nix;
      });
    in
    {
      devShells = forAllSystems ({ pkgs, ... }: {
        default = import ./nix/devShell.nix {
          inherit (pkgs) mkShell lib stdenv clang clang-tools pkg-config cmake ninja gdb lldb;
        };
      });

      packages = forAllSystems ({ pkgs, ... }: {
        default = import ./nix/buildPackage.nix {
          inherit (pkgs) lib stdenv clang cmake ninja;
        };
      });

      formatter = forAllSystems ({ pkgs, treefmt, ... }:
        import ./nix/formatter.nix {
          inherit pkgs treefmt;
        }
      );
    };
}
