#include <stdlib.h>
#include <stdio.h>
// Helper functions here to help clean stuff up.
#include "day23.h"

// Progresses down the path from the current state using an A* algorithm.
int pathFind(int length, int depth, State *startState);

int main(int argc, char **argv)
{
    int buffIndex = 0, fileSize;
    char *restrict buffer;
    FILE *input;
    input = fopen(argv[1], "r");
    fileSize = 0;
    fseek(input, 0, SEEK_END);
    fileSize = ftell(input);
    fseek(input, 0, SEEK_SET);
    buffer = malloc((sizeof * buffer) * (fileSize + 1));
    if (buffer == NULL)
    {
        printf("Could Not Allocate Memory\n");
        exit(-1);
    }
    fread(buffer, fileSize, 1, input);
    fclose(input);
    unsigned int length, depth;
    length = depth = 0;

    // Length is equal to the (length of the first line - 4) / 2. Rounded down.
    while (buffer[length++] != '\n')
    {
        continue;
    }
    length = (length - 5) / 2;

    // Depth = number of new lines - 3.
    while (buffIndex < fileSize)
    {
        if (buffer[buffIndex] == '\n')
        {
            depth++;
        }
        buffIndex++;
    }
    depth -= 2;
    buffIndex = 0;

    // Create initial state.
    State *startState = newState(length, depth, NULL, NULL);
    // Read our squids into the state.
    for (int j = 0; j < depth; j++)
    {
        for (int i = 0; i < length; i++)
        {
            startState->rooms[i][j] = getNextSquid(buffer, &buffIndex);
            buffIndex++;
        }
    }
    // We don't need our buffer anymore.
    free(buffer);

    // Part A (and B I guess):
    printf("Solving With:");
    startState->heuristic = calculateHeuristic(startState, length, depth);
    printState(startState, length, depth, 0);
    int cost = pathFind(length, depth, newState(length, depth, startState, NULL));
    printf("Cost: %d\n", cost);
    // Free all our stuff.
    freeState(length, depth, startState);
    return 0;
}

int pathFind(int length, int depth, State *startState)
{
    // The current best score.
    int score = 0;
    Queue *mainQueue = newQueue(startState);

    // Recursion? Why? Cool kids use looooooops.
    // This is a priority queue so we just go from the top until there is nothing left.
    while (!score)
    {
 
        // Everything in the hallways moving to their rooms.=
        hallToRoomMove(length, depth, mainQueue);
        // Everything in the rooms moving to hallways.
        roomToHallMove(length, depth, mainQueue);
        // Cut off this segment.

        Queue *temp = mainQueue;
        mainQueue = mainQueue->next;
        temp->next = NULL;
        if (gameWon(temp->state, length, depth))
        {
            score = temp->state->cost;
        }
        freeQueue(temp, length, depth);
    }
    printState(mainQueue->state, length, depth, 0);
    freeQueue(mainQueue, length, depth);
    return score;
}

