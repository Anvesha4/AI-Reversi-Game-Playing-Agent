/*
 * agent.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: Anvesha Sinha
 *		USC ID: 1739082640
 */
 
 /*This program determines the next move for a player in the Reversi 
game using the Greedy, Minimax, and Alpha-Beta pruning algorithms with positional weight 
evaluation functions.

INPUT:
It takes inputs from an input file called input.txt which has the following structure:
<task#> Greedy = 1, MiniMax = 2, Alpha-beta = 3
<my player: X or O>
<cutting off depth >
<current state>  

Eg.
2
X
3
********
********
********
***OX***
********
********
********
********

OUTPUT:
It outputs an output file called output.txt which has the following structure:
<next state>

Eg.
********
********
********
**XXX***
********
********
********
********
Node,Depth,Value
root,0,-Infinity
c4,1,4
root,0,4

*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

static char Board[9][9];
static int Heuristics[9][9];
int algorithm, cut_off_depth, weight_player, weight_opponent, returned_value;
static int level = 0;
char player, flip_player;
static int flag = 0;
char next_state [9][9];
ofstream solution("output.txt", ios::out);

//This class contains all the functions required for game board operations
class Boards
{
	public:
			void Initialize_Board();
			void Display_Board();
			void Initialize_Heuristics();
			void Display_Heuristics();
};

	
//This class encapsulates all the structures and functions required for building the game tree
class Adversarial_Tree					
{		
	public:
			//Linkedlist of the entire game tree
			struct Tree_Node
			{	char nodeboard[9][9];
			    string position;
			    int depth;
				int value;
				int minmax_value;
				int alpha;
				int beta;
				int ab_value;
			    
				struct Tree_Node *nextSibling;
				struct Tree_Node *firstChild;
				struct Tree_Node *parent;
			};
			
			//Linkedlist of a particular level in the game tree. Used as a temporary linkedlist for insertion
			struct Child_Queue
			{
				char childboard[9][9];
				string position;
				
				struct Child_Queue *next;
			};
			
			//Linkedlist to dynamically store and add game states using MinMax (MM)
			struct Log_MM_Queue
			{
				string position;
				int depth;
				int value;
				
				struct Log_MM_Queue *next;
			};
			
			//Linkedlist to dynamically store and add game states using Alpha-Beta pruning (AB)
			struct Log_AB_Queue
			{
				string position;
				int depth;
				int value;
				int alpha;
				int beta;
							
				struct Log_AB_Queue *next;
			};
			
			struct Tree_Node *front;
			struct Child_Queue *front_child;
			struct Log_MM_Queue *front_MM;
			struct Log_AB_Queue *front_AB;
			Adversarial_Tree()
			{	front = NULL;	
				front_child = NULL;
				front_MM = NULL;
				front_AB = NULL;	
			}
	
			//Function declarations:
			char flip(char);
			void Build_Tree();
			void Build_Root(char board[9][9]);
			void Build_Next_Level(char, int);
			void Insert_Recursive(struct Tree_Node *traverse_tree, char, int);
			void Insert_Log_MM_Queue(string, int, int);
			void Insert_Log_AB_Queue(string, int, int, int, int);
			void Display_Log_MM_Queue();
			void Display_Log_AB_Queue();
			void MinMax();
			int Traverse_Recursive_MinMax(struct Tree_Node *traverse_tree);
			void Alpha_Beta();
			int Traverse_Recursive_AlphaBeta(struct Tree_Node *traverse_tree, int, int);
			void Display_Next_State();
			void Next_State();
			void Add_X_Children(char nodeboard[9][9]);
			void Add_O_Children(char nodeboard[9][9]);
			void Sort_Queue(struct Child_Queue *old_list_front);
			void Find_Left(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Right(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Up(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Down(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Diagonal_LeftUp(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Diagonal_RightUp(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Diagonal_LeftDown(char, char, int, int, int, int, char nodeboard[9][9]);
			void Find_Diagonal_RightDown(char, char, int, int, int, int, char nodeboard[9][9]);
			int Compare_Arrays(char x[9][9], char y[9][9]);
			void Check_All_Directions(char, char new_nodeboard[9][9], int, int);
			int Check_Game_End_Condition(char new_nodeboard[9][9]);
			void Build_Node(char, int, int, int, int, char nodeboard[9][9]);

};
/*******************************	FUNCTION DEFINITIONS OF CLASS ADVERSARIAL_TREE	 ********************************************************/

//Returns the opponent's symbol
char Adversarial_Tree::flip(char play)
{	if(play == 'X')
		return 'O';
	else
		return 'X';
}

//Builds the next level till the cutoff depth mentioned in the input.txt file
void Adversarial_Tree::Build_Tree()
{	Build_Root(Board);
  	char next_player = player;
  
	while(cut_off_depth>0)
		{ 
		level++;
		Build_Next_Level(next_player, level);
		cut_off_depth--;
		
		//flip next player
		next_player = flip(next_player);
		}
}


//Builds the root of Adversarial_Tree
void Adversarial_Tree::Build_Root(char board[9][9])
{ 
	//Assign the board to the root
	struct Tree_Node *build = new Tree_Node;
	
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
			build->nodeboard[i][j] = Board[i][j];
	
	int weight_player = 0;
	int weight_opponent = 0;
			
	for(int i=1; i<9; i++)
		{
			for(int j=1; j<9; j++)
				{	
					if(build->nodeboard[i][j] == player)
						{
							weight_player += Heuristics[i-1][j-1];
							}
					if(build->nodeboard[i][j] == flip(player))
						{	
							weight_opponent += Heuristics[i-1][j-1];
						}
				}
		}
		
	build->value = weight_player-weight_opponent;		
	build->position = "root";		
	build->depth = 0;
	build->minmax_value = -1000;
	build->alpha = -1000;
	build->beta = 1000;
	build->nextSibling = NULL;
	build->firstChild = NULL;
	build->parent = build;
	front=build;
}

