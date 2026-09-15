#ifndef FT_NM_H
# define FT_NM_H

# include <elf.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <fcntl.h>
# include <stdbool.h>

typedef struct s_options
{
	bool				all;				// -a
	bool				extern_only;		// -g
	bool				undefined_only;		// -u
	bool				reverse;			// -r
	bool				no_sort;			// -p
}					t_options;

typedef struct s_symbol
{
	const char		*name;
	unsigned long	value;			//adresse du symbole
	int				has_value;		//a-til une adresse -> gere value == 0
	unsigned char	bind;			//local ou global
	unsigned char	type;			//STT_FUNC STT_OBJECT STT_FILE STT_SECTION STT_NOTYPE
	unsigned short	shndx;			//section dans lequel il se trouve
	char			type_char;		//T, t, U, D, d, B, b, W, w, A, C, R, r
}					t_symbol;

typedef struct s_elf_file
{
	const char		*path;
	void			*map;
	size_t			size;
	bool			is_64;
	bool			has_symtab;
	size_t			symtab_off;
	size_t			symtab_size;
	size_t			sym_entsize;
	size_t			strtab_off;
	size_t			strtab_size;
	bool			has_shstrtab;
	size_t			shstrtab_off;
	size_t			shstrtab_size;
	t_symbol		*symbols;
	size_t			nb_symbols;
}					t_elf_file;

bool	parse_args(int argc, char **argv, t_options *opts);
void	filter_symbols(t_elf_file *file, const t_options *opts);
void	reverse_symbols(t_elf_file *file);
void	print_error(const char *path, const char *msg);
bool	open_and_map(const char *path, t_elf_file *file);
void	close_map(t_elf_file *file);
bool	elf_check(t_elf_file *file);
bool	locate_symtab_64(t_elf_file *file);
bool	build_symbols_64(t_elf_file *file);
void	set_type_chars_64(t_elf_file *file);
bool	locate_symtab_32(t_elf_file *file);
bool	build_symbols_32(t_elf_file *file);
void	set_type_chars_32(t_elf_file *file);
void	sort_symbols(t_elf_file *file);
void	print_symbols(const t_elf_file *file);

#endif
