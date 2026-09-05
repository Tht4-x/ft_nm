/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dancel <dancel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 19:26:20 by dancel            #+#    #+#             */
/*   Updated: 2026/09/05 19:57:13 by dancel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H

# include "libft.h"
# include <elf.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <fcntl.h>

typedef struct s_options
{
	int				all;				// -a
	int				extern_only;		// -g
	int				undefined_only;		// -u
	int				reverse;			// -r
	int				no_sort;			// -p
}					t_options;

typedef struct s_symbol
{
	char			*name;
	unsigned long	value;
	int				has_value;
	unsigned char	bind;
	unsigned char	type;
	unsigned short	shndx;
	char			type_char;
}					t_symbol;

typedef struct s_elf_file
{
	const char		*path;
	int				fd;
	void			*map;
	size_t			size;
	int				is_64;
	t_symbol		*symbols;
	size_t			nb_symbols;
}					t_elf_file;

int		ft_nm(const char *path, const t_options *opts, int nb_files);

#endif

/*
Voici les notions à maîtriser pour ce projet, groupées par thème.

1. Format binaire ELF
Structure générale : ELF header → sections (code, data, tables...) → table des section headers.
ELF header (Ehdr) : magic \x7fELF, EI_CLASS (32/64 bits), EI_DATA (endianness), e_type (ET_REL=objet, ET_EXEC=exécutable, ET_DYN=bibliothèque partagée/PIE), e_shoff/e_shnum/e_shentsize (où trouver les sections).
Section headers (Shdr) : sh_type (SHT_SYMTAB, SHT_STRTAB, SHT_DYNSYM, SHT_NOBITS=bss...), sh_flags (SHF_ALLOC, SHF_WRITE, SHF_EXECINSTR), sh_offset/sh_size, sh_link (pointe vers la strtab associée pour une symtab).
Deux tables de strings différentes : la section header string table (e_shstrndx, donne les noms .text, .symtab...) et la symbol string table (donne les noms des symboles) — ne pas les confondre.
Symbole (Sym) : st_name (offset dans la strtab), st_value, st_info (macros ELF32_ST_BIND/ELF64_ST_BIND et ..._TYPE pour extraire bind/type d'un seul octet), st_shndx (section d'appartenance, ou valeur spéciale SHN_UNDEF/SHN_ABS/SHN_COMMON).
32 vs 64 bits : mêmes noms de champs, types Elf32_* /Elf64_* différents — tout le code de parsing doit exister en double, mais converge vers une structure interne commune (t_symbol).
.o vs exécutable vs .so : dans un .o les adresses ne sont pas finales (relogeable) ; un .so/exécutable stripped n'a souvent que .dynsym/.dynstr (pas de .symtab) → il faut prévoir le fallback.
2. Sémantique de nm
Lettres de type : T/t (code, .text), D/d (data initialisée), B/b (bss), R/r (rodata), U (undefined), W/w (weak), A (absolu), C (common), N (debug) — majuscule = global/weak, minuscule = local.
Symboles masqués par défaut : symboles de type STT_FILE ou STT_SECTION, réintégrés seulement avec -a.
Tri par défaut : alphabétique par nom ; -p = pas de tri (ordre brut) ; -r = inverse l'ordre final.
Formatage exact : largeur de l'adresse en hexa (8 chiffres en 32 bits, 16 en 64), zéro-padding, et les symboles undefined affichent des espaces à la place de l'adresse (pas des zéros).
Cas no symbols : fichier sans table de symboles exploitable → message spécifique, pas une erreur fatale.
3. Programmation système C
open/fstat/mmap/munmap : pourquoi mmap (accès direct par pointeur, pas de recopie) plutôt que read + buffer.
Vérifier chaque valeur de retour syscall (open peut échouer, fstat aussi, mmap peut renvoyer MAP_FAILED).
Endianness : le sujet ne cible que x86 (little-endian), mais il faut savoir que ça existe.
4. Robustesse — le point le plus noté du sujet
Jamais déréférencer un offset sans vérifier offset + taille <= size_du_fichier avant — pour l'en-tête, les section headers, les symboles, ET les chaînes de la strtab.
Une chaîne dans la strtab peut ne pas être terminée par \0 si le fichier est corrompu/tronqué : il faut le détecter, pas juste supposer.
e_shnum, sh_offset, sh_size, sh_link peuvent contenir des valeurs absurdes ou malveillantes dans un fichier trafiqué → tout borner.
Résultat : jamais de segfault/bus error/double free, même sur un fichier vide, tronqué, ou un .txt renommé .o.
Vérifier l'absence de fuites mémoire (valgrind), y compris sur les chemins d'erreur.
5. Options bonus
-a : réintègre les symboles de debug normalement masqués.
-g : ne garde que les symboles externes (bind GLOBAL/WEAK, exclut LOCAL).
-u : ne garde que les symboles undefined.
-r : inverse l'ordre d'affichage.
-p : désactive le tri.
Ordre d'application : filtrer d'abord (-u, -g, -a), trier ensuite (sauf -p), inverser en dernier (-r). Les flags sont combinables.
Parsing des options + plusieurs fichiers en argument (comportement quand plusieurs fichiers : nm affiche un en-tête par fichier).
6. Attentes spécifiques 42
Makefile avec les règles usuelles (all, clean, fclean, re) qui compile d'abord libft, puis le projet.
Correction humaine : il faut pouvoir justifier chaque choix (pourquoi mmap, pourquoi cette lettre de type, pourquoi cette structure) en soutenance — c'est explicitement rappelé dans le PDF.
Le bonus n'est même regardé que si le mandatory est irréprochable sur la gestion d'erreurs → prioriser la robustesse avant les 5 flags.
Validation systématique : comparer la sortie avec /usr/bin/nm sur de vrais fichiers (.o, exécutable non-strippé, .so) à chaque étape, pas seulement à la fin.
On peut continuer sur main.c (étape 2 du plan) dès que tu veux, ou creuser un de ces points plus en détail si quelque chose n'est pas clair.
*/