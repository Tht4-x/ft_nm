#include "ft_nm.h"
#include <errno.h>
#include <string.h>

static bool	init_and_open(const char *path, t_elf_file *file, int *fd)
{
	memset(file, 0, sizeof(*file));
	file->path = path;
	*fd = open(path, O_RDONLY);
	if (*fd < 0)
		return (print_error(path, strerror(errno)), false);
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

void	close_map(t_elf_file *file)
{
	if (file->map)
		munmap(file->map, file->size);
	file->map = NULL;
	free(file->symbols);
	file->symbols = NULL;
}
