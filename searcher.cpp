#include "searcher.h"

Searcher::Searcher(BGraph* graph, Evaluator *eval)
{
    graph_ = graph;
    eval_ =eval;
}


void Searcher::AStar() {

    current_method_ = SearcherTypes::kAStar;
}

void Searcher::PermutGraph() {
    BGraph* &g = graph_;
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> optsdis(1,3);
    Opts random_opt = static_cast<Opts>(optsdis(generator));
    std::uniform_int_distribution<int> vardis(0,25);
    int randrow = vardis(generator);
    int randcol = vardis(generator);

    // ensure we do not run into
    // a node pointing to itself.
    while(randrow == randcol) {
        randrow = vardis(generator);
        randcol = vardis(generator);
    }
    std::pair<int,int> selection;
    selection.first = randrow;
    selection.second = randcol;
    graph_->set_last_selection(selection);
    graph_->set_last_opt(random_opt);


    switch(random_opt) {
    case Opts::DELETE:
        g->SetGMember(randrow,randcol,false);
        break;
    case Opts::FLIP:
        g->Swap(randrow,randcol);
        break;
    case Opts::CONNECT:
        if(true != g->GMember(randcol,randrow))
            g->SetGMember(randrow,randcol,true);
        break;
    default:
        break;
    }

}
/***********************************************
  This function is applied, when regret to permute
  Graph.

  **********************************************/
void Searcher::RegPerG() {
    auto last_selection = graph_->last_selection();
    int randrow = last_selection.first;
    int randcol = last_selection.second;
    BGraph* &g = graph_;

    switch(graph_->last_opt()) {
    case Opts::DELETE:
        g->SetGMember(randrow,randcol,true);
        break;
    case Opts::FLIP:
        g->Swap(randrow,randcol);
        break;
    case Opts::CONNECT:
        g->SetGMember(randrow,randcol,false);
        break;
    default:
        break;
    }
}

void Searcher::GreedyHill(const unsigned int times) {
    cout << "Starting Greedy Hill Climbing..." << endl;
    graph_->set_directed(BGraph::DirectedNess::kDirected);
    double score = 0.0, old_score = -INFINITY;


    uint i = 0;
    //model.OutputGraph();
    while(i< times) {
        score = eval_->EvaluateBDeuModel();
        if(score < old_score) {
            if (i!=1)
                RegPerG();
            //cout << "regret " << test_score << " Opt: " <<  (int)last_opt << endl;
        } else {
            old_score = score;
        }

        PermutGraph();

        ++i;
        if(0 == i%1) {
            std::cout << i<< std::endl;
        }
    }
    std::cout << "The final score is :" << score << std::endl;
}

