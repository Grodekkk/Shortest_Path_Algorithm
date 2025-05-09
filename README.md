# Shortest_Path_Algorithm
second project for Algorithms and data structures class
Description:
#################################################################################################
The goal of the task is to find the shortest route through a given terrain. The terrain is divided into fields. Each field has a certain height, expressed as a non-negative integer. Moving to a field of height A from a field of height B takes:
A - B + 1 minutes, if A > B,
1 minute, if A ≤ B.
We can move only to fields adjacent by side, so from a given field we can move to at most four neighboring fields. We cannot leave the area described by the map.

There may be a number of lifts in the area. A lift allows you to travel from its starting point to its destination point (it is one-way). Using the lift takes a certain number of minutes. Additionally, lifts operate at specific times — if we are at the lift’s starting field at minute 8 and we know the lift's destination is (12, 12), it runs every 5 minutes, and the ride takes 3 minutes, then we will arrive at (12, 12) at minute 13 (13 = 8 + 2 + 3; 2 minutes waiting plus 3 minutes travel).

Solving the task does not require using numbers exceeding the range of the int type.

#################################################################################################
Input
The input will include, in order:
width and height of the map
starting position (column and row)
destination position (column and row)
number of lifts
description of the lifts – for each lift, the following in order:
starting position (column and row),
destination position (column and row),
travel time in minutes,
departure interval – the lift can be used at minutes that are multiples of this value,
heights of the map fields.
All values are non-negative.

#################################################################################################
Output
The output should be the shortest time needed to reach the destination from the starting point.
Example
Input
9 8 0 0 8 7 0
0 0 0 0 0 0 0 0 8 
0 9 9 9 9 9 9 9 0 
0 1 0 1 0 0 0 9 0 
0 9 0 0 0 1 0 9 0 
0 9 1 1 1 1 0 9 0 
0 9 0 0 0 0 0 9 0 
0 9 9 9 9 1 9 9 0 
8 0 0 0 0 0 0 0 0 


Output
18

Input
9 9 0 0 8 8 5
0 1 2 0 5 2
0 1 2 0 1 3
2 0 4 0 3 7
4 1 8 8 20 11
4 2 8 8 10 12
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0

Output
22
Comment for the second test:
We take a step down (1 minute). Here we have two lifts available, one departs every 2 minutes, the other every 3. We choose the latter, and although we wait longer (2 minutes), we will reach (2, 0) faster – at minute 4. There we wait another 3 minutes for the lift to (4, 0), arriving at minute 10. Then we take two steps down to (4, 2). We reach it at minute 12, take the lift, and arrive at (8, 8) after 22 minutes.

#################################################################################################
