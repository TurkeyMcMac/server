#include "escape-path.h"

static int bits_ok(unsigned byte)
{
	return byte >> 3 < 0x1F;
}

static int read_nibble(char hex)
{
	if (hex >= '0' && hex <= '9') return hex - '0';
	hex |= 0x20;
	if (hex >= 'a' && hex <= 'f') return 10 + hex - 'a';
	return -1;
}

static int read_byte(const char hex[2])
{
	return (read_nibble(hex[0]) << 4) | read_nibble(hex[1]);
}

int escape_path(const char *orig, char *esc)
{
	int status = 0;
	int orig_i = 0, esc_i = 0;
	int ch, last = '\0';
	while ((ch = orig[orig_i])) {
		int do_bit_check = 1;
		if (ch == '%') {
			++orig_i;
			if (orig[orig_i] == '\0'
			 || (ch = read_byte(orig + orig_i)) < 0)
				return -400;
			do_bit_check = 0;
			switch (ch) {
			case '/': ch = ESCAPED_SLASH; break;
			case '\0': ch = ESCAPED_NUL; break;
			default: do_bit_check = 1; break;
			}
			++orig_i;
		}
		if (do_bit_check && !bits_ok(ch)) return -400;
		if (ch == '.' && last == '/') status = -404;
		esc[esc_i] = ch;
		++orig_i;
		++esc_i;
		last = ch;
	}
	if (status) return status;
	esc[esc_i] = '\0';
	return esc_i;
}