void Searcher::DeterministicGreedyHill(const unsigned int times) {
    cout << "Starting Deterministics Greedy Hill Climbing..." << endl;
    typedef struct UpdateInfo {
        pair<int,int> pa_pair;
        double score;
        Opts opt;
    }uinfo ;

    // information of algorithm best strategy
    uinfo info;
    // for specifying graph direction
    graph_->set_directed(BGraph::DirectedNess::kDirected);
    // initial
    double score = 0.0;
    double temp_score = -INFINITY;
    double best_score = -INFINITY;
    double local_best_score = -INFINITY;
    double old_best_score = -INFINITY;
    uint i = 0;
    boost::numeric::ublas::matrix<double> score_map;

//    vector<int> temp_parents;
//    temp_parents.clear();
//    temp_parents.push_back(1);
//    temp_parents.push_back(2);
//    temp_parents.push_back(3);
//    cout << eval_->EvaluateBDeu(eval_->GetVarName(6),temp_parents,1.0) << endl;
    while(i< times) {
        // calculate all parents of each variable
        auto vec = graph_->GetPaNumMatrix();
        // evaluate score of the model
        score_map = eval_->EvaluateBDeuMapModel();
        score = 0.0;
        for(int l = 0; l < eval_->var_num_; ++l) {
            score += score_map(0,l);
        }
        for(int j = 0; j < eval_->var_num_; ++j) { // for each row
            for(int k = 0; k < eval_->var_num_; ++k) { // for each column
                if(j != k) {
                    Opts opt;
                    vector<int> temp_parents = vec.at(j);
                    // for any j'th element, we reevaluate
                    temp_score = 0.0;
                    local_best_score = -INFINITY;
                    // if k already in the list then remove it
                    if (true == graph_->GMember(j,k)) {
                        temp_parents.erase(std::remove(temp_parents.begin(), temp_parents.end(), k), temp_parents.end());
                        opt = Opts::DELETE;
                    }
                    // if k is not in the list then add it.
                    else {
                        temp_parents.push_back(k);
                        opt = Opts::CONNECT;
                    }
                    // substract j'th score and add the newly connected score.
                    temp_score = score - score_map(0,j) + eval_->EvaluateBDeu(eval_->GetVarName(j),temp_parents,1.0);
                    //cout << "j: " << eval_->GetVarName(j) << " k: " << eval_->GetVarName(k) << " temp score: " << temp_score << endl;
                    local_best_score = temp_score > local_best_score ? temp_score:local_best_score;

                    // test flip
                    vector<int> j_parents = vec.at(j);
                    vector<int> k_parents = vec.at(k);
                    if(false == graph_->GMember(j,k) && true == graph_->GMember(k,j)) {
                        k_parents.erase(std::remove(k_parents.begin(), k_parents.end(), j), k_parents.end());
                        j_parents.push_back(k);
                        temp_score = score - score_map(0,j) -score_map(0,k) \
                                + eval_->EvaluateBDeu(eval_->GetVarName(j),j_parents,1.0) \
                                + eval_->EvaluateBDeu(eval_->GetVarName(k),k_parents,1.0);
                    }
                    else if (true == graph_->GMember(j,k) && false == graph_->GMember(k,j)) {
                        j_parents.erase(std::remove(j_parents.begin(), j_parents.end(), k), j_parents.end());
                        k_parents.push_back(j);
                        temp_score = score - score_map(0,j) -score_map(0,k) \
                                + eval_->EvaluateBDeu(eval_->GetVarName(j),j_parents,1.0) \
                                + eval_->EvaluateBDeu(eval_->GetVarName(k),k_parents,1.0);
                    }

                    if( temp_score > local_best_score) {
                        local_best_score = temp_score;
                        opt = Opts::FLIP;
                    }

                    // update info and update local best to global best
                    if( local_best_score > best_score) {
                        best_score = local_best_score;
                        info.pa_pair = pair<int,int>(j,k);
                        info.opt = opt;
                        info.score = best_score;
                    }
                }
            }
        }
        // ensure an empty info container will not be updated.
        if (best_score > old_best_score) {
            old_best_score = best_score;
            bool change_value = false;
            switch(info.opt) {
            case Opts::DELETE:
                change_value = false;
                graph_->SetGMember(info.pa_pair.first,info.pa_pair.second,change_value);
                cout << "set: "<< eval_->GetVarName(info.pa_pair.first) << ", " << eval_->GetVarName(info.pa_pair.second) << " to be " << change_value << endl;
                break;
            case Opts::CONNECT:
                change_value = true;
                graph_->SetGMember(info.pa_pair.first,info.pa_pair.second,change_value);
                cout << "set: " << eval_->GetVarName(info.pa_pair.first) << ", " << eval_->GetVarName(info.pa_pair.second) << " to be " << change_value << endl;
                break;
            case Opts::FLIP:
                graph_->Swap(info.pa_pair.first,info.pa_pair.second);
                cout << "flip: " << eval_->GetVarName(info.pa_pair.first) << ", " << eval_->GetVarName(info.pa_pair.second) << endl;
                break;
            default:
                break;
            }
        }
        else {
        }

        cout << i << endl;
        cout << "best_score: " << best_score << endl;
        ++i;
    }
}

void Searcher::ChowLiu() {
    //eval_->EvaluateKL();
    eval_->EvaluateWeightedKL();
    const auto & queue = eval_->GetQueue();
    graph_->ZerosNet();
    graph_->set_directed(BGraph::DirectedNess::kUndirected);
    graph_->LearnWeightedSpanningTree(queue);
    //eval_->PrintOutKL();
}
