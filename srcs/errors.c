#include "ft_nm.h"
#include <stdio.h>

void	print_error(const char *path, const char *msg)
{
	dprintf(2, "ft_nm: '%s': %s\n", path, msg);
}
