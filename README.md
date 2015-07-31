#AI Reversi Game Playing Agent

The AI agent predicts the next move for a player in the Reversi game using the Greedy, MinMax, and Alpha-Beta pruning algorithms with positional weight evaluation functions as heuristic. Following three algorithms are used.

1. Greedy
2. MinMax
3. Alpha-Beta Pruning

#####Evaluation Function used:

````
      [ 
            99, -8, 8,  6,  6,  8,  -8, 99 
		    -8, -24,  -4, -3, -3, -4, -24,  -8  
		    8, -4, 7,  4,  4,  7,  -4, 8 
		    6,  -3, 4,  0,  0,  4,  -3, 6  
		    6,  -3, 4,  0,  0,  4,  -3, 6 
		    8,  -4, 7,  4,  4,  7,  -4, 8 
		    -8, -24,  -4, -3, -3, -4, -24,  -8 
		    99, -8, 8,  6,  6,  8,  -8, 99 
		  ]
`````

##MinMax Algorithm:
```` 
function Minimax-Decision(state) returns an action
  inputs: state, current state in game
  return the a in Actions(state) maximizing Min-Value(Result(a,state))
````

````
function Max-Value(state) returns a utility value
  if Terminal-Test(state) then return Utility(state)
  v ← −∞
  for a, s in Successors(state) do v ←Max(v, Min-Value(s))
  return v
`````

`````
function Min-Value(state) returns a utility value
  if Terminal-Test(state) then return Utility(state)
  v ←∞
  for a, s in Successors(state) do v ←Min(v, Max-Value(s))
  return v
`````  
============
##Alpha-Beta Pruning Algorithm:
````
function Alpha-Beta-Decision(state) returns an action
  return the a in Actions(state) maximizing Min-Value(Result(a,state))
````

````
function Max-Value(state,α, β) returns a utility value
  inputs: state, current state in game
  α, the value of the best alternative for max along the path to state
  β, the value of the best alternative for min along the path to state
  
  if Terminal-Test(state) then return Utility(state)
  v ← −∞
  
  for a, s in Successors(state) do
    v ←Max(v, Min-Value(s,α, β))

  if v ≥ β then return v
  α ← Max(α, v)
  return v
````

````
function Min-Value(state,α, β) returns a utility value
  same as Max-Value but with roles of α, β reversed
````
##Contents of the project and Instructions:
The project contains the following files/folders:

1. agent.cpp
2. grading_agent.pl
3. Makefile
4. test_cases (folder): The program can be tested against the 24 input and output text files contained in this folder.

##External Links:
Find out more about Reversi Game at 

1. (https://en.wikipedia.org/wiki/Reversi)
2. (http://www.samsoft.org.uk/reversi/)
3. (http://www.site-constructor.com/othello/othellorules.html)
4. (http://www.cs.vassar.edu/~weltyc/oldcourses/265.98f/assign/othello/rules.html)

===========
##Reference:
Artificial Intelligence: A Modern Approach (Third edition) by Stuart Russell and Peter Norvig
