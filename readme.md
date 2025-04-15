# Chess Engine Doc

## Overview
This is a UCI-compatible chess engine written in C++. The engine implements:
- Bitboard representation for efficient move generation
- Magic bitboards for sliding piece attacks
- Alpha-beta pruning with negamax search
- Quiescence search
- Basic evaluation function with material and positional scoring
- UCI protocol support for integration with chess GUIs

## Features

### Board Representation
- Uses 64-bit bitboards to represent pieces
- Separate bitboards for each piece type and color
- Occupancy bitboards for efficient attack generation

### Move Generation
- Legal move generation for all piece types
- Special moves: castling, en passant, promotions
- Magic bitboards for sliding pieces (rooks, bishops, queens)

### Search
- Negamax algorithm with alpha-beta pruning
- Quiescence search to avoid horizon effect
- Depth-limited search
- Basic evaluation function

### Evaluation
- Material values for each piece
- Piece-square tables for positional scoring
- Scores adjusted based on side to move

### UCI Protocol
- Supports standard UCI commands:
  - `uci` - identify engine
  - `isready` - check engine status
  - `position` - set up positions
  - `go` - start searching
  - `quit` - exit engine

## Building

The project uses Visual Studio 2022 (v143 toolset) and is configured for x64 platforms. 

Build configurations:
- Debug: For development with debugging symbols
- Release: Optimized build for production use

## Usage

1. Compile the engine
2. Connect to a UCI-compatible chess GUI (like Arena, CuteChess, etc.)
3. The engine will automatically respond to UCI commands

Alternatively, you can run it in debug mode with a fixed-depth search.

## Performance

The engine includes a perft test function to verify move generation correctness and measure performance. Example usage:
```cpp
parse_fen(start_position);
perft_test(5);  // Test to depth 5
```

## Limitations

Current limitations include:
- No opening book
- No endgame tablebases
- No advanced search features like null-move pruning or LMR
- Basic evaluation without pawn structure or king safety considerations

## Future Improvements

Potential enhancements:
- Implement more advanced search techniques
- Add more sophisticated evaluation terms
- Include opening book support
- Add endgame tablebase support
- Implement pondering
- Add time management

## Author

Marwan Elattar

This engine was developed as a learning project to understand chess programming concepts.
