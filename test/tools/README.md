# test/tools

## test.sh
Auto compile and use test scripts to check compiler working.

### test-units-auto
`test.sh` will call these(`test-<file>.yz.sh`) scripts automatically.
> Match the `test/<name>.yz`

#### args
- $1: src file.
- $2: executable file(compiler output).

### test-units
`test.sh` will call all scripts(`<name>.sh`) in this directory automatically.

#### args
- $1: compiler path.
- $2: "-a"
