#include "make-header.h"
#include <stdio.h>

int make_header(int status, char *buf, int buflen)
{
	const char *reason;
	int ok = 0;
	switch (status) {
	case 200: reason = "OK"; ok = 1; break;
	case 400: reason = "Bad Request"; break;
	case 404: reason = "Not Found"; break;
	case 501: reason = "Not Implemented"; break;
	default: reason = "???"; ok = 1; break;
	}
	return snprintf(buf, buflen, "HTTP/1.1 %d %s\r\n\r\n%s",
		status, reason, ok ? "" : reason);
}
