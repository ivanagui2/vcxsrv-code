/*
 * pthread_spin_trylock.c
 *
 * Description:
 * This translation unit implements spin lock primitives.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999-2017, Pthreads-win32 contributors
 *
 *      Homepage: https://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      https://sourceforge.net/p/pthreads4w/wiki/Contributors/
 *
 * This file is part of Pthreads-win32.
 *
 *    Pthreads-win32 is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Pthreads-win32 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Pthreads-win32.  If not, see <http://www.gnu.org/licenses/>. *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "pthread.h"
#include "implement.h"


int
pthread_spin_trylock (pthread_spinlock_t * lock)
{
  register pthread_spinlock_t s;

  if (NULL == lock || NULL == *lock)
    {
      return (EINVAL);
    }

  if (*lock == PTHREAD_SPINLOCK_INITIALIZER)
    {
      int result;

      if ((result = ptw32_spinlock_check_need_init (lock)) != 0)
	{
	  return (result);
	}
    }

  s = *lock;

  switch ((long)
	  PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG ((PTW32_INTERLOCKED_LONGPTR) &s->interlock,
					           (PTW32_INTERLOCKED_LONG) PTW32_SPIN_LOCKED,
					           (PTW32_INTERLOCKED_LONG) PTW32_SPIN_UNLOCKED))
    {
    case PTW32_SPIN_UNLOCKED:
      return 0;
    case PTW32_SPIN_LOCKED:
      return EBUSY;
    case PTW32_SPIN_USE_MUTEX:
      return pthread_mutex_trylock (&(s->u.mutex));
    }

  return EINVAL;
}
