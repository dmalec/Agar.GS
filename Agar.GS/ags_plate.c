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
#include <types.h>

#include "gte.h"

#include "ags_plate.h"
#include "ags_game_types.h"
#include "ags_gs_shim.h"
#include "ags_rules.h"
#include "ags_utils.h"


/* Constants */

#define PLATE_FILENAME         "PLATE\0"


/* Macros */

#define AGAR_IS_ALIVE(row, col) (ags_current_plate[row][col] < 8 ? 0 : 1)


/* State */

static Byte ags_starting_plate[AGS_ROW_COUNT][AGS_COLUMN_COUNT];
static Byte ags_current_plate[AGS_ROW_COUNT][AGS_COLUMN_COUNT];
static Byte ags_temporary_plate[AGS_ROW_COUNT][AGS_COLUMN_COUNT];


/* Local Prototypes */

void Plate_LoadFromText(char *plate_text, LongWord plate_length);


/* Methods */

void Plate_Create(void) {
  Word row, col;

  for (row=0; row<AGS_ROW_COUNT; row++) {
    for (col=0; col<AGS_ROW_COUNT; col++) {
      ags_starting_plate[row][col] = 0x00;
      ags_current_plate[row][col] = 0x00;
      ags_temporary_plate[row][col] = 0x00;
    }
  }
}

void Plate_Load(void) {
  Handle plate_handle;
  LongWord file_length;

  GsShim_LoadFile(PLATE_FILENAME, &plate_handle, &file_length);
  if (plate_handle != NULL) {
    HLock(plate_handle);
    Plate_LoadFromText(*plate_handle, file_length);
    HUnlock(plate_handle);

    DisposeHandle(plate_handle);
  }
  
  Plate_Reset();
}

void Plate_LoadFromText(char *plate_text, LongWord plate_length) {
  LongWord i = 0;
  Word row = 0;
  Word col = 0;
  
  while (i < plate_length && row < AGS_ROW_COUNT) {
    // Skip line endings
    if (plate_text[i] == '\r' || plate_text[i] == '\n') {
      i++;
      continue;
    }

    ags_starting_plate[row][col] = Utils_HexCharToByte(plate_text[i]);

    col++;
    if (col >= AGS_COLUMN_COUNT) {
      col = 0;
      row++;
    }

    i++;
  }
}

void Plate_Reset(void) {
  Word row, col;

  for (row=0; row<AGS_ROW_COUNT; row++) {
    for (col=0; col<AGS_COLUMN_COUNT; col++) {
      ags_current_plate[row][col] = ags_starting_plate[row][col];
      GTESetTile(col, row, 256 + ags_current_plate[row][col]);
    }
  }
}

Word count_neighbors(Word row, Word col) {
  Word neighbor_count = 0;
  
  if (row > 0) {
    if (col > 0) {
      neighbor_count += AGAR_IS_ALIVE(row - 1, col - 1);
    }
    neighbor_count += AGAR_IS_ALIVE(row - 1, col);
    if (col < AGS_COLUMN_COUNT - 2) {
      neighbor_count += AGAR_IS_ALIVE(row - 1, col + 1);
    }
  }
  
  if (col > 0) {
    neighbor_count += AGAR_IS_ALIVE(row, col - 1);
  }
  if (col < AGS_COLUMN_COUNT - 2) {
    neighbor_count += AGAR_IS_ALIVE(row, col + 1);
  }
  
  if (row < AGS_ROW_COUNT - 2) {
    if (col > 0) {
      neighbor_count += AGAR_IS_ALIVE(row + 1, col - 1);
    }
    neighbor_count += AGAR_IS_ALIVE(row + 1, col);
    if (col < AGS_COLUMN_COUNT - 2) {
      neighbor_count += AGAR_IS_ALIVE(row + 1, col + 1);
    }
  }
  return neighbor_count;
}

void Plate_CalculateNextGeneration(void) {
  Word row, col, neighbor_count, agar_value;

  for (row=0; row<AGS_ROW_COUNT; row++) {
    for (col=0; col<AGS_COLUMN_COUNT; col++) {
      neighbor_count = count_neighbors(row, col);
      ags_temporary_plate[row][col] = Rules_GetNextGeneration(ags_current_plate[row][col], neighbor_count);
    }
  }

  for (row=0; row<AGS_ROW_COUNT; row++) {
    for (col=0; col<AGS_COLUMN_COUNT; col++) {
      agar_value = ags_temporary_plate[row][col];
      
      if (agar_value != ags_current_plate[row][col]) {
        ags_current_plate[row][col] = agar_value;
        GTESetTile(col, row, 256 + agar_value);
      }
    }
  }
}
