
#include "graph.h"

using namespace boost;

BGraph::BGraph()
{
}

BGraph::BGraph(InitMehotd method) {
    last_opt_ = Searcher::Opts::DELETE;
    directed_ = DirectedNess::kUndefined;
    switch(method) {

    case InitMehotd::ZERO:
        ZerosNet();
        break;
    case InitMehotd::RANDOM:
        RandomizeNet();
        break;
    default:
        break;
    }
}

void BGraph::RandomizeNet() {
    boost::mt19937 rng;
    boost::uniform_01<boost::mt19937> zeroone(rng);
    for(int i = 0; i < 26; ++i) {
        for(int j = 0; j < 26; ++j) {
            g_[i][j] = zeroone();
        }
    }
}

void BGraph::ZerosNet() {
    for(int i = 0; i < 26; ++i) {
        for(int j = 0; j < 26; ++j) {
            g_[i][j] = 0;
        }
    }
}

void BGraph::SetGMember(const int row, const int col, bool value) {
    g_[row][col] = value;
}

bool BGraph::GMember(const int row, const int col) {
    return g_[row][col];
}

void BGraph::Swap(const int row, const int col) {
    bool temp_bool = g_[row][col];
    g_[row][col] = g_[col][row];
    g_[col][row] = temp_bool;
}

void BGraph::OutputGraph(Evaluator* eval) {
    assert(directed_ != DirectedNess::kUndefined);
    std::cout << std::endl;
    std::cout << "    ";
    for(int i = 0; i < var_num_; ++i) {
        std::cout << eval->GetVarName(i) << " ";
    }
    std::cout << std::endl;
    for(int i = 0; i < var_num_ ; ++i) {
        std::cout << "  " << eval->GetVarName(i) << " ";
        for(int j = 0; j < var_num_ ; ++j) {
            std::cout << g_[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void BGraph::OutputGraph(const char* file_name,Evaluator* eval) {
    assert(directed_ != DirectedNess::kUndefined);
    ofstream f;
    f.open (file_name);

    for(int i = 0; i < var_num_ ; ++i) {
        for(int j = 0; j < var_num_; ++j) {
            if(true == g_[i][j])
                f << eval->GetVarName(i) << " " << eval->GetVarName(j) << endl;
        }
    }
    f.close();
}

// double in pair<double,pair<int,int> > indicates the orther
void BGraph::LearnWeightedSpanningTree(const vector<pair<double,pair<int,int> > > & pri_queue) {
    for(const auto & elemt : pri_queue) {
        //cout << "circle now ? " << IsUndirCyclic() << endl;
        //OutputGraph();
        g_[elemt.second.first][elemt.second.second] = true;
        g_[elemt.second.second][elemt.second.first] = true;
        if(IsUndirCyclic()) {
            g_[elemt.second.first][elemt.second.second] = false;
            g_[elemt.second.second][elemt.second.first] = false;
        }
    }
}

bool BGraph::IsUnCycUntil(int i, int last, bool visited[], bool *rec_stack)
{
    if(visited[i] == false)
    {
        // Mark the current node as visited and part of recursion stack
        visited[i] = true;
        rec_stack[i] = true;

        // Recur for all the vertices adjacent to this vertex
        for(int j = 0; j < var_num_; ++j)
        {
            // ignore the last element as in undirected graph if g_[i][j] == true then g_[j][i]
            if(true == g_[i][j] && j != last) {
                if ( !visited[j] && IsUnCycUntil(j,i, visited, rec_stack) )
                    return true;
                else if (rec_stack[j])
                    return true;
            }
        }
    }
    if(i != last)
        rec_stack[i] = false;  // remove the vertex from recursion stack
    return false;
}

// Returns true if the graph contains a cycle, else false.
// This function is a variation of DFS() in http://www.geeksforgeeks.org/archives/18212
bool BGraph::IsUndirCyclic()
{
    // Mark all the vertices as not visited and not part of recursion
    // stack
    bool *visited = new bool[var_num_];
    bool *rec_stack = new bool[var_num_];
    for(int i = 0; i < var_num_; i++)
    {
        visited[i] = false;
        rec_stack[i] = false;
    }

    // Call the recursive helper function to detect cycle in different
    // DFS trees
    for(int i = 0; i < var_num_; i++)
        // there is no last value for compare then we set the value to max as there is
        // there is not value equaling to max
        if (IsUnCycUntil(i,std::numeric_limits<int>::max(), visited, rec_stack))
            return true;

    delete[] visited;
    delete[] rec_stack;
    return false;
}

void BGraph::OutputVizGraph(const char *file_name, Evaluator *eval) {
    assert(directed_ != DirectedNess::kUndefined);
    ofstream f;
    f.open (file_name);
    string edge_str;
    switch(directed_) {
    case DirectedNess::kUndirected:
        edge_str = "";
    case DirectedNess::kDirected:
        edge_str = "dir=none";
    default:
        break;
    }

    f << "digraph G {" << endl;

    // only output for once, so it only goes through half a matrix
    for(int i = 0; i < var_num_ ; ++i) {
        for(int j = 0; j < i; ++j) {
            if(true == g_[i][j])
                f << eval->GetVarName(i) << " -> " << eval->GetVarName(j) <<"[" << edge_str << "];" << endl;
        }
    }
    f << "}";
    f.close();
}

std::vector<int> BGraph::GetPaNumVector(int index) {
    int j = 0;
    vector<int> temp;
    do {
        if(1 == g_[index][j])
            temp.push_back(index);
        ++j;
    }while(j < 26);
    return temp;
}

std::vector<vector<int> > BGraph::GetPaNumMatrix() {

    vector<vector<int> > rank;
    for(int i = 0; i < 26 ; ++i) {
        rank.push_back(GetPaNumVector(i));
    }
    return rank;
}
