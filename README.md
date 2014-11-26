Reversi_Game
============

This project implements Reversi_Game using the following three algorithms:
1. Greedy
2. MinMax
3. Alpha-Beta Pruning

===========
MinMax Algorithm: 
function Minimax-Decision(state) returns an action
  inputs: state, current state in game
  return the a in Actions(state) maximizing Min-Value(Result(a,state))

function Max-Value(state) returns a utility value
  if Terminal-Test(state) then return Utility(state)
  v ← −∞
  for a, s in Successors(state) do v ←Max(v, Min-Value(s))
  return v

function Min-Value(state) returns a utility value
  if Terminal-Test(state) then return Utility(state)
  v ←∞
  for a, s in Successors(state) do v ←Min(v, Max-Value(s))
  return v
  
============
Alpha-Beta Pruning Algorithm:
function Alpha-Beta-Decision(state) returns an action
  return the a in Actions(state) maximizing Min-Value(Result(a,state))

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

function Min-Value(state,α, β) returns a utility value
  same as Max-Value but with roles of α, β reversed


===========
Reference:
Artificial Intelligence: A Modern Approach (Third edition) by Stuart Russell and Peter Norvig
