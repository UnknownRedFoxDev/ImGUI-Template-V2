# Dear ImGUI Template - Bare essential version

## Side note
This template uses SDL3 & GLFW. Morever, this is ImGUI v1.92.7. this may not be the latest version of it.
Also, this template is not visual studio or winslop friendly.

---

## How to build

```bash
cc -o nob nob.c
./nob
```

## How to run

```bash
./nob --run
```

---

## Overview

### The different commands

Flags are unordered and can be added onto of each others.

```bash
./nob --clean
```
Cleans the build directory. Effectively removes the cache

```bash
./nob --run
```
Compiles if needed and runs the program

```bash
./nob --debug
```
Compiles the program with debug info


### Template overview

This template is really simple. You have 3 main things:

1. Your source directory (src/)
2. Your thirdparties which includes ImGUI (thirdparty/)
2. Your makefile


#### The thick of it

`src/main.cpp` - This is where we init all. It was extracted from
                 the ImGUI examples. But this version is strippen
                 out in order to give only the bare minimum
                 required to run your ImGUI application.

`thirdparty/`  - This directory only contains the essentials of
                 imgui. You may add your libraries into it.

`nob.c`        - A bit more complex than a Makefile, but way more
                 explicit on what it does.
