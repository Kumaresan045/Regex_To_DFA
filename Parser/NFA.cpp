#include "NFA.h"

/* ------------------- Constructors ------------------- */
// Accepting a single terminal L(M) = {σ}
NFA::NFA(string symbol)
{   
    this->startState = new Node();
    Node* finalState = new Node();
    
    (this->startState)->addTransition(symbol,finalState);
    this->finalStates.push_back(finalState);
    
    this->allStates.push_back(startState);
    this->allStates.push_back(finalState);

    this->regex << symbol;
}

// Empty NFA
NFA::NFA()
{
    this->startState = nullptr;
}


/* -------------- NFA Combination Functions -------------- */ 
// Combining using '|'
NFA* NFA::makeUnion(NFA* nfa1, NFA* nfa2)
{
    NFA* unionNFA = new NFA();

    Node* newStartNode = new Node();
    newStartNode->addTransition(epsilon,nfa1->startState);
    newStartNode->addTransition(epsilon,nfa2->startState);

    unionNFA->startState = newStartNode;
    unionNFA->allStates.push_back(newStartNode);
    this->mergeNFAs(unionNFA, nfa1, nfa2, true);

    unionNFA->regex << nfa1->getRegex() << "|" << nfa2->getRegex();

    return unionNFA;
    
}

// Combining using concat operator
NFA* NFA::makeConcat(NFA* nfa1, NFA* nfa2)
{
    NFA* concatNFA = new NFA();

    concatNFA->startState = nfa1->startState;
    for(Node* finalNode: nfa1->finalStates) finalNode->addTransition(epsilon,nfa2->startState);

    this->mergeNFAs(concatNFA, nfa1, nfa2, false);

    concatNFA->regex << nfa1->getRegex() << nfa2->getRegex();

    return concatNFA;
}

// Combining using '*'
NFA* NFA::makeKleeneStar()
{
    NFA* kleeneStarNFA = new NFA();
    Node* specialNode = new Node();
    kleeneStarNFA->startState = specialNode;
    kleeneStarNFA->finalStates.push_back(specialNode);

    specialNode->addTransition(epsilon,this->startState);
    for(Node* node: this->finalStates){node->addTransition(epsilon, specialNode);}

    kleeneStarNFA->allStates = this->allStates;
    kleeneStarNFA->allStates.push_back(specialNode);

    kleeneStarNFA->regex << "*";

    return kleeneStarNFA;
}

// Merge NFAs (The set of states)
void NFA::mergeNFAs(NFA* nfa, NFA* nfa1, NFA* nfa2, bool mergeFirstFinal)
{
    for(Node* node: nfa1->allStates){nfa->allStates.push_back(node);}
    for(Node* node: nfa2->allStates){nfa->allStates.push_back(node);}

    if(mergeFirstFinal) for(Node* node: nfa1->finalStates){nfa->finalStates.push_back(node);}
    for(Node* node: nfa2->finalStates){nfa->finalStates.push_back(node);}
    
}


/* ------------------- DFA Utils ------------------- */
// Subset Construction
NFA* NFA::makeDFA()
{
    unordered_map< string , string > subsetTransition_a, subsetTransition_b;
    unordered_map< Node* , set<Node*> > delta_a, delta_b;
 
    // Initialise table for singleton set of states
    for(Node* node: this->allStates) this->findSingletonSetReachability(node, delta_a, delta_b);

    // Find reachability for subsets
    findSetReachability(subsetTransition_a ,subsetTransition_b , delta_a, delta_b);

    NFA* dfa = new NFA();

    string startStateName = "{"+ this->startState->getLabel() +"}";
    set<string> reachableSubsetStates = filterReachableStates(subsetTransition_a, subsetTransition_b, startStateName);

    // Get all final state labels
    vector<string> finalStateNames;
    for(Node* finalState: this->finalStates) finalStateNames.push_back(finalState->getLabel());
    
    // Create and Populate DFA states
    stateCnt = 0;
    unordered_map< string, Node* > createdStates;
    for(string subsetState: reachableSubsetStates) 
    {
        Node* newNode = new Node();
        createdStates[subsetState] = newNode;
        
        //Check if it contains any final states
        bool flag = false;
        for(string finalState: finalStateNames) if(subsetState.find(finalState) != string::npos){flag = true; break;}
        if(flag) dfa->finalStates.push_back(newNode);

        // Add state to list
        dfa->allStates.push_back(newNode);
    }

    // Add Start State
    dfa->startState = createdStates[startStateName];

    // Add transitions
    for(string subsetState: reachableSubsetStates) 
    {
        if(subsetTransition_a.find(subsetState) != subsetTransition_a.end() && createdStates.find(subsetTransition_a[subsetState]) != createdStates.end()) createdStates[subsetState]->addTransition("a", createdStates[subsetTransition_a[subsetState]]);
        if(subsetTransition_b.find(subsetState) != subsetTransition_b.end() && createdStates.find(subsetTransition_b[subsetState]) != createdStates.end()) createdStates[subsetState]->addTransition("b", createdStates[subsetTransition_b[subsetState]]);
    }
   
    return dfa;
}

// Removes Unreachable states
set<string> NFA::filterReachableStates(unordered_map< string , string >& subsetTransition_a, unordered_map< string , string >& subsetTransition_b, string startStateName)
{
    // BFS traversal
    queue< string > nodeQueue;
    set< string > reachableStates; 
    nodeQueue.push(startStateName);

    while (!nodeQueue.empty())
    {
        string node = nodeQueue.front();
        nodeQueue.pop();

        if(node == "{}" || reachableStates.find(node) != reachableStates.end()) continue;
        
        reachableStates.insert(node);
        if(subsetTransition_a.find(node) != subsetTransition_a.end()) nodeQueue.push(subsetTransition_a[node]);
        if(subsetTransition_b.find(node) != subsetTransition_b.end()) nodeQueue.push(subsetTransition_b[node]);
    }

    return reachableStates;
}

//Find other reachable subset's reachability
void NFA::findSetReachability(unordered_map< string , string >& subsetTransition_a, unordered_map< string , string >& subsetTransition_b, unordered_map< Node* , set<Node*> >& delta_a, unordered_map< Node* , set<Node*> >& delta_b)
{
    //Find other reachable subset's reachability
    for(auto iter: delta_a)
    {
        queue< set<Node*> > nodeQueue;
        set< Node* > startSubset; startSubset.insert(iter.first);
        nodeQueue.push(startSubset);

        while (!nodeQueue.empty())
        {
            set<Node*> nodeSet = nodeQueue.front();
            nodeQueue.pop();

            if(nodeSet.empty()) continue;

            if(subsetTransition_a.find(this->stringifySet(nodeSet)) == subsetTransition_a.end()) 
            {
                set<Node*> finalReachSet;
                for(Node* node: nodeSet) this->setUnion(finalReachSet, delta_a[node]);

                subsetTransition_a[this->stringifySet(nodeSet)] = this->stringifySet(finalReachSet);

                if(!finalReachSet.empty()) nodeQueue.push(finalReachSet);     
            }
            
            if(subsetTransition_b.find(this->stringifySet(nodeSet)) == subsetTransition_b.end()) 
            {
                set<Node*> finalReachSet;
                for(Node* node: nodeSet) this->setUnion(finalReachSet, delta_b[node]);

                subsetTransition_b[this->stringifySet(nodeSet)] = this->stringifySet(finalReachSet);

                if(!finalReachSet.empty()) nodeQueue.push(finalReachSet);
            }

            
        }
    }

}

// Populating δ({set},σ) = {set}
void NFA::findSingletonSetReachability(Node* node, unordered_map< Node* , set<Node*> >& delta_a,  unordered_map< Node* , set<Node*> >& delta_b)
{
    set<Node*> reachSet_a, reachSet_b, epsilonReach; 
    epsilonReach = this->epsilonClosure(node);

    for(Node* startState: epsilonReach)
    {
        for(auto transition: startState->getAllTransitions()) 
        {
            if(transition.first == "a") reachSet_a.insert(transition.second);
            else if(transition.first == "b") reachSet_b.insert(transition.second);
        }
    }
    delta_a[node] = this->epsilonClosure(reachSet_a);
    delta_b[node] = this->epsilonClosure(reachSet_b);
}

// Minimization Algo
NFA* NFA::minimalDFA()
{
    // Create an implicit dead state
    Node* dead_state = new Node("dead");
    this->allStates.push_back(dead_state);

    for(Node* state: this->allStates)
    {
        int a_mark = 0, b_mark = 0;
        for(pair<string,Node*> edge: state->getAllTransitions())
        {
            string chr = edge.first;
            if(chr == "a") a_mark=1;
            else if(chr == "b") b_mark=1;
        }
        if(!a_mark) state->addTransition("a",dead_state);
        if(!b_mark) state->addTransition("b",dead_state);
    }

    NFA* minDFA = new NFA();
    vector<Node*> stateList = this->allStates;

    // Initilize state type and index mapping
    int noStates = this->allStates.size();
    unordered_map< Node* , int > stateType;
    unordered_map< Node* , int > indexMap;
    for(int i=0; i<noStates; i++){stateType[this->allStates[i]] = 1; indexMap[this->allStates[i]] = i;} 
    for(Node* node: this->finalStates) stateType[node] = 0;

    // Initialize table
    vector<vector<int>> table(noStates,vector<int>(noStates,0));
    for(int i=0; i<noStates; i++) for(int j=0; j<i; j++) if(stateType[stateList[i]]+ stateType[stateList[j]] == 1) table[i][j] = 1;

    // Mark states
    bool flag = true;
    while(flag)
    {
        // Initilize flag as false
        flag = false;

        // Check for equivalent states
        for(int i=0; i<noStates; i++) 
        {
            for(int j=0; j<i; j++) if(table[i][j] == 0)
            {
                // Check if they lead to marked state
                for(auto e1: stateList[i]->getAllTransitions()) 
                    for(auto e2: stateList[j]->getAllTransitions()) 
                        if(e1.first == e2.first)
                        {
                            int newI = max(indexMap[e1.second], indexMap[e2.second]);
                            int newJ = min(indexMap[e1.second], indexMap[e2.second]);
                            if(table[newI][newJ] == 1){ table[i][j] = 1; flag = true;}
                        }                
            }
        }
    }

    // Remove dead state 
    this->allStates.pop_back();
    noStates--;
    for(Node* state: this->allStates)
    {
        vector<pair<string,Node*>> new_edges;
        for(auto edge: state->getAllTransitions())
        {
            if(edge.second->getLabel() != "dead") new_edges.push_back(edge);
        }
        state->emptyTransitions();
        for(auto edge: new_edges) state->addTransition(edge.first,edge.second);
    }
    delete dead_state;

    // Collapse map
    unordered_map <Node*, Node*> collapseMap;
    for(int i=0; i<noStates; i++) for(int j=0; j<i; j++) if(table[i][j] == 0) collapseMap[this->allStates[j]] = this->allStates[i];
    
    // Add Modified states
    for(Node* state: this->allStates)
    {
        if(collapseMap.find(state) == collapseMap.end())
        {
            minDFA->allStates.push_back(state);
            if(stateType[state]==0) minDFA->finalStates.push_back(state); 
            if(state == this->startState) minDFA->startState = state;
        }
        else
        {
            Node* replaceState = collapseMap[state];
            if(replaceState == this->startState || state == this->startState) minDFA->startState = replaceState; 
            for(auto edge: state->getAllTransitions()) replaceState->addTransition(edge.first,edge.second);

            // Replace all RHS with replaceState
            for(Node* node: this->allStates) for(auto& edge: node->getAllTransitions()) if(edge.second == state) edge.second = replaceState;
        }
    } 

    // Remove Duplicate Edges
    for(Node* node: minDFA->allStates) 
    {
        set<pair<string,Node*>> uniqueEdges;
        for(auto edge: node->getAllTransitions()) uniqueEdges.insert(edge);
        node->emptyTransitions();
        for(auto uniEdge: uniqueEdges) node->addTransition(uniEdge.first, uniEdge.second);
    }

    // Merge Edges to Same States
    for(Node* n1: minDFA->allStates) 
    { 
        unordered_map <Node*, vector<string> > edges;
        for(auto edge: n1->getAllTransitions()) edges[edge.second].push_back(edge.first);

        n1->emptyTransitions();
        for(auto ele: edges)
        {
            stringstream ss;
            for(size_t i=0; i < ele.second.size(); i++){ ss << ele.second[i]; if(i < ele.second.size()-1) ss << ",";}
            n1->addTransition(ss.str(),ele.first);
        }
    }

    //Rename States
    minDFA->startState->renameLabel("q0");
    int cnt = 1;
    for(Node* state: minDFA->allStates) if(state != minDFA->startState) state->renameLabel("q" + to_string(cnt++));
    return minDFA;
}


