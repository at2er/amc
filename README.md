# Atom(Yuanzi) Programming Language Complier
Implementation on C.

# Build
> Unsupport `Windows`!

## Dependencies
> [lib](lib) for more.

* [libgetarg](https://github.com/at2er/libgetarg)
* [libsclexer](https://github.com/at2er/libsclexer)
* [libsctrie](https://github.com/at2er/libsctrie)
* [libmcb](https://github.com/at2er/libmcb)

## Build 'amc'
> Try read [Makefile](Makefile) to learn more build options.

### Normal version
```sh
make
```

# Others
## Unsupport 32-bit!
The file output by the compiler dosen't support 32-bit devices.

# License
See [LICENSE](LICENSE).
But some module use different license, e.g.
- [test](test/LICENSE)
- [utils](utils/LICENSE).
And the build system use MIT License!
