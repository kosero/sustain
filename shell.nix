{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    xorg.libX11
    xorg.libXcursor
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXi
    xorg.libXext
    xorg.xorgproto

    glfw
    libGL
    mesa

    pkg-config
    gcc
    cmake
    clang
    libclang
    llvmPackages.libclang
  ];

  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (with pkgs; [
    xorg.libX11
    xorg.libXcursor
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXi
    xorg.libXext
    glfw
    libGL
    mesa
    libclang
  ]);

  CMAKE_PREFIX_PATH = "${pkgs.xorg.libX11.dev}";

  PKG_CONFIG_PATH =
    "${pkgs.xorg.libX11.dev}/lib/pkgconfig:${pkgs.xorg.libXcursor.dev}/lib/pkgconfig:${pkgs.xorg.libXrandr.dev}/lib/pkgconfig:${pkgs.xorg.libXinerama.dev}/lib/pkgconfig:${pkgs.xorg.libXi.dev}/lib/pkgconfig";

  shellHook = ''
    export X11_X11_INCLUDE_PATH="${pkgs.xorg.libX11.dev}/include"
    export X11_X11_LIB="${pkgs.xorg.libX11}/lib/libX11.so"
    export LIBCLANG_PATH="${pkgs.llvmPackages.libclang.lib}/lib"
  '';
}
