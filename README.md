My kid has to write a MIPS32 compiler + VM for his semester project. I'm writing this one in parallel, because why:
1. It was a PDP11 when I was in CS.
2. I didn't know how to code back then.
3. We didn't write a VM back then, just the assembler.
4. Never saw an assembler before that was based on RISC and had assembly instructions that devolved into multiple RISC ops.
5. It's fun.

So here it is. Only a little bit hacky. I particular like the VM.

PS: It's not really functional yet. It sort of compiles, but this isn't working yet:
1. labels
2. offsets
3. jump or branch or any of that
4. floating point
5. delayed slots
6. lots of other stuff.
7. 
