//Meeshaan Shah
//CPSC 406
//Partner in Crime: Ani Khalili
//nfa to dfa conversion

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <sstream>
#include <vector>
#include <set>
#include <map>

using namespace std;

//return number of words in a string
int countWords(string line)
{
  int numSpaces = 0;

  for(int i = 0; i < line.length(); i++)
  {
    if(isspace(line.at(i)))
    {
      ++numSpaces;
    }
  }
  // The number of words = the number of spaces + 1
  return numSpaces + 1;
}

//returns the factorial of a given number
int Factorial(int n)
{
    if (n == 0)
        return 1;
    else
        return n*Factorial(n-1);
}

//output sets to file
void outputSet(ofstream &output, set<string> aSet)
{
	//get number of commas
	int numCom = aSet.size() - 1;
	
	output << "{";
	if (aSet.empty())
	{
		output << "EM";
	}
	else
	{
		for(set<string>::iterator it=aSet.begin(); it!=aSet.end(); ++it)
		{
			output << *it;
			//place comma after outputting string
			if (numCom > 0)
			{
				output << ",";
				--numCom;
			}		
		}
	}
	output << "}";
}

//reads from command line
int main(int argc, char *argv[])
{	
	//shows how program needs to run if command line entry was wrong
	if (argc != 2)
	{
		cout << "usage: "<< argv[0] <<  " </../nameOfFile.txt>" << endl;
	}
	//actual program
	else
	{
		string fileName = argv[1];
		ifstream file(fileName);
		
		//check if did not open
		if (!file.is_open())
		{
     	cout<< "Could not open file" << endl;
		}
    	//if file is open
    	else 
    	{
    		//READ FILE AND PLACE IN NFA TABLE
    		
    		//loop through once to get number of states and transitions
    		string line;
    		
    		//gets number of states (line 1)
    		getline(file, line);
    		int numStates = countWords(line);
    		
    		//gets number of inputs (line 2)
    		getline(file, line);
    		int numInputs = countWords(line);
    		
    		//get q0 (line 3)
    		getline(file, line);
    		string startState = line;
    		
    		//get number of accept states (line 4)
    		getline(file, line);
    		int numAccStates = countWords(line);
    		
    		//gets number of transitions (line 5 to EOF)
    		int numTrans = 0;
    		while(file.peek() != EOF)
    		{
    			getline(file, line);
    			++numTrans;
    		}
    		
    		//sets the position back to the beginning of the input file to read throuh
    		file.clear();
    		file.seekg(0, file.beg);
   			
   			//create arrays to store states and inputs
    		string *NFAStates = new string[numStates];
    		string *NFAInputs = new string[numInputs+1];
    		string *NFAaccStates = new string[numAccStates];
    		
    		//for line parsing
    		string token;
			stringstream iss;
    		
    		//stores states
    		getline(file, line);
			iss << line;
			for (int i = 0; i < numStates; ++i)
			{
				getline(iss, token, '\t');
				NFAStates[i] = token;
			}
			iss.clear();
			
			//change NFAState[0] to the start state if not already done so
			bool notFound = true;
			int inc = 0;
			if (NFAStates[0] != startState)
			{
				while (notFound)
				{
					++inc;
					if (NFAStates[inc] == startState)
					{
						string temp = NFAStates[0];
						NFAStates[0] = NFAStates[inc];
						NFAStates[inc] = temp;
						notFound = false;
					}
				}
			}
			
			//stores inputs
    		getline(file, line);
			iss << line;
			for (int i = 0; i < numInputs; ++i)
			{
				getline(iss, token, '\t');
				NFAInputs[i] = token;
			}
			iss.clear();
			NFAInputs[numInputs] = "EPS"; //sets epsilon
			
			//skip q0 cause we already have it
			getline(file, line);
			
			//stores accept states
    		getline(file, line);
			iss << line;
			for (int i = 0; i < numAccStates; ++i)
			{
				getline(iss, token, '\t');
				NFAaccStates[i] = token;
			}
			iss.clear();
			
			//create maps for states and inputs
			map<string, int> stateMap;
			map<string, int> inputMap;
			
			//creates map for the state array
			for (int i = 0; i < numStates; ++i)
			{
				stateMap[NFAStates[i]] = i;
			}
			
			//creats map for the input array
			for (int i = 0; i < numInputs + 1; ++i)
			{
				inputMap[NFAInputs[i]] = i;
			}
			
			//declare NFA table, double vector of string sets
			vector<vector<set<string> > > NFATable;
			int inputSize = numInputs + 1;
			
			NFATable.resize(numStates);
			for (int i = 0; i < numStates; ++i)
			{
				NFATable[i].resize(inputSize);
			}
			
			//parse transitions and place states with given transitions into NFA Table
			string d1 = ",";
			string d2 = " = ";
			string x = "";		//state
			string y = "";		//input
			string z = "";		//state input goes to
			for (int i = 0; i < numTrans; ++i)
			{
				getline(file, line);
				x = line.substr(0, line.find(d1));
				y = line.substr(line.find(d1)+1, line.find(d2)-2);
				z = line.substr(line.find(d2)+3);
				NFATable[stateMap[x]][inputMap[y]].insert(z);
			}
			
			//adding states to their own epsilon closure
			for (int i = 0; i < numStates; ++i)
			{
			    NFATable[i][numInputs].insert(NFAStates[i]);
			}
			
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
			
			//DFA CONVERSION
			
			//declare DFA table, double vector of string sets
			vector<vector<set<string> > > DFATable;
			int DFATran = Factorial(numStates) + 1;		
			DFATable.resize(DFATran);
			for (int i = 0; i < DFATran; ++i)
			{
				DFATable[i].resize(numInputs);
			}			
			
			//declare insertion and DFAStates vector checker to check states we need to check
			set<string> getDFASet;
			vector<set<string> > DFAStates;
			DFAStates.resize(DFATran);
			
			//insert q0's epsilon states into checker vector to begin algorithm
			getDFASet.insert(NFATable[stateMap[startState]][numInputs].begin(), NFATable[stateMap[startState]][numInputs].end());
			DFAStates[0].insert(getDFASet.begin(), getDFASet.end());
			
			//for iterating through sets
			string NFAString = "";
			string NFAStringMulti = "";
			
			//for counting actual number of states
			int DFAStateCount = 1;

			//loop through the rows of DFA table (states)
			for (int z = 0; z < DFATran; ++z)
			{
				//loop through inputs on DFA table (inputs)
			    for(int j = 0; j < numInputs; ++j)
			    {
			    	//loop through epsilon set to fill DFA table
		            for (set<string>::iterator itEpsSet=getDFASet.begin(); itEpsSet!=getDFASet.end(); ++ itEpsSet)
		            {
		            	//if epsilon's NFA table location is 1
	                    if (NFATable[stateMap[*itEpsSet]][j].size() == 1)
	                    {
	                        //get that state in set and go to its epsilon closure
	                        for(set<string>::iterator it=NFATable[stateMap[*itEpsSet]][j].begin(); it!=NFATable[stateMap[*itEpsSet]][j].end(); ++it)
	                        {
	                            NFAString = *it;
	                            
	                            //insert epsilon set int DFA Table if not empty
	                            if (NFAString != "EM")
                            	{
                            		//insert into table
	                            	DFATable[z][j].insert(NFATable[stateMap[NFAString]][numInputs].begin(), NFATable[stateMap[NFAString]][numInputs].end());
                            	}
	                        }          
	                    }
	                    //if epsilon's NFA table location is greater that 1
	                    if (NFATable[stateMap[*itEpsSet]][j].size() > 1)
	                    {
	                    	//get that state in set and go to its epsilon closure
	                        for(set<string>::iterator it=NFATable[stateMap[*itEpsSet]][j].begin(); it!=NFATable[stateMap[*itEpsSet]][j].end(); ++it)
	                        {
	                            NFAStringMulti = *it;
	                            //insert into table
	                            DFATable[z][j].insert(NFATable[stateMap[NFAStringMulti]][numInputs].begin(), NFATable[stateMap[NFAStringMulti]][numInputs].end());
	                        }    
	
	                    }
		            }
			    }
			    
			//check the current row for any new states to be added to our DFA checker vector
		    bool duplicate = false;
		    
		    //loop through inputs
		    for (int a = 0; a < numInputs; ++a)
		    {
		    	//if given state is empty
	            if (DFATable[z][a].empty())
	            {
	                //do nothing
	            }
	            else
	        	{
	        		//loop through DFA vector checker
	            	for (int b = 0; b < DFATran; ++b)
		            {
		            	//check if we have a duplicate
		                if (DFAStates[b] == DFATable[z][a])
		                {
		                    duplicate = true;
		                    b = DFATran; //stops the loop if we found duplicate
		                }            
		                
		            }
		            //if not duplicate
		            if (duplicate == false)
		            {
		            	//loop through DFA vector checker and place state into next empty spot
		                for (int c = 0; c < DFATran; ++c)
		                {
		                    if(DFAStates[c].empty())
		                    {
		                        DFAStates[c].insert(DFATable[z][a].begin(), DFATable[z][a].end());
		                        ++DFAStateCount;	//for printing out, for counting actual number of states in DFA
		                        break;
		                    }
		
		                }
		                
		            }
		        }
		        //set duplicate to false for the next state
		        duplicate = false;
		    }
		    //erase DFA set getter to use again in loop
			getDFASet.erase(getDFASet.begin(), getDFASet.end());
		    
		    //make sure we don't loop through the size of the vector
		    if ((z+1) < DFATran)
		        getDFASet.insert(DFAStates[z+1].begin(), DFAStates[z+1].end());
			}
			
//--------------------------------------------------------------------------------------------------------------------------------------------------------
			
			//PRINT RESULTS TO FILE
			
			//declare output stream
			ofstream outFile;
			outFile.open("dfa.DFA");
			
			//output DFA states
			for (int i = 0; i < DFAStateCount; ++i)
			{
				outputSet(outFile, DFAStates[i]);
				//for handling tabs
				if (i < DFAStateCount - 1)
					outFile << "\t";
			}
			outFile << endl;
			
			//output inputs
			for (int i = 0; i < numInputs; ++i)
			{
				outFile << NFAInputs[i];
				//handling tabs
				if (i < numInputs - 1)
					outFile << "\t";
			}
			outFile << endl;
			
			//output start state
			outputSet(outFile, DFAStates[0]);
			outFile << endl;
			
			//output accept states
			int acceptCount = 0;
			//get number of accepts states to handle tabs
			for (int i = 0; i < numAccStates; ++i)
			{
				string acceptState = NFAaccStates[i];
				for (int j = 0; j < DFAStateCount; ++j)
				{
					for(set<string>::iterator it=DFAStates[j].begin(); it!=DFAStates[j].end(); ++it) 
					{
						string stringCheck = *it;
						//count if accept state
						if (acceptState.compare(stringCheck) == 0)
						{
							++acceptCount;
						}
					}
				}
			}
			//outputs the states
			for (int i = 0; i < numAccStates; ++i)
			{
				string acceptState = NFAaccStates[i];
				for (int j = 0; j < DFAStateCount; ++j)
				{
					for(set<string>::iterator it=DFAStates[j].begin(); it!=DFAStates[j].end(); ++it) 
					{
						string stringCheck = *it;
						//output state if string in set matches accept string from accept state array
						if (acceptState.compare(stringCheck) == 0)
						{
							outputSet(outFile, DFAStates[j]);
						}
					}
					//handles tabs
					if (acceptCount-1 > 0)
					{
						outFile << "\t";
						--acceptCount;
					}
				}
			}
			outFile << endl;
			
			//output transitions for dfa
			for (int i = 0; i < DFAStateCount; ++i)
			{
				for (int j = 0; j < numInputs; ++j)
				{
					outputSet(outFile, DFAStates[i]);
					outFile << "," << NFAInputs[j] << " = ";
					outputSet(outFile, DFATable[i][j]);
					outFile << endl;
				}
			}
        }
	}
	
	return 0;
}