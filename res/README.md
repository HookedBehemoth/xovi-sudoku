# Puzzles

These puzzles are generated with some javascript library [gen.js](gen.js) and then compressed with the attached python script [compress.py](compress.py).

Overkill but whatever.

imhex pattern file is attached (sudoku.pat).

## Header
| Offset | Type             |
| ------ | ---------------- |
| 0x00   | MAGIC 'SUDOKU00' |
| 0x04   | u32 count        |

## Puzzle
4 bits per number so 40.5 bytes, padded to 41.
1 bit per mask bit so 10.125, padded to 11.

# Fonts
Fonts are from the Relief-SingleLine Project
https://github.com/isdat-type/Relief-SingleLine

License is attached as [digits-OFL.txt](digits-OFL.txt)

If you'd like to add your own font, you can use the attached Script [digits.py](digits.py) to convert lines to points.
I've used InkScape "Extensions" > "Modify Path" > "Approximate Curves by Straight Lines" with a Flatness of 1.
