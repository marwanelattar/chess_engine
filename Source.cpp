#include <iostream>
#include <string >
#include <unordered_map>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#pragma warning(disable : 4996)
#define U64  uint64_t  // bitboard datatype 
using namespace std;


// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

enum { white, black, all }; // setting numbers to each color 
enum {rook , bishop };
//encode peices 
enum  piece { P, N, B, R, Q, K, p, n, b, r, q, k };
// numbering squares 
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,no_sq
};

enum{WK=1,WQ=2 ,BK=4 ,BQ=8};

const char* coordinates[] = {
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};
//ascii pieces 
char ascii_pieces[] = "PNBRQKpnbrqk";
 // converting ascii chars to encoded constants 
 unordered_map<char, piece >char_pieces= {
    {'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K},
    {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k}

 };
 //promoted piece num to char 
 unordered_map<int, char> promoted_pieces = {
     {0, ' '}, {1, 'n'}, {2, 'b'}, {3, 'r'}, {4, 'q'}, {7, 'n'}, {8, 'b'}, {9, 'r'}, {10, 'q'},
 };
 // macros 
#define set_bit(bitboard, square) (bitboard |= (1ull << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define reset_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= 1ull << square : 0)
// bit manipulations 

    //  count bits within a bitboard 
static int count_bits(U64 bitboard) 
{
    int count =0;
       // counting by setting lsb to 0 every iteration 
    while (bitboard ) 
    {
        
        
        count++; // increment 
        bitboard &= bitboard - 1; // resetting lsb   
    }


    return count;
}
static int  get_lsb_index(U64 bitboard)
{
    // make sure bitboard is not 0
    if (bitboard)
    {
        // count trailing bits before LS1B
        return count_bits((bitboard & - (long long int (bitboard))) - 1);
    }

    else return -1;
}
U64 set_occupancy(int index, int number_of_bits, U64 attack_mask)
{
    // occupancy board 
    U64 occupancy = 0ull;
    // get every bit within the attack mask
    for (int count = 0; count < number_of_bits; count++)
    {
        int square = get_lsb_index(attack_mask);
        reset_bit(attack_mask, square);
        if (index & (1 << count))
        {
            occupancy |= (1ull << square);
        }
    }
    return occupancy;
}

// attacks 
// not A file constant
const U64 not_a_file = 18374403900871474942ULL;

// not H file constant
const U64 not_h_file = 9187201950435737471ULL;

// not HG file constant
const U64 not_hg_file = 4557430888798830399ULL;

// not AB file constant
const U64 not_ab_file = 18229723555195321596ULL;
// relevant occupancy square for every square on board 
const int  relevant_bishop_bits[64] =
{
6, 5, 5, 5, 5, 5, 5, 6,
5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 7, 7, 7, 7, 5, 5,
5, 5, 7, 9, 9, 7, 5, 5,
5, 5, 7, 9, 9, 7, 5, 5,
5, 5, 7, 7, 7, 7, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5,
6, 5, 5, 5, 5, 5, 5, 6
};
const int relevant_rook_bits[64] = 
{
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};
const U64 bishop_magic_numbers[64] =
{
      0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};

const U64 rook_magic_numbers[64] =
{
     0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};
U64 pawn_attacks[2][64]; // 2 sides ,64 squares
U64 knight_attacks[64];  
U64 king_attacks[64]
;
U64 bishop_masks[64];
U64 rook_masks[64];


U64 bishop_attacks[64][512];
U64 rook_attacks[64][4096];


U64 mask_pawn_attacks(int side , int  square  ) // generate pawn attack
{
    U64 attacks = 0ull;
    U64 bitboard = 0ull;
    set_bit(bitboard, square);
    if (side == white ) // for white pieces 
    {
       
        attacks |= (bitboard & not_h_file) >>7;
        attacks |= (bitboard & not_a_file) >> 9;
    }
    else // for black peices 
    {
     
        attacks |=( bitboard & not_a_file ) <<7;
        attacks |= (bitboard & not_h_file) << 9;
    }
    return attacks;
   
}
U64 mask_knight_attacks( int square) 
{
    U64 attacks = 0ull;
    U64 bitboard = 0ull;
    set_bit(bitboard, square);
  
    
       attacks |= (bitboard & not_hg_file) >> 6;
        attacks |= (bitboard& not_ab_file  )<< 6;
        attacks |= (bitboard & not_ab_file) >> 10;
        attacks |= (bitboard & not_hg_file) <<10;
       attacks |= (bitboard & not_h_file) >> 15;
        attacks |= (bitboard & not_a_file) <<15;
        attacks |= (bitboard& not_a_file) >>17;
        attacks |= (bitboard&not_h_file) << 17;
        
        

    
    return attacks;
}
U64 mask_king_attacks(int square) 
{
    U64 attacks = 0ull;
    U64 bitboard = 0ull;
    set_bit(bitboard, square);
    attacks |= (bitboard & not_a_file) << 7;
    attacks |= (bitboard & not_h_file) >> 7;
    attacks |= (bitboard & not_h_file) << 1;
    attacks |= (bitboard & not_a_file) >> 1;
    attacks |= bitboard << 8;
    attacks |= bitboard >> 8;
    attacks |= (bitboard & not_h_file) << 9;
    attacks |= (bitboard & not_a_file) >>9 ;


    return attacks;


  
}
U64 mask_bishop_attacks(int square)
{
    U64 all_possible_squares = 0ull;
  
    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)  
        all_possible_squares |= (1ull << (r * 8 + f));
    
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) 
        all_possible_squares |= (1ull << (r * 8 + f));
  
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) 
        all_possible_squares |= (1ull << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
         all_possible_squares |= (1ull << (r * 8 + f));

    return all_possible_squares;
}
U64 mask_rook_attacks(int square)
{
    U64 all_possible_squares = 0ull;
    int r, f; // rank, file 
    int tr = square / 8; // target rank
    int tf = square % 8; // target file  
    for (r = tr + 1; r <= 6; r++)
        all_possible_squares |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--)
        all_possible_squares |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++)
        all_possible_squares |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--)
        all_possible_squares |= (1ULL << (tr * 8 + f));
    return all_possible_squares;

}

