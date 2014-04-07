#ifndef MODEL_H
#define MODEL_H



#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>

#include "graph.h"
#include "searcher.h"
#include "evaluator.h"

using namespace std;



class Model
{
public:


    Model();
    ~Model();

    void InitModel();
    void ReadFile(const char * file_name);
    void OutputGraph();
    void Search(Searcher::SearcherTypes type);
    void FamilyQuery(string node, vector<string> pa_nodes);
    void OutputFormatedGraph(const char* file_name);
    void OutputVizGraph(const char *file_name);
protected:

private:
    BGraph* model_graph_;
    Searcher* model_searcher_;
    Evaluator* model_eval_;



};

#endif // MODEL_H
