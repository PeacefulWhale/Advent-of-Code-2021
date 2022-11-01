# Advent of Code Day 23

Our little squid friends apparently have a Tower of Hanoi that they need me to solve, but on steroids? If it takes so much energy for some of these squids to move, how have they survived? And how big are these squids? Can I just move them myself so they don't waste any energy?

Something tells me that this elaborate problem might have more cultural origins than practical... Well, who am I to say no to a challenge?

## **Code Writeup**

We don't actually need to treat this like a map, because there are a limited number of possible positions that each squid could move to we can create this map of positions:

```text
Numbering:
#############
#X0 1 2 3 4Y#
###0#0#0#0###
  #1#1#1#1#
  #########
   0 1 2 3

X & Y are "Special Middles" that I've decided to call "pockets".
```

However with these positions there are still some more rules that we must follow:

- Squids may move twice, once to a middle, and once more to their final resting position. This means that:
  - Squids cannot enter rooms that are not their own.
  - Squids *can* stay in their room if it's their destination room.
- Squids cannot enter a room that has a squid that is not their letter.
- Squids cannot move through other squids.
  - Special Middles cannot block other squids, and hence have X / Y as their "position", but use 1 and 5 for as their position when calculating cost.

With this mapping it's not terribly difficult to calculate the cost.

I'll be honest I did cheat just a tiny bit, and I looked at what part B was before I finished my code for part A, so I already know by now that we'll be given a case with "deeper" burrows. With this in mind I had to create a system that would with any input size. It should also work with "wider" burrows. Up to a width of 26 at least, because I've only made it work with squids from 'A' to 'Z'.

So I decided that an A* path finding algorithm and treating each possible game state as "nodes" might be best, with a heuristic equal to every squid immediately moving into their first room (going through other squids and stacking up as needed). I could have done more, like making the heuristic factor in squids that need to move out so squids behind them can move, or squids that need to move deeper into the burrow than just the first room. However I decided to see if my current heuristic functioned as is.

Originally I had it so squids that had already moved turned into lowercase squids, but then I realized by following the rules laid out above, this was never even a problem. That being said, I kept in the "checking" for lowercase squids just in case the input got messed up.

I also added two "print" functions. One can print the current game state and the full history (if `KEEPHISTORY` is set to `1`) or the current queue. I used this while debugging (because it's makes visualizing both of those easier, especially as my current debugger makes me click through every linked variable to see their contents). It's also helpful to see the exact solution.

I could have probably used more recursion, but to be honest with a priority queue running through a loop just made more sense to me, especially as we're just looping through the top of it. Sure recursion might have made it look nicer but not by much. I doubt performance is impacted either way.

In the end my code solves part A (on my input) in 2 seconds, and part B in 4. It's definetly not the fasted code I've seen, but I think it runs quick enough for my purposes. It's also possibly just my input, as my code solves the example problem much faster.
