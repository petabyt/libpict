// Basic deadlock checks for the libpict runtime mutex.
//
// The runtime mutex is recursive (PTHREAD_MUTEX_RECURSIVE, see ptp_init()),
// so nested locking by the same thread is fine. Flags two bug classes:
//   1. missing_unlock - a return statement executed while the runtime
//                       mutex is still held (lock is leaked)
//   2. lock_at_exit   - the function can reach its end (fall off the
//                       end, goto, pthread_exit, break out of a loop...)
//                       with the lock still held, without any return
//                       statement on that path
//
// Usage:
//   spatch --sp-file mutex.cocci --no-includes src/*.c test/*.c
//
// Note: these are single-function, best-effort checks. Locking patterns
// that cross function boundaries are not detected.

// 1. Return while the lock is still held
@missing_unlock exists@
identifier f;
expression R;
@@

f(...) {
...
	ptp_mutex_lock(R);
	... when != ptp_mutex_unlock(R)
	    when != ptp_mutex_unlock_thread(R)
	    when any
*return ...;
...
}

// 2. Reaching the end of the function with the lock still held
@lock_at_exit exists@
identifier f;
expression R;
@@

f(...) {
...
*	ptp_mutex_lock(R);
	... when != ptp_mutex_unlock(R)
	    when != ptp_mutex_unlock_thread(R)
	    when != return ...;
	    when any
}
