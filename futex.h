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
 * @param addr The address of the futex.
 * @param val1 The value to compare the futex to.
 *
 * @retval -1 Futex hasn't blocked.
 * @retval 0  Futex has blocked.
 */
int futex_wait(void *addr, int val1);

/**
 * Wrapper around SYS_futex with FUTEX_WAKE.
 *
 * @param addr The address of the futex.
 * @param n    The number of threads to awake (or INT_MAX to wake all)
 *
 * @return The number of threads which were awakened.
 */
int futex_wake(void *addr, int n);

#endif /* __FUTEX_H */
