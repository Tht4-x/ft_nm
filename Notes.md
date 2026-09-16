# Notes ELF — projet ft_nm

## 1. En-tête ELF (`e_ident`, 16 premiers octets)

| Octet(s) | Champ | Valeur / rôle |
|---|---|---|
| 0-3 | magic | `\x7f` `E` `L` `F` |
| 4 | `EI_CLASS` | 1 = ELF32, 2 = ELF64 |
| 5 | `EI_DATA` | 1 = little endian, 2 = big endian |
| 6 | `EI_VERSION` | toujours 1 depuis la création du format |
| 7 | `EI_OSABI` | environnement/ABI (ex: 3 = Linux) |
| 8 | `EI_ABIVERSION` | version de l'ABI indiquée par `EI_OSABI` |
| 9-15 | padding | réservé, inutilisé |

## 2. Suite de l'en-tête (`Elf64_Ehdr`, après `e_ident`)

| Octets | Champ | Rôle |
|---|---|---|
| 16-17 | `e_type` | 1=REL (.o), 2=EXEC, 3=DYN (.so), 4=CORE (photo mémoire lors d'un crash, pour gdb) |
| 18-19 | `e_machine` | architecture cible (ex: 62 = x86-64) |
| 20-23 | `e_version` | toujours 1 |
| 24-31 | `e_entry` | adresse virtuelle du point d'entrée — `0` si ce n'est pas un exécutable |
| 32-39 | `e_phoff` | offset vers la Program Header Table (organisation du chargement en mémoire) |
| 40-47 | `e_shoff` | offset vers la Section Header Table (`.text`, `.data`, `.bss`, `.symtab`, `.strtab`, `.shstrtab`...) |
| 48-51 | `e_flags` | flags spécifiques à l'architecture |
| 52-53 | `e_ehsize` | taille du header (= 64 pour ELF64) |
| 54-55 | `e_phentsize` | taille d'une entrée de Program Header |
| 56-57 | `e_phnum` | nombre d'entrées dans la Program Header Table |
| 58-59 | `e_shentsize` | taille d'une entrée de Section Header |
| 60-61 | `e_shnum` | nombre d'entrées dans la Section Header Table |
| 62-63 | `e_shstrndx` | index de la section des noms de sections (`.shstrtab`) |

## 3. Vue d'ensemble du fichier

```
┌──────────────────────────────┐
│ ELF Header                   │ ← infos sur le fichier, dit où trouver le reste
├──────────────────────────────┤
│ .text                        │ ← code machine
├──────────────────────────────┤
│ .data                        │ ← variables initialisées
├──────────────────────────────┤
│ .bss                         │ ← variables non initialisées
├──────────────────────────────┤
│ .symtab                      │ ← table des symboles
├──────────────────────────────┤
│ .strtab                      │ ← noms des symboles
├──────────────────────────────┤
│ .shstrtab                    │ ← noms des sections
└──────────────────────────────┘
```

Chemin suivi par le parsing : `e_shoff/e_shnum` (header) → on scanne la table des sections jusqu'à `sh_type == SHT_SYMTAB` → `sh_offset`/`sh_link` de cette section donnent les symboles et l'index de la `.strtab` associée → `st_name` dans chaque `Sym` donne l'offset du nom dans cette `.strtab`.

## 4. Une entrée de symbole (`Elf64_Sym`)

Voir la structure complète en fin de document.

## 5. Lettres de type (implémentées dans `ft_nm`)

| Lettre | Sens | Condition |
|---|---|---|
| `U` | Undefined | `shndx == SHN_UNDEF`, bind ≠ WEAK |
| `w` | Weak non défini | `shndx == SHN_UNDEF`, bind == WEAK |
| `W` | Weak défini | bind == WEAK, quelle que soit la section |
| `T` / `t` | Code (`.text`) | `SHF_EXECINSTR` — majuscule si global |
| `D` / `d` | Donnée initialisée | writable, pas `SHT_NOBITS` |
| `B` / `b` | BSS (non initialisée) | `SHT_NOBITS` |
| `R` / `r` | Lecture seule (`.rodata`) | alloué, pas writable |
| `A` / `a` | Absolu | `shndx == SHN_ABS` |
| `C` / `c` | Common | `shndx == SHN_COMMON` — `c` (local) jamais atteint en pratique sur x86-64 |
| `n` | Symbole ordinaire dans une section non chargée en mémoire | section sans `SHF_ALLOC` (ex: `.group`) |
| `N` | Symbole `SECTION` dans une section de *debug* | `sym->type == STT_SECTION`, section sans `SHF_ALLOC` **et** dont le nom commence par `.debug` — visible seulement avec `-a`. Une section non-alloc mais pas "debug" (ex: `.comment`) reste `n`, pas `N` (vérifié empiriquement : `.comment` a les mêmes flags que `.debug_str` mais donne `n`) |
| `?` | Non classifiable | index de section corrompu/hors bornes (fichier malveillant) |

## 6. Arbre de décision (`compute_type_char`)

Ordre exact des tests dans le code :

```
shndx == SHN_UNDEF ?
├── OUI → bind == WEAK ?
│         ├── OUI → w
│         └── NON → U
│
└── NON → bind == WEAK ?
          ├── OUI → W
          │
          └── NON → shndx == SHN_ABS ?
                    ├── OUI → bind == LOCAL ?
                    │         ├── OUI → a
                    │         └── NON → A
                    │
                    └── NON → shndx == SHN_COMMON ?
                              ├── OUI → bind == LOCAL ?
                              │         ├── OUI → c
                              │         └── NON → C
                              │
                              └── NON → index de section valide (shndx < e_shnum) ?
                                        ├── NON → ?
                                        │
                                        └── OUI → SHF_ALLOC ?
                                                  ├── NON → type == STT_SECTION ET nom commence par ".debug" ?
                                                  │         ├── OUI → N
                                                  │         └── NON → n
                                                  │
                                                  └── OUI → SHF_EXECINSTR ?
                                                            ├── OUI → bind == LOCAL ?
                                                            │         ├── OUI → t
                                                            │         └── NON → T
                                                            │
                                                            └── NON → SHT_NOBITS ?
                                                                      ├── OUI → bind == LOCAL ?
                                                                      │         ├── OUI → b
                                                                      │         └── NON → B
                                                                      │
                                                                      └── NON → SHF_WRITE ?
                                                                                ├── NON → bind == LOCAL ?
                                                                                │         ├── OUI → r
                                                                                │         └── NON → R
                                                                                │
                                                                                └── OUI → bind == LOCAL ?
                                                                                          ├── OUI → d
                                                                                          └── NON → D
```

18 lettres possibles au total : `U w W a A c C ? N n t T b B r R d D`.

## 7. Structures C complètes

```c
typedef struct
{
  unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */
  Elf64_Half    e_type;             /* Object file type */
  Elf64_Half    e_machine;          /* Architecture */
  Elf64_Word    e_version;          /* Object file version */
  Elf64_Addr    e_entry;            /* Entry point virtual address */
  Elf64_Off     e_phoff;            /* Program header table file offset */
  Elf64_Off     e_shoff;            /* Section header table file offset */
  Elf64_Word    e_flags;            /* Processor-specific flags */
  Elf64_Half    e_ehsize;           /* ELF header size in bytes */
  Elf64_Half    e_phentsize;        /* Program header table entry size */
  Elf64_Half    e_phnum;            /* Program header table entry count */
  Elf64_Half    e_shentsize;        /* Section header table entry size */
  Elf64_Half    e_shnum;            /* Section header table entry count */
  Elf64_Half    e_shstrndx;         /* Section header string table index */
} Elf64_Ehdr;

typedef struct
{
  Elf64_Word    st_name;   // offset du nom dans .strtab
  unsigned char st_info;   // bind (4 bits haut) + type (4 bits bas)
  unsigned char st_other;  // visibilité, non utilisé ici
  Elf64_Section st_shndx;  // section d'appartenance, ou SHN_UNDEF/SHN_ABS/SHN_COMMON
  Elf64_Addr    st_value;  // adresse (exécutable/.so) ou offset dans la section (.o)
  Elf64_Xword   st_size;   // taille du symbole
} Elf64_Sym;
```
