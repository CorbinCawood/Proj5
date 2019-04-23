/*
 *Corbin Cawood, Preston Smith
 *COSC 302
 *23 April 2019
 *Dr. Scott Emrich
 *Project 5: "Word Dice"
 *Description:
 *	This program reads in a file of "Dice" and determines wheter or not
 *specific "words" from a different file can by spelled by rolling the given dice.
 *Edmonds-Karp algorithm was used to accomplish this. Assistance provided by Camille
 *Crumpton (Previous COSC 302 TA).
 */

#include <iostream>
#include<fstream>
#include<map>
#include<vector>
#include<list>
using namespace std;

typedef enum{SOURCE, SINK, DICE, WORD} nodeType;

struct Node{

	nodeType type;
	int id;
	int visited;
	vector<bool> letters;
	vector<class Edge*> adj;
	class Edge* backEdge;
};

struct Edge{

	Node* to;
	Node* from;
	Edge* reverse;
	int original;
	int residual;

};

struct Graph{

	bool BFS();
	bool spellCheck();
	void resetGraph();

	vector<int> spellingIDs;	
	vector<Node*> nodes;
	int minNodes;
};

int main(int argc, char* argv[]){


	string temp;
	ifstream inFile;
	vector<string> words;
	vector<Node*> dice;
	vector<Node*> currentLetters;
	int i;
	int j;
	int k;
	int l;
	int idIterator;
	Graph wordGraph;


	//Open the dice file and pull the dice nodes

	idIterator = 1;
	inFile.open(argv[1]);
	while(getline(inFile, temp)){
		Node * die = new Node;
		die->type = DICE;
		die->id = idIterator;
		die->letters.resize(26);
		for(j = 0; j < temp.length(); j++){
			die->letters.at(temp.at(j)-65) = 1;
		}
		dice.push_back(die);
		idIterator++;
	}

	//Open the words file and fill a vector of words

	inFile.close();
	inFile.open(argv[2]);

	while(getline(inFile, temp)){
		words.push_back(temp);
	}


	for(i = 0; i < words.size(); i++){

		//Create a source and a sink

		Node* source = new Node;
		Node* sink = new Node;

		source->id = 0;
		source->type = SOURCE;
		source->visited = 0;
		source->backEdge = NULL;

		sink->id = dice.size() + words.at(i).length() + 1;
		sink->type = SINK;
		sink->backEdge = NULL;


		//Break down the letters of the current word
		//into their own nodes

		for(j = 0; j < words.at(i).length(); j++){
			Node *letter = new Node;
			letter->id = idIterator;
			letter->type = WORD;
			letter->visited = 0;
			letter->letters.resize(26);
			letter->letters.at(words.at(i).at(j) -65) = 1;

			idIterator++;
			currentLetters.push_back(letter);
		}

		//Connect all die to the source, followed by connecting
		//all letter nodes to the sink


		for(j = 0; j < dice.size(); j++){
			Edge * newEdge = new Edge;
			Edge * reverseEdge = new Edge;

			newEdge->to = dice.at(j);
			newEdge->from = source;
			newEdge->original = 1;
			newEdge->residual = 0;

			reverseEdge->to = source;
			reverseEdge->from = dice.at(j);
			reverseEdge->original = 0;
			reverseEdge->residual = 1;

			newEdge->reverse = reverseEdge;
			reverseEdge->reverse = newEdge;

			source->adj.push_back(newEdge);
			dice.at(j)->adj.push_back(reverseEdge);
		}

		for(j = 0; j < currentLetters.size(); j++){
			Edge * newEdge = new Edge;
			Edge * reverseEdge = new Edge;

			newEdge->to = sink;
			newEdge->from = currentLetters.at(j);
			newEdge->original = 1;
			newEdge->residual = 0;

			reverseEdge->to = currentLetters.at(j);
			reverseEdge->from = sink;
			reverseEdge->original = 0;
			reverseEdge->residual = 1;

			newEdge->reverse = reverseEdge;
			reverseEdge->reverse = newEdge;

			currentLetters.at(j)->adj.push_back(newEdge);
			sink->adj.push_back(reverseEdge);
		}

		//Now connect all appropriate die to approprite letter nodes

		for(j = 0; j < dice.size(); j++){
			for(l = 0; l < currentLetters.size(); l++){
				for(k = 0; k < dice.at(j)->letters.size(); k++){
					if(dice.at(j)->letters.at(k) == 1 && currentLetters.at(l)->letters.at(k) == 1){

						//If both positions in the "letters" vector
						//for both dice and currentLetter are 1, then the two nodes
						//are linked.

						Edge * newEdge = new Edge;
						Edge * reverseEdge = new Edge;

						newEdge->to = currentLetters.at(l);
						newEdge->from = dice.at(j);
						newEdge->original = 1;
						newEdge->residual = 0;

						reverseEdge->to = dice.at(j);
						reverseEdge->from = currentLetters.at(l);
						reverseEdge->original = 0;
						reverseEdge->residual = 1;

						newEdge->reverse = reverseEdge;
						reverseEdge->reverse = newEdge;

						dice.at(j)->adj.push_back(newEdge);
						currentLetters.at(l)->adj.push_back(reverseEdge);
					}
				}
			}
		}

		//Set the minNodes value

		wordGraph.minNodes = dice.size();

		//Add all of the nodes to the graph class

		wordGraph.nodes.push_back(source);

		for(j = 0; j < dice.size(); j++){
			wordGraph.nodes.push_back(dice.at(j));
		}
		for(j = 0; j < currentLetters.size(); j++){
			wordGraph.nodes.push_back(currentLetters.at(j));
		}

		wordGraph.nodes.push_back(sink);

		//If there aren't enough dice to spell the word, then the word can't be spelled

		if(dice.size() < currentLetters.size()){
			cout << currentLetters.size() << endl;
			cout << "Cannot Spell: " << words.at(i) << endl;
		}
		else if(wordGraph.spellCheck()){

			//If spellcheck succeeds, then output the appropriate die used to spell the word
			//and output the spelled word

			for(j = 0; j < wordGraph.spellingIDs.size(); j++){

				if(j != 0){
					cout << "," << wordGraph.spellingIDs.at(j);
				}
				else{
					cout << wordGraph.spellingIDs.at(j);
				}
			}
			cout << ": " << words.at(i) << endl;
		}
		else{
			cout << "Cannot spell " << words.at(i) << endl;
		}

		//Reset things for another go

		currentLetters.clear();
		wordGraph.spellingIDs.clear();
		wordGraph.resetGraph();
		idIterator = dice.size() + 1;
		delete sink;

	}

}

