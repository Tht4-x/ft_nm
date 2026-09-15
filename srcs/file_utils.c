#include "ft_nm.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void	close_map(t_elf_file *file)
{
	if (file->map)
		munmap(file->map, file->size);
	file->map = NULL;
	free(file->symbols);
	file->symbols = NULL;
}

bool	elf_check(t_elf_file *file)
{
	const unsigned char	*e_ident;

	if (file->size < EI_NIDENT)
		return (print_error(file->path, "file format not recognized"), false);

	e_ident = (const unsigned char *)file->map;
	if (memcmp(e_ident, ELFMAG, SELFMAG) != 0)
		return (print_error(file->path, "file format not recognized"), false);

	if (e_ident[EI_CLASS] == ELFCLASS64)
		file->is_64 = true;
	else if (e_ident[EI_CLASS] == ELFCLASS32)
		file->is_64 = false;
	else
		return (print_error(file->path, "file format not recognized"), false);

	if (file->is_64 && file->size < sizeof(Elf64_Ehdr))
		return (print_error(file->path, "file format not recognized"), false);

	if (!file->is_64 && file->size < sizeof(Elf32_Ehdr))
		return (print_error(file->path, "file format not recognized"), false);

	return (true);
}

static bool	init_and_open(const char *path, t_elf_file *file, int *fd)
{
	memset(file, 0, sizeof(*file));
	file->path = path;
	*fd = open(path, O_RDONLY);
	if (*fd < 0)
	{
		if (errno == ENOENT)
			return (print_error(path, "No such file"), false);
		return (print_error(path, strerror(errno)), false);
	}
	return (true);
}

bool	open_and_map(const char *path, t_elf_file *file)
{
	int			fd;
	struct stat	stat;

	if (!init_and_open(path, file, &fd))
		return (false);

	if (fstat(fd, &stat) < 0)
		return (print_error(path, strerror(errno)), close(fd), false);

	if (S_ISDIR(stat.st_mode))
		return (print_error(path, strerror(EISDIR)), close(fd), false);

	if (stat.st_size == 0)
		return (close(fd), false);

	file->size = (size_t)stat.st_size;
	file->map = mmap(NULL, file->size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	
	if (file->map == MAP_FAILED)
		return (print_error(path, strerror(errno)), file->map = NULL, false);
	return (true);
}

bool	parse_args(int argc, char **argv, t_options *opts)
{
	int	c;

	memset(opts, 0, sizeof(*opts));
	while ((c = getopt(argc, argv, "agupr")) != -1)
	{
		if (c == 'a')
			opts->all = true;
		else if (c == 'g')
			opts->extern_only = true;
		else if (c == 'u')
			opts->undefined_only = true;
		else if (c == 'p')
			opts->no_sort = true;
		else if (c == 'r')
			opts->reverse = true;
		else
			return (false);
	}
	return (true);
}

