#include "ft_nm.h"
#include <string.h>
#include <ctype.h>

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

static char	compute_type_char_64(const t_elf_file *file, const t_symbol *sym)
{
	const Elf64_Ehdr	*eh;
	const Elf64_Shdr	*sh;
	char				base;

	if (sym->shndx == SHN_UNDEF)
		return (sym->bind == STB_WEAK ? 'w' : 'U');
	if (sym->bind == STB_WEAK)
		return ('W');
	if (sym->shndx == SHN_ABS)
		return (sym->bind == STB_LOCAL ? 'a' : 'A');
	if (sym->shndx == SHN_COMMON)
		return (sym->bind == STB_LOCAL ? 'c' : 'C');
	eh = (const Elf64_Ehdr *)file->map;
	if (sym->shndx >= eh->e_shnum)
		base = '?';
	else
	{
		sh = shdr_at(file, eh->e_shoff, eh->e_shentsize, sym->shndx);
		if (!(sh->sh_flags & SHF_ALLOC))
			base = 'n';
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

void	set_type_chars_64(t_elf_file *file)
{
	size_t	i;

	i = 0;
	while (i < file->nb_symbols)
	{
		file->symbols[i].type_char = compute_type_char_64(file, &file->symbols[i]);
		i++;
	}
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

U	Undefined — symbole utilisé mais défini ailleurs
T	Fonction dans .text (global)
t	Fonction dans .text (local)
D	Donnée initialisée dans .data (global)
d	Donnée initialisée dans .data (local)
B	Donnée non initialisée dans .bss (global)
b	Donnée non initialisée dans .bss (local)
R	Donnée en lecture seule (.rodata) (global)
r	Donnée en lecture seule (.rodata) (local)
W	Symbole weak défini
w	Symbole weak non défini
A	Symbole Absolute (SHN_ABS)
a	Absolute local
C	Common symbol
c	Common small
N	Debugging / symbole de type spécial
S	Section de petite taille
s	Section de petite taille locale
V	Weak object défini
v	Weak object non défini

                 section du symbole
                         │
                         ▼
              index valide ?
                /          \
              NON           OUI
               │             │
               ?             ▼
                         SHF_ALLOC ?
                         /       \
                       NON        OUI
                        │          │
                        n          ▼
                             SHF_EXECINSTR ?
                               /       \
                             OUI        NON
                              │          │
                              t          ▼
                                  SHT_NOBITS ?
                                    /      \
                                  OUI       NON
                                   │         │
                                   b         ▼
                                      SHF_WRITE ?
                                       /      \
                                     NON       OUI
                                      │         │
                                      r         d
*/