U64 generate_bishop_attacks(int square, U64 blockers)
{
    U64 attacks = 0ull;

    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ull << (r * 8 + f));
        if ((1ull << (r * 8 + f)) & blockers)
            break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ull << (r * 8 + f));
        if ((1ull << (r * 8 + f)) & blockers)
            break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) 
    {
        attacks |= (1ull << (r * 8 + f));
        if ((1ull << (r * 8 + f)) & blockers)
            break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ull << (r * 8 + f));
        if ((1ull << (r * 8 + f)) & blockers)
            break;
    }
    return attacks;
}
U64 generate_rook_attacks(int square, U64 blockers)
{


    U64 attacks = 0ull;
    int r, f; // rank, file 
    int tr = square / 8; // target rank
    int tf = square % 8; // target file  
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & blockers)
        break;
    }

    for (r = tr - 1; r >= 0; r--)
    {

        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & blockers) break;
    }
    for (f = tf + 1; f <= 7; f++) 
    {
            attacks |= (1ULL << (tr * 8 + f));
            if ((1ULL << (tr * 8 + f)) & blockers)
                break;
     }
    
    for (f = tf - 1; f >= 0; f--)
    {
            attacks |= (1ULL << (tr * 8 + f));
            if ((1ULL << (tr * 8 + f)) & blockers)
                break;
    }
    return attacks;

}

void initialize_leapers_attacks()
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init pawn attacks
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
        // knight attacks 
        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}
void initialize_sliders_attacks(int bishop)
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);

        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++)
        {
            // bishop
            if (bishop)
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - relevant_bishop_bits[square]);

                // init bishop attacks
                bishop_attacks[square][magic_index] = generate_bishop_attacks(square, occupancy);
            }

            // rook
            else
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                U64 magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - relevant_rook_bits[square]);

                // init bishop attacks
                rook_attacks[square][magic_index] = generate_rook_attacks(square, occupancy);

            }
        }
    }
}
void init_all()
{

    initialize_leapers_attacks();
    initialize_sliders_attacks(bishop);
    initialize_sliders_attacks(rook);
}

U64 get_bishop_attack(int square, U64 occupancy)
{
    //get bishop attack for a given square assuming board occupancy 
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - relevant_bishop_bits[square];

    return bishop_attacks[square][occupancy];
}
U64 get_rook_attack(int square, U64 occupancy)
{
    //get bishop attack for a given square assuming board occupancy 
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - relevant_rook_bits[square];
    return rook_attacks[square][occupancy];

}
U64 get_queen_attack(int square, U64 occupancy)
{
    U64 attacks = get_bishop_attack(square, occupancy) | get_rook_attack(square, occupancy);
    return attacks;

}
//set occupancies


//defining bitboards 
U64 bitboards[12];
//define piece occupancy bitboards 
U64 occupancies[3];
//side to move
int side=0;
 // enpassant square
int enpassant=no_sq;
// castling rights 
int castle=0;
/*
          binary move bits                               hexidecimal constants

    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/

uint32_t get_move_source(uint32_t move) { return move & 0x3f; }
uint32_t get_move_target(uint32_t move) { return (move >> 6) & 0x3f; }
uint32_t get_move_piece(uint32_t move) { return (move >> 12) & 0xf; }
uint32_t get_move_promoted(uint32_t move) { return (move >> 16) & 0xf; }
bool get_move_capture(uint32_t move) { return move & 0x100000; }
bool get_move_double(uint32_t move) { return move & 0x200000; }
bool get_move_enpassant(uint32_t move) { return move & 0x400000; }
bool get_move_castling(uint32_t move) { return move & 0x800000; }

// printing bitboard 
void print_bitboard(U64 bitboard)
{
    // ranks 
    for (int rank = 0; rank < 8; rank++)
    {
        // files 
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file; // defining a square 
            if (file == 0)
            {
                cout << 8 - rank << " "; // showing rank number 
            }

            cout << (get_bit(bitboard, square) ? 1 : 0) << ' '; // showing if a piece is on the square 
        }
        cout << endl;
    }

    cout << "  a b c d e f g h " << endl; // showing files 
    cout << "bitboard in decimal: " << bitboard << endl;
}
//print full board 
void print_board() {
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if (!file)
            {
                cout << 8 - rank << " ";
            }
            // define piece variable 
            int piece = -1;
            //loop over all pieces bitboards 
            for (int board_type = P; board_type <= k; board_type++)
            {
                if (get_bit(bitboards[board_type], square))
                {
                    piece = board_type;
                }
            }

            cout << char((piece == -1) ? '.' : ascii_pieces[piece]) << ' ';

        }
        cout << endl;
    } cout << "  a b c d e f g h " << endl; // showing files 
    // print side to move 
    cout << "side: " << (!side ? "white " : "black")<<endl;
    //enpassant 
    cout << "Enpassant: " << ((enpassant != no_sq) ? coordinates[enpassant] : "no") << endl;;
    // castling 
    cout << "castling: "
        << ((castle & WK) ? 'K' : '-')
        << ((castle & WQ) ? 'Q' : '-')
        << ((castle & BK) ? 'k' : '-')
        << ((castle & BQ) ? 'q' : '-')
        << endl;
} 

#define copy_board()                                                      \
    U64 bitboards_copy[12], occupancies_copy[3];                          \
    int side_copy, enpassant_copy, castle_copy;                           \
    memcpy(bitboards_copy, bitboards, 96);                                \
    memcpy(occupancies_copy, occupancies, 24);                            \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \

// restore board state
#define take_back()                                                       \
    memcpy(bitboards, bitboards_copy, 96);                                \
    memcpy(occupancies, occupancies_copy, 24);                            \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy;   \

// move types
enum { all_moves, only_captures };
/*

    bin  dec

   0001    1  white king can castle to the king side
   0010    2  white king can castle to the queen side
   0100    4  black king can castle to the king side
   1000    8  black king can castle to the queen side

   examples

   1111       both sides an castle both directions
   1001       black king => queen side
              white king => king side

*/
// castling rights update constants
const int castling_rights[64] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};


