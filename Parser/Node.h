#pragma once
#include<vector>
#include<iostream>
#include<string>

// Globals
static int stateCnt = 0;
const std::string epsilon = "ε";
using namespace std;

class Node{
private:
    string label;
    vector< pair<string, Node*> > transitions;
public:
    
    // To prevent double counting while generating code
    bool marked;

    //Constructor
    Node() 
    {
        string label = "q" + to_string(stateCnt++);
        this->label = label;
        this->marked = false;
    }

    Node(string label) 
    {
        this->label = label;
        this->marked = false;
    }

    //Getters
    string getLabel(){return this->label;}
    vector< pair<string, Node*> >& getAllTransitions(){return transitions;}

    //Setters
    void renameLabel(string label){this->label = label;}

    //Transitions
    void addTransition(string label, Node* node){this->transitions.push_back(make_pair(label,node));}

    //Empty Transitions
    void emptyTransitions(){this->transitions.clear();}
    
};