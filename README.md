# Browser History Manager
-# note: this readme was written using AI, specifically Claude AI, and it's just for explaining how to properly build it.

A browser history simulator built on a hand-written doubly linked list, with an undo stack for
deleted pages and JSON import/export. DSA mini-project, group 1.

Navigating backwards and forwards walks the `prev`/`next` pointers; visiting a new page from the
middle of the history discards everything ahead of it, exactly as a real browser does. The history
is capped at **50 pages** — once full, the oldest page is dropped.

## Requirements

| | |
|---|---|
| Compiler | Anything with C++20 support. Developed and tested with **g++ 13.2.0** (MSYS2 UCRT64); MSVC and Clang work too |
| CMake | **3.20** or newer |
| Dependencies | None to install — [nlohmann/json](https://github.com/nlohmann/json) 3.12.0 is vendored as a single header in `nlohmann/` |

## Project structure

```
DoublyLinkedList.h / .cpp    One history entry: url, title, prev, next.
                             Despite the name this is a NODE class, not the list itself.
BrowserHistory.h / .cpp      Owns head/current/tail, the undo stack, and JSON import/export.
main.cpp                     Interactive command-line menu.
tests/test_browser_history.cpp   Test suite (145 checks).
nlohmann/json.hpp            Vendored JSON library.
CMakeLists.txt               Two targets: dsa_miniproject and tests.
```

## Building

The project defines **two separate executables**, because each has its own `main()`:

| Target | What it is |
|---|---|
| `dsa_miniproject` | the interactive program |
| `tests` | the test suite |

### In CLion

Open the project folder — CLion picks up `CMakeLists.txt` automatically. Both targets appear in the
run-configuration dropdown in the toolbar. Pick one and press the hammer icon to build, or
Shift+F10 to build and run.

**Before running, change the working directory** (see [Where exported files go](#where-exported-files-go)):
Run → Edit Configurations… → select the configuration → set **Working directory** to
`$ProjectFileDir$`.

### From the command line with CMake

```bash
cmake -S . -B build
cmake --build build
```

Both executables land in `build/` (`build/Debug/` with MSVC). To build just one:

```bash
cmake --build build --target dsa_miniproject
cmake --build build --target tests
```

### Straight from the compiler, no CMake

Run these from the project root:

```bash
# the program
g++ -std=c++20 -Wall -Wextra main.cpp DoublyLinkedList.cpp BrowserHistory.cpp -o app

# the tests
g++ -std=c++20 -Wall -Wextra tests/test_browser_history.cpp DoublyLinkedList.cpp BrowserHistory.cpp -o tests
```

No `-I` flag is needed: `BrowserHistory.cpp` includes the JSON header as `"nlohmann/json.hpp"`,
which resolves relative to its own folder. Both commands compile warning-free.

## Running the program

```bash
./app              # or ./build/dsa_miniproject
```

It asks for a homepage URL and title (press Enter for `https://google.com` / `Google`), then shows
a menu:

| | | | |
|---|---|---|---|
| 1 | Visit new page | 6 | Delete a specific URL |
| 2 | Go back | 7 | Restore last deleted page (undo) |
| 3 | Go forward | 8 | Clear all history |
| 4 | View current page | 9 | Export history to JSON |
| 5 | Display full history | 10 | Import history from JSON |
| 0 | Exit | | |

In option 5's output, `[<]` marks the page you are currently on. Back and forward ask how many
steps to move and stop at the ends rather than failing. Page titles are optional.

## Running the tests

```bash
./tests            # or ./build/tests
```

It prints a `[ PASS ]` / `[ FAIL ]` line per check, grouped into sections, and finishes with a
tally. **The exit code is 0 when everything passes and 1 otherwise**, so it works in a script or
CI step:

```bash
./tests && echo "suite green"
```

Expected output ends with:

```
 145 of 145 checks passed
 ALL CHECKS PASSED
```

The suite covers the node class (including the getters that throw `std::out_of_range` at the ends of
the list), visiting, back/forward with zero, negative and absurdly large step counts, branching from
the middle of the history, the 50-page capacity limit, deleting from the head, middle, tail and
current page, the undo stack's last-in-first-out order, and JSON export/import — round trips, six
malformed-file cases, and ten unusual-but-valid files.

It writes temporary `test_tmp_*.json` fixtures into the working directory and deletes them all
before exiting, so it leaves nothing behind.

## Where exported files go

A filename typed at the export or import prompt is resolved **relative to the working directory**,
not the project folder. This is standard behaviour for any command-line program, but it surprises
people, because the working directory depends on how the program was started:

| Started from | Working directory |
|---|---|
| CLion, by default | `cmake-build-debug/` |
| Double-clicking the `.exe` | the folder holding the exe |
| A terminal | wherever that terminal is |

So a `history.json` exported from CLion lands in `cmake-build-debug/history.json`, not next to the
source files. Two ways to avoid hunting for it: set CLion's working directory to `$ProjectFileDir$`
as described above, or type an absolute path at the prompt, such as `C:/Users/you/history.json`.

## JSON file format

Export writes an object; import accepts anything matching this shape. Keys appear in alphabetical
order because `nlohmann::json` stores objects in a `std::map`.

```json
{
    "capacity": 50,
    "current_index": 1,
    "entries": [
        { "title": "Google", "url": "https://google.com" },
        { "title": "GitHub", "url": "https://github.com" }
    ],
    "size": 2,
    "version": 1
}
```

| Key | Meaning |
|---|---|
| `entries` | the pages in order, front of the history first. The order **is** the list structure — import rebuilds every `prev`/`next` pointer from it |
| `current_index` | which entry is the active page, or `-1` for none. Needed because page order alone cannot say where `current` sat |
| `size` | number of entries, counted while walking the list |
| `capacity` | the page limit the history was running with |
| `version` | format version, currently `1` |

Import is deliberately forgiving: `title` may be missing (it becomes empty), unknown keys are
ignored, an out-of-range `current_index` falls back to the newest page, and entries without a
usable `url` are skipped with a warning. It is strict about the things that matter — a file that
is not valid JSON, is not a JSON object, has no `entries` array, or declares a version other than
`1` is rejected.

Crucially, **a failed import leaves the existing history untouched.** The file is fully parsed and
validated into a temporary buffer before anything is modified, so a corrupt file cannot destroy
what is already loaded.

The undo stack is not part of the file. Importing clears it, since pending undos belong to the
session being replaced.

## Authors

salah0v, Leyla Aghayeva, Rasul Gurbanli
