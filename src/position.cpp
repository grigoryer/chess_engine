#include "attacks.hpp"
#include "constants.hpp"
#include <position.hpp>
#include <debug.hpp>
#include <sstream>



void Board::fenParser(const std::string& fen)
{
    //reset all board to empty;
    for (auto& bb : pieceBB) bb = 0ULL;
    for (auto& bb : sideBB) bb = 0ULL;
    occupancy = 0ULL;

    constexpr int fen_position = 0;
    constexpr int fen_color = 1;
    constexpr int fen_castling = 2;
    constexpr int fen_en_passant = 3;
    constexpr int fen_half_move = 4;
    constexpr int fen_full_move = 5;


    std::array<std::string, 6> parts;
    std::istringstream iss(fen);
    std::string word;

    int i = 0;

    while (iss >> word ) {
        parts[i++] = word;
    }

    //split up string into parts
    int rank = 0; 
    int file = 0;

    std::string position = parts[fen_position];
    std::string color = parts[fen_color];
    std::string castling = parts[fen_castling];
    std::string en_passant = parts[fen_en_passant];
    std::string half_move = parts[fen_half_move];
    std::string move = parts[fen_full_move];

    // handle the first position part of the fen string and add bits where specified.
    for(int i = 0; i < position.length(); i++)
    {
        char ch = position[i];

        if(ch == '/')
        {
            rank++;
            file = 0;
        }
        else if (std::isdigit(ch) != 0) 
        {
            file += (ch - '0');
        }
        else
        {
            Side isWhite = isupper(ch) ? WHITE : BLACK;
            int sq = ((RANK_8 - rank) * NUM_RANKS) + file;

            isWhite == WHITE ?  setBit(sideBB[WHITE],sq) : setBit(sideBB[BLACK],sq);

            ch = (char)tolower(ch);

            switch(ch)
            {
                case 'k': setBit(pieceBB[KING], sq); break;
                case 'q': setBit(pieceBB[QUEEN], sq); break;
                case 'r': setBit(pieceBB[ROOK], sq); break;
                case 'b': setBit(pieceBB[BISHOP], sq); break;
                case 'n': setBit(pieceBB[KNIGHT], sq); break;
                case 'p': setBit(pieceBB[PAWN], sq); break;
            }

            file++;   
        }
    }

    curSide = (color == "w" ) ?  WHITE : BLACK;

    curState.castlingRights = NO_CASTLING;
    
    for(int i = 0; i < castling.length(); i++)
    {
        char ch = castling[i];
        switch(ch) 
        {
            case 'K': curState.castlingRights |= WK; break;
            case 'Q': curState.castlingRights |= WQ; break;
            case 'k': curState.castlingRights |= BK; break;
            case 'q': curState.castlingRights |= BQ; break;
            case '-': break;
        }
    }

    //parse en passant if any.
    if(en_passant != "-")
    {
        int file = en_passant[0] - 'a';
        int rank = en_passant[1] - '0';

        if(rank == RANK_4) 
        {
            curState.epSq = static_cast<EpSquare>(file); // 0-7
        }
        else if(rank == RANK_7) 
        {
            curState.epSq = static_cast<EpSquare>(NUM_FILES + file); 
        }
    }
    else
    {
        curState.epSq = EP_NONE; //none value
    }

    /*if(half_move == "-" || parts[fen_half_move] == ""){ state->half_move = 0; }
    else { state->half_move = std::stoi(half_move); }

    if(move == "-" || parts[fen_full_move] == "" ){ state->full_move = 0; }
    else{ state->full_move = std::stoi(move); }*/
}

void Board::init()
{
    //set occupancy for all pieces
    occupancy = sideBB[WHITE] | sideBB[BLACK];

    //fill peice board
    pieceList.fill(noPiece);
    for (Piece piece = KING; piece <= PAWN; piece++)
    {
        Bitboard whitePieceBB = getUniquePiece(WHITE, piece);
        while (whitePieceBB != 0)
        {
            Square sq = popLsb(whitePieceBB);
            pieceList[sq] = piece; // K–P
        }

        Bitboard blackPieceBB = getUniquePiece(BLACK, piece);
        while (blackPieceBB != 0)
        {
            Square sq = popLsb(blackPieceBB);
            pieceList[sq] = (piece + NUM_PIECES); // k-p
        }
    }

    setCheckSqs(WHITE);
    setCheckSqs(BLACK);
}

Board::Board()
{
    fenParser(STARTING_FEN);
    init();
}

Board::Board(const std::string& fen)
{
    fenParser(fen);
    init();
}

void Board::setCheckSqs(Side s)
{
    Square sq = lsb(getUniquePiece(s, KING));

    curState.checkSqs[s][PAWN]   =   Attacks::getPieceAttacks<PAWN>(sq, occupancy, s);
    curState.checkSqs[s][KNIGHT] =   Attacks::getPieceAttacks<KNIGHT>(sq, occupancy, s);
    curState.checkSqs[s][BISHOP] =   Attacks::getPieceAttacks<BISHOP>(sq, occupancy, s);
    curState.checkSqs[s][ROOK]   =   Attacks::getPieceAttacks<ROOK>(sq, occupancy, s);
    curState.checkSqs[s][QUEEN]  =   curState.checkSqs[s][ROOK] | curState.checkSqs[s][BISHOP];    
}

bool Board::isCheck(Side s)
{
    curState.checkingSqs[s] = 0ULL;
    Side enemy = s ^ 1;

    for(Piece p = QUEEN; p <= PAWN; p++)
    {
        curState.checkingSqs[s] |= curState.checkSqs[s][p] & getUniquePiece(enemy, p);
    }
    return curState.checkingSqs[s] != 0ULL;
}