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

global enable_paging
enable_paging:
    ; Load the page directory base address into CR3
    mov eax, [esp+4]
    mov cr3, eax

    ; Enable paging by setting the paging bit (bit 31) in CR0
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; Flush the instruction cache (optional but recommended)
    mov eax, 0x20F
    mov cr4, eax

    ret