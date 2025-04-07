{
  description = "fluky development environment";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:nixos/nixpkgs/master";
    foolnotion.url = "github:foolnotion/nur-pkg";
    foolnotion.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = inputs @{ self, flake-parts, foolnotion, nixpkgs }:
    flake-parts.lib.mkFlake { inherit inputs; } rec {
      systems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];

      perSystem = { pkgs, system, ... }:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ foolnotion.overlay ];
          };
          stdenv_ = pkgs.llvmPackages_latest.stdenv;

          fluky = stdenv_.mkDerivation {
            name = "fluky";
            src = self;

            cmakeFlags = [
              "-DBUILD_SHARED_LIBS=${if pkgs.stdenv.hostPlatform.isStatic then "OFF" else "ON"}"
              "-DBUILD_TESTING=OFF"
              "-DCMAKE_BUILD_TYPE=Release"
            ];

            nativeBuildInputs = with pkgs; [ cmake git ];
          };

          enableTesting = true;
        in
        rec {
          packages = {
            default = fluky.overrideAttrs (old: {
              cmakeFlags = old.cmakeFlags ++ [
                "--preset ${if pkgs.stdenv.isLinux then "release-linux" else if pkgs.stdenv.isDarwin then "release-darwin" else ""}"
              ];
            });

            compat = fluky.overrideAttrs (old: {
              cmakeFlags = old.cmakeFlags;
            });
          };

          devShells.default = stdenv_.mkDerivation {
            name = "fluky";

            nativeBuildInputs = fluky.nativeBuildInputs ++ (with pkgs; [ clang-tools ]);

            buildInputs = fluky.buildInputs
              ++ (with pkgs; pkgs.lib.optionals pkgs.stdenv.isLinux [ gcc14 gdb hotspot linuxPackages_latest.perf valgrind ])
              ++ (with pkgs; pkgs.lib.optionals enableTesting [ catch2_3 hyperfine practrand-rng-test nanobench ]);
          };
        };
    };
}
