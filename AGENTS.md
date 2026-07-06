# Repository Guidelines

## Project Structure & Module Organization

This repository is an MPLAB/XC16 embedded project for `dsPIC33CK256MP506` with a small browser-side client stub. Firmware entry code currently lives in `main.c`. Client-facing files belong under `client/`; `client/zp-logdatapanel.html` is the current placeholder. VS Code and MPLAB metadata live in `.vscode/`. Generated build trees and artifacts belong in `_build/`, `build/`, `dist/`, `nbproject/`, and `out/`; these are ignored and should not be committed.

The README references `/home/ss1/Projects/zp-test-probe-dspic/AGENTS.md` for related probe-project context. Read it when borrowing patterns from that project, but keep changes here scoped to this repository.

## Build, Test, and Development Commands

- `Ctrl+Shift+B` in VS Code: build the default MPLAB project using the configured XC16 toolchain.
- `ninja -C _build/zp-prd-a-probe-pic-headfull/default`: run the generated build directly after MPLAB/CMake files exist.
- `rm -rf _build out`: remove generated build output when a clean local rebuild is needed.

No package manager or web build pipeline is configured for `client/`; edit the HTML directly unless tooling is added later.

## Coding Style & Naming Conventions

Use C suitable for XC16 embedded builds: fixed-width integer types from `<stdint.h>`, `static` for file-local helpers, and simple control flow. Match the existing 4-space indentation in `main.c`. Use lower_snake_case for functions and variables, and uppercase for macros or constants. Keep hardware setup in focused C modules as the firmware grows instead of expanding `main.c` indefinitely.

For client files, prefer plain, readable HTML/CSS/JavaScript until a framework is introduced. Name new client assets descriptively, for example `client/log-data-panel.css`.

## Testing Guidelines

There is no standalone test suite yet. Treat a clean MPLAB/XC16 build as the minimum validation for firmware changes. For behavior changes, test on target hardware when possible and record the device, build configuration, and observed result. If tests are added later, place them in `test/` or `tests/` and document the runner here.

## Commit & Pull Request Guidelines

The current history only shows `init`, so there is no established local convention. Use short imperative subjects such as `add uart probe startup` or `update log data panel`. Pull requests should describe the firmware or client area touched, list build or hardware checks, link related issues, and include screenshots for visible UI changes.

## Configuration & Safety Notes

Do not commit generated MPLAB, CMake, or output artifacts unless intentionally source-controlled. Keep `.vscode/zp-prd-a-probe-pic-headfull.mplab.json` aligned with the selected device, DFP pack, and XC16 toolchain. Preserve `cmake/**/user.cmake` if it appears; `.gitignore` allows that local configuration file.
