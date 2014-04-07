#ifndef SEARCHER_H
#define SEARCHER_H


class Evaluator;
class BGraph;

class Searcher
{

public:
    Searcher();
    Searcher(BGraph* graph, Evaluator *eval);
    void AStar();
    void GreedyHill(const unsigned int times);
    void DeterministicGreedyHill(const unsigned int times);
    void ChowLiu();

    // For greedy hill climbing
    void PermutGraph();
    void RegPerG();

    enum class Opts:int{DELETE=1,FLIP=2,CONNECT=3};

    // for search method
    enum class SearcherTypes{ kDStar, kGreedyHill, kDeterministicGreedyHill, kDynamic, kAStar, kChowLiu};

private:

    BGraph* graph_;
    Evaluator* eval_;
    SearcherTypes current_method_;
    void current_method();
    SearcherTypes set_current_method();

};

#include "graph.h"
#include "evaluator.h"
#endif // SEARCHER_H
