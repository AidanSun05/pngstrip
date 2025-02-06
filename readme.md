# pngstrip: Simple PNG File Validation and Optimization

`pngstrip` is a program to remove unnecessary chunks from PNG files which do not affect how the images are displayed. For example, PNGs can contain time and textual metadata, and removing them can reduce file size.

`pngstrip` will check the validity of each input file before processing it. This includes validating checksums embedded in PNG files to ensure they are not corrupted.

`pngstrip` serves as a basic example of processing PNG files in C++ without any dependencies.

## Compiling

`pngstrip` uses [xmake](https://xmake.io) to build. Run the `xmake` command in the repository root.

The code uses C++23 features so a recent compiler version is recommended.

## Usage

```
pngstrip <directory> [flags]
```

`pngstrip` will operate on all `.png` files in a given directory. The program also accepts an optional set of flags, which can contain the following characters:

- `r` (Recursive): Recurses through all subdirectories of the given directory.
- `o` (Overwrite): Replaces all input images with their stripped counterparts. (If this is not set, output files from `pngstrip` will end in `-min.png` and input images will be unaffected.)

Flags can be combined, such as passing `ro` to `pngstrip`.

Overwriting input files may leave you unable to access your original PNGs. Back up important data.

### Example Command and Output

```
> .\pngstrip .\images o
".\\images\\img1.png"
Found color type: 3
Found required chunks: PLTE IHDR IDAT IEND
Found non-required chunks: gAMA sRGB

".\\images\\img2.png"
Found color type: 3
Found required chunks: PLTE IHDR IDAT IEND
Found non-required chunks: gAMA sRGB

".\\images\\img3.png"
Found color type: 3
Found required chunks: PLTE IHDR IDAT IEND
Found non-required chunks: gAMA sRGB

".\\images\\img4.png"
Found color type: 2
Found required chunks: IHDR IDAT IEND
Found non-required chunks: gAMA cHRM bKGD tIME tEXt

".\\images\\img5.png"
Found color type: 2
Found required chunks: IHDR IDAT IEND
Found non-required chunks: gAMA cHRM bKGD tIME tEXt

Original: 147174 B, Stripped: 146697 B
Saved 0.324106% (477 B)
```

## Chunk Removal

`pngstrip` removes all chunks **except**:

- `IHDR` (Image header)
- `IDAT` (Image data)
- `IEND` (Image end)
- `PLTE` (Image color palette; not always present)
- `tRNS` (Transparency; not always present)

PNGs can have different color types, changing requirements on `PLTE`. Depending on the color type, `PLTE` may be optional, required, or disallowed. `pngstrip` will check these requirements before operating on a PNG file.
