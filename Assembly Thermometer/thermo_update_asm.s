  ####################################################################
  ############ Assembly implementation of thermo_update.c ############
  ######################### by Denis Rybkin ##########################
  ####################################################################

.text
.global  set_temp_from_ports

## ENTRY POINT FOR REQUIRED FUNCTION
set_temp_from_ports:

               ### tried 16-bit, but it didn't work ###
        ### decided to stick with my original implementation ###
 ### have to use 32-bit due to possibility of overflow near max values ###

## takes input from global variable, checks it and decides whether to increment
      movzwl THERMO_SENSOR_PORT(%rip), %edx  # load global variable into register
      cmpl $64000, %edx   # compare if thermo port above max value
      jg .ERROR           # jump to error if it is
      movw %dx, %cx       # store a copy of a number in a short
      sarl $6, %edx       # divide temperature by 64 (was told to do it this way)
## at this point, switched to 16-bit, since value is guaranteed to be
## within range of 16-bit values
      andw $0b100000, %cx # get the remainder by bitwise & and store it in cx
      cmpw $31, %cx       # check if value needs incrementing
      jle .TO_CELSIUS     # jump to convert part if %cx - 31 <= 0
      addw $1, %dx       # increment by one
## convert to Celsius by default
  .TO_CELSIUS:
      subw $500, %dx     # subtract 50 degrees from temperature
      movb  THERMO_STATUS_PORT(%rip), %cl   # copy global var to reg cl
      cmpb $0, %cl        # compare status port to 0
      je .UPDATE_STRUC    # if it's 0, jump to assign part
## convert to Fahrenheit
  .TO_FAHRENHEIT:
      imulw $9, %dx       # multiply celsius by 9
      movw %dx, %ax       # move dx to ax, since dx used in multiplication
      movw $5, %cx        # assign cx to be 5
      cwtl                # sign extend ax to long word
      cltq                # sign extend eax to quad word
      cqto                # sign extend ax to dx
      idivw %cx           # divide by 5
      movw %ax, %dx       # move result ro dx
      addw $320, %dx      # add 320 to the result
## assign computed values to struc pointers
  .UPDATE_STRUC:
      movw %dx, 0(%rdi)   # move temperature to temp->tenths_degrees
      movw THERMO_STATUS_PORT(%rip), %ax  # move status to register
      movb %al, 2(%rdi)   # move status port to temp->is_fahrenheit
      jmp .RETURN         # jump to return (implemented below)

### Data area associated with the next function
.data

array:                    # array of masks for each value in thermometer
      .int 0b0111111      # mask[1] represents 1
      .int 0b0000110      # mask[2] represents 2
      .int 0b1011011      # mask[3] represents 3
      .int 0b1001111      # and so on
      .int 0b1100110
      .int 0b1101101
      .int 0b1111101
      .int 0b0000111
      .int 0b1111111
      .int 0b1101111
      .int 0b0000000      # represents empty display
      .int 0b1000000      # represents negative sign
## removed const int due to not using it

.text
.global  set_display_from_temp

## ENTRY POINT FOR REQUIRED FUNCTION
set_display_from_temp:

