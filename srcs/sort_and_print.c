#include "ft_nm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

//============================================================================

void	reverse_symbols(t_elf_file *file)
{
	size_t		i;
	size_t		j;
	t_symbol	tmp;

	if (file->nb_symbols == 0)
		return ;
	i = 0;
	j = file->nb_symbols - 1;
	while (i < j)
	{
		tmp = file->symbols[i];
		file->symbols[i] = file->symbols[j];
		file->symbols[j] = tmp;
		i++;
		j--;
	}
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

//============================================================================

static bool	should_keep(const t_symbol *sym, const t_options *opts)
{
	if (!opts->all && (sym->type == STT_FILE || sym->type == STT_SECTION))
		return (false);
	if (opts->undefined_only && sym->shndx != SHN_UNDEF)
		return (false);
	if (opts->extern_only && sym->bind == STB_LOCAL)
		return (false);
	return (true);
}

void	filter_symbols(t_elf_file *file, const t_options *opts)
{
	size_t	i;
	size_t	kept;

	kept = 0;
	i = 0;
	while (i < file->nb_symbols)
	{
		if (should_keep(&file->symbols[i], opts))
		{
			file->symbols[kept] = file->symbols[i];
			kept++;
		}
		i++;
	}
	file->nb_symbols = kept;
}