//Builds the next level depending on the current player
void Adversarial_Tree::Build_Next_Level(char play, int level)
{
	//given player, build all the possible combinations; take care of the duplicates 
	struct Tree_Node *traverse_tree = new Tree_Node;

	if(play == 'X')
		{ 
//			cout<<"\nADDING CHILDREN TO TREE\n";
			
			struct Tree_Node *traverse_tree = new Tree_Node;
			traverse_tree = front;
			Insert_Recursive(traverse_tree, 'X', level);		
		}
		
	else
		{	
//			cout<<"\nADDING CHILDREN TO TREE\n";
			
			struct Tree_Node *traverse_tree = new Tree_Node;
			traverse_tree = front;
			Insert_Recursive(traverse_tree, 'O', level);		
				
		}
		
}


void Adversarial_Tree::Insert_Recursive(struct Tree_Node *traverse_tree, char play, int level)
{
	if(traverse_tree->firstChild !=NULL)
		{	
			Insert_Recursive(traverse_tree->firstChild, play, level);
		}
		
	else
		{	
			struct Tree_Node *traverse_level = new Tree_Node;
			traverse_level = traverse_tree;
			
			if(play == 'X')
				Add_X_Children(traverse_level->nodeboard);				//calls a function calculates all possible moves of X and adds it to the tree
			
			else
				Add_O_Children(traverse_level->nodeboard);				//calls a function calculates all possible moves of O and adds it to the tree
			
			if(front_child != NULL)
				{
					struct Child_Queue *traverse_child = new Child_Queue;			//traverse is used to traverse the queue which has all the children of a particular node
					struct Tree_Node *build = new Tree_Node;						//build is used to add a new node to the tree
									
					traverse_child = front_child;									
									
					for(int i=0; i<9; i++)
						{	for(int j=0; j<9; j++)
								{	build->nodeboard[i][j] = traverse_child->childboard[i][j];
								}
						}
			
					int weight_player = 0;
					int weight_opponent = 0;
					
					for(int i=1; i<9; i++)
						{
							for(int j=1; j<9; j++)
								{	
									if(build->nodeboard[i][j] == player)
										{
											weight_player += Heuristics[i-1][j-1];
											}
									if(build->nodeboard[i][j] == flip(player))
										{	
											weight_opponent += Heuristics[i-1][j-1];
										}
								}
						}
					build->value = weight_player-weight_opponent;
					build->position = traverse_child->position;
					build->depth = level;
					build->nextSibling = NULL;
					build->firstChild = NULL;
					build->alpha = -1000;
					build->beta = 1000;
					
					if(build->depth%2 != 0)
						build->minmax_value = 1000;
					else
						build->minmax_value = -1000;
					
					build->parent = traverse_tree;
					traverse_tree->firstChild = build;
					traverse_level = traverse_tree->firstChild;
					
					
					traverse_child = traverse_child->next;
		
					while(traverse_child!=NULL)
						{	
							struct Tree_Node *build = new Tree_Node;
							for(int i=0; i<9; i++)
								{	for(int j=0; j<9; j++)
										{	build->nodeboard[i][j] = traverse_child->childboard[i][j];
										}
								} 
			
							int weight_player = 0;
							int weight_opponent = 0;
								
							for(int i=1; i<9; i++)
								{
									for(int j=1; j<9; j++)
										{	
											if(build->nodeboard[i][j] == player)
												{
													weight_player += Heuristics[i-1][j-1];
													}
											if(build->nodeboard[i][j] == flip(player))
												{	
													weight_opponent += Heuristics[i-1][j-1];
												}
										}
								}
							
							build->value = weight_player-weight_opponent;				
							build->position = traverse_child->position;
							build->depth = level;		
							build->alpha = -1000;
							build->beta = 1000;
							
							if(build->depth%2 != 0)
								build->minmax_value = 1000;
							else
								build->minmax_value = -1000;
								
							traverse_child = traverse_child->next;
							build->nextSibling = NULL;
							build->firstChild = NULL;
							build->parent = traverse_tree;
							traverse_level->nextSibling = build;
							traverse_level = traverse_level->nextSibling;
						}
						
				
					front_child=NULL;	
				}
				
			else if (!Check_Game_End_Condition(traverse_level->nodeboard))
				{	
					struct Tree_Node *build = new Tree_Node;				//build is used to add a new node to the tree
					for(int i=0; i<9; i++)
						{	for(int j=0; j<9; j++)
								{	build->nodeboard[i][j] = traverse_level->nodeboard[i][j];
								}
						}
			
					int weight_player = 0;
					int weight_opponent = 0;
					
					for(int i=1; i<9; i++)
						{
							for(int j=1; j<9; j++)
								{
									if(build->nodeboard[i][j] == player)
										{
											weight_player += Heuristics[i-1][j-1];
											}
									if(build->nodeboard[i][j] == flip(player))
										{	
											weight_opponent += Heuristics[i-1][j-1];
										}
								}
						}
						
					build->value = weight_player-weight_opponent;
					build->position = "pass";
					build->depth = level;
					build->nextSibling = NULL;
					build->firstChild = NULL;
					build->alpha = -1000;
					build->beta = 1000;
					if(build->depth%2 != 0)
						build->minmax_value = 1000;
					else
						build->minmax_value = -1000;
					build->parent = traverse_tree;
					traverse_tree->firstChild = build;
					traverse_level = traverse_tree->firstChild;
					
					if(traverse_tree->position == "pass")
						{	
							cut_off_depth = 0;
						}	
				}					
					
		}
		
	if(traverse_tree->nextSibling != NULL)
		{	
			Insert_Recursive(traverse_tree->nextSibling, play, level);
		}
}

