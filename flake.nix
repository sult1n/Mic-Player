{
  inputs.nixpkgs.url = "nixpkgs";

  outputs = { nixpkgs, ... }:
  let pkgs = import nixpkgs { system = "x86_64-linux"; };
  in {
    devShells.x86_64-linux.default = pkgs.mkShell {
      nativeBuildInputs = with pkgs; [
        meson ninja pkg-config cmake
      ];
      buildInputs = with pkgs; [
        notcurses.dev
      ];
    };
  };
}
