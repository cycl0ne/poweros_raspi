/* Copyright 1995, 1996, 1998, 1999, 2000, 2003, 2004, 2005
   Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

.globl __lshrdi3
.globl __aeabi_llsr
__lshrdi3:
__aeabi_llsr:

	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	r0, r0, lsr r2
	movpl	r0, r1, lsr r3
	orrmi	r0, r0, r1, lsl ip
	mov	r1, r1, lsr r2
	mov	pc, lr
