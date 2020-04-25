#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"


long long get_file_size(const FILE* f) {
    fseek(f, 0, SEEK_END);
    long long res = ftell(f);
    fseek(f, 0, SEEK_SET);
    return res;
}

long long get_file_size_from_name(const char* name) {
    const FILE* f = fopen(name, "r");
    long long res = get_file_size(f);
    fclose(f);
    return res;
}