//Inserts the position on the board, depth of the game and value at that position into the LOG
void Adversarial_Tree::Insert_Log_MM_Queue(string node, int depth, int value)
{
	struct Log_MM_Queue *build = new Log_MM_Queue;
	struct Log_MM_Queue *traverse = new Log_MM_Queue;
	
	build->position = node;
	build->depth = depth;
	build->value = value;
	
	if(front_MM == NULL)
		{
			build->next = NULL;
			front_MM = build;	
		}
		
	else
		{
			traverse = front_MM;
			while(traverse->next!=NULL)
				{
					traverse = traverse->next;
				}
				
			build->next = NULL;
			traverse->next = build;
			
		}	
}

//Inserts the position on the board, depth of the game and value at that position into the LOG
void Adversarial_Tree::Insert_Log_AB_Queue(string node, int depth, int value, int alpha, int beta)
{
	struct Log_AB_Queue *build = new Log_AB_Queue;
	struct Log_AB_Queue *traverse = new Log_AB_Queue;
	
	build->position = node;
	build->depth = depth;
	build->value = value;
	build->alpha = alpha;
	build->beta = beta;
	
	if(front_AB == NULL)
		{
			build->next = NULL;
			front_AB = build;	
		}
		
	else
		{
			traverse = front_AB;
			while(traverse->next!=NULL)
				{
					traverse = traverse->next;
				}
				
			build->next = NULL;
			traverse->next = build;
			
		}	
}


//Displays LOG queue for MinMax
void Adversarial_Tree::Display_Log_MM_Queue()
{
//	cout<<endl<<"MinMax:"<<endl;
		struct Log_MM_Queue *traverse = new Log_MM_Queue;
		traverse = front_MM;
		solution<<"Node,Depth,Value"<<endl;
		while(traverse!=NULL)
			{	
				solution<<traverse->position<<","<<traverse->depth<<",";
				if(traverse->value == 1000)
					solution<<"Infinity"<<endl;
				else if (traverse->value == -1000)
					solution<<"-Infinity"<<endl;
				else
					solution<<traverse->value<<endl;
				traverse=traverse->next;
			}
	
}


//Displays LOG queue for Alpha-Beta pruning
void Adversarial_Tree::Display_Log_AB_Queue()
{	
//	cout<<endl<<"Alpha-Beta:"<<endl;
		struct Log_AB_Queue *traverse = new Log_AB_Queue;
		traverse = front_AB;
		solution<<"Node,Depth,Value,Alpha,Beta"<<endl;

		while(traverse!=NULL)
			{	
				solution<<traverse->position<<","<<traverse->depth<<",";
				if(traverse->value == 1000)
					solution<<"Infinity,";
				else if (traverse->value == -1000)
					solution<<"-Infinity,";
				else
					solution<<traverse->value<<",";
					
				if(traverse->alpha == 1000)
					solution<<"Infinity,";
				else if (traverse->alpha == -1000)
					solution<<"-Infinity,";
				else
					solution<<traverse->alpha<<",";
				
				if(traverse->beta == 1000)
					solution<<"Infinity"<<endl;
				else if (traverse->beta == -1000)
					solution<<"-Infinity"<<endl;
				else
					solution<<traverse->beta<<endl;
					
				traverse=traverse->next;
			}
}

//Calls the Minmax function which traverses the Adversarial_Tree using MinMax algorithm
void Adversarial_Tree::MinMax()
{
	front->minmax_value = Traverse_Recursive_MinMax(front);
	
}

//The traversal log is inserted into the LOG_MM queue by traversing the Adversarial_Tree recursively using MinMax algorithm
int Adversarial_Tree::Traverse_Recursive_MinMax( struct Tree_Node *traverse_tree)
{
	if(traverse_tree->firstChild != NULL)
		{	
			Insert_Log_MM_Queue(traverse_tree->position, traverse_tree->depth, traverse_tree->minmax_value);
						
			returned_value = Traverse_Recursive_MinMax(traverse_tree->firstChild);
			traverse_tree->minmax_value = returned_value;
		}

	if(traverse_tree->nextSibling != NULL)
		{	
			if(traverse_tree->firstChild == NULL)
				traverse_tree->minmax_value = traverse_tree->value;
			
			if(traverse_tree->depth %2 != 0)
				{ 
					if(traverse_tree->minmax_value > traverse_tree->parent->minmax_value)
						traverse_tree->parent->minmax_value = traverse_tree->minmax_value; 
				}
				
			else 
				{
					if(traverse_tree->minmax_value < traverse_tree->parent->minmax_value)
						traverse_tree->parent->minmax_value = traverse_tree->minmax_value; 
				}	
			
			Insert_Log_MM_Queue(traverse_tree->position, traverse_tree->depth, traverse_tree->minmax_value);
			Insert_Log_MM_Queue(traverse_tree->parent->position, traverse_tree->parent->depth, traverse_tree->parent->minmax_value);
			
			returned_value = Traverse_Recursive_MinMax(traverse_tree->nextSibling);
			traverse_tree->parent->minmax_value = returned_value;
			return traverse_tree->parent->minmax_value;
		}

	else
		{	if(traverse_tree->firstChild == NULL)
				traverse_tree->minmax_value = traverse_tree->value;
			
			if(traverse_tree->depth %2 != 0)
				{	
					if(traverse_tree->minmax_value > traverse_tree->parent->minmax_value)
						traverse_tree->parent->minmax_value = traverse_tree->minmax_value; 
				}
				
			else 
				{
					if(traverse_tree->minmax_value < traverse_tree->parent->minmax_value)
						traverse_tree->parent->minmax_value = traverse_tree->minmax_value; 
				}		
			Insert_Log_MM_Queue(traverse_tree->position, traverse_tree->depth, traverse_tree->minmax_value);
			return traverse_tree->parent->minmax_value;
		}	
}

