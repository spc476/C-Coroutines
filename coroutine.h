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

#ifndef I_5A22037D_5F90_5456_A562_661022F242BF
#define I_5A22037D_5F90_5456_A562_661022F242BF

#include <stddef.h>
#include <stdint.h>

#if defined(__i386)
#  define coroutine_init	coroutine_init32
#  define coroutine_yield	coroutine_yield32
#elif defined(__x86)
#  define coroutine_init	coroutine_init32
#  define coroutine_yield	coroutine_yield32
#elif defined(__x86_64)
#  define coroutine_init	coroutine_init64
#  define coroutine_yield	coroutine_yield64
#endif

typedef struct
{
  void   *csp;
  void   *base;
  size_t  size;
  void   *user;
} coroutine__s;


extern int          coroutine_create(coroutine__s **,size_t,uintptr_t (*fun)(coroutine__s *,uintptr_t));
extern uintptr_t    coroutine_yield (coroutine__s *,uintptr_t);
extern int          coroutine_free  (coroutine__s *);

#endif
