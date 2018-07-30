/*
 * pthread_spin_init.c
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
pthread_spin_init (pthread_spinlock_t * lock, int pshared)
{
  pthread_spinlock_t s;
  int cpus = 0;
  int result = 0;

  if (lock == NULL)
    {
      return EINVAL;
    }

  if (0 != ptw32_getprocessors (&cpus))
    {
      cpus = 1;
    }

  if (cpus > 1)
    {
      if (pshared == PTHREAD_PROCESS_SHARED)
	{
	  /*
	   * Creating spinlock that can be shared between
	   * processes.
	   */
#if _POSIX_THREAD_PROCESS_SHARED >= 0

	  /*
	   * Not implemented yet.
	   */

#error ERROR [__FILE__, line __LINE__]: Process shared spin locks are not supported yet.

#else

	  return ENOSYS;

#endif /* _POSIX_THREAD_PROCESS_SHARED */

	}
    }

  s = (pthread_spinlock_t) calloc (1, sizeof (*s));

  if (s == NULL)
    {
      return ENOMEM;
    }

  if (cpus > 1)
    {
      s->u.cpus = cpus;
      s->interlock = PTW32_SPIN_UNLOCKED;
    }
  else
    {
      pthread_mutexattr_t ma;
      result = pthread_mutexattr_init (&ma);

      if (0 == result)
	{
	  ma->pshared = pshared;
	  result = pthread_mutex_init (&(s->u.mutex), &ma);
	  if (0 == result)
	    {
	      s->interlock = PTW32_SPIN_USE_MUTEX;
	    }
	}
      (void) pthread_mutexattr_destroy (&ma);
    }

  if (0 == result)
    {
      *lock = s;
    }
  else
    {
      (void) free (s);
      *lock = NULL;
    }

  return (result);
}