//Calls the Minmax function which traverses the Adversarial_Tree using Alpha-Beta pruning algorithm
void Adversarial_Tree::Alpha_Beta()
{
	front->alpha = Traverse_Recursive_AlphaBeta(front, front->alpha, front->beta);	
}


//The traversal log is inserted into the LOG_MM queue by traversing the Adversarial_Tree recursively using Alpha-Beta pruning algorithm
int Adversarial_Tree::Traverse_Recursive_AlphaBeta(struct Tree_Node *traverse_tree, int parent_alpha, int parent_beta)
{
		if(traverse_tree->firstChild != NULL)
		{	
			
			traverse_tree->alpha = parent_alpha;
			traverse_tree->beta = parent_beta;
		
			if(traverse_tree->depth%2 != 0)							//MIN LEVEL, ab_value is beta
				traverse_tree->ab_value = 1000;						//While going deep into the tree
			else
				traverse_tree->ab_value =-1000;						//While going deep into the tree
		
			Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->ab_value,traverse_tree->alpha,traverse_tree->beta);
			returned_value = Traverse_Recursive_AlphaBeta(traverse_tree->firstChild, traverse_tree->alpha, traverse_tree->beta);

			if(traverse_tree->depth %2 == 0)						//MAX LEVEL; update alpha
				{  
							traverse_tree->alpha = returned_value;
							
				}	
			else													//MIN LEVEL; update beta
				{
								traverse_tree->beta = returned_value;
				}
			Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->ab_value,traverse_tree->alpha,traverse_tree->beta);
			
		}

	if(traverse_tree->nextSibling != NULL)
		{	
			traverse_tree->alpha = parent_alpha;
			traverse_tree->beta = parent_beta;
 
 			if(traverse_tree->depth %2 != 0)							//MIN LEVEL; update alpha of the parent
				{ 
					
				 	if(traverse_tree->firstChild == NULL)
				 		{	
				 			Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->minmax_value,traverse_tree->alpha,traverse_tree->beta);
			
							if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->value;
							
							else if(traverse_tree->parent->ab_value  < traverse_tree->value)
								traverse_tree->parent->ab_value = traverse_tree->value;
		
		
						 	if(parent_beta > traverse_tree->parent->ab_value)
								parent_alpha = (parent_alpha > traverse_tree->parent->ab_value)? parent_alpha : traverse_tree->parent->ab_value;
					 				
					 		else
			 					return parent_alpha;
										

				 		 }
				
					else
						{	
							if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
							
							else if(traverse_tree->parent->ab_value  < traverse_tree->ab_value)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
								
							if(parent_beta > traverse_tree->parent->ab_value)
								parent_alpha = (parent_alpha > traverse_tree->parent->ab_value)? parent_alpha : traverse_tree->parent->ab_value;
					 		else
								return parent_alpha;


						}
						
				
					
					Insert_Log_AB_Queue(traverse_tree->parent->position,traverse_tree->parent->depth,traverse_tree->parent->ab_value,parent_alpha,parent_beta);
					returned_value = Traverse_Recursive_AlphaBeta(traverse_tree->nextSibling,parent_alpha,parent_beta);
					return returned_value;
					
				}
				
			else if(traverse_tree->depth %2 == 0)							//MAX LEVEL; update beta of parent
				{
					if(traverse_tree->firstChild == NULL)
						{	
							Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->minmax_value,traverse_tree->alpha,traverse_tree->beta);
					
				 			if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->value;
							
							else if(traverse_tree->parent->ab_value  > traverse_tree->value)
								traverse_tree->parent->ab_value = traverse_tree->value;
							
							if(parent_alpha < traverse_tree->parent->ab_value)
						 			 parent_beta = (parent_beta < traverse_tree->parent->ab_value)? parent_beta: traverse_tree->parent->ab_value;				 				
				 			
							 else
				 				return parent_beta;	
									
						}
					
					else
						{
							if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
							
							else if(traverse_tree->parent->ab_value  > traverse_tree->ab_value)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
								
							if(parent_alpha < traverse_tree->parent->ab_value)
						 			 parent_beta = (parent_beta < traverse_tree->parent->ab_value)? parent_beta: traverse_tree->parent->ab_value;				 				
				 			else
				 				return parent_beta;
							}

					Insert_Log_AB_Queue(traverse_tree->parent->position,traverse_tree->parent->depth,traverse_tree->parent->ab_value,parent_alpha,parent_beta);
					returned_value = Traverse_Recursive_AlphaBeta(traverse_tree->nextSibling, parent_alpha,parent_beta);
					return returned_value;
							
				}	
			
		}

	else
		{
			if(traverse_tree == front && traverse_tree->firstChild == NULL)
				{
					Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->value,traverse_tree->alpha,traverse_tree->beta);
					return -1000;
				}
			traverse_tree->alpha = parent_alpha;
			traverse_tree->beta = parent_beta;
 			
			if(traverse_tree->depth %2 != 0)								//MIN LEVEL; update alpha of the parent
				{ 
				 	if(traverse_tree->firstChild == NULL)
				 		{	
				 			Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->minmax_value,traverse_tree->alpha,traverse_tree->beta);
			
							if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->value;
							
							else if(traverse_tree->parent->ab_value  < traverse_tree->value)
								traverse_tree->parent->ab_value = traverse_tree->value;

				 			if(parent_beta > traverse_tree->parent->ab_value)
								parent_alpha = (parent_alpha > traverse_tree->parent->ab_value)? parent_alpha : traverse_tree->parent->ab_value;
				 	
				 			else
				 				return parent_alpha;
									
				 		}
				
					else
						{
							if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
							
							else if(traverse_tree->parent->ab_value  < traverse_tree->ab_value)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
								
							if(parent_beta > traverse_tree->parent->ab_value)
								parent_alpha = (parent_alpha > traverse_tree->parent->ab_value)? parent_alpha : traverse_tree->parent->ab_value;
				 			else
				 				return parent_alpha;
				
						}
					
					return parent_alpha;
				}
				
			else if(traverse_tree->depth %2 == 0)								//MAX LEVEL; update beta of parent
				{
					if(traverse_tree->firstChild == NULL)
						{	
							Insert_Log_AB_Queue(traverse_tree->position,traverse_tree->depth,traverse_tree->minmax_value,traverse_tree->alpha,traverse_tree->beta);
					
				 			if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->value;
							
							else if(traverse_tree->parent->ab_value  > traverse_tree->value)
								traverse_tree->parent->ab_value = traverse_tree->value;
	
							if(parent_alpha < traverse_tree->parent->ab_value)
						 			 parent_beta = (parent_beta < traverse_tree->parent->ab_value)? parent_beta: traverse_tree->parent->ab_value;				 				
				 			
							 else
				 				return parent_beta;	
								
						}
					
					else
						{
							if(traverse_tree == traverse_tree->parent->firstChild)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
							
							else if(traverse_tree->parent->ab_value  > traverse_tree->ab_value)
								traverse_tree->parent->ab_value = traverse_tree->ab_value;
								
							if(parent_alpha < traverse_tree->parent->ab_value)
						 			 parent_beta = (parent_beta < traverse_tree->parent->ab_value)? parent_beta: traverse_tree->parent->ab_value;				 				
				 			else
				 				return parent_beta;	
						}
						
					return parent_beta;
				}
		}
}

