# Autumn.cpp — A C++ Implementation of Autumn that compiles to WASM (and more)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

C++ implementation of the [Autumn](https://doi.org/10.1145/3571249) DSL interpreter. Builds to a native binary, a Python extension module (`mara-autumn-cpp` wheel via pybind11), a WebAssembly bundle, and a Julia binding.

![Example Programs: Particles, Magnets, and Space Invader](assets/examples_small.gif)

## Using a pre-built release

Pre-built wheels and the WASM bundle are published per release at [BasisResearch/Autumn.cpp/releases](https://github.com/BasisResearch/Autumn.cpp/releases). To install the wheel:

```sh
pip install mara-autumn-cpp
# or, from a downloaded wheel:
pip install ./mara_autumn_cpp-*.whl
```

After install, the extension is importable as `MARA.autumn_cpp.interpreter_module` (the package layout matches the `MARA` monorepo's expectations, but the wheel does not depend on MARA itself).

## Building from source

### Prerequisites

- A C++20 compiler (clang or gcc)
- CMake ≥ 3.18
- Python ≥ 3.12 with development headers (use [`uv`](https://docs.astral.sh/uv/) to manage Python — see below)
- For the WASM target: [Emscripten](https://emscripten.org/) (a helper script is included)

### Quick start with `cmake`

This builds just the native CLI + the C++ unit test — no Python toolchain needed. To also build the pybind11 Python extension, drop `-DAUTUMN_BUILD_PYTHON_MODULE=OFF` (it defaults to `ON`).

```sh
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DAUTUMN_BUILD_PYTHON_MODULE=OFF

# Build
cmake --build build -j

# Run the test suite
ctest --test-dir build --output-on-failure
```

This produces:

| Artifact | Path |
|---|---|
| Native CLI interpreter | `build/interpreter` |
| Static C++ library | `build/libAutumnLib.a` |
| Token-type unit test | `build/TokenTypeTest` |
| Python extension module (when `AUTUMN_BUILD_PYTHON_MODULE=ON`) | `build/interpreter_module*.so` |

### Building a wheel with `uv`

```sh
uv build
# wheel lands in dist/
```

For multi-platform release wheels (manylinux x86_64/aarch64, macOS x86_64/arm64), the CI uses [`cibuildwheel`](https://cibuildwheel.pypa.io/) — see `.github/workflows/build.yml`.

### Building the WASM bundle

```sh
sh install_scripts/setup_emscripten.sh
source ./emsdk/emsdk_env.sh

# Use the emscripten cmake wrapper to configure
emcmake cmake -B build-wasm -S . -DCMAKE_BUILD_TYPE=Release

# Once configured, build the project like you normally would
cmake --build build-wasm -j
```

Outputs `build-wasm/interpreter_web.js` and `build-wasm/interpreter_web.wasm`.

### Building the Julia bindings

```sh
julia -e 'using Pkg; Pkg.add("CxxWrap")'
cmake -B build -S . -DCMAKE_PREFIX_PATH=$(julia -e 'using CxxWrap; print(joinpath(CxxWrap.prefix_path(), "lib/cmake"))')
cmake --build build -j
```

If the Julia toolchain is detected at configure time, an `AutumnInterpreter` package is staged into Julia's depot.

## Running Autumn programs

### From Python

After building (no wheel install required), a smoke-test runner:

```sh
PYTHONPATH=build .venv/bin/python python_test_mpl_ci.py tests/grow.sexp
```

For an interactive matplotlib viewer (accepts `step`, `click x y`, `left`/`right`/`up`/`down`, `q` on stdin):

```sh
.venv/bin/pip install matplotlib  # or: pip install -e '.[viz]'
PYTHONPATH=build .venv/bin/python python_test_mpl.py tests/grow.sexp
```

Additional matplotlib viewers (drag-and-drop / multiple-choice / animation modes) live in `extras/`.

### From the native CLI

```sh
./build/interpreter tests/grow.sexp
```

### From Julia

```julia
using Pkg
Pkg.develop(path=joinpath(first(DEPOT_PATH), "packages", "AutumnInterpreter"))
using AutumnInterpreter
```

See `test.jl` and `test2.jl` for example sessions.

## Repository layout

```
src/                 — interpreter sources
include/             — public headers (Expr, Stmt, Interpreter, Interner, …)
tools/               — entry points: native CLI, pybind11 bindings,
                       Julia bindings, WASM bindings, AST generator
autumnstdlib/        — Autumn standard library (stdlib.sexp)
tests/               — Autumn programs (.sexp) used as smoke / regression inputs
test_suites/         — C++ unit tests (registered with ctest)
extras/              — alternative matplotlib viewers
python_pkg/          — Python package layout used by `cmake --install`
.github/workflows/   — CI: wheel matrix + WASM build + tagged release
```

## Citation

```bibtex
@article{das2023autumn,
  author       = {Das, Ria and Tenenbaum, Joshua B. and Solar-Lezama, Armando and Tavares, Zenna},
  title        = {Combining Functional and Automata Synthesis to Discover Causal Reactive Programs},
  year         = {2023},
  publisher    = {Association for Computing Machinery},
  journal      = {Proc. ACM Program. Lang.},
  volume       = {7},
  number       = {POPL},
  articleno    = {56},
  numpages     = {31},
  doi          = {10.1145/3571249},
  keywords     = {synthesis, reactive, causal, automata}
}
```

## Acknowledgement

The s-expression parser is built on [Sexpresso](https://github.com/BitPuffin/sexpresso).

## License

MIT — see [LICENSE](LICENSE).

## Contributors

Dat Nguyen, Archana Warrier, Yichao Liang, Cambridge Yang, Michelangelo Naim, Yiyun Liu, Zenna Tavares.
