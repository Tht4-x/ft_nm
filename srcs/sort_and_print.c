#include "ft_nm.h"
#include <string.h>

static void	print_one(const t_symbol *sym, int width)
{
	if (sym->has_value)
		dprintf(1, "%0*lx %c %s\n", width, sym->value, sym->type_char, sym->name);
	else
		dprintf(1, "%*s %c %s\n", width, "", sym->type_char, sym->name);
}

void	print_symbols(const t_elf_file *file)
{
	int		width;

	width = 16;
	if (!file->is_64)
		width = 8;

	for (size_t i = 0; i < file->nb_symbols; i++)
		print_one(&file->symbols[i], width);
}

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
