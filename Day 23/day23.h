#include <stdlib.h>
#include <stdio.h>

#define ABS(x) (((x) > 0) ? (x) : ((x) * -1))
#define EMPTY '.'

// Used to determine if we want to print the whole path we took.
#define KEEPHISTORY 0

typedef struct State
{
    char **rooms;
    char *hallway;
    char pockets[2];
    int cost, heuristic;
    struct State *lastState;
} State;

typedef struct Queue
{
    State *state;
    struct Queue *next;
} Queue;

void printQueue(Queue *q);
int validMove(int length, int depth, int room, int roomDepth, int pocket, int hallway, State *state);
void roomToHallMove(int length, int depth, Queue *mainQueue);
void hallToRoomMove(int length, int depth, Queue *mainQueue);
int gameWon(State *restrict state, int length, int depth);
unsigned int calculateHeuristic(State *restrict state, int length, int depth);
unsigned int hallToRoomDistance(int hallway, int room);
Queue *newQueue(State *restrict state);
void insert(Queue *restrict q, Queue *restrict mainQueue, int length, int depth);
void freeQueue(Queue *restrict q, int length, int depth);
char getNextSquid(char *restrict buffer, int *restrict buffIndex);
void printState(State *restrict state, int length, int depth, int stateDepth);
State *newState(int length, int depth, State *restrict copy, State *restrict last);
void freeState(int length, int depth, State *restrict state);
int squidMultiplier(char squid);
int squidNum(char squid);

void printQueue(Queue *q)
{
    int i = 0;
    while(q->next != NULL)
    {
        printf("%d: %d\n", i++, q->state->cost + q->state->heuristic);
        q = q->next;
    }
}
// Check to see if the move is valid.
int validMove(int length, int depth, int room, int roomDepth, int pocket, int hallway, State *state)
{
    char squid;
    if (roomDepth == -1)
    {
        if (pocket == 1)
        {
            squid = state->pockets[0];
        }
        else if (pocket == 2)
        {
            squid = state->pockets[1];
        }
        else
        {
            squid = state->hallway[hallway];
        }
    }
    else
    {
        // Just quickly check to see if the target hallway is empty.
        if (state->hallway[hallway] != EMPTY)
        {
            return 0;
        }
        squid = state->rooms[room][roomDepth];
    }
    // Just in case we somehow passed an empty room...
    if (squidNum(squid) == -1)
    {
        return 0;
    }
    // Check to see if the squid can exit the room.
    for (int i = roomDepth - 1; i >= 0; i--)
    {
        if (state->rooms[room][i] != EMPTY)
        {
            return 0;
        }
    }
    // Check to see if the hallway is clear.
    int direction;
    int hallCheck;
    // We have to account for the rooms and hallway numbers not matching up perfectly...
    if (room >= hallway)
    {
        direction = -1;
        hallCheck = room;
    }
    else
    {
        direction = 1;
        hallCheck = room + 1;
    }
    for (; hallCheck != hallway; hallCheck += direction)
    {
        if (state->hallway[hallCheck] != EMPTY)
        {
            return 0;
        }
    }
    // Check to see if the squid can move into it's room.
    if (roomDepth == -1)
    {
        for (int check = 0; check < depth; check++)
        {
            char checkSquid = state->rooms[room][check];
            if (checkSquid != EMPTY)
            {
                if (squidNum(checkSquid) != room)
                {
                    return 0;
                }
            }
        }
    }
    // Everything is good!
    return 1;
}

void roomToHallMove(int length, int depth, Queue *mainQueue)
{
    for (int room = 0; room < length; room++)
    {
        for (int roomDepth = 0; roomDepth < depth; roomDepth++)
        {
            char squid = mainQueue->state->rooms[room][roomDepth];
            if (squid != EMPTY)
            {
                if (squidNum(squid) == room)
                {
                    int valid = 0;
                    // This squid is in it's room, but what about everything behind it?
                    for (int check = roomDepth + 1; check < depth; check++)
                    {
                        if (squidNum(mainQueue->state->rooms[room][check]) != room)
                        {
                            // There is a squid that must move!
                            valid = 1;
                            break;
                        }
                    }
                    if (!valid)
                    {
                        // No squids that need to move out.
                        break;
                    }
                }
                // If we've gotten here, then this squid needs to move!
                for (int hallway = 0; hallway < length + 1; hallway++)
                {
                    if (validMove(length, depth, room, roomDepth, 0, hallway, mainQueue->state))
                    {
                        int distance = roomDepth;
                        distance += hallToRoomDistance(hallway, room);
                        State *newS = newState(length, depth, mainQueue->state, mainQueue->state);
                        newS->rooms[room][roomDepth] = EMPTY;
                        newS->hallway[hallway] = squid;
                        newS->cost += squidMultiplier(squid) * distance;
                        newS->heuristic = calculateHeuristic(newS, length, depth);
                        Queue *newQ = newQueue(newS);
                        insert(newQ, mainQueue, length, depth);
                        if (hallway == 0 && mainQueue->state->pockets[0] == EMPTY)
                        {
                            // Move into the left pocket.
                            distance += 1;
                            newS = newState(length, depth, mainQueue->state, mainQueue->state);
                            newS->rooms[room][roomDepth] = EMPTY;
                            newS->pockets[0] = squid;
                            newS->cost += squidMultiplier(squid) * distance;
                            newS->heuristic = calculateHeuristic(newS, length, depth);
                            newQ = newQueue(newS);
                            insert(newQ, mainQueue, length, depth);
                        }
                        else if (hallway == length && mainQueue->state->pockets[1] == EMPTY)
                        {
                            // Move into the right pocket.
                            distance += 1;
                            newS = newState(length, depth, mainQueue->state, mainQueue->state);
                            newS->rooms[room][roomDepth] = EMPTY;
                            newS->pockets[1] = squid;
                            newS->cost += squidMultiplier(squid) * distance;
                            newS->heuristic = calculateHeuristic(newS, length, depth);
                            newQ = newQueue(newS);
                            insert(newQ, mainQueue, length, depth);
                        }
                    }
                }
            }
        }
    }
}

