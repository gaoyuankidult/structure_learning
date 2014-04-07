#include "evaluator.h"


static char* Str2Var(const vector<string>& strs) {
    char* array = new char[strs.size()];
    for (unsigned int i= 0; i < strs.size() ; ++i) {
        array[i] = boost::lexical_cast<char>(strs.at(i));
    }
    return array;
}
static vector<vector<int>> CartProduct ( vector<vector<int>> &  v) {
                           vector<vector<int>> s = {{}};
for (auto& u : v) {
    vector<vector<int>> r;
    for (auto& x : s) {
        for (auto y : u) {
            r.push_back(x);
            r.back().push_back(y);
        }
    }
    s.swap(r);
}
return s;
}


Evaluator::Evaluator()
{

}

Evaluator::~Evaluator() {
    for(auto& data : data_statistics_) {
        if(data.second) {
            delete[] data.second;
        }
        data.second = NULL;
    }
    for(auto& data : all_data_) {
        if(data.second) {
            delete[] data.second;
        }
        data.second = NULL;
    }
}

Evaluator::Evaluator(BGraph* graph)
{
    graph_ = graph;
}

void Evaluator::ProcessFile(const char *file_name) {
    ifstream infile;
    string str;
    infile.open(file_name);

    // get first line and split the characters
    getline(infile,str);
    vector<string> strs;
    boost::split(strs, str, boost::is_any_of(" "));

    char *array = Str2Var(strs);
    const int kTypeNum = 3;

    for (int i = 0; i < var_num_; i++) {
        string var_name = std::string(1,array[i]);
        var_name_.push_back(var_name);
        data_statistics_[var_name] = new int[kTypeNum]();
        all_data_[var_name] = new int[kLine_]();
    }

    int j = 0;
    while (true) {
        getline(infile,str);


        if(!str.empty()) {
            boost::split(strs, str, boost::is_any_of(" "));
            for (int i = 0; i < var_num_; ++i) {
                int type = std::stoi(strs.at(i));
                string var_name = var_name_.at(i);
                data_statistics_[var_name][type-1] ++ ;
                all_data_[var_name][j] = type;

            }
            ++j;
        }
        if(infile.eof()) break;

    }



    if(array) {
        delete[] array;
        array = nullptr;
    }
    infile.close();
}

double Evaluator::EvaluateBDeu(const string& var, const vector<string>& parents,double alpha) {
    //cout << "endter EvaluateBDeu" << endl;
    double score = 0.0;
    int q_i = pow(var_value_num_,parents.size());
    //cout << var << " q_i " << q_i << endl;
    int r_i = var_value_num_;
    //cout << var << " r_i " << r_i << endl;
    double alpha_i = alpha/q_i;
    double alpha_child = alpha_i/r_i;

    MakeStatistics(var,parents);



    // this represents all conbinations of node and parent together.
    int all_combi = q_i * r_i;
    double* node_value = new double[q_i]();
    double* node_alpha = new double[all_combi]();


    vector<int> temp;
    for(int i = 1; i <= 3; ++i) {
        temp.push_back(i);
    }
    vector<vector<int>> temp_vec;
    for(uint i = 0; i < parents.size()+1 ; ++i) {
        temp_vec.push_back(temp);
    }
    vector<vector<int>> cart = CartProduct(temp_vec);

    //-----------------------------------------------

    for(uint i = 0; i < current_counts_.size(); ++i) {
        node_value[i%q_i] += current_counts_[cart.at(i)];
        node_alpha[i] = (double)current_counts_[cart.at(i)] + alpha_child;
    }
    //cout << "q_i" << q_i << endl;
    for(uint i = 0; i < q_i; ++i) {
        node_value[i] += alpha_i;
//        cout << "enter loop" << node_value[i]<< endl;
    }
//    cout << "score1: " << score << "alpha_i: " << alpha_i << endl;

    score += lgamma(alpha_i) * q_i;
    // cout << "score2: " << score << endl;
    score -= lgamma(alpha_child) * q_i * r_i;
//    cout << "score3: " << score << endl;
    // cout << "all_combi: " << all_combi << endl;
    for(int i = 0; i < all_combi; ++i ){
        score += lgamma(node_alpha[i]);
    }
//    cout << "score4: " << score << endl;
    for(int i = 0; i < q_i; ++i ) {
        score -= lgamma(node_value[i]);
//        cout << "i:" << i << " " << node_value[i] << endl;
    }
    //cout << "score5: " << score << "node_value[k]" << node_value[0] << endl;
    delete[] node_value;
    delete[] node_alpha;
    // cout << "end EvaluateBDeu" << endl;
    return score;
}
double Evaluator::EvaluateBDeu(const string& var, const vector<int>& parents,double alpha) {
    vector<string> s_parents;
    for(const int& parent: parents) {
        s_parents.push_back(var_name_.at(parent));
    }
    return EvaluateBDeu(var, s_parents, alpha);
}

