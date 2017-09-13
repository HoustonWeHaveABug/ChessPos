# ChessPos

Solution to challenge https://www.reddit.com/r/dailyprogrammer/comments/6yu31a/20170908_challenge_330_hard_minichess_positions/.

Search the number of valid positions on a chessboard such that:
- Both kings must be on the board (and there can only be one of each color)
- Not both kings can be in check

The pawns may be placed anywhere on the board (including first and last rows).

The program takes 8 arguments on standard input:
- Number of rows (>= 3)
- Number of columns (>= 3)
- Include queens (0/1)
- Include rooks (0/1)
- Include bishops (0/1)
- Include knights (0/1)
- Include pawns (0/1)
- Maximum number of pieces allowed on the board (>= 0, excluding kings)

The program performs a top/down diagonal search, it backtracks when both kings are in check and computes the number of valid positions for the remaining squares when at least one king is safe.

Sample input files are included in the repository.
