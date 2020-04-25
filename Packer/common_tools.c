#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"


long get_file_size(const FILE* f) {
    fseek(f, 0, SEEK_END);
    long res = ftell(f);
    fseek(f, 0, SEEK_SET);
    return res;
}