### WARNING: Due to limited number of registers and the nature of assembly,  ###
### it was impossible to replicate C code to the minor detail. While I've   ###
### been trying to mimic my C implementation as much as possible, the       ###
### assembly code looks little alike its C version.                         ###

      movl %edi, %ecx     # assign struc to register ecx
      shrl $16, %ecx      # shift struc to access is_fahrenheit
      cmpb $0, %cl        # check if temperature set to C
      je .CELSIUS_CHECK   # jump to Celsius comparisons if equal to 0
      cmpb $1, %cl        # check if temperature not set to F
      je .FAHRENHEIT_CHECK# jump to Fahrenheit comparisons if equal to 1
      jmp .ERROR          # jump to error if none of the cases above
  .CELSIUS_CHECK:
      cmpw $500, %di      # check if short tenths_degrees > 500
      jg .ERROR           # jump to error if greater
      cmpw $-500, %di     # check if short tenths_degrees < -500
      jl .ERROR           # jump to error if less
      jmp .IS_NEGATIVE    # skip Fahrenheit part and jump to masking
  .FAHRENHEIT_CHECK:
      cmpw $1220, %di     # check if short tenths_degrees > 1220
      jg .ERROR           # jump to error if greater
      cmpw $-580, %di     # check if short tenths_degrees < -580
      jl .ERROR           # jump to error if less
  .IS_NEGATIVE:
      movw %di, %ax       # move tenths_degrees to register (for future division)
      movb $0, %r9b       # set negative flag to 0
      cmpw $0, %ax        # check if negative
      jge .SET_C_OR_F     # jump to next section if positive
      negw %ax            # negate tenths_degrees otherwise
      movb $1, %r9b       # set negative flag to 1
  .SET_C_OR_F:
      movl %edi, %ecx     # move struc to register
      shr $16, %ecx       # access is_fahrenheit value
      addl $1, %ecx       # add one to it (since C - 0b01, F - 0b10)
      sall $28, %ecx      # logical shift to place F or C on its place
      movq $0, (%rsi)     # initialize value to start with 0
      orl %ecx, (%rsi)    # set the bites to display C or F
  .DIVIDE_BY_1000:
      movw $1000, %cx     # move 1000 to "deviser" register
      cwtl                # sign extend ax to long word
      cltq                # sign extend eax to quad word
      cqto                # sign extend ax to dx
      idivw %cx           # divide input by 1000
  .SET_THOUSAND:
      leaq array(%rip), %r8 # r8 points to array, rip used to enable relocation
      movl (%r8, %rax, 4), %ecx # use result to move binary mask to ecx
      cmpl $0, %eax       # compare result to 0
      je .THOUSAND_ZERO   # if equal, jump to additional conditions
      sall $21, %ecx      # shift left by 21 bits
      orl %ecx, (%rsi)    # add bits to the display
      jmp .DIVIDE_BY_100  # jump to hundreds
  .THOUSAND_ZERO:
      ### thousand value set to be empty by default ###
      cmpb $1, %r9b       # check if flag indicated negative
      jne .DIVIDE_BY_100  # jump to hundreds if not
      movl 44(%r8), %ecx  # move negative sign to register
      sall $21, %ecx      # shift to proper place
      orl %ecx, (%rsi)    # apply negative sign to display
  .DIVIDE_BY_100:
      movw %dx, %ax       # move remainder to numerator
      movw $100, %cx      # move 100 to cx
      cwtl                # sign extend ax to long word
      cltq                # sign extend eax to quad word
      cqto                # sign extend ax to dx
      idivw %cx           # divide by 100
  .SET_HUNDRED:
      movl (%r8, %rax, 4), %ecx # move bin. mask of result to ecx
      cmpl $0, %eax       # check if result equal to 0
      je .HUNDRED_ZERO    # jump to additional cases if it is
      sall $14, %ecx      # shift left by 14
      orl %ecx, (%rsi)    # apply mask
      jmp .DIVIDE_BY_10   # jump to tens
  .HUNDRED_ZERO:
      cmpb $1, %r9b       # check if flag is negative
      jne .DIVIDE_BY_10   # jump to tens if not
      movl 44(%r8), %ecx  # set ecx to negative mask
      sall $21, %ecx      # shift appropriately
      xorl %ecx, (%rsi)   # remove negative sign from thousands
      shrl $7, %ecx       # shift right by 7
      orl %ecx, (%rsi)    # apply negative sign to hundreds
  .DIVIDE_BY_10:
      movw %dx, %ax       # move remainder to numerator
      movw $10, %cx       # move 10 to cx
      cwtl                # sign extend ax to long word
      cltq                # sign extend eax to quad word
      cqto                # sign extend ax to dx
      idivw %cx           # divide by 10
  .SET_TEN:
      movl (%r8, %rax, 4), %ecx   # get associated binary string
      sall $7, %ecx       # shift appropriately
      orl %ecx, (%rsi)    # set the bits
  .SET_ONE:
      movw %dx, %ax       # move remainder to numerator
      movl (%r8, %rax, 4), %ecx  # get associated binary string
      orl %ecx, (%rsi)    # apply the string
      jmp .RETURN         # jump to return (implemented below)

.text
.global thermo_update

## ENTRY POINT FOR REQUIRED FUNCTION
thermo_update:

### There's more efficient way of doing it with add and sub of rsp, but     ###
### since example code featured push and pop, I decided to stick with this  ###
### version.                                                                ###

  .SET_PORTS_PREP:
      pushq %rdx          # push any 64-bit register onto the stack
      pushq $0            # push 0 to the stack
      movq %rsp, %rdi     # store pointer to 0 in rdi
      call set_temp_from_ports  # call function using rdi as an empty struc
      cmpl $0, %eax       # check if return value indicates error
      jne .ERROR_PORTS    # jump to error case
  .SET_DISPLAY_PREP:
      movq (%rsp), %rdi   # move struc values to rdi
      pushq $0            # push 0
      movq %rsp, %rsi     # store pointer to 0 in rsi
      call set_display_from_temp  # call function with struc and pointer to 0
      cmpl $0, %eax       # check for errors
      jne .ERROR_DISPLAY  # jump to error case
  .UPDATE_DISPLAY:
      movl (%rsi), %edx   # move updated value to int register
      movl %edx, THERMO_DISPLAY_PORT(%rip)  # set display to be a new value
## could have popped different registers, but I doesn't matter for restoring stack
      popq %rdx           # pop the last stored element in stack
      popq %rdx           # pop the next one
      popq %rdx           # restore original state
      jmp .RETURN         # jump to return (implemented below)

##### Bunch of general statement used by different functions #####

  .RETURN:
      movl $0, %eax       # move 0 to return value
      ret                 # return 0

  .ERROR_DISPLAY:
      popq %rdx           # pop three times if error occurs after display
  .ERROR_PORTS:
      popq %rdx           # pop two times if after ports
      popq %rdx
  .ERROR:
      movl $1, %eax       # set return value to 1
      ret                 # return 1
