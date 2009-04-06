/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#ifndef __FUTEX_H
#define __FUTEX_H

/**
 * Wrapper around SYS_futex with FUTEX_WAIT.
 *
 * @retval -1 Futex hasn't blocked.
 * @retval 0  Futex has blocked.
 */
int futex_wait(void *addr, int val1);

/**
 * Wrapper around SYS_futex with FUTEX_WAKE.
 *
 * @return The number of threads which were awakened.
 */
int futex_wake(void *addr, int n);

#endif /* __FUTEX_H */