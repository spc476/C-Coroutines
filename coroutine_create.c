/***************************************************************************
*
* Copyright 2017 by Sean Conner.
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, see <http://www.gnu.org/licenses/>.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include "coroutine.h"

extern void coroutine_init(
                            coroutine__s *,
                            uintptr_t (*)(coroutine__s *,uintptr_t),
                            void         *
                          );

int coroutine_create(
        coroutine__s  **pco,
        size_t          stsize,
        uintptr_t     (*fun)(coroutine__s *,uintptr_t)
)
{
  char         *blob;
  coroutine__s *co;
  
  assert(pco != NULL);
  assert(fun);
  
  if (stsize == 0)
    stsize = 8192;
  
  if ((blob = malloc(stsize)) == NULL)
    return -1;
  
  co       = (coroutine__s *)&blob[stsize - sizeof(coroutine__s)];
  co->base = blob;
  co->size = stsize;
  
  coroutine_init(co,fun,co);
  *pco = co;
  return 0;
}

