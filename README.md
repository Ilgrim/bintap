# bintap

Binary files to ZX Spectrum *.tap* tape file converter to be used in ZX Spectrum emulating software.

## License

Public domain (see [LICENSE](LICENSE) file).

## Authors

See [AUTHORS](AUTHORS) file.

## Compile

```sh
cd src
make
```

Clean: 

```sh
make clean
```

## Install

```sh
cp bintap /usr/local/bin
```

## Usage

```
Usage: bintap [OPTIONS] INPUT_FILE

Options:
  -h, --help                            show this help and exit.
      --version                         show version and exit.
  -p, --program                         make `Program' instead of `Bytes'.
  -t TITLE, --title TITLE               set header name for all blocks.
  -s LINE, --start-line LINE            BASIC start line for program.
  -o FILENAME, --output FILENAME        set output filename.
      --auto-name                       make output filename from input.
  -a, --append                          append tape at end of file.
  -l ADDRESS, --load-address ADDRESS    load address of a binary file.
  -x ADDRESS, --extra-address ADDRESS   extra address of a binary file.

BASIC loader options:
  -b, --basic                           include BASIC loader.
  -d, --d80                             create D80 syntax loader.
  -c ADDRESS, --clear-address ADDRESS   set clear address.
  -e ADDRESS, --exec-address ADDRESS    set code start address.
      --bc COLOR, --border-color COLOR  set border color.
      --pc COLOR, --paper-color COLOR   set paper color.
      --ic COLOR, --ink-color COLOR     set ink color.
      --nph, --no-print-headers         hide header title when loading.

Maximum supported input file size is 49152 bytes.
Maximum `TITLE' length is 10.
`LINE' is a number in range [0; 9999].
`ADDRESS' is a number in range [0; 65535].
`COLOR' is a number in range [0; 7].
All numbers are decimal or hexadecimal (prefixed with `0x' or `0X').
```

## Links

* [GNU Operating System](https://www.gnu.org/)

* [GNU Core Utilities](https://www.gnu.org/software/coreutils/) ([package](https://pkgs.org/download/coreutils))

* [GNU Make](https://www.gnu.org/software/make/) - utility for directing compilation ([package](https://pkgs.org/download/make))

* [GNU Compiler Collection](https://www.gnu.org/software/gcc/) ([package](https://pkgs.org/download/gcc))

* [Description of TAP file format on faqwiki.zxnet.co.uk](https://faqwiki.zxnet.co.uk/wiki/TAP_format)

* [ZX-Spectrum utilities](https://zxspectrumutils.sourceforge.io/) - ZX-Spectrum emulators format utilities.
