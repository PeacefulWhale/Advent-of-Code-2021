# Advent of Code Day 24

Oh dear, it appears I've fried something in my attempts to descend further into the abyss.

Apparently this is critical for Christmas lights, oxygen, and engines.

We can't leave the Christmas lights off! And the other things are probably important too, but tbh I'm no sailor so maybe they're not.

Thankfully I apparently don't actually have to rebuild a computational unit in this now poorly lit submarine, and can instead just enter the fourteen-digit identification number that it was calculating when it up in flames.

If I die because this submarine is DRM locked... I will not be very happy.

## **Code Writeup**

My first attempt was just brute forcing the number. I wrote a python script to write C code because I was too lazy to do it in a regex. It's not optimal by far.

TL;DR: Way too slow...

Obviously there has to be a better way...

I noticed something when I was staring at this code, which is that the code repeats.

Not only that, but it appears to repeat in a such a way that I might be able to calculate digits individually, or in a smaller amount of digits than the *entire* number.

| Differs | Digit 1   | Digit 2   | Digit 3   | Digit 4   | Digit 5   | Digit 6   | Digit 7   | Digit 8   | Digit 9   | Digit 10  | Digit 11  | Digit 12  | Digit 13  | Digit 14  |
|   ---   |    ---    |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |
|         | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     | inp w     |
|         | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   | mul x 0   |
|         | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   | add x z   |
|         | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  | mod x 26  |
|   YES   | div z 1   | div z 1   | div z 1   | div z 1   | div z 1   | div z 26  | div z 1   | div z 26  | div z 26  | div z 1   | div z 26  | div z 26  | div z 26  | div z 26  |
|   YES   | add x 10  | add x 12  | add x 10  | add x 12  | add x 11  | add x -16 | add x 10  | add x -11 | add x -13 | add x 13  | add x -8  | add x -1  | add x -4  | add x -14 |
|         | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   | eql x w   |
|         | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   | eql x 0   |
|         | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   |
|         | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  | add y 25  |
|         | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   |
|         | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   | add y 1   |
|         | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   | mul z y   |
|         | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   | mul y 0   |
|         | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   | add y w   |
|   YES   | add y 12  | add y 7   | add y 8   | add y 8   | add y 15  | add y 12  | add y 8   | add y 13  | add y 3   | add y 13  | add y 3   | add y 9   | add y 4   | add y 13  |
|         | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   | mul y x   |
|         | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   | add z y   |

I can generate the following pseudo-code for this (my brain is smooth so this took me a little bit):

```txt
z = z / a
if (((z % 26) + b) != digit)
{
    z = z * 26
    z = z + (digit + c)
}
return z
```

Here are the "parameters":

| Parameters | Digit 1   | Digit 2   | Digit 3   | Digit 4   | Digit 5   | Digit 6   | Digit 7   | Digit 8   | Digit 9   | Digit 10  | Digit 11  | Digit 12  | Digit 13  | Digit 14  |
|   ---      |    ---    |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |   ---     |
|    A       | 1         | 1         |  1        |  1        |  1        |   26      | 1         | 26        | 26        |  1        |  26       |  26       |  26       |  26       |
|    B       | 10        | 12        |  10       |  12       |  11       |   -16     | 10        | -11       | -13       |  13       |  -8       |  -1       |  -4       |  -14      |
|    C       | 12        | 7         |  8        |  8        |  15       |   12      | 8         | 13        | 3         |  13       |  3        |  9        |  4        |  13       |

Parameter A is interesting to me, as it essentially determines whether I multiplies or divide our `z` value by `26`. And the only reason why I can end up with a `z` equal to `0` is because I've got 7 `a = 1` and 7 `a = 26`.

And this is also doing that whole `if (((z % 26) + b) != digit)`. The number `26` seems to be pretty important... And as I multiply by `26` (or divide by `26`) this means that I can essentially ignore what `z` actually is, and look at `z % 26`. If I treat `z` like a base `26` number, I can imagine this like a stack that I push and pop variables off of, depending on whether or not `a` is `26` or `1`. If `a` is `26`, I push my `digit + c`, and if `a` is `1` I pop the value (but don't use it, as it gets sent to the void).

So I should be able to match up my pushes and pops to figure out what values are actually acceptable, and then generate the largest value I can from that information...

I'm going to see if I can do this by hand before trying to write code to solve it.

```txt
Digit 1,  a = 1  ---------┐
Digit 2,  a = 1  -------┐ |
Digit 3,  a = 1  -----┐ | |
Digit 4,  a = 1  ---┐ | | |
Digit 5,  a = 1  \  | | | |
Digit 6,  a = 26 /  | | | |
Digit 7,  a = 1  \  | | | |
Digit 8,  a = 26 /  | | | |
Digit 9,  a = 26 ---┘ | | |
Digit 10, a = 1  \    | | |
Digit 11, a = 26 /    | | |
Digit 12, a = 26 -----┘ | |
Digit 13, a = 26 -------┘ |
Digit 14, a = 26 ---------┘
```

Without the visually pleasing image I spent too much time trying to figure out what unicode to use, the links are as follows:

```txt
Push <-> Pop
   1 <-> 14
   2 <-> 13
   3 <-> 12
   4 <-> 9
   5 <-> 6
   7 <-> 8
  10 <-> 11
```

Given how each number is linked up, and the pseudo-code of how `z` is calculated, I can write a function for each digit.

$$\text{Push}: \text{digit}_x + c = z$$
$$\text{Pop}: z + b = \text{digit}_y$$

With the above I can write a formula that defines the relationship between the two digits:

$$\text{digit}_x + c + b = \text{digit}_y$$

This is because when I push, I want `(((z % 26) + b) != digit)` to be true, and when I pop I want it to be false. Sense the number from when I push is `digit + c`, I do $x + c$. And sense `z + b` must be equal to `digit` when I pop, I also do $z + b = y$.

This means that the offset between the two numbers must be $C + B$, and the two numbers must also be between $1$ and $9$. This limits my options.

I could write a program to generate the correct numbers, or I could just do that by hand because this is pretty simple at this point.

I know that part B is to find the smallest number, so I'll just do both right now.

| Link      | + C + B | Offset |
|-----------|---------|--------|
| 1 <-> 14  | 12 - 14 | -2     |
| 2 <-> 13  | 7 - 4   | 3      |
| 3 <-> 12  | 8 - 1   | 7      |
| 4 <-> 9   | 8 - 13  | -5     |
| 5 <-> 6   | 15 - 16 | -1     |
| 7 <-> 8   | 8 - 11  | -3     |
| 10 <-> 11 | 13 - 8  | 4      |

With this I can solve for the digits:

| Digit | Largest | Smallest |
|-------|---------|----------|
| 1     | 9       | 3        |
| 2     | 6       | 1        |
| 3     | 2       | 1        |
| 4     | 9       | 6        |
| 5     | 9       | 2        |
| 6     | 8       | 1        |
| 7     | 9       | 4        |
| 8     | 6       | 1        |
| 9     | 4       | 1        |
| 10    | 4       | 1        |
| 11    | 9       | 6        |
| 12    | 9       | 8        |
| 13    | 9       | 4        |
| 14    | 7       | 1        |

For the largest number, I can simply put the numbers into order to get the following number: $96299896449997$.

For part B, it's just the smallest number: $31162141116841$

I won't write code for this day, not because it would be hard, but because it would be pretty simple and I don't think I'd learn anything from it.
