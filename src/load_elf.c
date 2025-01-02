#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

int load_elf(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    Elf64_Ehdr ehdr;
    size_t read_size = fread(&ehdr, 1, sizeof(Elf64_Ehdr), file);
    if (read_size != sizeof(Elf64_Ehdr)) {
        printf("Failed to read ELF header\n");
        fclose(file);
        return -1;
    }

    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        printf("Not an ELF file\n");
        fclose(file);
        return -1;
    }

    printf("ELF file loaded successfully!\n");
    printf("Entry point: 0x%lx\n", ehdr.e_entry);

    fseek(file, ehdr.e_phoff, SEEK_SET);
    Elf64_Phdr *phdrs = (Elf64_Phdr *)malloc(ehdr.e_phnum * sizeof(Elf64_Phdr));
    if (!phdrs) {
        printf("Failed to allocate memory for program headers\n");
        fclose(file);
        return -1;
    }
    read_size = fread(phdrs, sizeof(Elf64_Phdr), ehdr.e_phnum, file);
    if (read_size != ehdr.e_phnum) {
        printf("Failed to read program headers\n");
        free(phdrs);
        fclose(file);
        return -1;
    }

    for (int i = 0; i < ehdr.e_phnum; ++i) {
        Elf64_Phdr *phdr = &phdrs[i];
        printf("Segment %d:\n", i);
        printf("  Type: 0x%lx\n", phdr->p_type);
        printf("  Offset: 0x%lx\n", phdr->p_offset);
        printf("  Virtual address: 0x%lx\n", phdr->p_vaddr);
        printf("  Physical address: 0x%lx\n", phdr->p_paddr);
        printf("  File size: 0x%lx\n", phdr->p_filesz);
        printf("  Memory size: 0x%lx\n", phdr->p_memsz);
    }

    free(phdrs);
    fclose(file);

    return 0;
}