//double Evaluator::EvaluateBDeuModel(const int &row, const int &col, Searcher::Opts opt, double score) {
//}

double Evaluator::EvaluateBDeuModel() {
    //cout << "endter EvaluateModel" << endl;
    double score = 0.0;
    vector<string> paset;
    for(int i = 0; i < var_num_; ++i) {
        //cout << "start paset" << endl;
        paset.clear();
        for(int j = 0; j < var_num_; ++j) {
            if(true == graph_->GMember(i,j)) {
                paset.push_back(var_name_.at(j));
            }
        }
        score += EvaluateBDeu(var_name_.at(i),paset,1.0);
    }
    return score;
}

boost::numeric::ublas::matrix<double> Evaluator::EvaluateBDeuMapModel() {
    boost::numeric::ublas::matrix<double> score_map(1,26);
    vector<string> paset;
    for(int i = 0; i < var_num_; ++i) {
        paset.clear();
        for(int j = 0; j < var_num_; ++j) {
            if(true == graph_->GMember(i,j)) {
                paset.push_back(var_name_.at(j));
            }
        }
        score_map(0,i) = EvaluateBDeu(var_name_.at(i),paset,1.0);
    }
    return score_map;
}


void Evaluator::MakeStatistics(const string& var, const vector<string>& parents) {

    current_counts_.clear();

    vector<int> temp;
    for(int i = 1; i <= 3; ++i) {
        temp.push_back(i);
    }

    vector<vector<int>> temp_vec;
    for(uint i = 0; i < parents.size()+1 ; ++i) {
        temp_vec.push_back(temp);
    }

    vector<vector<int>> cart = CartProduct(temp_vec);


    const int kCat = cart.size();
    const int kPaSize = parents.size();

//    cout << "kCat: " << kCat << endl;
//    cout << "kPaSize: " << kPaSize << endl;

    for(int i = 0 ; i < kCat; ++i) {
        vector<int> vars = cart.at(i);
        //cout << "conbination " << endl;
        current_counts_[vars] = 0;
        for (int j = 0; j < kLine_; ++j) {
            if(all_data_[var][j] == vars.at(0)) {
                int k = 0;
                for(k = 0; k < kPaSize; ++k) {
                    if(all_data_[parents.at(k)][j] != vars.at(k+1))
                        break;
                }
                if( k == kPaSize)
                    current_counts_[vars]++;
            }
        }
    }
}






