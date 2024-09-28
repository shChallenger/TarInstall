#ifndef TARINSTALL_H

# define TARINSTALL_H

# include <stddef.h>

# define MAX_APPNAME_LEN 512
# define MAX_PATH_LEN 2048

typedef struct s_desktopapp
{
	char name[MAX_APPNAME_LEN];
	size_t name_size;
	char bin[MAX_PATH_LEN];
	size_t bin_size;
	char icon[MAX_PATH_LEN];
	size_t icon_size;
} DesktopApp;

#endif
