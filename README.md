# pragma

A WIP emulator for the Nintendo Switch 2 written in C++.

Join the **pragma Discord Server!**

## Current Features

* ARM64 CPU emulation
* 8-core CPU structure
* 12 GB of dynamically allocated guest memory
* GPU foundation
* Virtual filesystem (VFS)
* `.xci` / `.nso` file detection

> **Current limitations:** pragma cannot currently run commercial games or firmware. GPU emulation is currently non-functional. There is currently no publicly available homebrew software for the Nintendo Switch 2.

## Current Status

pragma is still **WIP**. Development is currently focused primarily on the GPU subsystem.

Additional libraries and components may be added as development continues.

## What We're Working On

* SM86 → Vulkan / OpenGL / Direct3D
* Joy-Con 2 driver
* Audio support for `.xci` / `.nso` content
* Firmware support

## Contributing

pragma is currently **not accepting direct contributions**.

You can still support the project by:

*  Starring the repository
*  Joining the pragma Discord server
*  Forking the repository
*  Sharing the project with other developers

When contributions are opened, contribution guidelines will be added here.

## Note

pragma contains **NO firmware, keys, or games**.

It will not be distributed with firmware, keys, or games, and it is **not affiliated with Nintendo**.

If you have legally obtained compatible content, place it in the `Games/` directory as appropriate.

## License

pragma uses third-party libraries and recompilers.

**Dynarmic (MIT License):**
Copyright (C) 2017 merryhime <git@mary.rs>

Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

See the LICENSE for More information 
