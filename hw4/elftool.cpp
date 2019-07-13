#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "elftool.hpp"

void elf_perror(const char *prefix) {
	int e = elf_errno();
	fprintf(stderr, "%s: %s\n", prefix, elf_errmsg(e));
	return;
}

static int _elf_tool_initialized = 0;

int
elf_init() {
	errno = 0;
	if(_elf_tool_initialized <= 0) {
		elf_version(1); // version must be set first
	}
	_elf_tool_initialized = 1;
	return 0;
}

elf_handle_t *
elf_open(const char *filename) {
	int fd = -1;
	char *ident = NULL;
	size_t idsize = 0;
	Elf *elf = NULL;
	elf_handle_t *e = NULL;

	if((fd = open(filename, O_RDONLY)) < 0) return NULL;
	if((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) goto err_quit;
	if((ident = elf_getident(elf, &idsize)) == NULL) goto err_quit;
	if((e = (elf_handle_t*) malloc(sizeof(elf_handle_t))) == NULL) goto err_quit;
	memset(e, 0, sizeof(elf_handle_t));
	e->fd = -1;

	if(ident[4] == 0x01) { // 32-bit
		e->clazz = ELFCLASS32;
		e->ehdr.ptr = elf32_getehdr(elf);
		e->phnum = e->ehdr.ptr32->e_phnum;
		e->shnum = e->ehdr.ptr32->e_shnum;
		e->shstrndx = e->ehdr.ptr32->e_shstrndx;
		e->entrypoint = e->ehdr.ptr32->e_entry;
	} else if(ident[4] == 0x02) { // 64-bit
		e->clazz = ELFCLASS64;
		e->ehdr.ptr = elf64_getehdr(elf);
		e->phnum = e->ehdr.ptr64->e_phnum;
		e->shnum = e->ehdr.ptr64->e_shnum;
		e->shstrndx = e->ehdr.ptr64->e_shstrndx;
		e->entrypoint = e->ehdr.ptr64->e_entry;
	} else {
		errno = ENOTSUP;
		goto err_quit;
	}

	e->fd = fd;
	e->elf = elf;
	return e;

err_quit:
	if(elf)		{ elf_end(elf); elf = NULL; }
	if(fd >= 0)	{ close(fd); fd = -1; }
	if(e)		{ free(e); e = NULL; }
	return NULL;
}

int
elf_close(elf_handle_t *e) {
	if(e == NULL)	return 0;
	if(e->dsym)	{ free(e->dsym);	e->dsym = NULL; }
	if(e->sym)	{ free(e->sym);		e->sym = NULL; }
	if(e->strtab)	{ elf_free_strtab(e->strtab); e->strtab = NULL; }
	if(e->shdr)	{ free(e->shdr);	e->shdr = NULL; }
	if(e->phdr)	{ free(e->phdr);	e->phdr = NULL; }
	if(e->elf)	{ elf_end(e->elf);	e->elf = NULL; }
	if(e->fd >= 0)	{ close(e->fd);		e->fd = -1; }
	free(e);
	return 0;
}

int
elf_load_all(elf_handle_t *e) {
	if(elf_load_phdr(e) == NULL)	goto err_quit;
	if(elf_load_shdr(e) == NULL)	goto err_quit;
	if(elf_load_strtab(e) == NULL)	goto err_quit;
	elf_load_symtab(e);	// optional, no check
	elf_load_dsymtab(e);	// optional, no check
	return 0;
err_quit:
	if(e->dsym)	{ free(e->dsym);	e->dsym = NULL; }
	if(e->sym)	{ free(e->sym);		e->sym = NULL; }
	if(e->strtab)	{ elf_free_strtab(e->strtab); e->strtab = NULL; }
	if(e->shdr)	{ free(e->shdr);	e->shdr = NULL; }
	if(e->phdr)	{ free(e->phdr);	e->phdr = NULL; }
	return -1;
}

elf_phdr_t *
elf_load_phdr(elf_handle_t *e) {
	int i;
	elf_phdr_t *phdr = NULL;
	if(e->phdr != NULL) return e->phdr;
	if(e->phnum <= 0) return NULL;
	if(e->clazz != ELFCLASS32 && e->clazz != ELFCLASS64) return NULL;
	if((phdr = (elf_phdr_t*) malloc(sizeof(elf_phdr_t) * e->phnum)) == NULL)
		return NULL;

#define ITER_PHDR(x)	do { Elf##x##_Phdr *p = elf##x##_getphdr(e->elf); \
	for(i = 0; i < e->phnum; i++) { \
			phdr[i].type   = p[i].p_type;	\
			phdr[i].offset = p[i].p_offset;	\
			phdr[i].vaddr  = p[i].p_vaddr;	\
			phdr[i].paddr  = p[i].p_paddr;	\
			phdr[i].filesz = p[i].p_filesz;	\
			phdr[i].memsz  = p[i].p_memsz;	\
			phdr[i].flags  = p[i].p_flags;	\
			phdr[i].align  = p[i].p_align;	\
	} } while(0);	// do it only once

	if(e->clazz == ELFCLASS32) {
		ITER_PHDR(32);
	} else {
		ITER_PHDR(64);
	}

	e->phdr = phdr;
	return phdr;
}