void Graph::resetGraph(){

	int i;

	//Clear out all the edges from the dice to the words
	for(i = 1; i < minNodes; i++){
		nodes.at(i)->adj.clear();
	}

	//Reset original and residual values for source to dice
	for(i = 0; i < nodes.at(0)->adj.size(); i++){
		nodes.at(0)->adj.at(i)->original = 1;
		nodes.at(0)->adj.at(i)->residual = 0;
	}

	//Remove all edges fom words and sink
	for(i = nodes.size()-1; i > minNodes; i--){
		nodes.pop_back();
	}
	nodes.resize(minNodes);
}

bool Graph::spellCheck(){

	vector<Node*>::iterator vit;

	int i;
	int j;

	while(BFS()){

		Node *curNode;
		Edge *curEdge;


		//Go to the sink and start from it's backEdge

		vit = nodes.end();
		vit--;
		curNode = *vit;

		while(curNode->type != SOURCE){

			//Set curEdge to the curNode's backEdge and reverse its values

			curEdge = curNode->backEdge;

			if(curEdge->original == 1 && curEdge->residual == 0){
				curEdge->original = 0;
				curEdge->residual = 1;
				curEdge->reverse->original = 1;
				curEdge->reverse->residual = 0;

			}
			else if(curEdge->original == 0 && curEdge->residual == 1){
				curEdge->original = 1;
				curEdge->residual = 0;
				curEdge->reverse->original = 0;
				curEdge->reverse->residual = 1;
			}

			curNode = curEdge->from;
		}
	}

	for(int i = minNodes; i < nodes.size()-1; i++){

		for(int j = 0; j < nodes.at(i)->adj.size(); j++){
			if(nodes.at(i)->adj.at(j)->to->type == SINK && nodes.at(i)->adj.at(j)->residual == 0){

				//If any of the word nodes edges to the sink are not residual, then it's not possible to spell the word

				return false;
			}
			else if(nodes.at(i)->type != SOURCE && nodes.at(i)->adj.at(j)->to->type == DICE && nodes.at(i)->adj.at(j)->original == 1){

				//If a node is connected to the die, and isn't the source node, then push_Back its ID into spelling IDS

				spellingIDs.push_back(nodes.at(i)->adj.at(j)->to->id -1);
			}
		}
	}

	return true;
}

bool Graph::BFS(){

	Node *curNode;
	list<Node*> queue;
	list<Node*>::iterator lit;

	int i;

	//Make sure all backEdges and visited fields are rest

	for(i = 0; i < nodes.size(); i++){
		nodes.at(i)->visited = 0;
		nodes.at(i)->backEdge = NULL;
	}

	//Start with the source

	nodes.at(0)->visited = 1;
	queue.push_back(nodes.at(0));


	while(!queue.empty()){
		curNode = queue.front();
		queue.pop_front();


		//Path is found if curNode == sink

		if(curNode->type == SINK){
			return 1;
		}

		//Check all of the children from the current Node, if they are valid, add them to the queue

		for(i = 0; i < curNode->adj.size(); i++){
			if(curNode->adj.at(i)->to->visited == false && curNode->adj.at(i)->original == 1){
				curNode->adj.at(i)->to->visited = true;
				curNode->adj.at(i)->to->backEdge = curNode->adj.at(i);
				queue.push_back(curNode->adj.at(i)->to);
			}
		}
	}

	return false;
}
