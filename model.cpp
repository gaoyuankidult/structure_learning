#include "model.h"


using namespace std;





Model::Model()
{

}

Model::~Model()
{
    if(model_searcher_) {
        delete model_searcher_;
    }
    if(model_eval_) {
        delete model_eval_;
    }
    if(model_graph_) {
        delete model_graph_;
    }


}
void Model::InitModel() {
    model_graph_ = new BGraph(BGraph::InitMehotd::ZERO);
    model_eval_ = new Evaluator(model_graph_);
    model_searcher_ = new Searcher(model_graph_,model_eval_);
}


void Model::OutputGraph() {
    model_graph_->OutputGraph(model_eval_);
}

void Model::ReadFile(const char *file_name) {
    model_eval_->ProcessFile(file_name);
}

void Model::OutputFormatedGraph(const char *file_name) {
    model_graph_->OutputGraph(file_name,model_eval_);
}
void Model::OutputVizGraph(const char *file_name) {
    model_graph_->OutputVizGraph(file_name,model_eval_);
}

void Model::Search(Searcher::SearcherTypes type) {
    switch (type) {
    case Searcher::SearcherTypes::kAStar:
        model_searcher_->AStar();
        break;
    case Searcher::SearcherTypes::kDeterministicGreedyHill:
        model_searcher_->DeterministicGreedyHill(50);
        break;
    case Searcher::SearcherTypes::kGreedyHill:
        model_searcher_->GreedyHill(1000);
        break;
    case Searcher::SearcherTypes::kDynamic:
        break;
    case Searcher::SearcherTypes::kChowLiu:
        model_searcher_->ChowLiu();
        break;
    case Searcher::SearcherTypes::kDStar:
        break;
    default:
        break;
    }
}


