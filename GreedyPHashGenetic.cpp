//
// Created by Administrator on 2019/6/26.
//

#include "GreedyPHashGenetic.h"
#include <random>
#include <iostream>
#include "string"
#include "Graph.h"
#include "fstream"
#include "vector"
#include "cstdlib"
#include "ctime"
#include "config.h"
using namespace std;

typedef default_random_engine Random;
typedef uniform_real_distribution<double> rDistribution;
typedef uniform_int_distribution<int> iDistribution;

GreedyPHashGenetic::GreedyPHashGenetic(Graph _graph,string _filename) {
    graph.copy(_graph);
    column = graph.column;
    row = graph.row;
    seed = SEED;
    cross_rate = CROSS_RATE;
    mutation_rate = MUTATE_RATE;
    filename = _filename;
    fitness_degree = new int[POP_NUM];
    time(&st);
    fitness_sum = 0;
    generation = 0;
    population = new bool*[POP_NUM];
    for (int k = 0; k < POP_NUM; ++k) {
        population[k] = new bool[row];
    }
    generateInitialize(0);
    for (int j = 0; j < HASH_SIZE; ++j) {
        timestamp[j] = -1;
    }
    for (int i = 0; i < POP_NUM; i++) {
        fitness_degree[i] = fitnessFun(population[i]);
        fitness_sum += fitness_degree[i];
    }

    out.open(filename);
    e.seed(seed);
    u_1 = rDistribution(0.0, 1.0);
    u_pop = iDistribution(0, POP_NUM-1);
    u_row = iDistribution(0, row-1);
}

void GreedyPHashGenetic::showBest(int *better, int iter) {
    int min = MAX_FIT;
    int pos = 0;


    for (int i = 0; i < POP_NUM; ++i) {
        if (fitness_degree[i] < min) {
            min = fitness_degree[i];
            pos = i;
        }
    }

    cout<< "best:" << min << endl;
    for (int j = 0; j < row ; ++j) {
        cout << population[pos][j] << " ";

    }
    cout << endl;

}

void GreedyPHashGenetic::minEstimate(int t,bool *child) {
    int fit ;
    int n = 2;
    fit = fitnessFun(child);
    int pfather = pow(fitness_degree[t], n),pchild=pow(fit, n);

    iDistribution us(0,pfather+pchild);
    if(us(e)>pchild){
        delete[] population[t];
        population[t] = child;
        fitness_degree[t]=fit;
    }
    else
        delete[] child;
}

void GreedyPHashGenetic::run() {
    time_t now;
    for (generation = 0; generation < MAX_GEN; ++generation) {
        for (int j = 0; j < POP_NUM; ++j) {
            if (u_1(e) < cross_rate)
                geneCrossover(j);
            if (u_1(e) < mutation_rate)
                geneMutation(j);
        }
        time(&now);
        //cout<< best_fit << endl;
        if(generation%100==0)
            cout << "generation:" << generation << "\ttime costs:" << (now - st)  << "s\tbest fit:" << best_fit << "\taccuracy num:" << accuracy_rate<<"\thash size:"<<hash_size << "\n";
        out << "generation:" << generation << "\ttime costs:" << (now - st)  << "s\tbest fit:" << best_fit << "\taccuracy num:" << accuracy_rate<<"\thash size:"<<hash_size << "\n";
        //showBest(nullptr, 0);
        if(generation % 5000==0){
            generateInitialize(best_fit);
        }
    }
    time(&now);
    cout<< "generation:" << generation << "\ttime costs:" << (now - st)  << "s\tbest fit:" << best_fit << "\taccuracy num:" << accuracy_rate<<"\thash size:"<<hash_size << "\n";
    return;
}

void GreedyPHashGenetic::geneCrossover(int t) {
    int yy = u_pop(e);
    bool *x = population[t], *y = population[yy];
    bool* child = new bool[row];
    int cut = u_row(e);
    if(u_1(e)<0.5){
        for (int i = 0; i < cut; i++)
            child[i] = x[i];
        for (int i = cut; i < row; i++)
            child[i] = y[i];
    }
    else{
        for (int i = 0; i < cut; i++)
            child[i] = y[i];
        for (int i = cut; i < row; i++)
            child[i] = x[i];
    }
    minEstimate(t, child);
}

void GreedyPHashGenetic::geneMutation(int t) {
    bool *child = new bool[row];
    int cut = u_row(e);
    for (int i = 0; i < row; ++i) {
        child[i] = population[t][i];
    }
    child[cut] = !child[cut];
    minEstimate(t, child);

}

int GreedyPHashGenetic::max(int x, int y) {
    return x>y?x:y;
}

int GreedyPHashGenetic::sumTrue(bool gen[]) {
    int sum=0;
    for (int i = 0; i < row; ++i) {
        sum+=gen[i]?1:0;
    }
    return sum;
}
int GreedyPHashGenetic::hashFun(bool gen[]){
    int64_t sum = 0;
    int64_t t = 0;
    iDistribution u(0,HASH_SIZE-1);

    Random te(0);

    for (int i = 0; i < row; ++i) {
        t<<1;
        if(gen[i])
            t+=1;
        if(i%32==31){
            te.seed(t+u(te));
            t=0;
        }
    }
    te.seed(t+u(te));
    return u(te);
}
int GreedyPHashGenetic::fitnessFun(bool gen[]) {
    int pos = hashFun(gen);
    if(timestamp[pos] != -1 and generation-timestamp[pos]<200){
        ++accuracy_rate;
        return hash[pos];
    }


    int fit = 0;
    int j;
    for (int i = 0; i < column; ++i) {
        for (j = 0; j < row; ++j)
            if(gen[j] && graph.map[i][j])
                break;
        if(j==row)
            fit+=10;
    }
    fit += sumTrue(gen);
    if(timestamp[pos]==-1)
        ++hash_size;
    timestamp[pos] = generation;
    hash[pos] = fit;
    if(fit<best_fit){
        best_fit = fit;
        best_gen = new bool[row];
        for (int i = 0; i < row; ++i) {
            best_gen[i] = gen[i];
        }
    }
    return fit;
}

int *GreedyPHashGenetic::bestDegree(int degree[]) {

}

bool **GreedyPHashGenetic::generateInitialize(int n) {
    if(!n){
        bernoulli_distribution u;
        for (int i = 0; i < POP_NUM; ++i)
            for (int j = 0; j < row; ++j)
                population[i][j] = u(e);
    }
    else{
        cout << "reset"<<endl;
//        time_t t;
//        time(&t);
        e.seed(generation);
//        double rate = (double)n/row;
//        for (int i = 0; i < POPULATION_NUM; ++i)
//            for (int j = 0; j < row; ++j)
//                population[i][j] = u_1(e)<rate;
        int p = 0;
        bool temp;
        for (int j = 0; j < POP_NUM; ++j) {
            for (int i = 0; i < n-1; ++i) {
                population[j][i] = 1;
            }
            for (int k = 0; k < row; ++k) {
                p = u_row(e);
                temp = population[j][p];
                population[j][p] = population[j][k];
                population[j][k] = temp;
            }
        }


        fitness_sum = 0;
        for (int i = 0; i < POP_NUM; i++) {
            fitness_degree[i] = fitnessFun(population[i]);
            fitness_sum += fitness_degree[i];
        }

    }
}