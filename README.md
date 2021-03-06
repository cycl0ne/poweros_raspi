Repo for the new multiplatform: https://github.com/cycl0ne/poweros_x86

History:

18.06.2015: Update new V0.2 from our working branch

* Dos.library
* aux: device
* ext2.handler Read/Write
* first shell
* loading of files from disk
* booting into shell with HD0: mounted
* first ram.handler

------------------------

---> At the moment the focus is back on the x86 plattform, since im on a big business trip and have no raspberry pi development kit with me (too much to take with me).
I will merge the x86 back to the raspberry pi, or create a new multiplatform version.

Summary:
========

This directory contains the source code for a experimental OS called PowerOS,
which I ported to the Raspberry PI as an educational OS. 
It is partly inspired by the good old AmigaOS, but it is not compatible. Some
Functions are the same others dont. So dont rely on it.

Status:
=======

At the moment the exec.library (80% complete) there, but still not finished. Semaphores are missing
and the allocmem routines dont make me happy. There is also a first version of the
timer.device (80% complete). One of the cool functions in timer.device missing is the 
WAITFOR_UNITHZ.
I will try to add more and more features to the next releases. At the moment im sitting
on USB support, SDHCI support. After that, I will try to get the old PowerDOS library to work
with the nil: ram: and fastfiling system handlers. Perhaps console.device will be somewhere in
between.
Last libraries/devices which will be consequently added are:
- intuition
- input
- keyboard
- mouse
- serial
- graphics
- bootmenu
- audio

Directory Hierarchy:
====================

/src
	/raspi -> all raspi related stuff, but its mixed with exec, so in future this directory will perhaps vanish and all will go to exec.
	/kickstart -> all "ROM" Files will be stored here
		/lib.exec -> Exec.library, heart of the OS, with Messages, Tasks,...
		/dev.timer -> timer.device, with this, you can ask for time, create stopwatches and get accurate time through e_time function (1.000.000hz)
		
System Initialization:
----------------------

The Raspberry Pi boots up a VideoCore, after that, it loads a kernel.img from SDCard and executes it on adress 0x8000. This is all accomplished in
the platform.c in the raspi directory

Thanks:
----------------------
- Raspberry Pi Forum, where you can find me (in the Baremetal Section)
- Dom, for all those questions I had
- tufty, for his LamdaPi (https://gitorious.org/lambdapi)
- dwelch67, for his bootloader #4 (https://github.com/dwelch67/raspberrypi/)
- Gonzo, for his ports of things to u-boot (https://github.com/gonzoua/u-boot-pi/tree/rpi)
- all other I forgot. (i will update this part of the readme :) )

License:
========
While chekcing which license fits best, take this as a note:
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 - The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
