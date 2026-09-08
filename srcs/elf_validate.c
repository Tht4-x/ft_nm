#include "ft_nm.h"
#include <string.h>

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

/*
ELF HEADER

e_ident (16 premiers octets) :

octets 0-3   : le magic \x7f 'E' 'L' 'F'

octet 4      : EI_CLASS
               1 = ELF32
               2 = ELF64

octet 5      : EI_DATA
               1 = little endian
               2 = big endian

octet 6      : EI_VERSION
               1 = version actuelle de ELF depuis sa creation

octet 7      : EI_OSABI
               identifie l'environnement/ABI
               ex : 3 = Linux

octet 8      : EI_ABIVERSION
               version de l'ABI indiquée par EI_OSABI

octets 9-15  : padding / réservés

octets 16-17  : e_type
                type du fichier ELF
                1 = relocatable (.o)
                2 = executable
                3 = shared object (.so)
                4 = core dump (photo lors d'un crash pour utiliser gdb)

octets 18-19  : e_machine
                architecture cible
                ex : 62 = x86-64

octets 20-23  : e_version (version ELF, toujours 1 depuis sa creation)

octets 24-31  : e_entry
                adresse virtuelle du point d'entrée (= 0 si ce n'est pas un executable)
                (où commence l'exécution)

octets 32-39  : e_phoff
                offset dans le fichier vers la Program Header Table (organisation du chargement en memoire du elf)

octets 40-47  : e_shoff
                offset dans le fichier vers la Section Header Table (.text, .data, .bss, .symtab .strtab .shstrtab avec taille, type offset,...)

octets 48-51  : e_flags
                flags spécifiques à l'architecture

octets 52-53  : e_ehsize
                taille du header ELF (= 64 pour ELF64)

octets 54-55  : e_phentsize
                taille d'une entrée de Program Header

octets 56-57  : e_phnum
                nombre d'entrées dans la Program Header Table

octets 58-59  : e_shentsize
                taille d'une entrée de Section Header

octets 60-61  : e_shnum
                nombre d'entrées dans la Section Header Table

octets 62-63  : e_shstrndx
                index de la section contenant les noms
                des sections (.shstrtab)
*/