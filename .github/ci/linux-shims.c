/*
 * CI-only link shims (not part of the runtime).
 *
 * Two gaps found by this workflow on first link:
 *  - the interpreter embedded in the compiler driver carries a
 *    Windows-only dynamic-loading builtin (LoadLibraryW / GetProcAddress)
 *    with no POSIX branch yet; the probes never call that path;
 *  - the console-contract byte-output path emits `_write` (the Windows
 *    CRT spelling) from its "linux" branch; glibc only has `write`.
 *    Proper fix (compiler-side) tracked: emit `write` on linux targets.
 * The shims only satisfy the linker.
 */
#include <stddef.h>
#include <unistd.h>

long LoadLibraryW(const void *name) { (void)name; return 0; }
long GetProcAddress(long hmod, const char *name) { (void)hmod; (void)name; return 0; }
long _write(int fd, const void *buf, unsigned long count) { return (long)write(fd, buf, count); }

