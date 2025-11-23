#include "attacks.hpp"
#include "between.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "moves.hpp"
#include <make_move.hpp>


//will see if current move results in a check, or dsicovered check. Adds all checking piece 1-2 onto a bitboard.
//only sees if piece that moved gives a direct check to king and then we do a check the piece "FROM" square if its on a diagonal 
//with king or straight line. Only 2 areas we check so very quick and incremental.
void updateChecking(Board &b, Piece piece, Square from, Square to)
{
    //get piece type thats checking
    PieceType pt = static_cast<PieceType>(piece);
    Side s = b.curSide;
    Side enemy = b.curSide ^ 1;
    b.checkingSqs[enemy] = 0ULL; //reset each time since we cant be in check twice.
    Square ksq = lsb(b.getUniquePiece(enemy, KING));

    //see if piece that move gives direct check
    b.checkingSqs[enemy] |= b.getUniquePiece(s, piece) & Attacks::getPieceAttacksRuntime(pt, ksq, b.occupancy, enemy);

    //get allignement for discovred check
    Alignment alligned = Between::getAlignment(ksq, from);
    if(alligned == Alignment::NONE) //if none no need to check
    {
        return;
    }
    else if(alligned == Alignment::STRAIGHT) //straight means only rook attack check. using magic bitboard rather than xray since it is faster.
    {
        //sees where rooks can attack enemy king. and overlaps it with our rooks nad queens.
        b.checkingSqs[enemy] |= Attacks::getRookAttacks(ksq, b.occupancy) & (b.getUniquePiece(b.curSide, ROOK) | b.getUniquePiece(b.curSide, QUEEN));
        return;
    }
    else if (alligned == Alignment::DIAGONAL)  //diagonal means only bishop attack check.
    {
        //sees where bishops can attack enemy king. and overlaps it with our rooks and queens.
        b.checkingSqs[enemy] |= Attacks::getBishopAttacks(ksq, b.occupancy) & (b.getUniquePiece(b.curSide, BISHOP) | b.getUniquePiece(b.curSide, QUEEN));
        return;
    }
}

//updates castling permissions depedning on piece.
void reviewCastlingPermissions(Board& b, Piece captured, Square from, Square to, Piece piece, Side s)
{
    Castling newCastlingPerm = b.curState.castlingRights;
    
    //check if rook is moved and remove it from permissions
    if (piece == ROOK) 
    {
        if (from == h1) newCastlingPerm &= ~Castling::WK;
        else if (from == a1) newCastlingPerm &= ~Castling::WQ;
        else if (from == h8) newCastlingPerm &= ~Castling::BK;
        else if (from == a8) newCastlingPerm &= ~Castling::BQ;
    }
    
    // Check if a rook was captured and update
    if (captured == ROOK) 
    {
        if (to == h1) newCastlingPerm &= ~Castling::WK;
        else if (to == a1) newCastlingPerm &= ~Castling::WQ;
        else if (to == h8) newCastlingPerm &= ~Castling::BK;
        else if (to == a8) newCastlingPerm &= ~Castling::BQ;
    }
    
    //if king move delete all permissions for that side
    if (piece == KING) 
    {
        newCastlingPerm &= ~(s == WHITE ? Castling::WHITE : Castling::BLACK);
    }
    
    if (newCastlingPerm != b.curState.castlingRights) 
    {
        updateCastlingPerm(b, newCastlingPerm);
    }
}

// see where to move rook depending on where king castles. 
void castlingMoveHelper(Board& b, Side s, Square kingTo)
{
    Square rookFrom = noSquare;
    Square rookTo = noSquare;
    
    //switch statement depednds which and where the rook moves
    switch (kingTo) {
        case g1:  // White kingside
            rookFrom = h1;
            rookTo = f1;
            break;
        case c1:  // White queenside
            rookFrom = a1;
            rookTo = d1;
            break;
        case g8:  // Black kingside
            rookFrom = h8;
            rookTo = f8;
            break;
        case c8:  // Black queenside
            rookFrom = a8;
            rookTo = d8;
            break;
    }
    
    //update permissions and move rook.
    Castling perm = static_cast<Castling>(b.curState.castlingRights & ~(s == WHITE ? Castling::WHITE : Castling::BLACK));
    updateCastlingPerm(b, perm);
    movePiece(b, s, ROOK, rookFrom, rookTo);
}

