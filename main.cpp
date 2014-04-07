#include <iostream>
#include "model.h"

using namespace std;




void ModelUnitTest() {
    Model model;
    model.InitModel ();
    model.ReadFile("training_data.txt");
    model.Search(Searcher::SearcherTypes::kDeterministicGreedyHill);
    model.OutputGraph();
    model.OutputVizGraph("viz_test.txt");
    model.OutputFormatedGraph("file_test.txt");
}



int main()
{
    //GraphUnitTest();
    ModelUnitTest();
}


