#include "ft_nm.h"
#include <string.h>

static int	cmp_symbol(const void *a, const void *b)
{
	const t_symbol	*sa;
	const t_symbol	*sb;

	sa = (const t_symbol *)a;
	sb = (const t_symbol *)b;
	return (strcoll(sa->name, sb->name));
}

void	sort_symbols(t_elf_file *file)
{
	qsort(file->symbols, file->nb_symbols, sizeof(t_symbol), cmp_symbol);
}
