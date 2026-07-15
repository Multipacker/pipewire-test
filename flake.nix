{
  description = "PipeWork: A Pipewire inspection and routing tool";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = 

  inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" "aarch64-linux" ];
      perSystem = { self', pkgs, ... }: 
        let
          pipework = { pkgs, useWayland ? true }: pkgs.stdenv.mkDerivation {
            name = "pipework";
            version = "latest";
            src = ./.;
            nativeBuildInputs = with pkgs; [
              clang
              pkg-config
              (if useWayland then wayland-scanner else null)
              (if useWayland then wayland-protocols else null)
            ];
            buildInputs = with pkgs; [
              pipewire
              libxkbcommon
              libGL
              (if useWayland then wayland else null)
              (if useWayland then null else libxcb)
              (if useWayland then null else libxcb-cursor)
            ];

            WAYLAND_PROTOCOLS_DIR = if useWayland then "${pkgs.wayland-protocols}/share/wayland-protocols/" else "";

            buildPhase = ''
              bash scripts/build_clang.sh release ${if useWayland then "wayland" else "x11"}

              mkdir -p $out/bin
              cp build/pipework $out/bin/pipework
            '';

            meta = with pkgs.lib; {
              description = "A Pipewire inspection and routing tool";
              homepage = "https://github.com/Multipacker/pipework";
              platforms = platforms.linux;
              license = licenses.mit;
              mainProgram = "pipework";
            };
          };
        in
        {
          packages.default = self'.packages.pipework;
          packages.pipework = self'.packages.pipework-wayland;
          packages.pipework-wayland = pkgs.callPackage pipework { useWayland = true; };
          packages.pipework-x11 = pkgs.callPackage pipework { useWayland = false; };
        };
    };
}
