# CS 205: Artificial Intelligence Project 1
## The Eight Puzzle, Dr. Eamonn Keogh

Author: Milan Maragiri  
SID: 862548940  
Email: mmara031@ucr.edu  
Date: May 8th, 2025  

The 8-puzzle is a classic problem in artificial intelligence involving a 3x3 grid of tiles, where eight numbered tiles are arranged with one blank space. It is a smaller version of the well-known 15-puzzle. The objective is to reach a specific goal configuration by sliding tiles into the blank space, one move at a time. The tiles are numbered from 1 to 8 so that each tile can be uniquely identified. Though the puzzle appears simple at first glance, finding an optimal solution path can become computationally challenging as depth increases, making it a useful benchmark for evaluating search algorithms.  
  
In this project, we were asked by Dr.Keogh to write a program in any language that would solve this puzzle. I used C++ to write a program that solves the 8-puzzle using 3 algorithms: Uniform Cost Search, A* with the Misplaced Tile heuristic, and A* with the Manhattan Distance heuristic.  
  
I compared their performance using time taken, memory used and nodes expanded as comparison metrics for solution depths ranging from 0 to 31. The program starts with an initial puzzle configuration and searches for the goal state using valid moves under the selected strategy. The code and the results for the same are attached in the report below. The program also supports giving a custom goal state as user input. For the structure of the report, I have followed the sample report structure provided in the “project handout”.  