// parse FEN position 
void parse_fen( string  fen)
{
    // reset all board data 
    memset(bitboards, 0ull, sizeof(bitboards));
    memset(occupancies, 0ull, sizeof(occupancies));
    side = 0;
    enpassant = no_sq;
    castle = 0;
    int i = 0;
    // loop over board squares
    for (int square = 0; square < 64 && fen[i] && fen[i] != ' '; )
    {
        // match ascii pieces within FEN string
        if ((fen[i] >= 'b' && fen[i] <= 'r') || (fen[i] >= 'B' && fen[i] <= 'R'))
        {
            // init piece type
            int piece = char_pieces[fen[i]];

            // set piece on corresponding bitboard
            set_bit(bitboards[piece], square);

            // increment square and pointer to FEN string
            square++;
            i++;
        }

        // match empty square numbers within FEN string
        else if (fen[i] >= '1' && fen[i] <= '8')
        {
            // init offset (convert char 0 to int 0)
            int offset = fen[i] - '0';

            // increment square and pointer to FEN string
            square += offset;
            i++;
        }

        // match rank separator
        else if (fen[i] == '/')
        {
            // increment pointer to FEN string
            i++;
        }
        else
        {
            i++; // error
        }
    }
        i++;
        if (fen[i] == 'w')// reading witch sie to play 
            side = white;

        else
            side = black;
        i += 2; // skipping to castling part of the string 

        while (fen[i] != ' ')
        {
            switch (fen[i])
            {
            case 'K': castle |= WK; break;
            case 'Q': castle |= WQ; break;
            case 'k': castle |= BK; break;
            case 'q': castle |= BQ; break;
            case '-': break;
            }
            i++;
        }
        i++;
         // got to parsing en passant square (increment pointer to FEN string)
  
        // parse en passant square
        if (fen[i] != '-')
        {
            // parse en passant file & rank
            int file = fen[i] - 'a';
            int rank = 8 - (fen[i+1] - '0');

            // init enpassant square
            enpassant = rank * 8 + file;
        }

        // no enpassant square
        else
            enpassant = no_sq;

// loop over white pieces bitboards
        for (int piece = P; piece <= K; piece++)
            // populate white occupancy bitboard
            occupancies[white] |= bitboards[piece];

        // loop over black pieces bitboards
        for (int piece = p; piece <= k; piece++)
            // populate white occupancy bitboard
            occupancies[black] |= bitboards[piece];

        // init all occupancies
        occupancies[all] |= occupancies[white];
        occupancies[all] |= occupancies[black];
        
            
    

}
// check if a square is attacked by a given side 
int is_attacked(int square , int side ) {
    // attacked by white pawns
    if ((side == white) && (pawn_attacks[black][square] & bitboards[P])) return 1;

    // attacked by black pawns
    if ((side == black) && (pawn_attacks[white][square] & bitboards[p])) return 1;

    // attacked by knights
    if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])) return 1;

    // attacked by bishops
    if (get_bishop_attack(square, occupancies[all]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;

    // attacked by rooks
    if (get_rook_attack(square, occupancies[all]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;

    // attacked by bishops
    if (get_queen_attack(square, occupancies[all]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;

    // attacked by kings
    if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])) return 1;
    // return false by defalt 
    return 0;
}
/*
          binary move bits                               hexidecimal constants

    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 000
    
    
    
    0 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/

// encode moves 
uint32_t encode_move(uint32_t source, uint32_t target, uint32_t piece, uint32_t promoted,
    uint32_t capture, uint32_t double_push, uint32_t enpassant, uint32_t castling) {
    return source | (target << 6) | (piece << 12) | (promoted << 16) |
        (capture << 20) | (double_push << 21) | (enpassant << 22) | (castling << 23);
}

// move  list structure 
struct moves {
    int moves[256];
    // moves count 
    int move_count;



};
void add_move(moves* move_list, int move)
{
    // strore move
    move_list->moves[move_list->move_count] = move;

    // increment move count
    move_list->move_count++;
}

void  print_move(int move) {
    if (get_move_promoted(move))
    {
        cout << coordinates[get_move_source(move)]
            << coordinates[get_move_target(move)]
            << promoted_pieces.at(get_move_promoted(move));
    }
    else {
        cout << coordinates[get_move_source(move)]
            << coordinates[get_move_target(move)];
         
    }
}
void print_move_list(moves* move_list) {
    cout << "\nmove    piece   capture   double    enpass    castling\n\n";

    for (int i = 0; i < move_list->move_count; ++i) {
        int move = move_list->moves[i];
        cout << coordinates[get_move_source(move)]
            << coordinates[get_move_target(move)]
            << promoted_pieces[get_move_promoted(move)] << "     "
            << ascii_pieces[get_move_piece(move)] << "        "
            << (get_move_capture(move) ? 1 : 0) << "          "
            << (get_move_double(move) ? 1 : 0) << "          "
            << (get_move_enpassant(move) ? 1 : 0) << "          "
            << (get_move_castling(move) ? 1 : 0) << '\n';
    }
    cout << "\n\n  Total number of moves: " << move_list->move_count;
}
// generate all moves 
void generate_all_moves(moves *move_list ) 
{
    // init move count
    move_list->move_count = 0;
    
    // source & target squares 
    int source_sq, target_sq;
    //   define a copy of the bitboard for the current piece and its attacks
    U64 bitboard, attacks;
    // loop over all  piece bitboards 
    for (int piece  = P; piece  <= k; piece ++)
    {
        //copy peice bitboard 
        bitboard = bitboards[piece];
        // generate white pawns and white castling moves 
        if (side == white )
        {
            //  check if the peice is a white pawn 
            if (piece == P )
            {
                // loop over white pawns within the bitboard 
                while (bitboard )
                {
                    // get source square 
                    source_sq = get_lsb_index(bitboard);
                    target_sq = source_sq - 8 ; 
                    reset_bit(bitboard, source_sq); // remove square from bitboard 
                    // generate quite moves 
                    if (!(target_sq < a8) && ! get_bit(occupancies[all], target_sq))
                    {
                        // pawn promotion
                        if (source_sq >= a7 && source_sq <= h7)
                        {
                            add_move(move_list, encode_move(source_sq, target_sq, piece, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, N, 0, 0, 0, 0));
                        }

                        else
                        {
                            // one square ahead pawn move
                            add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));

                            // two squares ahead pawn move
                            if ((source_sq >= a2 && source_sq <= h2) && !(get_bit(occupancies[all], target_sq - 8)))
                                add_move(move_list, encode_move(source_sq, target_sq-8, piece, 0, 0, 1, 0, 0));
                        }
                    }
                    // init pawn attacks bitboard
                    attacks = pawn_attacks[side][source_sq] & occupancies[black];

                    // generate pawn captures
                    while (attacks)
                    {
                        // init target square
                        target_sq = get_lsb_index(attacks);

                        // pawn promotion
                        if (source_sq >= a7 && source_sq <= h7)
                        {
                            add_move(move_list, encode_move(source_sq, target_sq, piece, Q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, R, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, B, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, N, 1, 0, 0, 0));
                        }

                        else
                            // one square ahead pawn capture 
                            add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
                        // pop ls1b of the pawn attacks
                        reset_bit(attacks, target_sq);
                    }

                    // generate enpassant captures
                    if (enpassant != no_sq)
                    {
                        // lookup pawn attacks and bitwise AND with enpassant square (bit)
                        U64 enpassant_attacks = pawn_attacks[side][source_sq] & (1ULL << enpassant);

                        // make sure enpassant capture available
                        if (enpassant_attacks)
                        {
                            // init enpassant capture target square
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            add_move(move_list, encode_move(source_sq, target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    // pop ls1b from piece bitboard copy
                    reset_bit(bitboard, source_sq);
                }
            }
            //castling  
            if ( piece == K )
            {
              
                //king side castle 
                if (castle & WK)
                {
                    //make sure sqaures are empty 
                    if (!get_bit(occupancies[all], f1) && !get_bit(occupancies[all], g1))
                    {
                        if (!is_attacked(e1, black) && !is_attacked(f1, black))
                        {
                            add_move(move_list, encode_move(e1,g1, piece, 0, 0, 0, 0, 1));
                        }
                    }

                 }

                // queen side castle 
                     if (castle & WQ)
                     {
                         // make sure squares are empty 
                         if ((!get_bit(occupancies[all], c1) && !get_bit(occupancies[all], d1)&& !get_bit(occupancies[all], b1)))
                         {
                             if (!is_attacked(e1, black) && !is_attacked(d1, black) && !is_attacked(c1, black))
                             {
                                 add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
                             }
                         }

                     }
            }
         
        }
        // generate black pawns and castling moves 
        else
        {
            //  check if the peice is a black  pawn 
            if (piece == p)
            {
                // loop over white pawns within the bitboard 
                while (bitboard)
                {
                    // get source square 
                    source_sq = get_lsb_index(bitboard);
                    target_sq = source_sq + 8;
                    reset_bit(bitboard, source_sq); // remove square from bitboard 
                    // generate quite moves 
                    if (!(target_sq > h1) && !get_bit(occupancies[all], target_sq))
                    {
                        // pawn promotion
                        if (source_sq >= a2 && source_sq <= h2)
                        {
                            add_move(move_list, encode_move(source_sq, target_sq, piece, q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, r, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, b, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, n, 0, 0, 0, 0));
                        }

                        else
                        {
                            // one square ahead pawn move
                            add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));

                            // two squares ahead pawn move
                            if ((source_sq >= a7 && source_sq <= h7) && !get_bit(occupancies[all], target_sq + 8))
                                add_move(move_list, encode_move(source_sq, target_sq + 8, piece, 0, 0, 1, 0, 0));
                        }
                    }
                    // init pawn attacks bitboard
                    attacks = pawn_attacks[side][source_sq] & occupancies[white];

                    // generate pawn captures
                    while (attacks)
                    {
                        // init target square
                        target_sq = get_lsb_index(attacks);

                        // pawn promotion
                        if (source_sq >= a2 && source_sq <= h2)
                        {
                            add_move(move_list, encode_move(source_sq, target_sq, piece, q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, r, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, b, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_sq, target_sq, piece, n, 1, 0, 0, 0));
                        }

                        else
                        
                            // one square ahead pawn capture 
                            add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
                        // pop ls1b of the pawn attacks
                        reset_bit(attacks, target_sq);
                    }

                    // generate enpassant captures
                    if (enpassant != no_sq)
                    {
                        // lookup pawn attacks and bitwise AND with enpassant square (bit)
                        U64 enpassant_attacks = pawn_attacks[side][source_sq] & (1ULL << enpassant);

                        // make sure enpassant capture available
                        if (enpassant_attacks)
                        {
                            // init enpassant capture target square
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            add_move(move_list, encode_move(source_sq, target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                    }
                    // pop lsb from piece bitboard copy
                    reset_bit(bitboard, source_sq);
                }
            }
            //castling  
            if (piece == k)
            {

                //king side castle 
                if (castle & BK)
                {
                    //make sure sqaures are empty 
                    if (!get_bit(occupancies[all], f8) && !get_bit(occupancies[all], g8))
                    {
                        if (!is_attacked(e8, white) && !is_attacked(f8, white))
                        {
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                        }
                    }

                }

                // queen side castle 
                if (castle & BQ)
                {
                    // make sure squares are empty 
                    if ((!get_bit(occupancies[all], c8) && !get_bit(occupancies[all], d8) && !get_bit(occupancies[all], b8)))
                    {
                        if (!is_attacked(e8, white) && !is_attacked(d8, white) && !is_attacked(c8, white))
                        {
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                        }
                    }

                }
            }
        }
        // generate knight moves 
        if (( side == white ) ? piece == N : piece == n )
        { // loop over squares of piece bitboard 
            while (bitboard )
            {
                source_sq = get_lsb_index(bitboard);
                // get set of target squares 
                attacks = knight_attacks[source_sq]&((side==white )? ~occupancies[white]:~occupancies[black]);
                while (attacks )
                {
                    // init target square 
                    target_sq = get_lsb_index(attacks);

                    // quite move 
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_sq))
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
                    else 
                       // capture move 
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
                    // pop lsb from attacks bitboard 
                    reset_bit(attacks, target_sq);

                }
                // pop lsb 
                reset_bit(bitboard, source_sq);
            }

        }
        // generate bishop moves 
        if ((side == white) ? piece == B : piece == b)
        { // loop over squares of piece bitboard 
            while (bitboard)
            {
                source_sq = get_lsb_index(bitboard);
                // get set of target squares 
                attacks = get_bishop_attack(source_sq, occupancies[all]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks)
                {
                    // init target square 
                    target_sq = get_lsb_index(attacks);

                    // quite move 
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_sq))
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
                    else
                        // capture move 
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));

                    // pop lsb from attacks bitboard 
                    reset_bit(attacks, target_sq);

                }
                // pop lsb from bitboard 
                reset_bit(bitboard, source_sq);
            }

        }
        //generate rook moves 
        if ((side == white) ? piece == R : piece == r)
        { // loop over squares of piece bitboard 
            while (bitboard)
            {
                source_sq = get_lsb_index(bitboard);
                // get set of target squares 
                attacks = get_rook_attack(source_sq, occupancies[all]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks)
                {
                    // init target square 
                    target_sq = get_lsb_index(attacks);

                    // quite move 
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_sq))
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
                    else
                        // capture move 
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));

                    // pop lsb from attacks bitboard 
                    reset_bit(attacks, target_sq);

                }
                // pop lsb from bitboard 
                reset_bit(bitboard, source_sq);
            }

        }
        // generate queen moves 

        if ((side == white) ? piece == Q : piece == q)
        { // loop over squares of piece bitboard 
            while (bitboard)
            {
                source_sq = get_lsb_index(bitboard);
                // get set of target squares 
                attacks = get_queen_attack(source_sq, occupancies[all]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks)
                {
                    // init target square 
                    target_sq = get_lsb_index(attacks);

                    // quite move 
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_sq))
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
                    else
                        // capture move 
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
                    // pop lsb from attacks bitboard 
                    reset_bit(attacks, target_sq);

                }
                // pop lsb from bitboard 
                reset_bit(bitboard, source_sq);
            }

        }
        //king moves 
        if ((side == white) ? piece == K : piece == k)
        { // loop over squares of piece bitboard 
            while (bitboard)
            {
                source_sq = get_lsb_index(bitboard);
                // get set of target squares 
                attacks = (king_attacks[source_sq] & ((side == white) ? ~occupancies[white] : ~occupancies[black]));

                while (attacks)
                {
                    // init target square 
                    target_sq = get_lsb_index(attacks);

                    // quite move 
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_sq))
                        add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
                    else 
                        // capture move 
                    add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));

                    // pop lsb from attacks bitboard 
                    reset_bit(attacks, target_sq);

                }
                // pop lsb from bitboard 
                reset_bit(bitboard, source_sq);
            }

        }
    }
}
// make move on chess board
  static inline int make_move(int move, int move_flag)
{
    // quite moves
    if (move_flag == all_moves)
    {
        // preserve board state
        copy_board();

        // parse move
        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted_piece = get_move_promoted(move);
        int capture = get_move_capture(move);
        int double_push = get_move_double(move);
        int enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        // move piece
        reset_bit(bitboards[piece], source_square);
        set_bit(bitboards[piece], target_square);
        if (capture)
        {
            // pick up bitboard piece ranges depending on side 
            int start_piece, end_piece;
            if (side == white)
            {
                start_piece = p;
                end_piece = k;

            }
            else
            {
                start_piece = P;
                end_piece = K;
            }
            // loop over bitboards opposite to the current side to move 
            for (int piece = start_piece; piece <= end_piece;piece++)
            {
                // remove piece if it is on the opposite target sqaure 
                if (get_bit(bitboards[piece], target_square))
                {
                    reset_bit(bitboards[piece], target_square);
                    break;
                }

            }
        }
        // handle promotions 
        if (promoted_piece)
        {
            // erase the pawn from the target square
            reset_bit(bitboards[(side == white) ? P : p], target_square);

            // set up promoted piece on chess board
            set_bit(bitboards[promoted_piece], target_square);
        }
        //handle enpassant 
        if (enpass)
        {
            if (side == white)
            {

                reset_bit(bitboards[p], target_square + 8);

            }
            else {

                reset_bit(bitboards[P], target_square - 8);
            }



        }
        //reset en passant square 
        enpassant = no_sq;
        // Handle double pawn push 
        if (double_push)
        {
            // set enpassant square depending on side 
            (side == white) ? (enpassant = target_square + 8) : (enpassant = target_square - 8);

        }

        if (castling)
        {


            switch (target_square)
            {
                // white king side 
            case(g1):

                reset_bit(bitboards[R], h1);
                set_bit(bitboards[R], f1);
                break;
                // white queen side 
            case(c1):

                reset_bit(bitboards[R], a1);
                set_bit(bitboards[R], d1);
                break;

                // black king side 
            case(g8):

                reset_bit(bitboards[r], h8);
                set_bit(bitboards[r], f8);
                break;
                // black queen side 
            case(c8):

                reset_bit(bitboards[r], a8);
                set_bit(bitboards[r], d8);
                break;
            }


        }
        // update castling rights 
        castle &= castling_rights[source_square];
        castle &= castling_rights[target_square];
        // reset occupancies 
        memset(occupancies, 0ull, sizeof(occupancies));
        for (int bb_piece = P; bb_piece <= K; bb_piece++)
            // update white occupancies
            occupancies[white] |= bitboards[bb_piece];

        // loop over black pieces bitboards
        for (int bb_piece = p; bb_piece <= k; bb_piece++)
            // update black occupancies
            occupancies[black] |= bitboards[bb_piece];

        // update both sides occupancies
        occupancies[all] |= occupancies[white];
        occupancies[all] |= occupancies[black];

        //change side 
        side ^= 1;
        // make sure that king has not been exposed into a check
        if (is_attacked((side == white) ? get_lsb_index(bitboards[k]) : get_lsb_index(bitboards[K]), side))
        {
            // take move back
            take_back();

            // return illegal move
            return 0;
        }

        //
        else
            // return legal move
            return 1;

    }

    // capture moves
    else
    {
        // make sure move is the capture
        if (get_move_capture(move))
            make_move(move, all_moves);

        // otherwise the move is not a capture
        else
            // don't make it
            return 0;
    }
}

// calulate number of positions at a given depth 
long nodes = 0;
void perft_driver(int depth) 
{
    // recursion escape condition 
    if (depth==0)
    {
        // increment nodes count 
        nodes++;
       
        return;
    }

    // create move list instance
    moves move_list[1];

    // generate moves
    generate_all_moves(move_list);
    for (int move_count = 0; move_count < move_list->move_count; move_count++)
    {
      

        // preserve board state
        copy_board();
        if (!make_move(move_list->moves[move_count], all_moves))
            // skip to the next move 
            continue;

        //call function
        perft_driver(depth - 1);


        // take back
        take_back();
     

    }
}
 // perft test 
void perft_test(int depth)
{
    cout<<"\n     Performance test\n\n";

    // create move list instance
    moves move_list[1];

    // generate moves
    generate_all_moves(move_list);

    // init start time
    auto start = chrono::high_resolution_clock::now();;

    // loop over generated moves
    for (int move_count = 0; move_count < move_list->move_count; move_count++)
    {
        // preserve board state
        copy_board();

        // make move
        if (!make_move(move_list->moves[move_count], all_moves))
            // skip to the next move
            continue;

        // cummulative nodes
        long cummulative_nodes = nodes;

        // call perft driver recursively
        perft_driver(depth - 1);

        // old nodes
        long old_nodes = nodes - cummulative_nodes;

        // take back
        take_back();
        
        // print move
            cout << coordinates[get_move_source(move_list->moves[move_count])]
            << coordinates[get_move_target(move_list->moves[move_count])]
            << promoted_pieces.at(get_move_promoted(move_list->moves[move_count]))<<"nodes:" << old_nodes  << endl;
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
   
    // print results
   cout<<endl<<"     Depth:"<< depth<<endl;
    cout<<"    Nodes:"<< nodes<<endl;
    cout<<"    Time:"<< duration.count() << " ms." << endl;

}
/////////////////////////////////////////////////////////evaluation ////////////////////////////////////////////

// material scrore

/*
    ♙ =   100   = ♙
    ♘ =   300   = ♙ * 3
    ♗ =   350   = ♙ * 3 + ♙ * 0.5
    ♖ =   500   = ♙ * 5
    ♕ =   1000  = ♙ * 10
    ♔ =   10000 = ♙ * 100

*/

int material_score[12] = {
    100,      // white pawn score
    300,      // white knight scrore
    350,      // white bishop score
    500,      // white rook score
   1000,      // white queen score
  10000,      // white king score
   -100,      // black pawn score
   -300,      // black knight scrore
   -350,      // black bishop score
   -500,      // black rook score
  -1000,      // black queen score
 -10000,      // black king score
};
// pawn positional score
const int pawn_score[64] =
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] =
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score
const int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
const int mirror_score[128] =
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

// position evaluation
 int evaluate()
 {
     // static evaluation score
     int score = 0;

     // current pieces bitboard copy
     U64 bitboard;

     // init piece & square
     int piece, square;

     // loop over piece bitboards
     for (int bb_piece = P; bb_piece <= k; bb_piece++)
     {
         // init piece bitboard copy
         bitboard = bitboards[bb_piece];

         // loop over pieces within a bitboard
         while (bitboard)
         {
             // init piece
             piece = bb_piece;

             // init square
             square = get_lsb_index(bitboard);

             // score material weights
             score += material_score[piece];

             // score positional piece scores
             switch (piece)
             {
                 // evaluate white pieces
             case P: score += pawn_score[square]; break;
             case N: score += knight_score[square]; break;
             case B: score += bishop_score[square]; break;
             case R: score += rook_score[square]; break;
             case K: score += king_score[square]; break;

                 // evaluate black pieces
             case p: score -= pawn_score[mirror_score[square]]; break;
             case n: score -= knight_score[mirror_score[square]]; break;
             case b: score -= bishop_score[mirror_score[square]]; break;
             case r: score -= rook_score[mirror_score[square]]; break;
             case k: score -= king_score[mirror_score[square]]; break;
             }


             // pop ls1b
             reset_bit(bitboard, square);
         }
     }

     // return final evaluation based on side
     return (side == white) ? score : -score;
 }



///////////////////////////////////////////search/////////////////////////////////////////////////////
 // half move counter
 int ply;

 // best move
 int best_move;

 // negamax alpha beta search
 int quiescence(int alpha, int beta )

 {
     // escape condition 
     int eval = evaluate();

     // fail-hard beta cutoff
     if (eval >= beta)
     {
         // node (move) fails high
         return beta;
     }

     // found a better move
     if (eval > alpha)
     {
         // PV node (move)
         alpha = eval ;


     }
     // create move list instance
     moves move_list[1];

     // generate moves
     generate_all_moves(move_list);

     // loop over moves within a movelist
     for (int count = 0; count < move_list->move_count; count++)
     {
         // preserve board state
         copy_board();
       
         // increment ply
         ply++;

         // make sure to make only legal moves
         if (make_move(move_list->moves[count],only_captures) == 0)
         {
             // decrement ply
             ply--;

             // skip to next move
             continue;
         }
       
         // score current move
         int score = -quiescence(-beta, -alpha );

         // decrement ply
         ply--;

         // take move back
         take_back();

         // fail-hard beta cutoff
         if (score >= beta)
         {
             // node (move) fails high
             return beta;
         }

         // found a better move
         if (score > alpha)
         {
             // PV node (move)
             alpha = score;


         }
     }
     return alpha;
 }
  int negamax(int alpha, int beta, int depth)
 {
     // recurrsion escape condition
     if (depth == 0)
         // return evaluation
         return quiescence(alpha , beta ) ;

     // increment nodes count
     nodes++;
     // is king in check 
     int in_check = is_attacked((side == white) ? get_lsb_index(bitboards[K]) :
         get_lsb_index(bitboards[k]),
         side ^ 1);
     // legal moves count 
     int legal_moves = 0;
     // best move so far
     int best_sofar=0;

     // old value of alpha
     int old_alpha = alpha;

     // create move list instance
     moves move_list[1];

     // generate moves
     generate_all_moves(move_list);

     // loop over moves within a movelist
     for (int count = 0; count < move_list->move_count; count++)
     {
         // preserve board state
         copy_board();

         // increment ply
         ply++;

         // make sure to make only legal moves
         if (make_move(move_list->moves[count], all_moves) == 0)
         {
             // decrement ply
             ply--;

             // skip to next move
             continue;
         }
         // increment legal move 
         legal_moves++;
         // score current move
         int score = -negamax(-beta, -alpha, depth - 1);

         // decrement ply
         ply--;

         // take move back
         take_back();

         // fail-hard beta cutoff
         if (score >= beta)
         {
             // node (move) fails high
             return beta;
         }

         // found a better move
         if (score > alpha)
         {
             // PV node (move)
             alpha = score;

             // if root move
             if (ply == 0)
                 // associate best move with the best score
                 best_sofar = move_list->moves[count];
         }
     }
     // we don't have any legal moves to make in the current postion
     if (legal_moves == 0)
     {
         // king is in check
         if (in_check)
             // return mating score (assuming closest distance to mating position)
             return -49000 + ply;

         // king is not in check
         else
             // return stalemate score
             return 0;
     }
     // found better move
     if (old_alpha != alpha)
         // init best move
         best_move = best_sofar;

     // node (move) fails low
     return alpha;
 }
  void search_position(int depth)
  {
      // find best move within a given position
      int score = negamax(-50000, 50000, depth);

      if (best_move) {
          printf("info score cp %d depth %d nodes %ld\n", score, depth, nodes);
          // best move placeholder
          cout << "bestmove ";

          print_move(best_move);
          cout << endl;
      }
  }
/////////////////////////// UCI  PROTOCOL/////////////////////////////////////////////////////////////
// parse user/GUI move string input (e.g. "e7e8q")
int parse_move(const char* move_string)
{
    // create move list instance
    moves move_list[1];

    // generate moves
    generate_all_moves(move_list);

    // parse source square
    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;

    // parse target square
    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;

    // loop over the moves within a move list
    for (int move_count = 0; move_count < move_list->move_count; move_count++)
    {
        // init move
        int move = move_list->moves[move_count];

        // make sure source & target squares are available within the generated move
        if (source_square == get_move_source(move) && target_square == get_move_target(move))
        {
            // init promoted piece
            int promoted_piece = get_move_promoted(move);

            // promoted piece is available
            if (promoted_piece)
            {
                // promoted to queen
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
                    // return legal move
                    return move;

                // promoted to rook
                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
                    // return legal move
                    return move;

                // promoted to bishop
                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
                    // return legal move
                    return move;

                // promoted to knight
                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
                    // return legal move
                    return move;

                // continue the loop on possible wrong promotions (e.g. "e7e8f")
                continue;
            }

            // return legal move
            return move;
        }
    }

    // return illegal move
    return 0;
}

/*
    Example UCI commands to init position on chess board

    // init start position
    position startpos

    // init start position and make the moves on chess board
    position startpos moves e2e4 e7e5

    // init position from FEN string
   position fen r3k2r/p1ppqpb 1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1

    // init position from fen string and make moves on chess board
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves e2a6 e8g8
*/

// parse UCI "position" command
void parse_position( const char* command)
{
    // shift pointer to the right where next token begins
    command += 9;

    // init pointer to the current character in the command string
   const  char* current_char = command;

    // parse UCI "startpos" command
    if (strncmp(command, "startpos", 8) == 0)
        // init chess board with start position
        parse_fen(start_position);

    // parse UCI "fen" command 
    else
    {
        // make sure "fen" command is available within command string
        current_char = strstr(command, "fen");

        // if no "fen" command is available within command string
        if (current_char == NULL)
            // init chess board with start position
            parse_fen(start_position);

        // found "fen" substring
        else
        {
            // shift pointer to the right where next token begins
            current_char += 4;

            // init chess board with position from FEN string
            parse_fen(current_char);
        }
    }

    // parse moves after position
    current_char = strstr(command, "moves");

    // moves available
    if (current_char != NULL)
    {
        // shift pointer to the right where next token begins
        current_char += 6;

        // loop over moves within a move string
        while (*current_char)
        {
            // parse next move
            int move = parse_move(current_char);

            // if no more moves
            if (move == 0)
                // break out of the loop
                break;

            // make move on the chess board
            make_move(move, all_moves);

            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;

            // go to the next move
            current_char++;
        }
    }

    // print board
    print_board();
}

/*
    Example UCI commands to make engine search for the best move

    // fixed depth search
    go depth 64

*/

// parse UCI "go" command
void parse_go( const char* command)
{
    // init depth
    int depth = -1;

    // init character pointer to the current depth argument
  const  char* current_depth = NULL;

    // handle fixed depth search
    if (current_depth = strstr(command, "depth"))
        //convert string to integer and assign the result value to depth
        depth = atoi(current_depth + 6);

    // different time controls placeholder
    else
        depth = 6;

    // search position
    search_position(depth);
}

/*
    GUI -> isready
    Engine -> readyok
    GUI -> ucinewgame
*/

// main UCI loop
void uci_loop()
{
    // reset STDIN & STDOUT buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // define user / GUI input buffer
    char input[2000];

    // print engine info
    printf("id name chess_engine \n");
    printf("id name Marwan ELattar \n");
    printf("uciok\n");

    // main loop
    while (1)
    {
        // reset user /GUI input
        memset(input, 0, sizeof(input));

        // make sure output reaches the GUI
        fflush(stdout);

        // get user / GUI input
        if (!fgets(input, 2000, stdin))
            // continue the loop
            continue;

        // make sure input is available
        if (input[0] == '\n')
            // continue the loop
            continue;

        // parse UCI "isready" command
        if (strncmp(input, "isready", 7) == 0)
        {
            printf("readyok\n");
            continue;
        }

        // parse UCI "position" command
        else if (strncmp(input, "position", 8) == 0)
            // call parse position function
            parse_position(input);

        // parse UCI "ucinewgame" command
        else if (strncmp(input, "ucinewgame", 10) == 0)
            // call parse position function
            parse_position("position startpos");

        // parse UCI "go" command
        else if (strncmp(input, "go", 2) == 0)
            // call parse go function
            parse_go(input);

        // parse UCI "quit" command
        else if (strncmp(input, "quit", 4) == 0)
            // quit from the chess engine program execution
            break;

        // parse UCI "uci" command
        else if (strncmp(input, "uci", 3) == 0)
        {
            // print engine info
            printf("id name BBC\n");
            printf("id name Code Monkey King\n");
            printf("uciok\n");
        }
    }
}

 int main() {

     init_all();

     int debug = 0;

     // if debugging
     if (debug)
     {
         // parse fen
         parse_fen(start_position);
         print_board();
         search_position(5);
     }

     else
         // connect to the GUI
         uci_loop();
 }