//Writes the next state and log(if there is) into the output.txt file
void Adversarial_Tree::Display_Next_State()
{
	struct Tree_Node *traverse_first_level = new Tree_Node;
	struct Tree_Node *max = new Tree_Node;
	
	traverse_first_level = front->firstChild;
	
//	cout<<"\nDISPLAYING NEXT STATE\n";
	if(algorithm == 1)
		{	max = traverse_first_level;
			
			if(traverse_first_level == NULL)
				{
					for(int i=1; i<9; i++)
						{
							for(int j=1; j<9; j++)
								{
									solution<<front->nodeboard[i][j];
								}
							solution<<endl;
						}
				}
			else
				{
					while(traverse_first_level != NULL)
						{
							if(traverse_first_level->value > max->value)
								max = traverse_first_level;
								
							traverse_first_level = traverse_first_level->nextSibling;	
						}
						
					for(int i=1; i<9; i++)
						{
							for(int j=1; j<9; j++)
								{
									solution<<max->nodeboard[i][j];
								}
							solution<<endl;
						}
					
				}
						
			
		}
		
	if(algorithm == 2 || algorithm == 3)
		{
			if(traverse_first_level == NULL)
				{
						for(int i=1; i<9; i++)
							{
								for(int j=1; j<9; j++)
									{
										solution<<front->nodeboard[i][j];
									}
								solution<<endl;
							}
		
				}
				
			else
				{
						while(traverse_first_level->nextSibling != NULL)
							{	
								if(front->minmax_value == traverse_first_level->minmax_value)
										break;
										
								traverse_first_level = traverse_first_level->nextSibling;
							}
							
						for(int i=1; i<9; i++)
							{
								for(int j=1; j<9; j++)
									{
										solution<<traverse_first_level->nodeboard[i][j];
									}
								solution<<endl;
							}
				}
		
		}
}


void Adversarial_Tree::Next_State()
{
	struct Tree_Node *traverse_first_level = new Tree_Node;
	struct Tree_Node *max = new Tree_Node;
	
	traverse_first_level = front->firstChild;
	
	if(traverse_first_level == NULL)
		{
			solution<<"PASS";
		}
				
	else
		{
			while(traverse_first_level->nextSibling != NULL)
				{	
					if(front->minmax_value == traverse_first_level->minmax_value)
						break;
								
					traverse_first_level = traverse_first_level->nextSibling;
				}
							
			solution<<traverse_first_level->position;
		}

}

