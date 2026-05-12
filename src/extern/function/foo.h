#pragma once

// This header is shared between foo.c (C compiler) and main.cpp (C++ compiler).
// When the C++ side includes it, we wrap the declarations in extern "C" so the
// C++ compiler uses C-style (non-mangled) linkage and matches the symbol that
// the C compiler produced for foo().
#ifdef __cplusplus
extern "C" {
#endif

void foo(void);

#ifdef __cplusplus
}
#endif
