# remarkable sudoku for xovi

## TODO
- [x] proper fonts (at least smoothed)
- [ ] fix race conditions
- [x] history
- [x] actually hard puzzles
- [x] rmpp support

## Building
Requires bash for devenv source
```bash
export XOVI_REPO=/home/behemoth/source/xovi
. /opt/codex/rm2/5.2.96-dirty/environment-setup-cortexa7hf-neon-remarkable-linux-gnueabi

qmake6 && make
```
