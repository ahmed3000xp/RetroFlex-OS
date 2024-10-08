; Copyright (C) 2024 Ahmed
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.

BITS 32
section .text
    align 4
    DD 0x1BADB002
    DD 0x00000003
    DD -(0x1BADB002 + 0x00000003)

global start
extern main

start:
    cli ; Disable interrupts
    mov esp, stack_space ; Setup the Stack
    push ebx ; pointer to the Multiboot header
    push eax ; Magic Number
    call main ; call the kernel         
halt_kernel:
    cli
    hlt
    jmp halt_kernel

section .bss
    resb 65536             
stack_space: