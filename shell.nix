{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    gnumake
    clang
    gdb

    clang-tools
    pkg-config
  ];

  buildInputs = with pkgs; [
    libx11
    libxrandr
    libxinerama
    libxcursor
    libxi
    libGL
  ];

  shellHook = ''
    export C_INCLUDE_PATH="$C_INCLUDE_PATH:${pkgs.glibc.dev}/include"
  '';
}
