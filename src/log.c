// Optional implementation of a logger - you can and should replace this file.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <libpict.h>

int ptp_verbose = 1;
__attribute__((weak))
void ptp_verbose_log(struct PtpRuntime *r, const char *fmt, ...) {
	if (ptp_verbose) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

__attribute__((weak))
void ptp_error_log(struct PtpRuntime *r, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

__attribute__ ((noreturn))
void ptp_panic(const char *fmt, ...) {
	printf("PTP abort: ");
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
	putchar('\n');
	abort();
}
