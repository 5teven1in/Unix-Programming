#ifndef __ELF_TOOL_H__
#define __ELF_TOOL_H__

#include <libelf.h>

typedef struct elf_phdr_s {
	long long type;		// segment type
	long long offset;	// file offset
	long long vaddr;	// virtual address
	long long paddr;	// physical address
	long long filesz;	// size in file
	long long memsz;	// size in memory
	long long flags;	// flags
	long long align;	// alignment
}	elf_phdr_t;

typedef struct elf_shdr_s {
	long long name;		// section name
	long long type;		// section type
	long long flags;	// section flags
	long long addr;		// section virtual addr at execution
	long long offset;	// section file offset
	long long size;		// section size in bytes
	long long link;		// link to another section
	long long info;		// additional section information
	long long addralign;	// section alignment
	long long entsize;	// entry size if section holds table
}	elf_shdr_t;

typedef struct elf_strtab_s {
	int id;
	int datasize;
	char *data;
	struct elf_strtab_s *next;
}	elf_strtab_t;

typedef struct elf_symbol_s {
	int name;		// symbol name (string table index)
	unsigned char info;	// symbol type and binding
	unsigned char other;	// symbol visibility
	int shndx;		// section index
	long long value;	// symbol value
	long long size;		// symbol size
	// extra attributes
	unsigned char bind, type, visibility;
} elf_symbol_t;

typedef struct elf_symtab_s {
	int count;
	elf_strtab_t *strtab;
	elf_symbol_t *symbol;
}	elf_symtab_t;

typedef struct elf_handle_s {
	/* real handles */
	int fd;			// file descriptor
	Elf *elf;		// elf ptr from libelf

	/* elf header */
	int clazz;
	int phnum;		// # of phdr
	int shnum;		// # of sections
	int shstrndx;		// section header string table index
	long entrypoint;	// entrypoint
	union {
		void *ptr;
		Elf32_Ehdr *ptr32;
		Elf64_Ehdr *ptr64;
	} ehdr;			// elf header

	/* other headers and tables */
	elf_phdr_t *phdr;	// program header
	elf_shdr_t *shdr;	// section header
	elf_strtab_t *strtab;	// string table
	elf_symtab_t *sym, *dsym; // symbol table and dynamic symbol table

}	elf_handle_t;

void elf_perror(const char *prefix);
int elf_init();
int elf_close(elf_handle_t *handle);
elf_handle_t * elf_open(const char *filename);

int elf_load_all(elf_handle_t *e);
elf_phdr_t * elf_load_phdr(elf_handle_t *e);
elf_shdr_t * elf_load_shdr(elf_handle_t *e);
elf_strtab_t * elf_load_strtab(elf_handle_t *e);
void elf_free_strtab(elf_strtab_t *strtab);
elf_symtab_t * elf_load_symtab(elf_handle_t *e);
elf_symtab_t * elf_load_dsymtab(elf_handle_t *e);

#endif /* __ELF_TOOL_H__ */
