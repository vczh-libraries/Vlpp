# Linux Test Build

Run commands from this folder:

```bash
cd Vlpp/Test/Linux
```

Build:

```bash
../../.github/Ubuntu/build.sh
```

Rebuild:

```bash
../../.github/Ubuntu/build.sh -f
```

Run tests:

```bash
./Bin/UnitTest /C
```

## Debugging With LLDB

Start LLDB in a PTY-backed tool session:

```bash
lldb -- ./Bin/UnitTest /C
```

Keep the session id returned by the tool. Send debugger commands as newline-terminated stdin, one round at a time, and wait for output between rounds.

End the session with:

```text
quit
```

If the debugged process is still running or stuck, send Ctrl-C (`\u0003`), then send:

```text
process kill
quit
```

For non-interactive one-shot debugging, wrap LLDB with `timeout` so it cannot block forever.
