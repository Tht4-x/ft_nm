#include "ft_nm.h"

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
			if (sh->sh_link >= eh->e_shnum || sh->sh_entsize == 0) //link renvoie vers l'adresse de strtab si dans symtab
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
*/