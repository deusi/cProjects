// Template for parsing an ELF file to print its symbol table. You are
// free to rename any variables that appear below as you see fit.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

// Added pointer arithmetic macros from previous problem to make mu life easier

// macro to add a byte offset to a pointer, arguments are a pointer
// and a # of bytes (usually size_t)
#define PTR_PLUS_BYTES(ptr,off) ((void *) (((size_t) (ptr)) + ((size_t) (off))))

// macro to add a byte offset to a pointer, arguments are a pointer
// and a # of bytes (usually size_t)
#define PTR_MINUS_BYTES(ptr,off) ((void *) (((size_t) (ptr)) - ((size_t) (off))))

// macro to add a byte offset to a pointer, arguments are a pointer
// and a # of bytes (usually size_t)
#define PTR_MINUS_PTR(ptr,ptq) (((size_t) (ptr)) - ((size_t) (ptq)))

int DEBUG = 0;                  // controls whether to print debug messages

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s [-d] <file>\n",argv[0]);
    return 0;
  }

  char *objfile_name = argv[1];

  // check for debug mode
  if(argc >=3){
    if(strcmp("-d",argv[1])==0){ // command line arg -d enables debug printing
      DEBUG = 1;
      objfile_name = argv[2];
    }
    else{
      printf("incorrect usage\n");
      return 1;
    }
  }

  // Open file descriptor and set up memory map for objfile_name
  int fd = open(objfile_name, O_RDONLY);
  struct stat stat_buf;
  fstat(fd, &stat_buf); // get info about fd
  int size = stat_buf.st_size;  // assign variable to fd's size
  char *file_bytes =
    mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);


  // CREATE A POINTER to the intial bytes of the file which are an ELF64_Ehdr struct
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *) file_bytes;

  // CHECK e_ident field's bytes 0 to for for the sequence {0x7f,'E','L','F'}.
  // Exit the program with code 1 if the bytes do not match
  int ident_matches =
    ehdr->e_ident[0] == 0x7f &&
    ehdr->e_ident[1] == 'E'  &&
    ehdr->e_ident[2] == 'L'  &&
    ehdr->e_ident[3] == 'F';

    if(!ident_matches){
      printf("Magic bytes wrong, this is not an ELF file\n");
      exit(1);
    }

  // PROVIDED: check for a 64-bit file
  if(ehdr->e_ident[EI_CLASS] != ELFCLASS64){
    printf("Not a 64-bit file ELF file\n");
    return 1;
  }

  // PROVIDED: check for x86-64 architecture
  if(ehdr->e_machine != EM_X86_64){
    printf("Not an x86-64 file\n");
    return 1;
  }

  // DETERMINE THE OFFSET of the Section Header Array (e_shoff), the
  // number of sections (e_shnum), and the index of the Section Header
  // String table (e_shstrndx). These fields are from the ELF File
  // Header.
    // use values stored in Ehdr struct to set up values mentioned above
  Elf64_Off header_offset = ehdr->e_shoff;
  uint16_t sections_num = ehdr->e_shnum;
  uint16_t string_index = ehdr->e_shstrndx;

    // Set up a pointer to the array of section headers. Use the section
    // header string table index to find its byte position in the file
    // and set up a pointer to it.
  Elf64_Shdr *sec_hdrs = (Elf64_Shdr *) PTR_PLUS_BYTES(file_bytes, header_offset);
    // get the position of the string table by finding offset to it in the array
  char *sec_names = PTR_PLUS_BYTES(file_bytes, sec_hdrs[string_index].sh_offset);


  // Search the Section Header Array for the secion with name .symtab
  // (symbol table) and .strtab (string table).  Note their positions
  // in the file (sh_offset field).  Also note the size in bytes
  // (sh_size) and and the size of each entry (sh_entsize) for .symtab
  // so its number of entries can be computed.
    // values to store symbol and table offsets
  Elf32_Off symtab_off = 0, strtab_off = 0;
  uint64_t bytes = 0, entry = 0;
  for(int i=0; i<sections_num; i++){
    // put a string from a given position of string table into character array
    char *arr = PTR_PLUS_BYTES(sec_names, sec_hdrs[i].sh_name);
    // if a string has a name of symtab, make symtab_off equal to array's index offset
    // and get the number of bytes and entry of given index
    if(strcmp(arr, ".symtab") == 0){
      symtab_off = sec_hdrs[i].sh_offset;
      bytes = sec_hdrs[i].sh_size;
      entry = sec_hdrs[i].sh_entsize;
    }
    // make strtab_off to be the size of array's index offset if strtab is encountered
    if(strcmp(arr, ".strtab") == 0){
      strtab_off = sec_hdrs[i].sh_offset;
    }
  }
    // check if both values have been found
  if(!symtab_off){
    printf("Couldn't find symbol table\n");
    return 1;
  }

  if(!strtab_off){
    printf("Couldn't find string table\n");
    return 1;
  }

  // PRINT byte information about where the symbol table was found and
  // its sizes. The number of entries in the symbol table can be
  // determined by dividing its total size in bytes by the size of
  // each entry.
    // compute number of entries
  uint64_t num_of_entries = bytes / entry;
  printf("Symbol Table\n");
  printf("- %ld bytes offset from start of file\n", (uint64_t) symtab_off);
  printf("- %ld bytes total size\n", bytes);
  printf("- %ld bytes per entry\n",entry);
  printf("- %ld entries\n",num_of_entries);


  // Set up pointers to the Symbol Table and associated String Table
  // using offsets found earlier.
  Elf64_Sym *sym_ptr = PTR_PLUS_BYTES(file_bytes, symtab_off);
  char *str_ptr = PTR_PLUS_BYTES(file_bytes, strtab_off);


  // Print column IDs for info on each symbol
  printf("[%3s]  %8s %4s %s\n",
         "idx","TYPE","SIZE","NAME");

  // Iterate over the symbol table entries
  for(int i=0; i<num_of_entries; i++){
    // Determine size of symbol and name. Use <NONE> name has zero
    // length.
    uint64_t sym_size = sym_ptr[i].st_size;
    uint64_t name_num = sym_ptr[i].st_name;
    char *arr;
    if(name_num == 0){
      arr = "<NONE>";
    }
    else{
      arr = PTR_PLUS_BYTES(str_ptr, sym_ptr[i].st_name);
    }

    // Determine type of symbol. See assignment specification for
    // fields, macros, and definitions related to this.
      // get the type of given entry in the array and do checks do determine what
      // it is
    unsigned char typec = ELF64_ST_TYPE(sym_ptr[i].st_info);
    char *symb;
      // assign symb to a name string depending on which type it is
    if(typec == STT_NOTYPE){
      symb = "NOTYPE";
    }
    else if(typec == STT_OBJECT){
      symb = "OBJECT";
    }
    else if(typec == STT_FUNC){
      symb = "FUNC";
    }
    else if(typec == STT_FILE){
      symb = "FILE";
    }
    else if(typec == STT_SECTION){
      symb = "SECTION";
    }

    // Print symbol information
    printf("[%3d]: %8s %4lu %s\n", i, symb, sym_size, arr);
  }


  // Unmap file from memory and close associated file descriptor
  munmap(file_bytes, size);
  close(fd);

  return 0;
}
