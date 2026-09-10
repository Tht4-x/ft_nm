#include "ft_nm.h"
#include <errno.h>
#include <string.h>
#include <locale.h>

static int	handle_file(const char *path)
{
	t_elf_file	file;

	if (!open_and_map(path, &file))
		return (1);

	if (!elf_check(&file))
		return (close_map(&file), 1);

	if (!file.is_64)
		return (dprintf(1, "%s: 32 bits ELF (pas encore gere)\n", path), close_map(&file), 0);

	if (!locate_symtab_64(&file))
		return (print_error(path, "file format not recognized"), close_map(&file), 1);

	if (!file.has_symtab)
		return (print_error(path, "no symbols"), close_map(&file), 0);

	if (!build_symbols_64(&file))
		return (print_error(path, strerror(errno)), close_map(&file), 1);
	set_type_chars_64(&file);
	sort_symbols(&file);
	print_symbols(&file);
	close_map(&file);
	return (0);
}

int	main(int argc, char **argv)
{
	int	i;
	int	status;

	setlocale(LC_COLLATE, "");
	status = 0;
	if (argc == 1)
		status = handle_file("a.out");
	else
	{
		i = 1;
		while (i < argc)
		{
			if (handle_file(argv[i]) != 0)
				status = 1;
			i++;
		}
	}
	return (status);
}
