#include <constants.hpp>
#include <between.hpp>

bool isAllinged(Square s1, Square s2)
{
    if(s1 == s2) { return false; }

    U8 s1_rank = squareRank(s1);
    U8 s2_rank = squareRank(s2);

    U8 s1_file = squareFile(s1);
    U8 s2_file = squareFile(s2);

    if(s1_rank == s2_rank || s1_file == s2_file) { return true; }

    if(abs(s1_rank - s2_rank) == abs(s1_file - s2_file)) { return true; }

    return false;
}

Bitboard generateBetween(Square s1, Square s2)
{
    if(!isAllinged(s1, s2)) { return 0ULL; }

    U8 s1_rank = squareRank(s1);
    U8 s2_rank = squareRank(s2);

    U8 s1_file = squareFile(s1);
    U8 s2_file = squareFile(s2);

    
    int rank_dir = (s2_rank > s1_rank) ? 1 : (s2_rank < s1_rank) ? -1 : 0;
    int file_dir = (s2_file > s1_file) ? 1 : (s2_file < s1_file) ? -1 : 0;

    Bitboard line = 0ULL;
    int current_rank = s1_rank + rank_dir;
    int current_file = s1_file + file_dir;

    while (current_rank != s2_rank || current_file != s2_file) {
        int square = (current_rank * 8) + current_file;
        line |= (1ULL << square);
        
        current_rank += rank_dir;
        current_file += file_dir;
    }
    
    return line;
}


Bitboard generateThrough(Square s1, Square s2)
{
    if(!isAllinged(s1, s2)) { return 0ULL; }

    U8 s1_rank = squareRank(s1);
    U8 s2_rank = squareRank(s2);

    U8 s1_file = squareFile(s1);
    U8 s2_file = squareFile(s2);

    
    int rank_dir = (s2_rank > s1_rank) ? 1 : (s2_rank < s1_rank) ? -1 : 0;
    int file_dir = (s2_file > s1_file) ? 1 : (s2_file < s1_file) ? -1 : 0;

    Bitboard line = 0ULL;
    int current_rank = s1_rank + rank_dir;
    int current_file = s1_file + file_dir;

    while (current_rank >= 0 && current_rank < 8 &&
           current_file >= 0 && current_file < 8) 
    {
        int square = (current_rank * 8) + current_file;
        line |= (1ULL << square);
        
        current_rank += rank_dir;
        current_file += file_dir;
    }


    current_rank = s1_rank + rank_dir;
    current_file = s1_file + file_dir;

    while (current_rank >= 0 && current_rank < 8 &&
           current_file >= 0 && current_file < 8) 
    {
        int square = (current_rank * 8) + current_file;
        line |= (1ULL << square);
        
        current_rank -= rank_dir;
        current_file -= file_dir;
    }

    
    return line;
}


namespace Between
{
    void initializeBetween()
    {
        for(Square s1 = a1; s1 <= h8; s1++)
        {
            for(Square s2 = a1; s2 <= h8; s2++)
            {
                betweenArray[s1][s2] = generateBetween(s1, s2);
                throughArray[s1][s2] = generateThrough(s1, s2);
            }
        }
    }
      
}