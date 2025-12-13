import { getSudoku } from 'sudoku-gen';
import { writeFileSync } from 'fs';

function gen(difficulty, count) {
    let puzzles = [];
    for (let i = 0; i < count; i++) {
        let puzzle = getSudoku(difficulty);
        puzzles.push(puzzle);
    }

    writeFileSync(`${difficulty}.json`, JSON.stringify(puzzles, null, 2));
}

gen('easy', 1000);
gen('medium', 1000);
gen('hard', 1000);
gen('expert', 1000);
