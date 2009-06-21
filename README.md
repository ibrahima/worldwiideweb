# WorldWiideWeb Wii HTTP Sample

The Wii HTTP Sample is a simple program which downloads a file.
The purpose of the program is to be a pedagogical tool, since I found that
there were no good tutorials on network programming on the Wii.
However, libogc uses a Berkely sockets-style socket API, so a tutorial on
BSD sockets can be used relatively easily for the Wii.

### Where can I learn more?

I've found that [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/) works fairly well, although you have to be careful to match up functions
in libogc properly. There are some newer functions used in the tutorial which
are not available in libogc. However, you can probably figure out which ones
those are, and if all else fails, read the sample again.