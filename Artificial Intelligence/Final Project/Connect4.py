# Code from tutorial at https://www.youtube.com/watch?v=XpYz-q1lxu8&t=812s

import numpy as np

ROW_COUNT = 6
COLUMN_COUNT = 7


def create_board():
    board = np.zeros((ROW_COUNT, COLUMN_COUNT))
    return board


def drop_piece(board, row, col, piece):
    board[row][col] = piece


def is_valid_location(board, col):
    return board[ROW_COUNT - 1][col] == 0


def get_next_open_row(board, col):
    for r in range(ROW_COUNT):
        if board[r][col] == 0:
            return r


def print_board(board):
    print(np.flip(board, 0))


def winning_move(board, piece):

    # check for horizontal win
    for col in range(COLUMN_COUNT-3):
        for row in range(ROW_COUNT):
            if board[row][col] == piece and board[row][col+1] == piece and board[row][col+2] == piece and board[row][col+3] == piece:
                return True

    # check for vertical win
    for col in range(COLUMN_COUNT-3):
        for row in range(ROW_COUNT):
            if board[row][col] == piece and board[row+1][col] == piece and board[row+2][col] == piece and board[row+3][col] == piece:
                return True

    # check for positively-sloped diagonal win
    for col in range(COLUMN_COUNT-3):
        for row in range(ROW_COUNT-3):
            if board[row][col] == piece and board[row+1][col+1] == piece and board[row+2][col+2] == piece and board[row+3][col+3] == piece:
                return True

    for col in range(COLUMN_COUNT-3):
        for row in range(3, ROW_COUNT):
            if board[row][col] == piece and board[row-1][col+1] == piece and board[row-2][col+2] == piece and board[row-3][col+3] == piece:
                return True


board = create_board()
game_over = False
turn = 0

while not game_over:
    col = None
    # ask for player 1 input
    if turn == 0:
        while (col is None or col > 7 or col < 1):
            col = int(input("Player 1 Make your Selection (1-7): "))

        if is_valid_location(board, col-1):
            row = get_next_open_row(board, col-1)
            drop_piece(board, row, col-1, 1)

            if winning_move(board, 1):
                print_board(board)
                print("Player 1 wins!")
                game_over = True
                break

    # ask for player 2 input
    else:
        while (col is None or col > 7 or col < 1):
            col = int(input("Player 2 Make your Selection (1-7): "))

        if is_valid_location(board, col-1):
            row = get_next_open_row(board, col-1)
            drop_piece(board, row, col-1, 2)

            if winning_move(board, 2):
                print_board(board)
                print("Player 2 wins!")
                game_over = True
                break
            

    print_board(board)
    if turn == 1:
        turn = 0
    else:
        turn = 1