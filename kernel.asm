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

global test_ints
test_ints:
    mov eax,0
    mov ebx,'d'
    int 128
    mov ebx,10
    int 128
    mov esi,test_string
    mov eax,1
    int 128
    ret

test_string: db 'ayo what da dog doin',0