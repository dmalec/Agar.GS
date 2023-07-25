/*
 * MIT License
 *
 * Copyright (c) 2023 Dan Malec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <memory.h>

#include "ags_game_types.h"
#include "ags_gs_shim.h"
#include "ags_rules.h"
#include "ags_utils.h"


/* Constants */

#define RULES_FILENAME         "RULES\0"


/* State */

static Byte ags_rules[AGS_RULE_COUNT][AGS_NEIGHBOR_COUNT];


/* Local Prototypes */

void Rules_LoadFromText(char *rules_text, LongWord rules_length);


/* Methods */

void Rules_Create(void) {
  Word rule, neighbor;

  for (rule=0; rule<AGS_RULE_COUNT; rule++) {
    for (neighbor=0; neighbor<AGS_NEIGHBOR_COUNT; neighbor++) {
      ags_rules[rule][neighbor] = 0x00;
    }
  }
}

void Rules_Load(void) {
  Handle rules_handle;
  LongWord file_length;

  GsShim_LoadFile(RULES_FILENAME, &rules_handle, &file_length);
  if (rules_handle != NULL) {
    HLock(rules_handle);
    Rules_LoadFromText(*rules_handle, file_length);
    HUnlock(rules_handle);

    DisposeHandle(rules_handle);
  }
}

void Rules_LoadFromText(char *rules_text, LongWord rules_length) {
  LongWord i = 0;
  Byte value_index = 0;
  Byte neighbor_index = 0;
  
  while (i < rules_length && value_index < 16) {
    // Skip line endings
    if (rules_text[i] == '\r' || rules_text[i] == '\n') {
      i++;
      continue;
    }

    ags_rules[value_index][neighbor_index] = Utils_HexCharToByte(rules_text[i]);

    neighbor_index++;
    if (neighbor_index > 7) {
      neighbor_index = 0;
      value_index++;
    }

    i++;
  }
}

Byte Rules_GetNextGeneration(Byte currentGenerationValue, Byte neighborCount) {
  return ags_rules[currentGenerationValue][neighborCount];
}
