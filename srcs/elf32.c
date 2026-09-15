#include "ft_nm.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static bool	region_fits(size_t off, size_t len, size_t total)
{
	return (off <= total && len <= total - off);
}

static const Elf32_Shdr	*shdr_at(const t_elf_file *file, size_t shoff, size_t shentsize, size_t i)
{
	return ((const Elf32_Shdr *)((const unsigned char *)file->map + shoff + i * shentsize));
}

bool	locate_symtab_32(t_elf_file *file)
{
	const Elf32_Ehdr	*eh;
	const Elf32_Shdr	*sh;
	const Elf32_Shdr	*link_sh;
	size_t				i;

	eh = (const Elf32_Ehdr *)file->map;
	if (eh->e_shnum == 0 || eh->e_shentsize < sizeof(Elf32_Shdr))
		return (false);

	if (!region_fits((size_t)eh->e_shoff, (size_t)eh->e_shnum * (size_t)eh->e_shentsize, file->size))
		return (false);

	file->has_shstrtab = false;
	if (eh->e_shstrndx < eh->e_shnum)
	{
		sh = shdr_at(file, eh->e_shoff, eh->e_shentsize, eh->e_shstrndx);
		if (region_fits((size_t)sh->sh_offset, (size_t)sh->sh_size, file->size))
		{
			file->has_shstrtab = true;
			file->shstrtab_off = (size_t)sh->sh_offset;
			file->shstrtab_size = (size_t)sh->sh_size;
		}
	}

	i = 0;
	while (i < eh->e_shnum)
	{
		sh = shdr_at(file, eh->e_shoff, eh->e_shentsize, i);
		if (sh->sh_type == SHT_SYMTAB)
		{
			if (sh->sh_link >= eh->e_shnum || sh->sh_entsize < sizeof(Elf32_Sym))
				return (false);

			if (!region_fits((size_t)sh->sh_offset, (size_t)sh->sh_size, file->size))
				return (false);

			link_sh = shdr_at(file, eh->e_shoff, eh->e_shentsize, sh->sh_link);
			if (!region_fits((size_t)link_sh->sh_offset, (size_t)link_sh->sh_size, file->size))
				return (false);

			file->has_symtab = true;
			file->symtab_off = (size_t)sh->sh_offset;
			file->symtab_size = (size_t)sh->sh_size;
			file->sym_entsize = (size_t)sh->sh_entsize;
			file->strtab_off = (size_t)link_sh->sh_offset;
			file->strtab_size = (size_t)link_sh->sh_size;
			return (true);
		}
		i++;
	}
	file->has_symtab = false;
	return (true);
}

static const char	*resolve_str(const t_elf_file *file, size_t tab_off, size_t tab_size, size_t off)
{
	const char	*base;

	if (off >= tab_size)
		return (NULL);
	base = (const char *)file->map + tab_off + off;
	if (!memchr(base, '\0', tab_size - off))
		return (NULL);
	return (base);
}

static const char	*section_symbol_name(const t_elf_file *file, unsigned short shndx)
{
	const Elf32_Ehdr	*eh;
	const Elf32_Shdr	*target;

	if (!file->has_shstrtab)
		return (NULL);
	eh = (const Elf32_Ehdr *)file->map;
	if (shndx >= eh->e_shnum)
		return (NULL);
	target = shdr_at(file, eh->e_shoff, eh->e_shentsize, shndx);
	return (resolve_str(file, file->shstrtab_off, file->shstrtab_size, target->sh_name));
}

bool	build_symbols_32(t_elf_file *file)
{
	const Elf32_Sym	*raw;
	size_t			count;
	size_t			i;
	const char		*name;

	count = file->symtab_size / file->sym_entsize;
	file->symbols = malloc(sizeof(t_symbol) * count);
	if (!file->symbols)
		return (false);

	file->nb_symbols = 0;
	i = 1;
	while (i < count)
	{
		raw = (const Elf32_Sym *)((const unsigned char *)file->map + file->symtab_off + i * file->sym_entsize);
		name = resolve_str(file, file->strtab_off, file->strtab_size,
				raw->st_name);
		if (name && ELF32_ST_TYPE(raw->st_info) == STT_SECTION)
			name = section_symbol_name(file, raw->st_shndx);
		if (name)
		{
			file->symbols[file->nb_symbols].name = name;
			file->symbols[file->nb_symbols].value = (unsigned long)raw->st_value;
			file->symbols[file->nb_symbols].has_value = (raw->st_shndx != SHN_UNDEF);
			file->symbols[file->nb_symbols].bind = ELF32_ST_BIND(raw->st_info);
			file->symbols[file->nb_symbols].type = ELF32_ST_TYPE(raw->st_info);
			file->symbols[file->nb_symbols].shndx = raw->st_shndx;
			file->symbols[file->nb_symbols].type_char = '?';
			file->nb_symbols++;
		}
		i++;
	}
	return (true);
}

static char	compute_type_char_32(const t_elf_file *file, const t_symbol *sym)
{
	const Elf32_Ehdr	*eh;
	const Elf32_Shdr	*sh;
	char				base;

	if (sym->shndx == SHN_UNDEF)
		return (sym->bind == STB_WEAK ? 'w' : 'U');
	if (sym->bind == STB_WEAK)
		return ('W');
	if (sym->shndx == SHN_ABS)
		return (sym->bind == STB_LOCAL ? 'a' : 'A');
	if (sym->shndx == SHN_COMMON)
		return (sym->bind == STB_LOCAL ? 'c' : 'C');
	eh = (const Elf32_Ehdr *)file->map;
	if (sym->shndx >= eh->e_shnum)
		base = '?';
	else
	{
		sh = shdr_at(file, eh->e_shoff, eh->e_shentsize, sym->shndx);
		if (!(sh->sh_flags & SHF_ALLOC))
		{
			if (sym->type == STT_SECTION)
				return ('N');
			base = 'n';
		}
		else if (sh->sh_flags & SHF_EXECINSTR)
			base = 't';
		else if (sh->sh_type == SHT_NOBITS)
			base = 'b';
		else if (!(sh->sh_flags & SHF_WRITE))
			base = 'r';
		else
			base = 'd';
	}
	if (sym->bind == STB_LOCAL)
		return (base);
	return ((char)toupper((unsigned char)base));
}

void	set_type_chars_32(t_elf_file *file)
{
	size_t	i;

	i = 0;
	while (i < file->nb_symbols)
	{
		file->symbols[i].type_char = compute_type_char_32(file, &file->symbols[i]);
		i++;
	}
}
