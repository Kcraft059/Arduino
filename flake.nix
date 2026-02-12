{
  description = "Dev-Shell for aduino-developpement";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    {
      nixpkgs,
      ...
    }@inputs:
    let
      system = "aarch64-darwin";

      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
        overlays = [ ];
      };
    in
    {
      devShells."${system}".default = pkgs.mkShell {
        packages = with pkgs; [
          arduino-cli
        ];

        shellHook = ''
          export ARDUINO_DIRECTORIES_DATA=$PWD/arduino-data # Arduino-packages-path -> ~/Library/Arduino*

          cat > .zsh-shell <<'EOF'
          TRAPEXIT() {
            rm -rf .zsh-shell
          }
          EOF

          exec ${pkgs.zsh}/bin/zsh --rcs -i -c "source .zsh-shell; ${pkgs.zsh}/bin/zsh -i"
        '';
      };
    };
}
