/*
 * CI-only link shims (not part of the runtime).
 *
 * The interpreter embedded in the compiler driver carries a Windows-only
 * dynamic-loading builtin (LoadLibraryW / GetProcAddress) that has no
 * POSIX branch yet, so those two symbols appear in the Linux-target IR.
 * The probes never call the dynload path; these stubs only satisfy the
 * linker.  Compiler-side linux guard for the dynload builtin is tracked
 * as a pending tiec item.
 */
#include <stddef.h>

long LoadLibraryW(const void *name) { (void)name; return 0; }
long GetProcAddress(long hmod, const char *name) { (void)hmod; (void)name; return 0; }
