// Optional implementation of a logger - you can and should replace this file.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <libpict.h>

int ptp_verbose = 1;
void ptp_verbose_log(char *fmt, ...) {
	if (ptp_verbose) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

void ptp_error_log(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

__attribute__ ((noreturn))
void ptp_panic(char *fmt, ...) {
	printf("PTP abort: ");
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
	putchar('\n');
	abort();
}
