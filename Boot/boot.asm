; Copyright (C) 2024 The RetroFlex OS Project
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
extern kmain

start:
    cli
    mov esp, stack_space   
    push ebx
    push eax
    call kmain             
halt_kernel:
    cli
    hlt
    jmp halt_kernel

section .bss
    resb 8192              
stack_space:
