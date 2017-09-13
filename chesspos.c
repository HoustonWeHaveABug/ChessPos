/* Solution to challenge https://www.reddit.com/r/dailyprogrammer/comments/6yu31a/20170908_challenge_330_hard_minichess_positions/ */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define SIDE_MIN 3
#define MEM_OFFSET 2
#define ROYAL_MOVES_N 8
#define ROOK_MOVES_N 4
#define BISHOP_MOVES_N 4
#define KNIGHT_MOVES_N 8
#define PAWN_MOVES_N 2
#define PIECES_N 17
#define COLORS_N 2
#define CHOICES_MAX (PIECES_N-4)
#define CHECKPOINT_DELTA 1000000000

typedef enum {
	COLOR_IDX_WHITE,
	COLOR_IDX_BLACK,
	COLOR_IDX_NONE
}
color_idx_t;

typedef enum {
	PIECE_TYPE_KING,
	PIECE_TYPE_QUEEN,
	PIECE_TYPE_ROOK,
	PIECE_TYPE_BISHOP,
	PIECE_TYPE_KNIGHT,
	PIECE_TYPE_PAWN,
	PIECE_TYPE_EMPTY,
	PIECE_TYPE_UNDEFINED,
	PIECE_TYPE_OUTSIDE
}
piece_type_t;

typedef struct {
	color_idx_t color_idx;
	piece_type_t type;
	int moves_n;
	int *moves;
	int repeat_move;
}
piece_t;

typedef struct {
	int row;
	int column;
	piece_t *piece;
}
square_t;

typedef struct {
	int pawn_moves[PAWN_MOVES_N];
	piece_t *piece_king;
	piece_t *piece_queen;
	piece_t *piece_rook;
	piece_t *piece_bishop;
	piece_t *piece_knight;
	piece_t *piece_pawn;
	square_t *square_king;
	int outcome;
	int in_check;
}
color_t;

void set_piece(piece_t *, color_idx_t, piece_type_t, int, int *, int);
void set_squares_row(int, piece_t *);
void set_square(square_t *, int, int, piece_t *);
int compare_squares(const void *, const void *);
void chesspos(int, int, int, int64_t);
void choose_square(int, int, int, int64_t, square_t *);
void choose_piece(int, int, int, int64_t, square_t *, piece_t *);
void set_outcome(square_t *, piece_t *, int *);
void set_check(square_t *, piece_t *, int *);
void update_positions_n(int64_t);

int ptr_columns_n, include_queens, include_rooks, include_bishops, include_knights, include_pawns, pieces_max, columns_n, royal_moves[ROYAL_MOVES_N], rook_moves[ROOK_MOVES_N], bishop_moves[BISHOP_MOVES_N], knight_moves[KNIGHT_MOVES_N], choices_n, full_outcome, ptr_squares_n;
int64_t positions_n, checkpoint;
piece_t pieces[PIECES_N], *piece_empty, *piece_undefined, *piece_outside, *piece_king_orth, *piece_king_diag, *choices[CHOICES_MAX];
square_t *squares, **ptr_squares;
color_t colors[COLORS_N];

int main(void) {
int ptr_rows_n, rows_n, squares_n, i, j;
square_t **ptr_square;
	if (scanf("%d%d%d%d%d%d%d%d", &ptr_rows_n, &ptr_columns_n, &include_queens, &include_rooks, &include_bishops, &include_knights, &include_pawns, &pieces_max) != 8 || ptr_rows_n < SIDE_MIN || ptr_columns_n < SIDE_MIN || pieces_max < 0 || pieces_max > ptr_rows_n*ptr_columns_n-COLORS_N) {
		fprintf(stderr, "Invalid parameters\n");
		return EXIT_FAILURE;
	}
	rows_n = MEM_OFFSET+ptr_rows_n+MEM_OFFSET;
	columns_n = MEM_OFFSET+ptr_columns_n+MEM_OFFSET;

	/* Define moves for each piece */
	royal_moves[0] = -1;
	royal_moves[1] = -columns_n-1;
	royal_moves[2] = -columns_n;
	royal_moves[3] = -columns_n+1;
	royal_moves[4] = 1;
	royal_moves[5] = columns_n+1;
	royal_moves[6] = columns_n;
	royal_moves[7] = columns_n-1;
	rook_moves[0] = -1;
	rook_moves[1] = -columns_n;
	rook_moves[2] = 1;
	rook_moves[3] = columns_n;
	bishop_moves[0] = -columns_n-1;
	bishop_moves[1] = -columns_n+1;
	bishop_moves[2] = columns_n+1;
	bishop_moves[3] = columns_n-1;
	knight_moves[0] = -columns_n-2;
	knight_moves[1] = -columns_n*2-1;
	knight_moves[2] = -columns_n*2+1;
	knight_moves[3] = -columns_n+2;
	knight_moves[4] = columns_n+2;
	knight_moves[5] = columns_n*2+1;
	knight_moves[6] = columns_n*2-1;
	knight_moves[7] = columns_n-2;
	colors[COLOR_IDX_WHITE].pawn_moves[0] = -columns_n-1;
	colors[COLOR_IDX_WHITE].pawn_moves[1] = -columns_n+1;
	colors[COLOR_IDX_BLACK].pawn_moves[0] = columns_n+1;
	colors[COLOR_IDX_BLACK].pawn_moves[1] = columns_n-1;

	/* Set pieces */
	set_piece(pieces, COLOR_IDX_WHITE, PIECE_TYPE_KING, ROYAL_MOVES_N, royal_moves, 0);
	colors[COLOR_IDX_WHITE].piece_king = pieces;
	set_piece(pieces+1, COLOR_IDX_BLACK, PIECE_TYPE_KING, ROYAL_MOVES_N, royal_moves, 0);
	colors[COLOR_IDX_BLACK].piece_king = pieces+1;
	set_piece(pieces+2, COLOR_IDX_WHITE, PIECE_TYPE_QUEEN, ROYAL_MOVES_N, royal_moves, 1);
	colors[COLOR_IDX_WHITE].piece_queen = pieces+2;
	set_piece(pieces+3, COLOR_IDX_BLACK, PIECE_TYPE_QUEEN, ROYAL_MOVES_N, royal_moves, 1);
	colors[COLOR_IDX_BLACK].piece_queen = pieces+3;
	set_piece(pieces+4, COLOR_IDX_WHITE, PIECE_TYPE_ROOK, ROOK_MOVES_N, rook_moves, 1);
	colors[COLOR_IDX_WHITE].piece_rook = pieces+4;
	set_piece(pieces+5, COLOR_IDX_BLACK, PIECE_TYPE_ROOK, ROOK_MOVES_N, rook_moves, 1);
	colors[COLOR_IDX_BLACK].piece_rook = pieces+5;
	set_piece(pieces+6, COLOR_IDX_WHITE, PIECE_TYPE_BISHOP, BISHOP_MOVES_N, bishop_moves, 1);
	colors[COLOR_IDX_WHITE].piece_bishop = pieces+6;
	set_piece(pieces+7, COLOR_IDX_BLACK, PIECE_TYPE_BISHOP, BISHOP_MOVES_N, bishop_moves, 1);
	colors[COLOR_IDX_BLACK].piece_bishop = pieces+7;
	set_piece(pieces+8, COLOR_IDX_WHITE, PIECE_TYPE_KNIGHT, KNIGHT_MOVES_N, knight_moves, 0);
	colors[COLOR_IDX_WHITE].piece_knight = pieces+8;
	set_piece(pieces+9, COLOR_IDX_BLACK, PIECE_TYPE_KNIGHT, KNIGHT_MOVES_N, knight_moves, 0);
	colors[COLOR_IDX_BLACK].piece_knight = pieces+9;
	set_piece(pieces+10, COLOR_IDX_WHITE, PIECE_TYPE_PAWN, PAWN_MOVES_N, colors[COLOR_IDX_WHITE].pawn_moves, 0);
	colors[COLOR_IDX_WHITE].piece_pawn = pieces+10;
	set_piece(pieces+11, COLOR_IDX_BLACK, PIECE_TYPE_PAWN, PAWN_MOVES_N, colors[COLOR_IDX_BLACK].pawn_moves, 0);
	colors[COLOR_IDX_BLACK].piece_pawn = pieces+11;
	set_piece(pieces+12, COLOR_IDX_NONE, PIECE_TYPE_EMPTY, 0, NULL, 0);
	piece_empty = pieces+12;
	set_piece(pieces+13, COLOR_IDX_NONE, PIECE_TYPE_UNDEFINED, 0, NULL, 0);
	piece_undefined = pieces+13;
	set_piece(pieces+14, COLOR_IDX_NONE, PIECE_TYPE_OUTSIDE, 0, NULL, 0);
	piece_outside = pieces+14;
	set_piece(pieces+15, COLOR_IDX_NONE, PIECE_TYPE_KING, ROOK_MOVES_N, rook_moves, 0);
	piece_king_orth = pieces+15;
	set_piece(pieces+16, COLOR_IDX_NONE, PIECE_TYPE_KING, BISHOP_MOVES_N, bishop_moves, 0);
	piece_king_diag = pieces+16;

	/* Set choices */
	choices[0] = colors[COLOR_IDX_WHITE].piece_king;
	choices[1] = colors[COLOR_IDX_BLACK].piece_king;
	choices_n = 2;
	if (include_queens) {
		choices[choices_n] = colors[COLOR_IDX_WHITE].piece_queen;
		choices[choices_n+1] = colors[COLOR_IDX_BLACK].piece_queen;
		choices_n += 2;
	}
	if (include_rooks) {
		choices[choices_n] = colors[COLOR_IDX_WHITE].piece_rook;
		choices[choices_n+1] = colors[COLOR_IDX_BLACK].piece_rook;
		choices_n += 2;
	}
	if (include_bishops) {
		choices[choices_n] = colors[COLOR_IDX_WHITE].piece_bishop;
		choices[choices_n+1] = colors[COLOR_IDX_BLACK].piece_bishop;
		choices_n += 2;
	}
	if (include_knights) {
		choices[choices_n] = colors[COLOR_IDX_WHITE].piece_knight;
		choices[choices_n+1] = colors[COLOR_IDX_BLACK].piece_knight;
		choices_n += 2;
	}
	if (include_pawns) {
		choices[choices_n] = colors[COLOR_IDX_WHITE].piece_pawn;
		choices[choices_n+1] = colors[COLOR_IDX_BLACK].piece_pawn;
		choices_n += 2;
	}
	choices[choices_n++] = piece_empty;

	/* Set full outcome */
	full_outcome = ROYAL_MOVES_N;
	if (include_knights) {
		full_outcome += KNIGHT_MOVES_N;
	}

	/* Set squares */
	squares_n = rows_n*columns_n;
	squares = malloc(sizeof(square_t)*(size_t)squares_n);
	if (!squares) {
		fprintf(stderr, "Could not allocate memory for squares\n");
		return EXIT_FAILURE;
	}
	for (i = 0; i < MEM_OFFSET; i++) {
		set_squares_row(i, piece_outside);
	}
	for (; i < MEM_OFFSET+ptr_rows_n; i++) {
		set_squares_row(i, piece_undefined);
	}
	for (; i < rows_n; i++) {
		set_squares_row(i, piece_outside);
	}

	/* Count chess positions */
	ptr_squares_n = ptr_rows_n*ptr_columns_n;
	ptr_squares = malloc(sizeof(square_t *)*(size_t)ptr_squares_n);
	if (!ptr_squares) {
		fprintf(stderr, "Could not allocate memory for ptr_squares\n");
		free(squares);
		return EXIT_FAILURE;
	}
	ptr_square = ptr_squares;
	for (i = MEM_OFFSET; i < MEM_OFFSET+ptr_rows_n; i++) {
		for (j = MEM_OFFSET; j < MEM_OFFSET+ptr_columns_n; j++) {
			*ptr_square = squares+i*columns_n+j;
			ptr_square++;
		}
	}
	qsort(ptr_squares, (size_t)ptr_squares_n, sizeof(square_t *), compare_squares);
	positions_n = 0;
	checkpoint = 0;
	colors[COLOR_IDX_WHITE].square_king = NULL;
	colors[COLOR_IDX_WHITE].outcome = 0;
	colors[COLOR_IDX_WHITE].in_check = 0;
	colors[COLOR_IDX_BLACK].square_king = NULL;
	colors[COLOR_IDX_BLACK].outcome = 0;
	colors[COLOR_IDX_BLACK].in_check = 0;
	chesspos(0, 0, 0, 1);
	printf("%" PRIi64 "\n", positions_n);
	free(ptr_squares);

	/* Free data and exit */
	free(squares);
	return EXIT_SUCCESS;
}

void set_piece(piece_t *piece, color_idx_t color_idx, piece_type_t type, int moves_n, int *moves, int repeat_move) {
	piece->color_idx = color_idx;
	piece->type = type;
	piece->moves_n = moves_n;
	piece->moves = moves;
	piece->repeat_move = repeat_move;
}

void set_squares_row(int row, piece_t *piece) {
int i;
	for (i = 0; i < MEM_OFFSET; i++) {
		set_square(squares+row*columns_n+i, row-MEM_OFFSET, i-MEM_OFFSET, piece_outside);
	}
	for (; i < MEM_OFFSET+ptr_columns_n; i++) {
		set_square(squares+row*columns_n+i, row-MEM_OFFSET, i-MEM_OFFSET, piece);
	}
	for (; i < columns_n; i++) {
		set_square(squares+row*columns_n+i, row-MEM_OFFSET, i-MEM_OFFSET, piece_outside);
	}
}

void set_square(square_t *square, int row, int column, piece_t *piece) {
	square->row = row;
	square->column = column;
	square->piece = piece;
}

int compare_squares(const void *a, const void *b) {
int sum_a, sum_b;
const square_t *square_a = *(square_t * const *)a, *square_b = *(square_t * const *)b;
	sum_a = square_a->row+square_a->column;
	sum_b = square_b->row+square_b->column;
	if (sum_a != sum_b) {
		return sum_a-sum_b;
	}
	else {
		return square_a->row-square_b->row;
	}
}

void chesspos(int ptr_square_idx, int kings_n, int pieces_n, int64_t positions_factor) {
	if (colors[COLOR_IDX_WHITE].in_check && colors[COLOR_IDX_BLACK].in_check) {
		return;
	}
	if (ptr_square_idx < ptr_squares_n) {
		if ((colors[COLOR_IDX_WHITE].outcome == full_outcome && !colors[COLOR_IDX_WHITE].in_check) || (colors[COLOR_IDX_BLACK].outcome == full_outcome && !colors[COLOR_IDX_BLACK].in_check)) {
			while (kings_n < COLORS_N) {
				positions_factor *= ptr_squares_n-ptr_square_idx;
				ptr_square_idx++;
				kings_n++;
			}
			while (ptr_square_idx < ptr_squares_n) {
				positions_factor *= choices_n-COLORS_N;
				ptr_square_idx++;
			}
			update_positions_n(positions_factor);
		}
		else {
			choose_square(ptr_square_idx, kings_n, pieces_n, positions_factor, ptr_squares[ptr_square_idx]);
		}
	}
	else {
		update_positions_n(positions_factor);
	}
}

void choose_square(int ptr_square_idx, int kings_n, int pieces_n, int64_t positions_factor, square_t *square) {
int choices_cur, i;
	if (ptr_square_idx+COLORS_N-kings_n == ptr_squares_n) {
		choices_cur = COLORS_N;
	}
	else {
		choices_cur = choices_n;
	}
	for (i = 0; i < choices_cur; i++) {
		choose_piece(ptr_square_idx, kings_n, pieces_n, positions_factor, square, choices[i]);
	}
}

void choose_piece(int ptr_square_idx, int kings_n, int pieces_n, int64_t positions_factor, square_t *square, piece_t *piece) {
int outcome[COLORS_N] = { 0 }, in_check[COLORS_N] = { 0 }, i;
square_t *target;
	if (piece->type == PIECE_TYPE_KING) {
		if (colors[piece->color_idx].square_king) {
			return;
		}
		colors[piece->color_idx].square_king = square;
		if (include_queens) {
			set_outcome(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_queen, &colors[piece->color_idx].outcome);
		}
		else {
			if (include_rooks) {
				set_outcome(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_rook, &colors[piece->color_idx].outcome);
			}
			else {
				set_outcome(square, piece_king_orth, &colors[piece->color_idx].outcome);
			}
			if (include_bishops) {
				set_outcome(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_bishop, &colors[piece->color_idx].outcome);
			}
			else {
				set_outcome(square, piece_king_diag, &colors[piece->color_idx].outcome);
			}
		}
		if (include_knights) {
			set_outcome(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_knight, &colors[piece->color_idx].outcome);
		}
		set_check(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_king, &colors[piece->color_idx].in_check);
		if (include_queens) {
			set_check(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_queen, &colors[piece->color_idx].in_check);
		}
		if (include_rooks) {
			set_check(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_rook, &colors[piece->color_idx].in_check);
		}
		if (include_bishops) {
			set_check(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_bishop, &colors[piece->color_idx].in_check);
		}
		if (include_knights) {
			set_check(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_knight, &colors[piece->color_idx].in_check);
		}
		if (include_pawns) {
			set_check(square, colors[COLOR_IDX_BLACK-piece->color_idx].piece_pawn, &colors[piece->color_idx].in_check);
		}
		kings_n++;
	}
	else if (piece != piece_empty) {
		if (pieces_n == pieces_max) {
			return;
		}
		pieces_n++;
	}
	square->piece = piece;
	for (i = 0; i < piece->moves_n; i++) {
		target = square+piece->moves[i];
		if (piece->repeat_move) {
			while (target->piece == piece_empty) {
				target += piece->moves[i];
			}
		}
		if (target->piece->type == PIECE_TYPE_KING) {
			outcome[target->piece->color_idx]++;
			if (target->piece->color_idx != piece->color_idx) {
				in_check[target->piece->color_idx]++;
			}
		}
	}
	for (i = 0; i < COLORS_N; i++) {
		colors[i].outcome += outcome[i];
		colors[i].in_check += in_check[i];
	}
	chesspos(ptr_square_idx+1, kings_n, pieces_n, positions_factor);
	for (i = 0; i < COLORS_N; i++) {
		colors[i].in_check -= in_check[i];
		colors[i].outcome -= outcome[i];
	}
	square->piece = piece_undefined;
	if (piece->type == PIECE_TYPE_KING) {
		colors[piece->color_idx].in_check = 0;
		colors[piece->color_idx].outcome = 0;
		colors[piece->color_idx].square_king = NULL;
	}
}

void set_outcome(square_t *square, piece_t *piece, int *outcome) {
int i;
square_t *target;
	for (i = 0; i < piece->moves_n; i++) {
		target = square-piece->moves[i];
		if (piece->repeat_move) {
			while (target->piece == piece_empty) {
				target -= piece->moves[i];
			}
		}
		if (target->piece != piece_undefined) {
			*outcome = *outcome+1;
		}
	}
}

void set_check(square_t *square, piece_t *piece, int *in_check) {
int i;
square_t *target;
	for (i = 0; i < piece->moves_n; i++) {
		target = square-piece->moves[i];
		if (piece->repeat_move) {
			while (target->piece == piece_empty) {
				target -= piece->moves[i];
			}
		}
		if (target->piece == piece) {
			*in_check = *in_check+1;
		}
	}
}

void update_positions_n(int64_t positions_factor) {
	positions_n += positions_factor;
	if (positions_n-checkpoint >= CHECKPOINT_DELTA) {
		checkpoint = positions_n;
		printf("Checkpoint %" PRIi64 "\n", checkpoint);
	}
}
