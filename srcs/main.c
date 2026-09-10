#include "ft_nm.h"
#include <errno.h>
#include <string.h>
#include <locale.h>

void	print_error(const char *path, const char *msg)
{
	dprintf(2, "ft_nm: '%s': %s\n", path, msg);
}

static int	handle_file(const char *path, bool multi)
{
	t_elf_file	file;
	bool		ok;

	if (!open_and_map(path, &file))
		return (1);

	if (!elf_check(&file))
		return (close_map(&file), 1);

	if (file.is_64)
		ok = locate_symtab_64(&file);
	else
		ok = locate_symtab_32(&file);
	if (!ok)
		return (print_error(path, "file format not recognized"), close_map(&file), 1);

	if (multi)
		dprintf(1, "\n%s:\n", path);

	if (!file.has_symtab)
		return (print_error(path, "no symbols"), close_map(&file), 0);

	if (file.is_64)
		ok = build_symbols_64(&file);
	else
		ok = build_symbols_32(&file);
	if (!ok)
		return (print_error(path, strerror(errno)), close_map(&file), 1);

	if (file.is_64)
		set_type_chars_64(&file);
	else
		set_type_chars_32(&file);
		
	sort_symbols(&file);
	print_symbols(&file);
	close_map(&file);
	return (0);
}

int	main(int argc, char **argv)
{
	int		i;
	int		status;
	bool	multi;

	setlocale(LC_COLLATE, "");
	status = 0;
	if (argc == 1)
		status = handle_file("a.out", false);
	else
	{
		multi = (argc > 2);
		i = 1;
		while (i < argc)
		{
			if (handle_file(argv[i], multi) != 0)
				status = 1;
			i++;
		}
	}
	return (status);
}
