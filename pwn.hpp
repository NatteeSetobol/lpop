#ifndef __PWN__H
#define __PWN__H
#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"
#include "array.hpp"
#include "strings.hpp"

void HexPrint(char* data, i32 dataSize);
char *Dup(char ch, int times);
i64 *p64(i64 addr);

#endif
