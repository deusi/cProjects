* name: Denis Rybkin
* x500: rybki001 */

PURPOSE:
  The purpose of this program is to implement an algorithm that translates virtual
addresses to physical addresses using two-level page tables (PTs) and a TLB (Translation
Lookaside Buffer) that implements the "First In First Out" policy for page replacement.

HOW TO COMPILE:
  Typing "make" in the src directory of the project will compile all required files for the
project and the executable vmanager will be created.

HOW TO EXECUTE:
  The project can be executed by typing ./vmanager in the terminal. By default, the program
runs virtual memory addresses specified in INPUT_FILE in FIFO policy. In order to implement
least recently used policy, type ./vmanager -lru instead.

WHAT OUR PROGRAM DOES:
  General purpose of this program is to translate virtual addresses into physical addresses by
using two page tables and tlb. After that, the program prints out corresponding physical
addresses of each virtual address as well as the hit ratio and tlb contents (inside tlb_out.txt).
  Within the vmemory .c/.h are functions to implement a tlb with lru functionality. init and
replace lru are for managing and maintaining the usage of the lru queue in the best manor. lru
hit manages to record how many times a tlb entry hits the given item. enqueue and dequeue allow
for management of the tlb for adding elements. translate virtual address allows for the virtual
address to be looked up in the tlb and to see if it needs to be looked up in physical memory
get tlb entry returns the physical address from the tlb. populate tlb adds a new virtual physical
address pair into the tlb and if it is full removes the last recently added value. get hit ratio
returns the ratio of tlb hits over the total number of requests. print tlb prints the virtual and
physical adress pairs.

ASSUMPTIONS:
  The program reads input from "../bin/virtual.txt", so all virtual addresses should be placed
there. In addition, virtual.txt should contain only one virtual address in hexadecimal value
per line. Current implementation doesn't allow to read memory addresses from any random .txt
file and only accepts hexadecimal numbers (in a form 0x12345678, 8 numbers max), one per line.
