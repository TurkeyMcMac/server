#ifndef ESCAPE_PATH_INCLUDED_
#define ESCAPE_PATH_INCLUDED_

#define ESCAPED_SLASH	0xFF
#define ESCAPED_NUL	0xFE

int escape_path(const char *orig, char *esc);

#endif /* ESCAPE_PATH_INCLUDED_ */