void Adversarial_Tree::Add_X_Children(char nodeboard[9][9])
{	
	//find all the X in the board.
	for(int i=1; i<9; i++)
		{	for(int j=1; j<9; j++)
				{	if(nodeboard[i][j] == 'X')
						{	flag = 0;
							Find_Left('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Right('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Up('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Down('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_LeftUp('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_RightUp('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_LeftDown('O', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_RightDown('O', '*', i, j, i, j, nodeboard);
 
						}
					
				}
		}
		
	Sort_Queue(front_child);

}

void Adversarial_Tree::Add_O_Children(char nodeboard[9][9])
{	
	//find all the O in the board.
	int pass = 1;										//to check whether O passes its turn; this happens when there is no
	for(int i=1; i<9; i++)
		{	for(int j=1; j<9; j++)
				{	if(nodeboard[i][j] == 'O')
						{	
							pass = 0;
							flag = 0;
							Find_Left('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Right('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Up('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Down('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_LeftUp('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_RightUp('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_LeftDown('X', '*', i, j, i, j, nodeboard);
							flag = 0;
							Find_Diagonal_RightDown('X', '*', i, j, i, j, nodeboard);
 
						}

				}
		}
		
	Sort_Queue(front_child);

}

void Adversarial_Tree::Sort_Queue(struct Child_Queue *old_list_front)
{
	struct Child_Queue *new_list_rear = new Child_Queue;
	struct Child_Queue *new_list_front = new Child_Queue;
	struct Child_Queue *old_list = new Child_Queue;
	struct Child_Queue *old_list_previous = new Child_Queue;
	struct Child_Queue *temp_previous = new Child_Queue;
	struct Child_Queue *temp = new Child_Queue;
	
	string min = "i9";
	new_list_front = NULL;
	int length_of_list = 0;

	old_list = old_list_front;
	while(old_list!=NULL)
		{	
			old_list = old_list->next;
			length_of_list++; 
		}	

	while(length_of_list >0)
		{	string min = "i9";
			old_list = old_list_front;
			old_list_previous = old_list_front;
			while(old_list != NULL)
				{
					if(old_list->position[1] < min[1])
						{	
							min = old_list->position;
							temp = old_list;
							temp_previous = old_list_previous;
						}
						
					else if(old_list->position[1] == min[1])
						{
							if(old_list->position < min)
								{
									min = old_list->position;
									temp = old_list;
									temp_previous = old_list_previous;
				
								}
						}							
						
					old_list = old_list->next;
					if(old_list != old_list_front->next)
						old_list_previous = old_list_previous->next;
				}
				
			if(new_list_front == NULL)
				{	
					if(temp == old_list_front)
						{
							old_list_front = old_list_front->next;
							temp->next = NULL;
							new_list_front = temp;
							new_list_rear = temp;
						}
					else
						{
							temp_previous->next = temp->next;
							temp->next = NULL;
							new_list_front = temp;
							new_list_rear = temp;
							}	
				}	
				
			else
				{	if(temp == old_list_front)
						{
							old_list_front = old_list_front->next;
							temp->next = NULL;
							new_list_rear->next = temp;
							new_list_rear = temp;
						}
						
					else
						{
							temp_previous->next = temp->next;
							temp->next = NULL;
							new_list_rear->next = temp;
							new_list_rear = temp;
						}
							
				}
			
			length_of_list = 0;
			old_list = old_list_front;
			while(old_list!=NULL)
				{	
					old_list = old_list->next;
					length_of_list++; 
				}	
		}
		
		front_child = new_list_front;
}

void Adversarial_Tree::Find_Left(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{	
	char play = flip(flip_play);
	
//	cout<<"FIND_LEFT";
	if(nodeboard[row][column-1] == flip_play)
		{	flag = 1;
	
	 		Find_Left(flip_play, end_character, row, column-1, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row][column-1] == end_character)
	     		{
				 	Build_Node(play, row, column-1, start_row, start_column, nodeboard);
	     		}
	 	}
}

void Adversarial_Tree::Find_Right(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	char play = flip(flip_play);	

//	cout<<"FIND_RIGHT";	
	if(nodeboard[row][column+1] == flip_play)
		{	flag = 1;
	 		Find_Right(flip_play, end_character, row, column+1, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row][column+1] == end_character)
		 		{//	cout<<"FOUND ON RIGHT";
		     		Build_Node(play, row, column+1, start_row, start_column, nodeboard);

		 		}
	 	}
}

void Adversarial_Tree::Find_Up(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	char play = flip(flip_play);	
//	cout<<"_UP_";	
	if(nodeboard[row-1][column] == flip_play)
		{	flag = 1;
		//	cout<<nodeboard[row-1][column];
	 		Find_Up(flip_play, end_character, row-1, column, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row-1][column] == end_character)
	     		Build_Node(play, row-1, column, start_row, start_column, nodeboard);
	 	}
}

void Adversarial_Tree::Find_Down(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	char play = flip(flip_play);	
	
//	cout<<"FIND_DOWN";	
	if(nodeboard[row+1][column] == flip_play)
		{	flag = 1;
//			cout<<player<<nodeboard[row+1][column]<<"FOUND SOMETHING DOWN HERE";
	 		Find_Down(flip_play, end_character, row+1, column, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row+1][column] == end_character)
	     		Build_Node(play, row+1, column, start_row, start_column, nodeboard);
	 	}
}

void Adversarial_Tree::Find_Diagonal_LeftUp(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	char play = flip(flip_play);	
//	cout<<"_DLU_";	
	if(nodeboard[row-1][column-1] == flip_play)
		{	flag = 1;
	 		Find_Diagonal_LeftUp(flip_play, end_character, row-1, column-1, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row-1][column-1] == end_character)
	     		Build_Node(play, row-1, column-1, start_row, start_column, nodeboard);
	 	}
}

void Adversarial_Tree::Find_Diagonal_RightUp(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	
	char play = flip(flip_play);	
//	cout<<"_DRU_";	
	if(nodeboard[row-1][column+1] == flip_play)
		{	flag = 1;
//			cout<<player<<"on"<<row-1<<column+1;
	 		Find_Diagonal_RightUp(flip_play, end_character, row-1, column+1, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row-1][column+1] == end_character)
	     		{
				 	Build_Node(play, row-1, column+1, start_row, start_column, nodeboard);
	     		}	
	 	}
}

void Adversarial_Tree::Find_Diagonal_LeftDown(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	char play = flip(flip_play);	
	
	if(nodeboard[row+1][column-1] == flip_play)
		{	flag = 1;
	 		Find_Diagonal_LeftDown(flip_play, end_character, row+1, column-1, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row+1][column-1] == end_character)
		 		Build_Node(play, row+1, column-1, start_row, start_column, nodeboard);
	 	}
}

void Adversarial_Tree::Find_Diagonal_RightDown(char flip_play, char end_character, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{
	char play = flip(flip_play);	
	
	if(nodeboard[row+1][column+1] == flip_play)
		{	flag = 1;
	 		Find_Diagonal_RightDown(flip_play, end_character, row+1, column+1, start_row, start_column, nodeboard);
	 	}
	
	else
	 	{ 
	 		if(flag == 1 && nodeboard[row+1][column+1] == end_character)
	     		Build_Node(play, row+1, column+1, start_row, start_column, nodeboard);
	 	}
}


int Adversarial_Tree::Compare_Arrays(char x[9][9], char y[9][9])
{	
	for(int i=0; i<9; i++)
		{	for(int j=0; j<9; j++)
				{	if(x[i][j] != y[i][j])
					return 0;
				}
		}
	return 1;
}


void Adversarial_Tree::Check_All_Directions(char play, char new_nodeboard[9][9], int row, int column)
{
	//check up
	int i;
	int flip = 0;
	int out_of_loop = 0;
	
	for(i=row-1; i>0; i--)
		{
			if(new_nodeboard[i][column] == play)
				{	flip = 1;
					break;
				}
			else if(new_nodeboard[i][column] == '*')
				break;	
		}
	
	if(flip == 1)
		{	for(int x=i; x<row; x++)
				new_nodeboard[x][column] = play;
				
		}

	//check left
	int j;
	flip = 0;
	for(j=column-1; j>0; j--)
		{
			if(new_nodeboard[row][j] == play)
				{	
					flip = 1;
					break;
				}
			else if(new_nodeboard[row][j] == '*')
				break;	
				
		}
	
	if(flip == 1)
		{
			for(int x=j; x<column; x++)
				new_nodeboard[row][x] = play;
		}
	
	//check down
	
	flip = 0;
	for(i=row+1; i<9; i++)
		{
			if(new_nodeboard[i][column] == play)
				{
					flip = 1;
					break;
				}
			else if(new_nodeboard[i][column] == '*')
				break;	

		}
	
	if(flip == 1)
		{
			for(int x=i; x>row; x--)
				new_nodeboard[x][column] = play;
		}

	//check right
	
	flip = 0;
	for(j=column+1; j<9; j++)
		{
			if(new_nodeboard[row][j] == play)
				{
					flip = 1;
					break;
				}
			else if(new_nodeboard[row][j] == '*')
				break;	

		}
	
	if(flip == 1)
		{
			for(int x=j; x>column; x--)
				new_nodeboard[row][x] = play;
		}
	
	//check diagonal_left_up
	flip = 0;
	
	for(j=column-1,i=row-1; j>0,i>0; j--,i--)
		{
			if(new_nodeboard[i][j] == play)
				{	
					flip = 1;
					break;
				}
			else if(new_nodeboard[i][j] == '*')
					break;	
		}	
	
	if(flip == 1)
		{	int x,y;
			for(x=i,y=j; x<row,y<column; x++,y++)
				new_nodeboard[x][y] = play;
		}

	//check diagonal_right_up
	flip = 0;
	
	for(j=column+1, i=row-1; i>0,j<9; j++,i--)
		{
			if(new_nodeboard[i][j] == play)
				{	
					flip = 1;
					break;
				}
			else if(new_nodeboard[i][j] == '*')
					break;
		}		
	
	if(flip == 1)
		{	int x,y;
			for(x=i,y=j; x<row,y>column; x++,y--)
				new_nodeboard[x][y] = play;
		}
	
	//check diagonal_right_down
	flip = 0;
	for(j=column+1,i=row+1; j<9,i<9; j++,i++)
		{	if(new_nodeboard[i][j] == play)
				{	
					flip = 1;
					break;
				}
			else if(new_nodeboard[i][j] == '*')
					break;
		}
			
	if(flip == 1)
		{
			int x,y;
			for(x=i, y=j; x>row,y>column; x--,y--)
				new_nodeboard[x][y] = play;
		}
	

	//check diagonal_left_down
	flip = 0;
	
	for(j=column-1,i=row+1; j>0,i<9; j--,i++)
		{
			if(new_nodeboard[i][j] == play)
				{	
					flip = 1;
					break;
				}
			else if(new_nodeboard[i][j] == '*')
					break;
		}	
	
	if(flip == 1)
		{	int x,y;
			for(int x=i,y=j; x>row,y<column; x--, y++)
				new_nodeboard[x][y] = play;
		}
}

int Adversarial_Tree::Check_Game_End_Condition(char new_nodeboard[9][9])
{
	int num_of_X = 0;
	int num_of_O = 0;
	
	for(int i=1; i<9; i++)
		{
			for(int j=1; j<9; j++)
				{
					if(new_nodeboard[i][j] == 'X')
						num_of_X++;
					else if (new_nodeboard[i][j] == 'O')
						num_of_O++;
				}
		}
//	cout<<num_of_X<<num_of_O;
	if(num_of_X == 0 || num_of_O == 0)
		return 1;
	else
		return 0;
}

void Adversarial_Tree::Build_Node(char play, int row, int column, int start_row, int start_column, char nodeboard[9][9])
{	
	static char new_nodeboard [9][9];
	string position;
	
	for(int i=0; i<9; i++)
		{	for(int j=0; j<9; j++)
				{	new_nodeboard[i][j] = nodeboard[i][j];
				}
		}
//	cout<<"INITIALIZED AGAIN";	
	
	//cout<<"ANOTHER";
	if(nodeboard[row][column] == '*')
		{
			new_nodeboard[row][column] = play;
			position = Board[0][column];
			position = position + Board[row][0];
		}		

	//To find out all the other diections where flip could occur
	Check_All_Directions(play, new_nodeboard, row, column);

	struct Child_Queue *build = new Child_Queue;
	struct Child_Queue *traverse = new Child_Queue;
	int add = 1;
	for(int i=0; i<9; i++)
		{	for(int j=0; j<9; j++)
			{	build->childboard[i][j]=new_nodeboard[i][j];
			}
		}
	
	build->position = position;
//	cout<<"("<<position<<")";
	
	if(front_child == NULL)
		{	build->next = NULL;
			front_child = build;
		}	
		
	else
		{	traverse = front_child;
			while(traverse->next != NULL)
				{	if(Compare_Arrays(traverse->childboard, build->childboard))
						{	add=0;
						//	cout<<traverse->position<<build->position<<"SIMILAR ARRAYS";
							break;
						}	
					else
							traverse = traverse->next;	
				}
				
			if(Compare_Arrays(traverse->childboard,build->childboard))
				add=0;
				
						
			if(add == 1)
				{	build->next = NULL;
					traverse->next = build;
				}	
		}
		
}




/*******************************	FUNCTION DEFINITIONS OF CLASS BOARDS	********************************************************/
		
void Boards::Initialize_Board()
{
 int ascii_a = 97;
    int one = 49;

	for(int j=1; j<9; j++)
		Board[0][j] = ascii_a++;
	
	for(int i=1; i<9; i++)
		Board[i][0] = one++;
}

void Boards::Display_Board()
{
	for(int i=0; i<9; i++)
	 { for(int j=0; j<9; j++)
			cout<<setw(5)<<Board[i][j]<<" "; 
		
		cout<<endl;
	}
}


void Boards::Initialize_Heuristics()
{
int v[10]= {99,-8,8,6,-24,-4,-3,7,4,0};       //populating triangles in clockwise direction
int x = 0;					//1st triangle
	for(int i=0; i<4; i++)			
	 for(int j=i; j<4; j++)
	    Heuristics[i][j] = v[x++];
	
x=0;						//2nd triangle
	for(int i=0; i<4; i++)
	 for(int j=7-i; j>=4; j--)
	   Heuristics[i][j] = v[x++];
	
x=0;						//3rd triangle
	for(int j=7; j>=4; j--)
	 for(int i=7-j; i<4; i++)
	   Heuristics[i][j] = v[x++];
	
x=0;						//4th triangle
	for(int j=7; j>=4; j--)
	 for(int i=j; i>=4; i--)
	   Heuristics[i][j] = v[x++];
	
x=0;						//5th triangle
	for(int i=7; i>=4; i--)
	 for(int j=i; j>=4; j--)
	    Heuristics[i][j] = v[x++];
	

x=0;						//6th triangle
	for(int i=7; i>=4; i--)
	 for(int j=7-i; j<4; j++)
	    Heuristics[i][j] = v[x++];
	

x=0;						//7th triangle
	for(int j=0; j<4; j++)
	 for(int i=7-j; i>=4; i--)
	   Heuristics[i][j] = v[x++];
	
x=0;						//8th triangle
	for(int j=0; j<4; j++)
	 for(int i=j; i<4; i++)
	    Heuristics[i][j] = v[x++];
	

}

 
void Boards::Display_Heuristics()
{
	cout<<endl;
	for(int i=0; i<8; i++)
	 { for(int j=0; j<8; j++)
		{cout<<setw(5)<<Heuristics[i][j]<<" "; }
	  cout<<endl; }

}



/*******************************	INPUT PROBLEM FROM input.txt FILE	********************************************************/
void Input_Problem()
{
	ifstream problem("input.txt", ios::in);

	if(!problem) cout<<"Problem opening the input file";

	problem>>algorithm>>player>>cut_off_depth;

	for(int i=1; i<9; i++)
	 for(int j=1; j<9; j++)
	   problem>>Board[i][j];
}



int main()
{	
	Input_Problem();
	Boards b;
	Adversarial_Tree at;
	b.Initialize_Board();
	b.Initialize_Heuristics();
	
	at.Build_Tree();
	
	if(algorithm == 1)
		{
			at.Display_Next_State();	
		}
			
	else if(algorithm == 2)
		{
			at.MinMax();
			at.Display_Next_State();
			at.Display_Log_MM_Queue();
	
		}	
		
	else if(algorithm == 3)
		{
			at.MinMax();
			at.Alpha_Beta();
			at.Display_Next_State();
			at.Display_Log_AB_Queue();
		}
		
	return 0;
}

