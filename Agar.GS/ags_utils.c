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

#include "ags_utils.h"


/* Methods */

Byte Utils_HexCharToByte(char c) {
  Byte value;

  switch (c) {
    case '0': value =  0; break;
    case '1': value =  1; break;
    case '2': value =  2; break;
    case '3': value =  3; break;
    case '4': value =  4; break;
    case '5': value =  5; break;
    case '6': value =  6; break;
    case '7': value =  7; break;
    case '8': value =  8; break;
    case '9': value =  9; break;
    case 'A': value = 10; break;
    case 'B': value = 11; break;
    case 'C': value = 12; break;
    case 'D': value = 13; break;
    case 'E': value = 14; break;
    case 'F': value = 15; break;
    default:  value =  0; break;
  }

  return value;
}
