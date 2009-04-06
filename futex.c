/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#define _GNU_SOURCE

#include <unistd.h>

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>

#include "futex.h"

int futex_wait(void *addr, int val1)
{
  return syscall(SYS_futex, addr, FUTEX_WAIT, 
                 val1, NULL, NULL, 0);
}

int futex_wake(void *addr, int n)
{
  return syscall(SYS_futex, addr, FUTEX_WAKE, 
                 n, NULL, NULL, 0);
}
