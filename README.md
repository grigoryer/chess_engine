# GreenTeaTree Chess Engine

A UCI-compliant chess engine written in C++ featuring bitboard representation, magic bitboards, and advanced search algorithms. The engine achieves approximately **1700 ELO** on Lichess and demonstrates strong tactical play through iterative deepening and transposition tables.

[![Play on Lichess](https://img.shields.io/badge/Play-Lichess-blue)](https://lichess.org/@/GreenTeaTree)

## Features

### Board Representation
- **Bitboard Architecture**: Efficient 64-bit representation for all piece positions
- **Magic Bitboards**: O(1) sliding piece attack generation for rooks and bishops
- **Incremental Updates**: Zobrist hashing for position identification with minimal overhead

### Search & Evaluation
- **Negamax with Alpha-Beta Pruning**: Efficient minimax search with cutoffs
- **Iterative Deepening**: Progressive depth search with best-move tracking
- **Quiescence Search**: Reduces horizon effect by searching tactical sequences
- **Transposition Tables**: 16M+ entry hash table with bucket replacement strategy
- **Move Ordering**: 
  - MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
  - Transposition table move prioritization

### Evaluation
- **Material Counting**: Base piece values
- **Positional Evaluation**: Piece-square tables for strategic positioning
- **Game Phase Detection**: Dynamic King evaluation adjustments for endgame

## Technical Achievements

### Performance Personal Metrics
- **Search Speed**: Efficiently searches millions of nodes
- **ELO Progression**: Documented improvement through SPRT testing
  - Base engine: ~1000 ELO
  - + Iterative deepening: ~1500 ELO (+460 ELO)
  - + Position evaluation: ~1700 ELO (+200 ELO)
  - + Transposition tables: ~1800 ELO (+100 ELO)
  - + Move ordering: ~1880 ELO (+80 ELO)
  - + Quiescence search: **~2060 ELO (+180 ELO)**

### Testing & Validation
- **SPRT Framework**: Statistical testing validates each feature addition
- **Perft Testing**: Move generation validated to depth 7 with 100% accuracy
- **UCI Protocol**: Full compliance with Universal Chess Interface standard

## Architecture

### Core Components

```
├── src/
│   ├── attacks.cpp         # Attack generation & magic bitboards
│   ├── between.cpp         # Between-square calculations
│   ├── evaluation.cpp      # Position evaluation
│   ├── hash.cpp           # Zobrist hashing
│   ├── make_move.cpp      # Move execution & undo
│   ├── move_generation.cpp # Legal move generation
│   ├── position.cpp       # Board state management
│   ├── search.cpp         # Alpha-beta search & iterative deepening
│   ├── tt.cpp            # Transposition table
│   └── uci.cpp           # UCI protocol implementation
```

### Key Design Decisions

1. **Magic Bitboards**: Pre-computed attack tables for O(1) sliding piece moves
2. **Separate Search Thread**: Asynchronous search allows UCI communication during thinking
3. **Bucket-Based TT**: 4-entry buckets with depth-based replacement for better hit rates
4. **Incremental Hash Updates**: Position hashes updated during make/unmake for speed

## Building & Running

### Prerequisites
- C++20 compatible compiler (GCC 10+ or Clang 12+)
- CMake 3.10+

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/grigoryer/chess-engine.git
cd chess-engine

# Build with optimizations
mkdir build && cd build
cmake ..
make

# Run the engine
./chess_engine
```

### Compilation Flags
- `-O3`: Maximum optimization
- `-march=native`: CPU-specific optimizations for maximum performance

## Usage

### UCI Commands

```
uci                    # Initialize engine
isready               # Check if ready
ucinewgame            # Start new game
position startpos     # Set starting position
position fen [FEN]    # Set custom position
go depth [N]          # Search to depth N
go wtime [ms] btime [ms] winc [ms] binc [ms]  # Timed search
stop                  # Stop searching
quit                  # Exit engine
```

### Play on Lichess

The engine runs as a bot on Lichess:
**[Challenge TeaTreeBot](https://lichess.org/@/TeaTreeBot)**

### Integration with GUI

Compatible with any UCI-compliant chess GUI:
- **Arena Chess GUI** (Windows)
- **Cute Chess** (Cross-platform)
- **PyChess** (Linux)
- **BanksiaGUI** (Cross-platform)

## Development Roadmap

### Completed Features
- Bitboard representation
- Magic bitboard attack generation
- Alpha-beta pruning with negamax
- Iterative deepening
- Transposition tables with bucket replacement
- MVV-LVA move ordering
- Quiescence search
- Position evaluation with piece-square tables
- UCI protocol implementation
- Multi-threaded search architecture

### Planned Enhancements
- Aspiration windows for faster search
- Principal variation (PV) extraction and display
- Null move pruning
- Late move reductions (LMR)
- Opening book integration --- Already Implemented in Lichess and Testing Frameworks
- Advanced king safety evaluation
- Mobility evaluation

## Testing Methodology

### SPRT (Sequential Probability Ratio Test)
Each feature is validated through head-to-head matches against the previous version:
- **Alpha**: 0.05 (5% false positive rate)
- **Beta**: 0.05 (5% false negative rate)
- **ELO0**: 0 (null hypothesis: no improvement)
- **ELO1**: 5-10 (alternative hypothesis: minimum ELO gain)

See `improvements.txt` for detailed test results.

### Perft Validation
Move generation tested against known positions:
- All validated to 100% accuracy

## Performance Characteristics

### Search Depth
- **Midgame**: Typically reaches depth 7-9 in standard time controls
- **Endgame**: Can reach depth 10-12 with fewer pieces
- **Tactical positions**: Quiescence search extends to resolve captures


## License
This project is open source and available for educational purposes.

## Acknowledgments
- **Chess Programming Wiki**: Invaluable resource for algorithms and techniques
- **Stockfish**: Inspiration for search and evaluation approaches
- **Lichess**: Platform for bot hosting and testing
- **Fastchess SPRT Testing Framework**: Chess Statistical validation methodology

---

*Last Updated: January 2025*
