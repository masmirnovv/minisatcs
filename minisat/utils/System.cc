/***************************************************************************************[System.cc]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include "minisat/utils/System.h"

#include <stdexcept>

#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>

using namespace MinisatCS;

// TODO: split the memory reading functions into two: one for reading high-watermark of RSS, and
// one for reading the current virtual memory size.

static inline int memReadStat(int field)
{
    char  name[256];
    pid_t pid = getpid();
    int   value;

    sprintf(name, "/proc/%d/statm", pid);
    FILE* in = fopen(name, "rb");
    if (in == NULL) return 0;

    for (; field >= 0; field--)
        if (fscanf(in, "%d", &value) != 1)
            printf("ERROR! Failed to parse memory statistics from \"/proc\".\n"), exit(1);
    fclose(in);
    return value;
}


static inline int memReadPeak(void)
{
    char  name[256];
    pid_t pid = getpid();

    sprintf(name, "/proc/%d/status", pid);
    FILE* in = fopen(name, "rb");
    if (in == NULL) return 0;

    // Find the correct line, beginning with "VmPeak:":
    int peak_kb = 0;
    while (!feof(in) && fscanf(in, "VmPeak: %d kB", &peak_kb) != 1)
        while (!feof(in) && fgetc(in) != '\n')
            ;
    fclose(in);

    return peak_kb;
}

double MinisatCS::memUsed() { return (double)memReadStat(0) * (double)getpagesize() / (1024*1024); }
double MinisatCS::memUsedPeak() {
    double peak = memReadPeak() / 1024;
    return peak == 0 ? memUsed() : peak; }

#elif defined(__FreeBSD__)

double MinisatCS::memUsed(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_maxrss / 1024; }
double MiniSat::memUsedPeak(void) { return memUsed(); }

#elif defined(__APPLE__)
#include <malloc/malloc.h>

double MinisatCS::memUsed(void) {
    malloc_statistics_t t;
    malloc_zone_statistics(NULL, &t);
    return (double)t.max_size_in_use / (1024*1024); }

#else
double MinisatCS::memUsed() {
    return 0; }
#endif

std::optional<std::string> MinisatCS::str_printf_va(const char* fmt,
                                                  va_list uap) {
    va_list ap;

    va_copy(ap, uap);
    char tmp;
    int size = vsnprintf(&tmp, 0, fmt, ap);
    va_end(ap);

    if (size < 0) {
        return std::nullopt;
    }

    size++; /* For '\0' */
    std::string ret;
    ret.resize(size);
    ret[size] = 0;

    va_copy(ap, uap);
    size = vsnprintf(ret.data(), size, fmt, ap);
    va_end(ap);

    if (size < 0) {
        return std::nullopt;
    }

    return ret;
}

void MinisatCS::uassert_failed(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    auto msg = str_printf_va(fmt, ap);
    va_end(ap);
    if (msg.has_value()) {
        throw std::runtime_error{std::move(msg.value())};
    }
    throw std::runtime_error{"failed to format error message"};
}

