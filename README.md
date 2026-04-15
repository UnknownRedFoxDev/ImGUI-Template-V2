# Dear ImGUI Template - Bare essential version

## Side note
This template uses SDL3 & GLFW. Morever, this is ImGUI v1.92.7. this may not be the latest version of it.
Also, this template is not visual studio or winslop friendly.

---

## How to build

```bash
make
```

---

## Template overview

This template is really simple. You have 3 main things:

1. Your source directory (src/)
2. Your thirdparties which includes ImGUI (thirdparty/)
2. Your makefile


### The thick of it

`src/main.cpp` - This is where we init all. It was extracted from
                 the ImGUI examples. But this version is strippen
                 out in order to give only the bare minimum
                 required to run your ImGUI application.

`thirdparty/`  - This directory only contains the essentials of
                 imgui. You may add your libraries into it.

`Makefile`     - It contains the base rules to compile your app.
                 It will create both build/ and build/bin
                 directories, and compile object and the binary
                 in their respective folders.
                 Moreover, it includes the thirdparty folder,
                 src folder and the root folder. It also links
                 with SDL3.