void hallToRoomMove(int length, int depth, Queue *mainQueue)
{
    for (int hallway = 0; hallway < length + 1; hallway++)
    {
        if (mainQueue->state->hallway[hallway] != EMPTY ||
            (hallway == 0 && mainQueue->state->pockets[0] != EMPTY) ||
            (hallway == length && mainQueue->state->pockets[1] != EMPTY))
        {
            char squid = mainQueue->state->hallway[hallway];
            int pocket = 0;
            if (squid == EMPTY)
            {
                if (hallway == 0)
                {
                    squid = mainQueue->state->pockets[0];
                    pocket = 1;
                }
                else
                {
                    squid = mainQueue->state->pockets[1];
                    pocket = 2;
                }
            }
            int room = squidNum(squid);
            int distance = 0;
            if (!validMove(length, depth, room, -1, pocket, hallway, mainQueue->state))
            {
                continue;
            }
            // The squid can move into it's room! Send it all the way in!
            int roomDepth = 0;
            while (roomDepth < depth - 1 && mainQueue->state->rooms[room][roomDepth + 1] == EMPTY)
            {
                roomDepth++;
            }
            // Add this move to the Queue!
            distance += roomDepth;
            distance += hallToRoomDistance(hallway, room);
            State *newS = newState(length, depth, mainQueue->state, mainQueue->state);
            if (pocket == 1)
            {
                newS->pockets[0] = EMPTY;
                distance += 1;
            }
            else if (pocket == 2)
            {
                newS->pockets[1] = EMPTY;
                distance += 1;
            }
            else
            {
                newS->hallway[hallway] = EMPTY;
            }
            newS->rooms[room][roomDepth] = squid;
            newS->cost += squidMultiplier(squid) * distance;
            newS->heuristic = calculateHeuristic(newS, length, depth);
            Queue *newQ = newQueue(newS);
            insert(newQ, mainQueue, length, depth);
        }
    }
}

int gameWon(State *restrict state, int length, int depth)
{
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < depth; j++)
        {
            if (state->rooms[i][j] == EMPTY || squidNum(state->rooms[i][j]) != i)
            {
                return 0;
            }
        }
    }
    return 1;
}

unsigned int calculateHeuristic(State *restrict state, int length, int depth)
{
    unsigned int heuristic = 0;
    // Anything in the left pocket?
    if (state->pockets[0] != EMPTY)
    {
        heuristic += squidMultiplier(state->pockets[0]) * (hallToRoomDistance(0, squidNum(state->pockets[0])) + 1);
    }
    // Anything in the right pocket?
    if (state->pockets[1] != EMPTY)
    {
        heuristic += squidMultiplier(state->pockets[1]) * (hallToRoomDistance(length, squidNum(state->pockets[1])) + 1);
    }
    // Squids in hallways moving to rooms.
    for (int i = 0; i < length + 1; i++)
    {
        if (state->hallway[i] != EMPTY)
        {
            heuristic += squidMultiplier(state->hallway[i]) * hallToRoomDistance(i, squidNum(state->hallway[i]));
        }
    }
    // Squids in rooms moving to rooms.
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < depth; j++)
        {
            if (state->rooms[i][j] != EMPTY)
            {
                char squid = state->rooms[i][j];
                if (squidNum(squid) != i)
                {
                    heuristic += squidMultiplier(squid) * (j + 2 + hallToRoomDistance(i + (i < squidNum(squid) ? 1 : 0), squidNum(squid)));
                }
            }
        }
    }
    return heuristic;
}

unsigned int hallToRoomDistance(int hallway, int room)
{
    if (hallway <= room)
    {
        return (ABS(hallway - room) * 2) + 2;
    }
    else
    {
        return (ABS(hallway - room) * 2);
    }
}

Queue *newQueue(State *restrict state)
{
    Queue *new;
    new = malloc(sizeof * new);
    new->state = state;
    new->next = NULL;
    return new;
}

