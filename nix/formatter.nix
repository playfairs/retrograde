{
  pkgs,
  treefmt
}:

(treefmt.lib.evalModule pkgs (_: {
  projectRootFile = ".git/config";
  programs = {
    nixfmt.enable = true;
    nixf-diagnose.enable = true;
    taplo.enable = true;
    clang-format.enable = true;
  };
  settings.formatter.clang-format = {
    options = [
      "--style=file:.clang-format"
    ];
  };
})).config.build.wrapper
