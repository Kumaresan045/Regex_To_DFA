#pragma once
#include "Node.h"
#include <unordered_map>
#include <set>
#include <algorithm>
#include <queue>
#include <sstream>

class NFA{

private:

    /* -------------- Class Members -------------- */ 
    Node* startState;
    vector<Node*> allStates;
    vector<Node*> finalStates;

    /* -------------- Helper Utils -------------- */ 
    
    // Populating δ({set1},σ) = {set2}, |{set1}| = 1
    void findSingletonSetReachability(Node* node, unordered_map< Node* , set<Node*> >& subsetTransition_a,  unordered_map< Node* , set<Node*> >& subsetTransition_b);
    
    // Populating δ({set1},σ) = {set2}, |{set1}| > 1
    void findSetReachability(unordered_map< string , string >& subsetTransition_a, unordered_map< string , string >& subsetTransition_b, unordered_map< Node* , set<Node*> >& delta_a, unordered_map< Node* , set<Node*> >& delta_b);

    // Wrapper for set1 = set1 U set2 
    void setUnion(set<Node*>& set1, set<Node*> set2);

    // Epsilon reachable states from a state
    set<Node*> epsilonClosure(Node* node);

    // Union of epsilon eeachable states from a set of states
    set<Node*> epsilonClosure(set<Node*> set1);

    // Gets the stringified version of a set (for using set as key)
    string stringifySet(set<Node*>& set1);

    // Filters unreachable states
    set<string> filterReachableStates(unordered_map< string , string >& subsetTransition_a, unordered_map< string , string >& subsetTransition_b, string startStateName);
    

public:

    // Regex
    stringstream regex;

    // -------------- Constructor -------------- //
    NFA(string symbol);
    NFA();
    
    // -------------- Combination Functions -------------- //
    NFA* makeUnion(NFA* nfa1, NFA* nfa2);
    NFA* makeConcat(NFA* nfa1, NFA* nfa2);
    NFA* makeKleeneStar();
    void mergeNFAs(NFA* nfa, NFA* nfa1, NFA* nfa2, bool mergeFirstFinal);

    // -------------- DFA Transformers -------------- //
    NFA* makeDFA();
    NFA* minimalDFA();

    // ------------ Test Functions ------------ //
    void test();

    // -------------- GraphWiz Code Generator and getters -------------- //
    string getCode();
    string getRegex(){return regex.str();}
    void printNFA();
        
};