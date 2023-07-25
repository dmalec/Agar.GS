#include <loader.h>
#include <locator.h>
#include <memory.h>
#include <misctool.h>
#include <types.h>

#include "gte.h"

#include "ags_engine.h"

#define TOOLFAIL(string) \
  if (toolerror()) SysFailMgr(toolerror(), "\p" string "\n\r    Error Code -> $");


int main (void) {
  Word gte_cap_flags = 0x0000;
  Word user_id;
  Handle dp_handle;
  Word dp_word;

  TLStartUp();
  TOOLFAIL("Unable to start tool locator");

  user_id = MMStartUp();
  TOOLFAIL("Unable to start memory manager");

  MTStartUp();
  TOOLFAIL("Unable to start misc tools");

  LoadGTEToolSet(user_id);

  dp_handle = NewHandle(0x0200, user_id, 0x4015, 0);
  if (dp_handle == NULL) {
    TOOLFAIL("Unable to allocate bank 0 memory");
  }
  dp_word = (Word)(*dp_handle);
  if ((dp_word & 0x00FF) != 0x0000) {
    TOOLFAIL("Allocated bank 0 memory is not aligned");
  }

#ifndef GTE_IS_SYSTEM_TOOLS_INSTALL
  gte_cap_flags |= ENGINE_MODE_USER_TOOL;
#endif // ! GTE_IS_SYSTEM_TOOLS_INSTALL

  GTEStartUp(dp_word, gte_cap_flags, user_id);
  TOOLFAIL("Unable to start GTE");

  GTESetScreenMode(320, 200);

  Engine_Create();

  Engine_Run();
  
  GTEShutDown();
  
  Engine_Destroy();

  DisposeHandle(dp_handle);

  MTShutDown();
  TOOLFAIL("Unable to shutdown misc tool");

  MMShutDown(user_id);
  TOOLFAIL("Unable to shutdown memory manager");

  TLShutDown();
  TOOLFAIL("Unable to shutdown tool locator");
}
