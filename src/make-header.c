#include "make-header.h"
#include <stdio.h>
#include <string.h>

#define SUB_FLOOR(var, amount) if (((var) -= (amount)) < 0) (var) = 0

int make_header(int status, char *buf, int buflen,
        const struct file_info *infop)
{
	struct file_info info;
	int header, entities;
	const char *reason;
	int ok = 0;
	switch (status) {
	case 200: reason = "OK"; ok = 1; break;
	case 400: reason = "Bad Request"; break;
	case 404: reason = "Not Found"; break;
	case 500: reason = "Internal Server Error"; break;
	case 501: reason = "Not Implemented"; break;
	default: reason = "???"; break;
	}
	header = snprintf(buf, buflen, "HTTP/1.1 %d %s\r\n", status, reason);
	SUB_FLOOR(buflen, header);
	if (ok) {
		info = *infop;
	} else {
		info.type = "text/plain";
		info.encoding = "identity";
		info.size = strlen(reason) + 6;
	}
	entities = snprintf(buf + header, buflen,
		"Content-Type: %s\r\n"
		"Content-Encoding: %s\r\n"
		"Content-Size: %d\r\n\r\n",
		info.type, info.encoding, info.size);
	if (ok) {
		return header + entities;
	} else {
		int total = header + entities;
		SUB_FLOOR(buflen, entities);
		total += snprintf(buf + total, buflen, "%s\r\n", reason);
		return total;
	}
}