elf_shdr_t *
elf_load_shdr(elf_handle_t *e) {
	Elf_Scn *scn = NULL;	// section descriptor
	elf_shdr_t *shdr = NULL;
	if(e->shdr != NULL) return e->shdr;
	if(e->shnum <= 0) return NULL;
	if(e->clazz != ELFCLASS32 && e->clazz != ELFCLASS64) return NULL;
	if((shdr = (elf_shdr_t*) malloc(sizeof(elf_shdr_t) * e->shnum)) == NULL)
		return NULL;

#define ASSIGN_SHDR(x)	do { Elf##x##_Shdr *s = elf##x##_getshdr(scn);	\
				shdr[idx].name      = s->sh_name;		\
				shdr[idx].type      = s->sh_type;		\
				shdr[idx].flags     = s->sh_flags;	\
				shdr[idx].addr      = s->sh_addr;		\
				shdr[idx].offset    = s->sh_offset;	\
				shdr[idx].size      = s->sh_size;		\
				shdr[idx].link      = s->sh_link;		\
				shdr[idx].info      = s->sh_info;		\
				shdr[idx].addralign = s->sh_addralign;	\
				shdr[idx].entsize   = s->sh_entsize;	\
			} while(0);	// do it only once

	while((scn = elf_nextscn(e->elf, scn)) != NULL) {
		int idx = elf_ndxscn(scn) % e->shnum;
		if(e->clazz == ELFCLASS32) { ASSIGN_SHDR(32); }
		else { ASSIGN_SHDR(64); }
	}

	e->shdr = shdr;
	return shdr;
}

elf_strtab_t *
elf_load_strtab(elf_handle_t *e) {
	int i, realerror;
	elf_strtab_t *strtab = NULL, *curr = NULL;
	if(e->strtab != NULL)		return e->strtab;
	if(elf_load_shdr(e) == NULL)	return NULL;
	for(i = 0; i < e->shnum; i++) {
		if(e->shdr[i].type != SHT_STRTAB) continue;
		if((curr = (elf_strtab_t*) malloc(sizeof(elf_strtab_t))) == NULL)
			goto err_quit;
		curr->id = i;
		curr->datasize = e->shdr[i].size;
		if((curr->data = (char*) malloc(e->shdr[i].size)) == NULL)
			goto err_quit;
		if(pread(e->fd, curr->data, e->shdr[i].size, e->shdr[i].offset) != e->shdr[i].size)
			goto err_quit;
		curr->next = strtab;
		strtab = curr;
	}
	e->strtab = strtab;
	return strtab;
err_quit:
	realerror = errno;
	elf_free_strtab(strtab);
	errno = realerror;
	return NULL;
}

