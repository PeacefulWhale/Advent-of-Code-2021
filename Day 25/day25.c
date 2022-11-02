#include <stdlib.h>
#include <stdio.h>

#define PRINT 0

unsigned long simulateBoard(char *board, int width, int height);
void printBoard(char *board, int width, int height);
void moveRight(char *board, int width, int height);
void moveDown(char *board, int width, int height);
void resetBoard(char *board, int width, int height);
int canMove(char *board, int width, int height);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Not enough arguments!\n");
        return -1;
    }
    int fileSize;
    char *restrict buffer;
    FILE *input;
    input = fopen(argv[1], "r");
    fileSize = 0;
    fseek(input, 0, SEEK_END);
    fileSize = ftell(input);
    fseek(input, 0, SEEK_SET);
    buffer = malloc((sizeof * buffer) * (fileSize + 1));
    fread(buffer, fileSize, 1, input);
    fclose(input);
    // Parse input.
    int width = 0, height;
    while (buffer[++width] != '\n');
    height = fileSize / (width + 1);
    // Get the board state.
    char *board;
    board = malloc((sizeof * board) * (height * width));
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // + y for the new lines.
            board[(y * width) + x] = buffer[(y * width) + x + y];
        }
    }
    free(buffer);
    // Run the simulation.
    printf("Simulating Board:\n");
    printBoard(board, width, height);
    unsigned long partA = simulateBoard(board, width, height);
    printf("Part A Solution: %lu\n", partA + 1);
    return 0;
}

int canMove(char *board, int width, int height)
{
    for (int y = height - 1; y >= 0; y--)
    {
        for (int x = width - 1; x >= 0; x--)
        {
            if (board[(y * width) + x] == '>')
            {
                int xPos = (x == width - 1) ? 0 : x + 1;
                if (board[(y * width) + xPos] == '.')
                {
                    return 1;
                }
            }
            if (board[(y * width) + x] == 'v')
            {
                int yPos = (y == height - 1) ? 0 : y + 1;
                if (board[(yPos * width) + x] == '.')
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void moveRight(char *board, int width, int height)
{
    // Squids moving right.
    for (int y = height - 1; y >= 0; y--)
    {
        for (int x = width - 1; x >= 0; x--)
        {
            if (board[(y * width) + x] == '>')
            {
                int xPos = (x == width - 1) ? 0 : x + 1;
                // See if this squid can move.
                if (board[(y * width) + xPos] == '.')
                {
                    board[(y * width) + xPos] = 'r';
                    board[(y * width) + x] = 'X';
                }
            }
        }
    }
}

void moveDown(char *board, int width, int height)
{
    // Squids moving down.
    for (int y = height - 1; y >= 0; y--)
    {
        for (int x = width - 1; x >= 0; x--)
        {
            if (board[(y * width) + x] == 'v')
            {
                int yPos = (y == height - 1) ? 0 : y + 1;
                // See if this squid can move.
                if (board[(yPos * width) + x] == '.')
                {
                    board[(yPos * width) + x] = 'd';
                    board[(y * width) + x] = 'X';
                }
            }
        }
    }
}

void resetBoard(char *board, int width, int height)
{
    // Go through and replace the moved squids / squares.
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (board[(y * width) + x] == 'X')
            {
                board[(y * width) + x] = '.';
            }
            else if (board[(y * width) + x] == 'r')
            {
                board[(y * width) + x] = '>';
            }
            else if (board[(y * width) + x] == 'd')
            {
                board[(y * width) + x] = 'v';
            }
        }
    }
}

unsigned long simulateBoard(char *board, int width, int height)
{
    unsigned long moves = 0;
    while (canMove(board, width, height))
    {
        moves++;
        moveRight(board, width, height);
        resetBoard(board, width, height);
        moveDown(board, width, height);
        resetBoard(board, width, height);
        if (PRINT)
        {
            printf("Move %lu:\n", moves);
            printBoard(board, width, height);
        }
    }
    return moves;
}

void printBoard(char *board, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            printf("%c", board[(y * width) + x]);
        }
        printf("\n");
    }
    printf("\n");
}
