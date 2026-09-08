#include "ft_nm.h"

static int	handle_file(const char *path)
{
	t_elf_file	file;

	if (!open_and_map(path, &file))
		return (1);

	if (!elf_check(&file))
		return (close_map(&file), 1);
		
	dprintf(1, "%s: %s bits ELF, size=%zu\n", path, file.is_64 ? "64" : "32", file.size);
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
