#ifndef BGRAPH_H
#define BGRAPH_H

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "searcher.h"
class Searcher;
class Evaluator;

class BGraph
{
    friend class Evaluator;
public:
    enum class InitMehotd {ZERO, RANDOM};
    enum class DirectedNess {kDirected, kUndirected, kUndefined};
    BGraph();
    BGraph(InitMehotd method);
    void ZerosNet();
    void RandomizeNet();
    void SetGMember(const int row, const int col,bool value);
    bool GMember(const int row, const int col);
    void Swap(const int row, const int col);
    void LearnWeightedSpanningTree(const std::vector<std::pair<double,std::pair<int,int> > > & pri_queue);

    // return bit map of matrix
    std::vector<int> GetPaNumVector(int index);
    std::vector<std::vector<int> > GetPaNumMatrix();
    void OutputGraph(Evaluator* eval);
    void OutputGraph(const char* file_name,Evaluator* eval);
    void OutputVizGraph(const char * file_name, Evaluator* eval);

    // getter and setters
    Searcher::Opts last_opt(){return last_opt_;}
    void set_last_opt(Searcher::Opts opt ){last_opt_ = opt;}

    std::pair<int,int> last_selection(){return last_selection_;}
    void set_last_selection(std::pair<int,int>last_selection){ last_selection_ = last_selection;}

    void set_directed(DirectedNess directed){directed_ = directed;}
    DirectedNess directed(){return directed_;}


private:
    //row is source and column is child
    bool g_[26][26];

    static const int var_num_ = 26;

    std::pair<int,int> last_selection_;
    Searcher::Opts last_opt_;


    DirectedNess directed_;

    bool IsUnCycUntil(int i,int last, bool visited[], bool *rec_stack);
    bool IsUndirCyclic();

};

#endif // GRAPH_H