void Evaluator::EvaluateKL() {
    double** staticstics = new double*[var_num_]();
    for(int i = 0; i < var_num_ ; ++i) {
        staticstics[i] = new double[var_value_num_]();
    }
    for (int i = 0; i < var_num_; ++i) {
        for(int j = 0; j < var_value_num_; ++j) {
            staticstics[i][j] = data_statistics_[var_name_.at(i)][j]/(double)kLine_;
        }
    }

    elem elemenets[var_num_][var_num_];

    double temp_sum = 0.0;
    for (int i = 0; i < var_num_; ++i) {
        temp_sum = 0.0;
        for(int j = 0; j < var_num_; ++j) {
            if(i != j) {
                for(int k = 0; k < var_value_num_; ++k) {
                    temp_sum += staticstics[i][k]*log(staticstics[i][k]/staticstics[j][k]);
                }

                // D_{kl}(Pi|Qi)
                elemenets[i][j].first = temp_sum;
                elemenets[i][j].second = pair<int,int>(i,j);
                queue.push_back(elemenets[i][j]);
            }

        }
    }
    // sort queue menually
    std::sort(queue.begin(), queue.end(), CompareDist());

    for(int i = 0; i < var_num_ ; ++i) {
        delete[] staticstics[i];
    }
    delete[] staticstics;
}

double Evaluator::EvaluateQuery(const string& a,const int a_value, const string& b,const int b_value) {

    double sum = 0.0;
    for(int i = 0; i < kLine_;  ++i) {
        if(a_value == all_data_[a][i] && b_value == all_data_[b][i]) {
            sum+=1;
        }
    }
    return sum/kLine_;
}

void Evaluator::EvaluateWeightedKL() {
    // in case the queue has stored some thing
    queue.clear();
    double** sigal_prob = new double*[var_num_]();
    for(int i = 0; i < var_num_ ; ++i) {
        sigal_prob[i] = new double[var_value_num_]();
    }
    for (int i = 0; i < var_num_; ++i) {
        for(int j = 0; j < var_value_num_; ++j) {
            sigal_prob[i][j] = data_statistics_[var_name_.at(i)][j]/(double)kLine_;
        }
    }

    elem elemenets[var_num_][var_num_];

    double temp_sum = 0.0;
    double joint_prob = 0.0;
    for (int i = 0; i < var_num_; ++i) {
        for(int j = 0; j < var_num_; ++j) {
            temp_sum = 0.0;
            if(i != j) {
                for(int k = 0; k < var_value_num_; ++k) {
                    for(int m = 0; m < var_value_num_; ++m) {
                        joint_prob = EvaluateQuery(var_name_.at(i),k+1,var_name_.at(j),m+1);

                        if(0 == joint_prob) {
                            temp_sum += 0;
                        }
                        else {
                            temp_sum += joint_prob *log(joint_prob/sigal_prob[i][k]/sigal_prob[j][m]);
                        }
                        //                        if( 15 == i && 24 == j) {
                        //                            cout << "joint_prob " << joint_prob << endl;
                        //                            cout << "sigal_prob[i][k] " << sigal_prob[i][k] <<  endl;
                        //                            cout << "sigal_prob[j][m] " << sigal_prob[j][m] <<  endl;
                        //                            cout << "log: " << log(joint_prob/sigal_prob[i][k]/sigal_prob[j][m]) <<  endl;
                        //                            cout << var_name_.at(i) << "->" << var_name_.at(j) << ":" << temp_sum << endl;
                        //                        }
                    }
                }

                elemenets[i][j].first = temp_sum;
                elemenets[i][j].second = pair<int,int>(i,j);
                queue.push_back(elemenets[i][j]);
            }
        }
    }
    // sort queue menually
    std::sort(queue.begin(), queue.end(), CompareDist());

    for(int i = 0; i < var_num_ ; ++i) {
        delete[] sigal_prob[i];
    }
    delete[] sigal_prob;
}

void Evaluator::PrintOutKL() {
    uint total = var_num_* (var_num_ - 1);
    assert(total == queue.size());
    for(uint i = 0; i< total; ++i) {
        cout << "kl value: " << std::scientific <<  queue.at(i).first \
             << " i: " << var_name_.at(queue.at(i).second.first) \
             << " j: " << var_name_.at(queue.at(i).second.second) << endl;
    }
}

const std::vector<Evaluator::elem> & Evaluator::GetQueue() {
    return queue;
}
