import json
import struct
import sys
from typing import List, Dict


def compress_sudoku(puzzle_str: str, solution_str: str) -> bytes:
    """
    Compress a sudoku puzzle into binary format.
    
    Format:
    - 41 bytes for solution values (81 cells, 2 per byte, 4 bits each)
    - 11 bytes for mask (81 bits indicating which cells are given in the puzzle)
    
    Total: 52 bytes per puzzle
    
    The puzzle can be reconstructed by applying the mask to the solution.
    """
    # Build the mask (81 bits) and solution values
    mask_bits = []
    solution_values = []
    
    for i in range(81):
        puzzle_char = puzzle_str[i]
        solution_char = solution_str[i]
        
        # Mask bit: 1 if given (not '-'), 0 if empty
        mask_bits.append(1 if puzzle_char != '-' else 0)
        solution_values.append(int(solution_char))
    
    # Pack solution values (2 per byte, 4 bits each)
    solution_bytes = bytearray(41)
    for i in range(0, 81, 2):
        high_nibble = solution_values[i]
        low_nibble = solution_values[i + 1] if i + 1 < 81 else 0
        solution_bytes[i // 2] = (high_nibble << 4) | low_nibble
    
    # Convert mask bits to bytes (need 11 bytes for 81 bits)
    mask_bytes = bytearray(11)
    for i, bit in enumerate(mask_bits):
        if bit:
            byte_idx = i // 8
            bit_idx = i % 8
            mask_bytes[byte_idx] |= (1 << bit_idx)
    
    return bytes(solution_bytes + mask_bytes)


def decompress_sudoku(data: bytes) -> Dict[str, str]:
    """
    Decompress a sudoku puzzle from binary format.
    
    Returns a dict with 'puzzle' and 'solution' keys.
    """
    if len(data) != 52:
        raise ValueError(f"Invalid data length: {len(data)}, expected 52 bytes")
    
    # Extract components
    solution_bytes = data[0:41]
    mask_bytes = data[41:52]
    
    # Extract solution values
    solution_values = []
    for byte_val in solution_bytes:
        high_nibble = (byte_val >> 4) & 0xF
        low_nibble = byte_val & 0xF
        solution_values.append(high_nibble)
        solution_values.append(low_nibble)
    solution_values = solution_values[:81]
    
    # Extract mask bits
    mask_bits = []
    for byte_val in mask_bytes:
        for bit_idx in range(8):
            mask_bits.append((byte_val >> bit_idx) & 1)
    mask_bits = mask_bits[:81]  # Only need 81 bits
    
    # Build puzzle string using mask over solution
    puzzle_str = ''
    for i in range(81):
        if mask_bits[i]:
            puzzle_str += str(solution_values[i])
        else:
            puzzle_str += '-'
    
    # Build solution string
    solution_str = ''.join(str(v) for v in solution_values)
    
    return {'puzzle': puzzle_str, 'solution': solution_str}


def compress_json_file(input_file: str, output_file: str):
    """
    Compress a JSON file containing sudoku puzzles into binary format.
    """
    with open(input_file, 'r') as f:
        puzzles = json.load(f)
    
    with open(output_file, 'wb') as f:
        # Write header: magic bytes "SUDOKU00" + number of puzzles (4 bytes)
        f.write(b'SUDOKU00')
        f.write(struct.pack('<I', len(puzzles)))
        
        # Write compressed puzzles
        for puzzle in puzzles:
            compressed = compress_sudoku(puzzle['puzzle'], puzzle['solution'])
            f.write(compressed)
    
    original_size = len(json.dumps(puzzles))
    compressed_size = 12 + len(puzzles) * 52
    ratio = compressed_size / original_size * 100
    
    print(f"Compressed {len(puzzles)} puzzles")
    print(f"Original size: {original_size:,} bytes")
    print(f"Compressed size: {compressed_size:,} bytes")
    print(f"Compression ratio: {ratio:.1f}%")


def decompress_binary_file(input_file: str, output_file: str):
    """
    Decompress a binary file back into JSON format.
    """
    with open(input_file, 'rb') as f:
        # Read and validate header
        magic = f.read(8)
        if magic != b'SUDOKU00':
            raise ValueError(f"Invalid file format. Expected 'SUDOKU00', got {magic}")
        
        count_bytes = f.read(4)
        count = struct.unpack('<I', count_bytes)[0]
        
        # Read puzzles
        puzzles = []
        for _ in range(count):
            compressed = f.read(52)
            if len(compressed) != 52:
                raise ValueError("Unexpected end of file")
            puzzle = decompress_sudoku(compressed)
            puzzles.append(puzzle)
    
    with open(output_file, 'w') as f:
        json.dump(puzzles, f, indent=2)
    
    print(f"Decompressed {len(puzzles)} puzzles to {output_file}")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:")
        print("  Compress:   python compress.py compress <input.json> <output.bin>")
        print("  Decompress: python compress.py decompress <input.bin> <output.json>")
        print("  Test:       python compress.py test <input.json>")
        sys.exit(1)
    
    command = sys.argv[1]
    
    if command == 'compress':
        if len(sys.argv) != 4:
            print("Usage: python compress.py compress <input.json> <output.bin>")
            sys.exit(1)
        compress_json_file(sys.argv[2], sys.argv[3])
    
    elif command == 'decompress':
        if len(sys.argv) != 4:
            print("Usage: python compress.py decompress <input.bin> <output.json>")
            sys.exit(1)
        decompress_binary_file(sys.argv[2], sys.argv[3])
    
    elif command == 'test':
        if len(sys.argv) != 3:
            print("Usage: python compress.py test <input.json>")
            sys.exit(1)
        
        # Test compression and decompression
        with open(sys.argv[2], 'r') as f:
            puzzles = json.load(f)
        
        print(f"Testing {len(puzzles)} puzzles...")
        for i, puzzle in enumerate(puzzles):
            compressed = compress_sudoku(puzzle['puzzle'], puzzle['solution'])
            decompressed = decompress_sudoku(compressed)
            
            if decompressed['puzzle'] != puzzle['puzzle']:
                print(f"ERROR: Puzzle {i} puzzle mismatch!")
                print(f"Original:  {puzzle['puzzle']}")
                print(f"Restored:  {decompressed['puzzle']}")
                sys.exit(1)
            
            if decompressed['solution'] != puzzle['solution']:
                print(f"ERROR: Puzzle {i} solution mismatch!")
                print(f"Original:  {puzzle['solution']}")
                print(f"Restored:  {decompressed['solution']}")
                sys.exit(1)
        
        print(f"✓ All {len(puzzles)} puzzles passed compression/decompression test!")
        
        # Show compression stats
        original_size = len(json.dumps(puzzles))
        compressed_size = 12 + len(puzzles) * 52
        ratio = compressed_size / original_size * 100
        
        print(f"\nCompression stats:")
        print(f"  Original size: {original_size:,} bytes")
        print(f"  Compressed size: {compressed_size:,} bytes")
        print(f"  Compression ratio: {ratio:.1f}%")
    
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)
