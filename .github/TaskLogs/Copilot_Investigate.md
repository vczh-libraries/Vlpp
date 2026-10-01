# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Here are all unit test projects that supports web assembly:
- `Vlpp/Test/Linux`
- `VlppOS/Test/Linux/UnitTest`
- `VlppRegex/Test/Linux`
- `VlppReflection/Test/Linux/UnitTest`
- `VlppParser2/Test/Linux/ParserTest_ParserGen_Generated`
- `Workflow/Test/Linux/CppTest`
- `Workflow/Test/Linux/CpTest_Metaonly`
- `Workflow/Test/Linux/CppTest_Reflection`
- `Workflow/Test/Linux/LibraryTest`
- `Workflow/Test/Linux/RuntimeTest`
- `GacUI/Test/Linux/UnitTest`

Only the vlpp one is working properly, others all fails in firefox printing like:

```plaintext
WebAssembly Unit Tests

Failed

............................

ppppppppppppppppppppppppppppppppp

]]]]]]]]]]]]]]]]]]]]]]

]]]]]]]]]]]]]]]]]]]]]]]

]]]]]]]]]]]]]]]]]]]]]]]]

]]]]]]]]]]]]]]]]]]]]]]]]]

]]]]]]]]]]]]]]]]]]]]]]]]]]

11111111111111

0000000000000000000000000

000000000000000000000000000000000000

ppppppppppppppppppppppppppppppp

rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr

eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee

))))))))))))))))))))))))))))))))))))))
wasm_main returns 1.
```

The first thing to do is to fix the `Tools/Ubuntu/vl/wasm-unittest/app.sh`, currently the usage is `vbuild [port]`, it looks wired, I would like the usage to be:
- `app.sh port`: run the http server with the specified port
- `app.sh`: use 8888 as the port
You will need to search for documents and see if any markdown file describes the usage, fix that if any.
And then release ubuntu tools to all repo mentioned above plus `Release` repo.
commit and push all local changes before executing below.

Now you are going to verify all web assembly compatible unit test projects.
`cd` to each one and do `vbuild -fbw`, it would rebuild the unit test with web assembly, and use `app.sh` to start the server, and you are going to load the `app.html` and see why they all fails.
After fixing all of them, commit and push all local changes.

# TEST [CONFIRMED]

After committing and pushing the launcher checkpoint in Tools and all eight requested downstream repositories, rebuild each listed project with `vbuild -fbw` and open its generated `app.html` in Firefox through `Bin/app.sh`. The actual Workflow directory is `CppTest_Metaonly`.

The initial Vlpp build passes 473/473 cases in Firefox 146.0.1. The initial VlppOS build reproduces the exact repeated-character output and returns 1. All runs use the complete existing browser test selection. Success requires readable output, every retained case passing, exactly one `wasm_main returns 0.` line, cross-origin isolation, and no browser errors or runner diagnostics.

# PROPOSALS

- No.1 Propagate the existing upstream UTF-16 conversion fix [CONFIRMED]

## No.1 Propagate the existing upstream UTF-16 conversion fix

Vlpp commit `2be1b0f` already fixes incompatible integer aliases in `Source/Strings/Conversion.cpp`. The current Vlpp release contains that fix, but VlppOS, VlppRegex, VlppReflection, VlppParser2 and Workflow still import the older implementation. The imported converter writes `char16_t` buffers through `vuint16_t` references and reads/writes through incompatible aliases, allowing optimized Wasm builds of the amalgamation to miscompile conversions. This corrupts both console output and strings passed to browser APIs. GacUI's imports already match the current upstream releases and need separate verification after the requested full rebuild.

Regenerate the owning Vlpp release with CodePack and copy its root C++ artifacts into those downstream imports. Keep the already-correct upstream implementation and existing conversion regression tests. Rebuild and verify every requested project in Firefox; investigate any remaining failures independently.

### CODE CHANGE

The launcher checkpoint changes `app.sh` to accept zero or one port argument and locate `../vbuild` relative to itself. It updates the two Ubuntu command guides and shared Building.md, and distributes the canonical Ubuntu tools plus that guide to the requested repositories. Launcher checks cover default/custom ports, invalid arguments, an unrelated working directory, a path containing spaces, and HTTP isolation headers.

CodePack regenerates `Vlpp/Release`; copy the generated root `.cpp` and `.h` files, excluding IncludeOnly, into the five stale downstream imports. Only `Vlpp.cpp` differs in each of those repositories.

### VERIFICATION

Each project below was rebuilt with `vbuild -fbw` from its own project directory, served by its generated `Bin/app.sh`, and opened at `/app.html` in Firefox 146.0.1 on Linux. The existing browser test selections are unchanged. Completed runs have readable output, cross-origin isolation, no browser errors or failure diagnostics, and exactly one `wasm_main returns 0.` line.

| Project | Passed files | Passed cases |
| --- | ---: | ---: |
| Vlpp/Test/Linux | 32/32 | 473/473 |
| VlppOS/Test/Linux/UnitTest | 10/10 | 107/107 |
| VlppRegex/Test/Linux | 9/9 | 226/226 |
| VlppReflection/Test/Linux/UnitTest | 9/9 | 54/54 |
| VlppParser2/Test/Linux/ParserTest_ParserGen_Generated | 2/2 | 463/463 |
| Workflow/Test/Linux/CppTest | 2/2 | 232/232 |
| Workflow/Test/Linux/CppTest_Metaonly | 2/2 | 232/232 |
| Workflow/Test/Linux/CppTest_Reflection | 2/2 | 232/232 |
| Workflow/Test/Linux/LibraryTest | 3/3 | 21/21 |
| Workflow/Test/Linux/RuntimeTest | 4/4 | 264/264 |
| GacUI/Test/Linux/UnitTest | 91/91 | 1787/1787 |

Build, browser, server and result logs are retained in `/tmp/wasm-verification`. The initial failing VlppOS run is recorded as `VlppOS-before`. A short GacUI performance trial using a memory-backed disposable Firefox profile was stopped after it showed no large improvement; the complete verification uses the original disk-backed browser session.

The regenerated Vlpp release is unchanged from its existing committed version. All root Vlpp C++ artifacts in the six downstream test repositories match it byte for byte. The eleven distributed Ubuntu tool files and shared Building.md match their canonical Tools sources in all eight requested repositories.

### CONFIRMED

All eleven full WebAssembly builds and Firefox runs succeeded, totaling 4,091 passing cases. The initial VlppOS build reproduced the user's exact garbled output; replacing only the stale generated Vlpp implementation and rebuilding made all 107 cases pass with readable output. The other affected downstream suites also pass after the same import refresh. No additional C++ implementation changes or test exclusions were needed.

GacUI already contained the corrected imports. Its clean build completed all 1,787 cases successfully, so no further GacUI source fix was necessary. The full browser run took about 50 minutes and completed normally with return zero.

Code review confirms that each changed import is copied directly from the owning Vlpp release and contains only the existing UTF-16 aliasing fix. Generated-release consistency and whitespace checks pass. Verification was performed on Linux with Firefox 146.0.1.
