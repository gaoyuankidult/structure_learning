#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <string>
#include <map>
#include <fstream>
#include <cmath>
#include <ctgmath>
#include <cstdint>
#include <cstdlib>
#include <queue>
#include <iomanip>

#include <boost/numeric/ublas/matrix.hpp>

#include "graph.h"
#include "searcher.h"

using namespace std;

class Searcher;

// for compare two typedef pair<double,pair<int,int>*> types
struct CompareDist
{
public:
    bool operator()(pair<double,pair<int,int> > p1,pair<double,pair<int,int> > p2)
    {

        if(p1.first>p2.first)
            return true;
        else
            return false;
    }
};

class Evaluator
{
public:
    Evaluator();
    ~Evaluator();
    Evaluator(BGraph *graph);

    static const int kLine_ = 2500;
    static const int var_num_ = 26;
    static const int var_value_num_ = 3;

    typedef pair<double,pair<int,int> > elem;

    void ProcessFile(const char* file_name);

    // input node names and their corresponding values, output the probability
    double EvaluateQuery(const string& a,const int a_value, const string& b,const int b_value);

    void EvaluateMDL(BGraph* graph, int* data_statistics);

    // only return total score
    double EvaluateBDeuModel();
    // return score and 26 x 26 score map
    boost::numeric::ublas::matrix<double> EvaluateBDeuMapModel();

    double EvaluateBDeuModel(const int & row, const int& col, Searcher::Opts opt, double score);

    void EvaluateBDe(BGraph* graph, int* data_statistics);
    double EvaluateBDeu(const string& var, const vector<string>& parents, double alpha);
    double EvaluateBDeu(const string& var, const vector<int>& parents, double alpha);

    void EvaluateBIC(BGraph* graph, int* data_statistics);

    void EvaluateKL();
    void EvaluateWeightedKL();
    void PrintOutKL();
    const std::vector<elem> & GetQueue();

    const string & GetVarName(int index);

private:

    void MakeStatistics(const string& var, const vector<string>& parents);
    BGraph* graph_;
    map<vector<int>,int> current_counts_;
    vector<string> var_name_;
    map<string,int*>  data_statistics_;
    map<string,int*>  all_data_;

    // for ChowLiuAlgorithm

    std::vector<elem> queue;
};

inline const string & Evaluator::GetVarName(int index){ return var_name_.at(index);}

#endif // EVALUATOR_H
