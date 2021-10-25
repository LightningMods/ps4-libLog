# libLog

to Compile this PRX correctly you must replace your link.x with this one (from OO) here
`$OO_PS4_TOOLCHAIN/link.x`

[Fix building libraries](https://github.com/sleirsgoevy/OpenOrbis-PS4-Toolchain/commit/f1cda6002d8e3af6f530cfaa5929c5d8b8167b0d)

ALT DL: [Discord Download (Linux)](https://discordapp.com/channels/296133488111779841/463854616573116416/902147728056922122)

AND
Replace `$OO_PS4_TOOLCHAIN/bin/linux/create-lib` and `$OO_PS4_TOOLCHAIN/bin/linux/create-eboot` with these

[Fix create-eboot generating non-contiguous relro and data segments #112 create-eboot/create-lib #45](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/actions/runs/503278524)

ALT DL: [Discord Download (Linux)](https://discordapp.com/channels/296133488111779841/463854616573116416/902147641394229308)

## TODO

- [ ] Double check flawfinder hits
- [ ] Rigorous testing
  - [ ] PC
  - [ ] PS4
- [ ] Remove non-PS4 stuff, and split the library header and stub header accordingly
- [ ] Choose a permanent new name/naming
- [ ] Document everything
- [ ] Release