void
elf_free_strtab(elf_strtab_t *strtab) {
	elf_strtab_t *curr, *nextab;
	for(curr = strtab; curr != NULL; curr = nextab) {
		nextab = curr->next;
		if(curr->data) free(curr->data);
		free(curr);
	}
	return;
}

static elf_symtab_t *
elf_load_symtab_internal(elf_handle_t *e, elf_shdr_t *shdr) {
	int i, realerror;
	elf_strtab_t *tab = NULL;
	elf_symtab_t *symtab = NULL;
	if(shdr == NULL)		return NULL;
	if(elf_load_shdr(e) == NULL)	return NULL;
	if(elf_load_strtab(e) == NULL)	return NULL;
	if((symtab = (elf_symtab_t*) malloc(sizeof(elf_symtab_t))) == NULL) return NULL;
	if(e->clazz != ELFCLASS32 && e->clazz != ELFCLASS64) { errno = EINVAL; return NULL; }
	for(tab = e->strtab; tab != NULL; tab = tab->next) {
		if(shdr->link == tab->id) {
			symtab->strtab = tab;
			break;
		}
	}
	if(symtab->strtab == NULL) { // table not found
		//fprintf(stderr, "! cannot find the string table (%llx,%lld)\n", shdr->type, shdr->link);
		realerror = ESRCH;
		goto err_quit;
	}
	symtab->count = (e->clazz == ELFCLASS32) ? (shdr->size/sizeof(Elf32_Sym)) : (shdr->size/sizeof(Elf64_Sym));
	if((symtab->symbol = (elf_symbol_t*) malloc(sizeof(elf_symbol_t) * symtab->count + shdr->size)) == NULL)
		goto err_quit;
	if(pread(e->fd, ((unsigned char*) symtab->symbol) + (sizeof(elf_symbol_t)*symtab->count), shdr->size, shdr->offset) != shdr->size)
		goto err_quit;

#define ASSIGN_SYM(x)	do { Elf##x##_Sym *s = (Elf##x##_Sym*) (((unsigned char*) symtab->symbol) + (sizeof(elf_symbol_t)*symtab->count));	\
				symtab->symbol[i].name      = s[i].st_name;	\
				symtab->symbol[i].info      = s[i].st_info;	\
				symtab->symbol[i].other     = s[i].st_other;	\
				symtab->symbol[i].shndx     = s[i].st_shndx;	\
				symtab->symbol[i].value     = s[i].st_value;	\
				symtab->symbol[i].size      = s[i].st_size;	\
				symtab->symbol[i].bind      = ELF##x##_ST_BIND(s[i].st_info);	\
				symtab->symbol[i].type      = ELF##x##_ST_TYPE(s[i].st_info);	\
				symtab->symbol[i].visibility= ELF##x##_ST_VISIBILITY(s[i].st_other);	\
			} while(0);	// do it only once

	for(i = 0; i < symtab->count; i++) {
		if(e->clazz == ELFCLASS32) {
			ASSIGN_SYM(32);
		} else {
			ASSIGN_SYM(64);
		}
	}

	return symtab;

err_quit:
	realerror = errno;
	if(symtab) free(symtab);
	errno = realerror;
	return NULL;
}

elf_symtab_t *
elf_load_symtab(elf_handle_t *e) {
	int i;
	if(e->sym != NULL)		return e->sym;
	if(elf_load_shdr(e) == NULL)	return NULL;
	for(i = 0; i < e->shnum; i++) {
		if(e->shdr[i].type == SHT_SYMTAB) {
			e->sym = elf_load_symtab_internal(e, &e->shdr[i]);
			return e->sym;
		}
	}
	return NULL;
}

elf_symtab_t *
elf_load_dsymtab(elf_handle_t *e) {
	int i;
	if(e->dsym != NULL)		return e->dsym;
	if(elf_load_shdr(e) == NULL)	return NULL;
	for(i = 0; i < e->shnum; i++) {
		if(e->shdr[i].type == SHT_DYNSYM) {
			e->dsym = elf_load_symtab_internal(e, &e->shdr[i]);
			return e->dsym;
		}
	}
	return NULL;
}

