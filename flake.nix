{
  description = "Dev-Shell for aduino-developpement";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    {
      nixpkgs,
      ...
    }:
    let
      system = "aarch64-darwin";

      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
        overlays = [ ];
      };
    in
    {
      devShells."${system}" = rec {
        avr_utils = pkgs.mkShell {
          buildInputs = with pkgs; [
            pkgsCross.avr.buildPackages.gcc
            pkgsCross.avr.avrlibc
            avrdude
            jq
          ];

          packages = with pkgs; [
            vimPlugins.mini-align
            asm-lsp
          ];

          shellHook = ''
            export AVR_LIBC_INCLUDE=${pkgs.pkgsCross.avr.avrlibc}/avr/include

            cat > .nvim.lua <<-'EOF'
            vim.opt.runtimepath:prepend('${pkgs.vimPlugins.mini-align}')
            require('mini.align').setup()

            vim.lsp.config('asm_lsp', {
              cmd = { 'asm-lsp' },
              filetypes = { 'asm' },
              root_dir = function(bufnr, on_dir)
                local fname = vim.api.nvim_buf_get_name(bufnr)
                local found = vim.fs.find('.asm-lsp.toml', { path = fname, upward = true })[1]
                local dir = vim.fs.dirname(found) or vim.fs.dirname(fname)
                --print("asm-lsp root_dir: " .. tostring(dir))
                --print("fname: " .. tostring(fname))
                --print("found toml: " .. tostring(found))
                on_dir(dir)
              end,
            })
            vim.lsp.enable('asm_lsp')
            EOF

            cat > .zsh-shell <<-'EOF'
            TRAPEXIT() {
            rm -rf .zsh-shell
            	rm -rf .nvim.lua
            }
            EOF

            exec ${pkgs.zsh}/bin/zsh --rcs -i -c "source .zsh-shell; ${pkgs.zsh}/bin/zsh -i"
          '';
        };
        arduino = pkgs.mkShell {
          packages = with pkgs; [
            arduino-cli
          ];

          shellHook = ''
            export ARDUINO_DIRECTORIES_DATA=$PWD/.arduino-data # Arduino-packages-path -> ~/Library/Arduino*
            exec ${pkgs.zsh}/bin/zsh --rcs -i -c "${pkgs.zsh}/bin/zsh -i"
          '';
        };
        default = arduino;
      };
    };
}
