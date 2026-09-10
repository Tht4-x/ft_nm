#include "ft_nm.h"
#include <errno.h>
#include <string.h>

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

	for (size_t i = 0; i < file.nb_symbols; i++)
	{
		if (file.symbols[i].has_value)
			dprintf(1, "%016lx %c %s\n", file.symbols[i].value,
				file.symbols[i].type_char, file.symbols[i].name);
		else
			dprintf(1, "%16s %c %s\n", "",
				file.symbols[i].type_char, file.symbols[i].name);
	}
	close_map(&file);
	return (0);
}

int	main(int argc, char **argv)
{
	int	i;
	int	status;

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
