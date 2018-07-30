/*
 * sched_setschedparam.c
 *
 * Description:
 * POSIX thread functions that deal with thread scheduling.
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
#include "sched.h"

int
pthread_setschedparam (pthread_t thread, int policy,
		       const struct sched_param *param)
{
  int result;

  /*
   * Validate the thread id. This method works for pthreads-win32 because
   * pthread_kill and pthread_t are designed to accommodate it, but the
   * method is not portable.
   */
  result = pthread_kill (thread, 0);
  if (0 != result)
    {
      return result;
    }

  /* Validate the scheduling policy. */
  if (policy < SCHED_MIN || policy > SCHED_MAX)
    {
      return EINVAL;
    }

  /* Ensure the policy is SCHED_OTHER. */
  if (policy != SCHED_OTHER)
    {
      return ENOTSUP;
    }

  return (ptw32_setthreadpriority (thread, policy, param->sched_priority));
}


int
ptw32_setthreadpriority (pthread_t thread, int policy, int priority)
{
  int prio;
  ptw32_mcs_local_node_t threadLock;
  int result = 0;
  ptw32_thread_t * tp = (ptw32_thread_t *) thread.p;

  prio = priority;

  /* Validate priority level. */
  if (prio < sched_get_priority_min (policy) ||
      prio > sched_get_priority_max (policy))
    {
      return EINVAL;
    }

#if (THREAD_PRIORITY_LOWEST > THREAD_PRIORITY_NORMAL)
/* WinCE */
#else
/* Everything else */

  if (THREAD_PRIORITY_IDLE < prio && THREAD_PRIORITY_LOWEST > prio)
    {
      prio = THREAD_PRIORITY_LOWEST;
    }
  else if (THREAD_PRIORITY_TIME_CRITICAL > prio
	   && THREAD_PRIORITY_HIGHEST < prio)
    {
      prio = THREAD_PRIORITY_HIGHEST;
    }

#endif

  ptw32_mcs_lock_acquire (&tp->threadLock, &threadLock);

  /* If this fails, the current priority is unchanged. */
  if (0 == SetThreadPriority (tp->threadH, prio))
    {
      result = EINVAL;
    }
  else
    {
      /*
       * Must record the thread's sched_priority as given,
       * not as finally adjusted.
       */
      tp->sched_priority = priority;
    }

  ptw32_mcs_lock_release (&threadLock);

  return result;
}
