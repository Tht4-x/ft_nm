#include "ft_nm.h"
#include <string.h>

static bool	region_fits(size_t off, size_t len, size_t total)
{
	return (off <= total && len <= total - off);
}

static const Elf64_Shdr	*shdr_at(const t_elf_file *file, size_t shoff, size_t shentsize, size_t i)
{
	return ((const Elf64_Shdr *)((const unsigned char *)file->map + shoff + i * shentsize));
}

bool	locate_symtab_64(t_elf_file *file)
{
	const Elf64_Ehdr	*eh;
	const Elf64_Shdr	*sh;
	const Elf64_Shdr	*link_sh;
	size_t				i;

	eh = (const Elf64_Ehdr *)file->map;
	if (eh->e_shnum == 0 || eh->e_shentsize < sizeof(Elf64_Shdr))
		return (false);

	if (!region_fits((size_t)eh->e_shoff, (size_t)eh->e_shnum * (size_t)eh->e_shentsize, file->size))
		return (false);
		
	i = 0;
	while (i < eh->e_shnum)
	{
		sh = shdr_at(file, eh->e_shoff, eh->e_shentsize, i);
		if (sh->sh_type == SHT_SYMTAB)
		{
			if (sh->sh_link >= eh->e_shnum || sh->sh_entsize < sizeof(Elf64_Sym)) //link renvoie vers l'adresse de strtab si dans symtab
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

static const char	*resolve_name(const t_elf_file *file, size_t st_name)
{
	const char	*base;

	if (st_name >= file->strtab_size)
		return (NULL);
	base = (const char *)file->map + file->strtab_off + st_name;
	if (!memchr(base, '\0', file->strtab_size - st_name))
		return (NULL);
	return (base);
}

bool	build_symbols_64(t_elf_file *file)
{
	const Elf64_Sym	*raw;
	size_t			count;
	size_t			i;
	const char		*name;

	count = file->symtab_size / file->sym_entsize;
	file->symbols = malloc(sizeof(t_symbol) * count);
	if (!file->symbols)
		return (false);

	file->nb_symbols = 0;
	i = 1;
	while (i < count) //espace toujours vide en i = 0
	{
		raw = (const Elf64_Sym *)((const unsigned char *)file->map + file->symtab_off + i * file->sym_entsize);
		name = resolve_name(file, raw->st_name); //st_name = offset du str du symbole a partir du debut de strtab
		if (name && ELF64_ST_TYPE(raw->st_info) != STT_FILE	&& ELF64_ST_TYPE(raw->st_info) != STT_SECTION)
		{
			file->symbols[file->nb_symbols].name = name;
			file->symbols[file->nb_symbols].value = (unsigned long)raw->st_value;
			file->symbols[file->nb_symbols].has_value = (raw->st_shndx != SHN_UNDEF);
			file->symbols[file->nb_symbols].bind = ELF64_ST_BIND(raw->st_info);
			file->symbols[file->nb_symbols].type = ELF64_ST_TYPE(raw->st_info);
			file->symbols[file->nb_symbols].shndx = raw->st_shndx;
			file->symbols[file->nb_symbols].type_char = '?';
			file->nb_symbols++;
		}
		i++;
	}
	return (true);
}

/*
typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/ Magic number and other info /
  Elf64_Half	e_type;			/ Object file type /
  Elf64_Half	e_machine;		/ Architecture /
  Elf64_Word	e_version;		/ Object file version /
  Elf64_Addr	e_entry;		/ Entry point virtual address /
  Elf64_Off		e_phoff;		/ Program header table file offset /
  Elf64_Off		e_shoff;		/ Section header table file offset /
  Elf64_Word	e_flags;		/ Processor-specific flags /
  Elf64_Half	e_ehsize;		/ ELF header size in bytes /
  Elf64_Half	e_phentsize;	/ Program header table entry size /
  Elf64_Half	e_phnum;		/ Program header table entry count /
  Elf64_Half	e_shentsize;	/ Section header table entry size /
  Elf64_Half	e_shnum;		/ Section header table entry count /
  Elf64_Half	e_shstrndx;		/ Section header string table index /
} Elf64_Ehdr;

typedef struct
{
  Elf64_Word	sh_name;		/ Section name (string tbl index) /
  Elf64_Word	sh_type;		/ Section type /
  Elf64_Xword	sh_flags;		/ Section flags /
  Elf64_Addr	sh_addr;		/ Section virtual addr at execution /
  Elf64_Off		sh_offset;		/ Section file offset /
  Elf64_Xword	sh_size;		/ Section size in bytes /
  Elf64_Word	sh_link;		/ Link to another section /
  Elf64_Word	sh_info;		/ Additional section information /
  Elf64_Xword	sh_addralign;	/ Section alignment /
  Elf64_Xword	sh_entsize;		/ Entry size if section holds table /
} Elf64_Shdr;
 
typedef struct
{
  Elf64_Word	st_name;		/ Symbol name (string tbl index) /
  unsigned char	st_info;		/ Symbol type and binding /
  unsigned char st_other;		/ Symbol visibility /
  Elf64_Section	st_shndx;		/ Section index /
  Elf64_Addr	st_value;		/ Symbol value /
  Elf64_Xword	st_size;		/ Symbol size /
} Elf64_Sym;
*/