// Inserts q into mainQueue.`
void insert(Queue *restrict q, Queue *restrict mainQueue, int length, int depth)
{
    Queue *position = mainQueue;
    while (position->next != NULL &&
    (q->state->cost + q->state->heuristic) >= (position->next->state->cost + position->next->state->heuristic))
    {
        State *a = q->state;
        State *b = position->state;
        position = position->next;
        // If it's identical to the current node, delete it.
        // Check heuristics (not cost, as you can get to the same place with different moves).
        if (a->heuristic == b->heuristic)
        {
            // Check Pockets
            if (a->pockets[0] == b->pockets[0] && a->pockets[1] == b->pockets[1])
            {
                int equal = 1;
                // Check hallways.
                for (int i = 0; i < length + 1 && equal; i++)
                {
                    if (a->hallway[i] != b->hallway[i])
                    {
                        // They're not equal.
                        equal = 0;
                        break;
                    }
                }
                // Check rooms.
                for (int i = 0; i < length && equal; i++)
                {
                    for (int j = 0; j < depth && equal; j++)
                    {
                        if (a->rooms[i][j] != b->rooms[i][j])
                        {
                            // They're not equal.
                            equal = 0;
                            break;
                        }
                    }
                }
                if (equal)
                {
                    // If we get here they are equal. Delete this node!
                    freeQueue(q, length, depth);
                    return;
                }
            }
        }
    }
    // Insert q into the Queue.
    q->next = position->next;
    position->next = q;
    return;
}

void freeQueue(Queue *restrict q, int length, int depth)
{
    if (q)
    {
        freeState(length, depth, q->state);
        freeQueue(q->next, length, depth);
        free(q);
        q = NULL;
    }
}

// Returns the next squid character.
char getNextSquid(char *restrict buffer, int *restrict buffIndex)
{
    // Get the next squid friend.
    while (buffer[*buffIndex] < 'A' || buffer[*buffIndex] > 'Z')
    {
        (*buffIndex)++;
    }
    return buffer[*buffIndex];
}

void printState(State *restrict state, int length, int depth, int stateDepth)
{
    printf("\nDepth: %d, Cost: %d, Heuristic: %d\n", stateDepth, state->cost, state->heuristic);
    printf("#####");
    for (int i = 0; i < length; i++)
    {
        printf("##");
    }
    printf("\n#%c", state->pockets[0]);
    for (int i = 0; i < length + 1; i++)
    {
        printf("%c%c", state->hallway[i], EMPTY);
    }
    printf("\b%c#\n", state->pockets[1]);
    for (int j = 0; j < depth; j++)
    {
        printf("##");
        for (int i = 0; i < length; i++)
        {
            printf("#%c", state->rooms[i][j]);
        }
        printf("###\n");
    }
    printf("#####");
    for (int i = 0; i < length; i++)
    {
        printf("##");
    }
    printf("\n");
    if (state->lastState)
    {
        printState(state->lastState, length, depth, stateDepth + 1);
    }
}

State *newState(int length, int depth, State *restrict copy, State *restrict last)
{
    State *new;
    // Copy the last state chain (if it exists).
    new = malloc(sizeof * new);
    if (last && KEEPHISTORY)
    {
        new->lastState = newState(length, depth, last, last->lastState);
    }
    else
    {
        new->lastState = NULL;
    }
    new->rooms = malloc((sizeof * new->rooms) * length);
    for (int i = 0; i < length; i++)
    {
        new->rooms[i] = malloc((sizeof * new->rooms[i]) * depth);
    }
    new->hallway = malloc((sizeof * new->hallway) * (length + 1));

    // Copy the given state to this new state.
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < depth; j++)
        {
            if (copy)
            {
                new->rooms[i][j] = copy->rooms[i][j];
            }
            else
            {
                new->rooms[i][j] = EMPTY;
            }
        }
    }
    for (int i = 0; i < length + 1; i++)
    {
        if (copy)
        {
            new->hallway[i] = copy->hallway[i];
        }
        else
        {
            new->hallway[i] = EMPTY;
        }
    }
    if (copy)
    {
        new->pockets[0] = copy->pockets[0];
        new->pockets[1] = copy->pockets[1];
        new->cost = copy->cost;
        new->heuristic = copy->heuristic;
    }
    else
    {
        new->pockets[0] = EMPTY;
        new->pockets[1] = EMPTY;
        new->cost = 0;
        new->heuristic = 0;
    }

    return new;
}

void freeState(int length, int depth, State *restrict state)
{
    if (state)
    {
        for (int i = 0; i < length; i++)
        {
            free(state->rooms[i]);
        }
        free(state->rooms);
        free(state->hallway);
        if (state->lastState)
        {
            freeState(length, depth, state->lastState);
        }
        free(state);
        state = NULL;
    }
}

// Returns the cost to move the squid.
int squidMultiplier(char squid)
{
    int cost = 1;
    int power = squidNum(squid);
    while (power-- > 0)
    {
        cost *= 10;
    }
    return cost;
}

// The squid's "number".
int squidNum(char squid)
{
    if (squid >= 'a' && squid <= 'z')
    {
        return squid - 'a';
    }
    else if (squid >= 'A' && squid <= 'Z')
    {
        return squid - 'A';
    }
    else
    {
        return -1;
    }
}
