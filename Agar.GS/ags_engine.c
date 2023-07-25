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

#include "gte.h"

#include "ags_engine.h"
#include "ags_gs_shim.h"
#include "ags_plate.h"
#include "ags_rules.h"


/* Constants */

Word palette[16] = {
  0x0000,    // RGB (0-255):    0,    0,    0
  0x0777,    // RGB (0-255):  119,  119,  119
  0x0841,    // RGB (0-255):  136,   68,   17
  0x072C,    // RGB (0-255):  119,   34,  204
  0x000F,    // RGB (0-255):    0,    0,  255
  0x0080,    // RGB (0-255):    0,  136,    0
  0x0F70,    // RGB (0-255):  255,  119,    0
  0x0D00,    // RGB (0-255):  221,    0,    0
  0x0FA9,    // RGB (0-255):  255,  170,  153
  0x0FF0,    // RGB (0-255):  255,  255,    0
  0x00E0,    // RGB (0-255):    0,  238,    0
  0x04DF,    // RGB (0-255):   68,  221,  255
  0x0DAF,    // RGB (0-255):  221,  170,  255
  0x078F,    // RGB (0-255):  119,  136,  255
  0x0CCC,    // RGB (0-255):  204,  204,  204
  0x0FFF     // RGB (0-255):  255,  255,  255
};

#define TILES_FILENAME         "AgarGS.Data/agartile.dat\0"
#define MAP_FILENAME           "AgarGS.Data/agarmap.dat\0"


/* State */

static Handle tilemap_handle;
static Handle map_handle;


/* Methods */

void Engine_Create(void) {
  LongWord file_length;

  GsShim_LoadFile(TILES_FILENAME, &tilemap_handle, &file_length);
  HLock(tilemap_handle);
  GTELoadTileSet(0, 512, *tilemap_handle);
  GTESetPalette(0, (Pointer)palette);

  GsShim_LoadFile(MAP_FILENAME, &map_handle, &file_length);
  HLock(map_handle);
  GTESetBG0TileMapInfo(40, 25, *map_handle);
  GTESetBG0Origin(0, 0);

  GTERender(0);

  Plate_Create();
  Rules_Create();

  Plate_Load();
  Rules_Load();
}


void Engine_Run(void) {
  boolean done = false;
  boolean is_key_pressed = false;
  boolean is_key_released = false;
  boolean playing = false;
  boolean step = false;
  LongWord last_tick_sample = 0;
  LongWord next_generation_tick = 0;
  LongWord ticks = 0;
  Word control_mask;
  Word key_press;
  Word pressed_key;
  Word released_key;
  Word seconds = GTEGetSeconds();
  Word ticks_per_second;

  while (!done) {
    control_mask = GTEReadControl();
    key_press = control_mask & 0x007F;
  
    if (is_key_pressed) {
      if (pressed_key != key_press) {
        is_key_released = true;
        released_key = pressed_key;
      }
    }

    if (key_press) {
      is_key_pressed = true;
      pressed_key = key_press;
    } else {
      is_key_pressed = false;
    }

    if (is_key_released) {
      is_key_released = false;
      
      switch (released_key) {
        case 'P':
        case 'p':
          playing = !playing;
          
          if (!playing) {
            GTESetTile(4, 21, 'L');
            GTESetTile(5, 21, 'A');
            GTESetTile(6, 21, 'Y');
            GTESetTile(7, 21, ' ');
          } else {
            GTESetTile(4, 21, 'A');
            GTESetTile(5, 21, 'U');
            GTESetTile(6, 21, 'S');
            GTESetTile(7, 21, 'E');
          }
          break;

        case 'Q':
        case 'q':
          done = true;
          break;

        case 'R':
        case 'r':
          Plate_Reset();
          break;

        case 'S':
        case 's':
          step = true;
          break;
      }
    }

    if ((playing || step) && ticks >= next_generation_tick) {
      Plate_CalculateNextGeneration();
      next_generation_tick = ticks + ticks_per_second / 4;
      step = false;
    }

    GTERender(0);

    if (seconds != GTEGetSeconds()) {
      seconds = GTEGetSeconds();
      ticks_per_second = ticks - last_tick_sample;
      last_tick_sample = ticks;
    }
    
    ticks++;
  }
}

void Engine_Destroy(void) {
  if (tilemap_handle != NULL) {
    HUnlock(tilemap_handle);
    DisposeHandle(tilemap_handle);
  }

  if (map_handle != NULL) {
    HUnlock(map_handle);
    DisposeHandle(map_handle);
  }
}
