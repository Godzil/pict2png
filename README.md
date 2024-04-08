pict2png
========

Pict2png is a utility to convert old Macintosh PICT file (version 1) to PNG.

Surprisingly there is little to no utility supporting the original PICT file format dubbed version 1, and most 
documentation you can find only treat PICT format version 2.

This utility only support version 1 files, and only a subset which is most likely be the most
common PICT file you can find.

It also support both PICT files with teh 512 header or no header.
It also currently do not support most of the drawing commands from the pict file format, but
as far as I can tell, most PICT file do not use them.
Region also are not supported at the moment.

### How to build
No external dependency should be needed, to build you only need any version of cmake 3.11 or more recent.

To build:

```shell
git clone 
mkdir build
cd build
cmake ..
make -j8
```

### How to use

```shell
./pict2png -i /path/to/file.pict -o /path/to/outputfile.png
```

### Troubleshot

If you get any problem with the generated PNG file, please feel free to open a ticket here, this app has been tested
on numerous PICT file I have, I don't have existing at hand, so I may have overlooked some issues with the bitmap unpacking.

If you get any error messages about "Unknown opcode" please report that in a ticket, I'll try to add support to these opcode
as soon as possible, if you are a programmer your contribution is also welcome if you want to give it a try.


### TODO

- [ ] Add local documentation on the PICT v1 file format, especially all the missing bits and clear format documentation
- [ ] Add missing opcodes support
- [ ] Add support for non packed images
- [ ] Add support for region

### Format Documentation

You'll find some documents in the [doc](doc/) folder,

Other, you'll find information about the PICT file format in the Inside Macintosh Books:
 - Inside Macintosh I, Chapter 6
 - Inside Macintosh V, Chapter 4
 - Inside Macintosh - Imaging With QuickDraw

Some of the old books can be find in PDF form on VintageApple:
 - [Original Inside Macintosh books](https://vintageapple.org/inside_o/)
 - [Newer Inside Macintosh books](https://vintageapple.org/inside_r/)

The original QuickDraw source code has also been looked in to understand some of the behaviour of the original code and
undocumented behaviour. You can find the code on the [Computer History museum website](https://computerhistory.org/blog/macpaint-and-quickdraw-source-code/)