void afterMoveHelper(Board &b, ExtdMove* move)
{

    //update halfmove clock if its not a capture or pawn move. //full move every other turn
    b.curState.halfmoveCount++;
    if(b.curSide == BLACK) { b.curState.fullmoveCount++; }
    if(move->getPiece() == PAWN || move->getCapture() != NONE) { b.curState.halfmoveCount = 0; }   
    
    reviewCastlingPermissions(b, move->getCapture(), move->getFrom(), move->getTo(), move->getPiece(), b.curSide);
    //redo checking for other side. if this move puts the other side in check
    updateChecking(b, move->getPiece(), move->getFrom(), move->getTo());
    swapSides(b);
}

void doMove(Board &b, ExtdMove* move)
{
    Side s = b.curSide;
    Side enemy = b.curSide ^ 1;
    Square from = move->getFrom();
    Square to = move->getTo();
    Piece piece = move->getPiece();

    b.stateHistory[b.historyCount] = b.curState;
    b.historyCount++;
    //increment history


    b.curState.epSq = EpSquare::NONE;

    if(move->getCapture() != NONE)
    {
        Piece captured = move->getCapture();
        removePiece(b, enemy, captured, to);
        b.curState.capturedPiece = captured;
    }

    if(move->isEnpassant())
    {
        removePiece(b, enemy, PAWN, squareToEPCaptureSquare(to, s));
        b.curState.capturedPiece = PAWN;
    }
    
    if(move->getPromoted() != NONE) //early return, since we are changing piece
    {
        removePiece(b, enemy, piece, from);
        putPiece(b, s, move->getPromoted(), to);
        afterMoveHelper(b, move);
        return;
    }

    if(move->isDouble())
    {
        b.curState.epSq = squareToEpsquare(to, s);
    }

    if(move->isCastle())
    {
        castlingMoveHelper(b, s, to);
    }

    movePiece(b, b.curSide, piece, from, to);
    afterMoveHelper(b, move);
}


void undoMove(Board &b, ExtdMove* move)
{
    
    swapSides(b);
    
    Side s = b.curSide;
    Side enemy = b.curSide ^ 1;
    Square from = move->getFrom();
    Square to = move->getTo();
    Piece piece = move->getPiece();
    
    // Handle castling first (before moving piece)
    if(move->isCastle())
    {
        const bool isKingside = (to > from);
        Square rookFrom, rookTo;
        
        if (s == WHITE)
        {
            rookFrom = isKingside ? h1 : a1;
            rookTo = isKingside ? f1 : d1;
        }
        else
        {
            rookFrom = isKingside ? h8 : a8;
            rookTo = isKingside ? f8 : d8;
        }
        movePiece(b, s, ROOK, rookTo, rookFrom);
    }
    
    // Handle promotion
    if(move->getPromoted() != NONE)
    {
        removePiece(b, s, move->getPromoted(), to);
        putPiece(b, s, piece, from);
    }
    else
    {
        movePiece(b, s, piece, to, from);
    }
    

    if(move->isEnpassant())
    {
        putPiece(b, enemy, PAWN, squareToEPCaptureSquare(to, s));
        b.curState.capturedPiece = PAWN;
    }

    // Restore captured piece
    if(move->getCapture() != NONE)
    {
        putPiece(b, enemy, move->getCapture(), to);
    }
    

    //restore to prev state
    b.historyCount--;
    b.curState = b.stateHistory[b.historyCount]; 
}