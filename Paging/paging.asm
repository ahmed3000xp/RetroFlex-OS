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

bits 32

section .bss
align 4096
page_directory:
    resd 1024
page_table:
    resd 1024

section .text

global enable_paging
enable_paging:
    pusha
    xor eax, eax
    mov ecx, 1024
    lea edi, [page_directory]
    rep stosd

    ; Map the first 4 MB of memory (identity map)
    ; Clear the page table
    lea edi, [page_table]
    rep stosd

    ; Fill page table entries (identity map 4MB)
    mov ebx, 0x00000000
    mov ecx, 1024
    lea edi, [page_table]
.map_pages:
    mov eax, ebx
    or eax, 0x3                ; Present and Read/Write flags
    stosd
    add ebx, 4096              ; Move to the next 4 KB page
    loop .map_pages

    ; Point the first page directory entry to the page table
    lea eax, [page_table]
    or eax, 0x3                ; Present and Read/Write flags
    mov [page_directory], eax

    lea eax, [page_directory]
    mov cr3, eax                 

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    popa
    ret