/* ------------------- Code Generators and Debug strings ------------------- */
// GraphWiz Code Generator
string NFA::getCode()
{

    /*
        digraph DFA {
        rankdir=LR;
        q0[label="q0", shape=doublecircle];
        q0 -> q0 [label="a, b"];
    }
    */

    stringstream finalStr;

    finalStr << "digraph DFA { \n";
    finalStr << "   rankdir=LR;\n";
    finalStr << "    start[shape=point, width=0];\n";

    // Add Final states
    for(Node* node: this->finalStates)
    {
        finalStr << "\t" << "\"" << node->getLabel() << "\"" << "[label=\"" <<  node->getLabel() << "\", shape=doublecircle];\n";
        node->marked = true;
    }
    
    // Add Normal States
    for(Node* node: this->allStates)
    {
        if(node->marked) continue;
        finalStr << "\t" << "\"" << node->getLabel() << "\"" << "[label=\"" <<  node->getLabel() << "\", shape=circle];\n";
    }

    // Add Edges
    finalStr << "\tstart -> \""<< this->startState->getLabel() << "\" [label=\"\", style=solid];\n";
    for(Node* node: this->allStates)
    {
        for(auto edge: node->getAllTransitions()) 
        {
            finalStr << "\t" << "\"" << node->getLabel() << "\"" << " -> " << "\"" << edge.second->getLabel() << "\"" <<"[label = \"" << edge.first <<"\"];\n";
        }
    }
    
    finalStr << "}\n";
    return finalStr.str();
}

//Print NFA
void NFA::printNFA()
{
    stringstream finalStr;
    for(Node* node: this->allStates) for(auto edge: node->getAllTransitions()) 
        finalStr<< node->getLabel() << " -----(" << edge.first <<  ")-----> " << edge.second->getLabel() <<'\n';
    cout << "----------------(" << this->getRegex() << ")----------------\n";
    cout << "Start: "<< this->startState->getLabel() << '\n';
    cout << "Final: ";
    for(Node* finalState: this->finalStates) cout << finalState->getLabel() << ",";
    cout<<'\n';
    cout << finalStr.str() <<'\n';
}

// Gets the stringified version of a set (for using set as key)
string NFA::stringifySet(set<Node*>& set1)
{
    stringstream finalStr;
    finalStr << "{";
    
    vector<string> allLabels;
    for(Node* node: set1) allLabels.push_back(node->getLabel());
    sort(allLabels.begin(),allLabels.end());

    for(string node: allLabels) finalStr << node << ",";
    
    string retString = finalStr.str();
    if(retString != "{") retString.back() = '}';
    else retString = "{}";

    return retString;
}


/* ------------------- Closure Functions ------------------- */
// Closure functions to remove ε transitions
set<Node*> NFA::epsilonClosure(Node* node)
{
    queue< Node* > nodeQueue;
    set<Node*> finalReachSet;
    nodeQueue.push(node);

    while (!nodeQueue.empty())
    {
        Node* nextState = nodeQueue.front();
        nodeQueue.pop();

        if(finalReachSet.find(nextState) != finalReachSet.end()) continue;
        finalReachSet.insert(nextState);

        for(auto edge: nextState->getAllTransitions()) if(edge.first == epsilon) nodeQueue.push(edge.second);
    }
    return finalReachSet;
}

// ε closure for a set
set<Node*> NFA::epsilonClosure(set<Node*> set1)
{
    set<Node*> finalSet;
    for(Node* node: set1) this->setUnion(finalSet, this->epsilonClosure(node));
    return finalSet;
}


/* ------------------- Set Utils ------------------- */
// Wrapper for set1 = set1 U set2 
void NFA::setUnion(set<Node*>& set1, set<Node*> set2)
{
    std::set_union(set1.begin(), set1.end(),     // First set's range
                   set2.begin(), set2.end(),     // Second set's range
                   inserter(set1, set1.begin()));
}


/* ------------------- Test Utils ------------------- */
void NFA::test()
{
    NFA* dfa = new NFA();
    Node* s0 = new Node("0"); dfa->allStates.push_back(s0);
    Node* s1 = new Node("1"); dfa->allStates.push_back(s1); dfa->finalStates.push_back(s1);
    Node* s2 = new Node("2"); dfa->allStates.push_back(s2);
    Node* s3 = new Node("3"); dfa->allStates.push_back(s3); 
    Node* s4 = new Node("4"); dfa->allStates.push_back(s4); dfa->finalStates.push_back(s4);
    Node* s5 = new Node("5"); dfa->allStates.push_back(s5); 

    s0->addTransition("a",s1);
    s1->addTransition("a",s2);
    s2->addTransition("a",s3);
    s3->addTransition("a",s4);
    s4->addTransition("a",s5);
    s5->addTransition("a",s0);

    cout << dfa->getCode() << '\n'; 

    dfa = dfa->minimalDFA();
    cout << dfa->getCode();

}
