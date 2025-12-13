# Puzzles

These puzzles are generated with some javascript library and then compressed with the attached python script.

Overkill but whatever.

imhex pattern file is attached.

## Header
| Offset | Type             |
| ------ | ---------------- |
| 0x00   | MAGIC 'SUDOKU00' |
| 0x04   | u32 count        |

## Puzzle
4 bits per number so 40.5 bytes, padded to 41.
1 bit per mask bit so 10.125, padded to 11.
