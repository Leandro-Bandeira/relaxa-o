#include "Bnb.h"
#include <cstdio>
#include <time.h>
#include <iostream>
#include <algorithm>


int Bnb::idGeneral = 0;
int Bnb::t = 0;


Bnb::Bnb(){
}

/* Retorna o indice do nó dado pela estratégia de branching
 * BFS = 0 (sempre será escolhido o primeiro nó)
 * DFS = 1 (sempre será escolhido o ultimo nó)
 */
int Bnb::branchStrategy(){
  int size = 2;
  std::srand(time(0));

  return (std::rand() % size > 0 ? this->tree.size() - 1: 0); 
  
}


/*void Bnb::initDfs(){
  this->visitedVertices.clear();
  this->visitedVertices.resize(listAdj.size(), 0);
  this->PE.clear();
  this->PE.resize(listAdj.size(), 0);
  this->v_fathers.clear();
  this->v_fathers.resize(listAdj.size(), 0);
  this->PS.clear();
  this->PS.resize(listAdj.size(), 0);
  Bnb::t = 0;
}
void Bnb::dfs(int v, NodeInfoBnb* node){
  
  Bnb::t += 1;
  visitedVertices[v]++;
  PE[v] = Bnb::t;
  std::vector < int > neighbors = listAdj[v];
  int sizeNeighbors = neighbors.size();
  int foundCicle = 0;
  for(int i = 0; i < sizeNeighbors; i++){
    int w = neighbors[i];
    /* Se a gente nunca visitou w é porque o pai dele é v 
     * caso já tenhamos visitado W, vamos verificar se já foi visitado todos os vértices uma unica vez
     * e w é a raiz, caso sim, temos um ciclo hamiltoniano 
    if(PE[w] == 0){
      v_fathers[w] = v;
      dfs(w, node);     
    }
    if(v_fathers[v] != w and  w == 0 and PE[w] != 0){

      bool allVerticesVisitedOnce = false;
      int count = 0;
      for(int i = 1; i < visitedVertices.size(); i++){
        if(visitedVertices[i] == 1)
          count++;
      }
      if (count == visitedVertices.size() - 1){
        allVerticesVisitedOnce = true;
      }
      if(allVerticesVisitedOnce){
        node->feasible = true;
      }else{
        node->feasible = false;
      }
      
    }
  }
  Bnb::t += 1;
  PS[v] = Bnb::t;
}

*/ 

bool viabilityChecker(std::vector<std::vector<int>>* matrixAdj){
  
  bool feasible = true;
  for(int i = 0; i < matrixAdj->size(); i++){
    int counter = 0;
    for(int j = 0; j < matrixAdj->at(i).size(); j++){
      if(matrixAdj->at(i)[j] == 1)
        counter++;
      if(counter > 2){
        feasible = false;
      }
        
    }
    if(!feasible)
      break;
  }
  return feasible;
  
}
void Bnb::fullNode(NodeInfoBnb* node){
  Lagrange *lagrangeSolution = node->lagrangeSolution;
  node->lower_bound = lagrangeSolution->getBestLowerBound();
  std::vector < std::vector <int >> *matrixAdj = lagrangeSolution->getLagrangeMatrix();
  std::vector < int > rates(matrixAdj->size(), 0);
  
  std::vector < int > neighboors;
  this->listAdj.clear();
  for(int i = 0; i < matrixAdj->size(); i++){
    for(int j = 0; j < matrixAdj->size(); j++){
      if(matrixAdj->at(i)[j])
        neighboors.push_back(j);
    }
    listAdj.push_back(neighboors);
    neighboors.clear();
  } 
  for(int i = 0; i < matrixAdj->size(); i++){
    for(int j = i + 1; j < matrixAdj->at(i).size(); j++){
      if(matrixAdj->at(i)[j] == 1){
        rates[i] += 1;
        rates[j] += 1;
      }
    }
  }
  
  //std::cout << "tamanho da lista de adjacencia: " << listAdj.size() << "\n";
 // std::cout << "lista de adjacencia:\n";
  /*for(int i = 0; i < listAdj.size(); i++){
    for(int j = 0; j < listAdj[i].size(); j++){
      std::cout << listAdj[i][j] << " ";
    }
    
    std::cout << "\n";
  }
  */
  //for(int i = 0; i < matrixAdj->size(); i++){
   // for(int j = 0; j < matrixAdj->size(); j++){
    //  std::cout << matrixAdj->at(i)[j] << " ";
   // }
    //std::cout << "\n";
 // }
  //getchar();
  /* Vamos agora aplicar a busca em profundidade para procurar ciclos, achado o primeiro ciclo a solucao nao eh viavel */ 

  //initDfs();
  //node->feasible = false;
  //dfs(0, node);
  node->feasible = viabilityChecker(matrixAdj);
  //std::cout << "Grau dos vertices da solução lagrangiana: ";
  //for(int i = 0; i < rates.size(); i++){
   // std::cout << rates[i] << " ";
  //}
  //std::cout << std::endl; 
  int indexNodeMoreRate = std::distance(rates.begin(), std::max_element(rates.begin(), rates.end()));
  //std::cout << "indice do vértice de maior grau: " << indexNodeMoreRate << "\n";
  node->nodeChosen = indexNodeMoreRate;
  node->id = Bnb::idGeneral;
  Bnb::idGeneral+=1;
}

void Bnb::algorithm(Lagrange* lagrangeSolutionInit, double upper_bound_lagrange){
  NodeInfoBnb root; /* Nó raiz = será dado pela resolução da relaxacao */
  root.lagrangeSolution = lagrangeSolutionInit;
  fullNode(&root);
  this->tree.push_back(&root);
  int initBnb = 0;
  double upper_bound = root.lower_bound;
  /*Bnb algorithm */
  while(!this->tree.empty()){
    int indexNode = this->tree.size() - 1;
    /* Retorna o nó dependendo do método utilizado */
    NodeInfoBnb* node = (indexNode == 0 ? this->tree.front() : this->tree.back());
    if (initBnb != 0){
      fullNode(node);
    }
    //std::cout << "id do nó escolhido: " << node->id << "\n";
    /* Avança o iterator para a posição do nó escolhido para depois apagá-lo */ 
    std::list < NodeInfoBnb*>::iterator init = this->tree.begin();
    advance(init, indexNode);
    //std::cout << "Id do nó apos avanço do iterator: " << (*init)->id << "\n";
    //std::cout << "tamanho da arvore: " << tree.size() << "\n";
    if(node->lower_bound > upper_bound_lagrange){
      this->tree.erase(init);
      //getchar();
      continue;
    }
    

    if(node->feasible){
      upper_bound = std::max(upper_bound, node->lower_bound);
      std::cout << "best Upper_bound found: " << upper_bound << "\n";
      getchar();
    }
    /* Para criar os filhos, as seguintes etapas são necessarias 
     * 1 - Armazenar a matriz de custos do pai
     * 2 - Saber quem são os arcos proibidos
     * 3 - Modificar a matriz de custos original com base nos arcos proibidos 
     * 4 - Calcular a solução de lagrange para o nó */
    else{
      std::vector<std::vector<double>> fatherCosts = *(node->lagrangeSolution->getLagrangeCosts());
      std::vector < int > neighborsForbidden = listAdj[node->nodeChosen];

      for(int i = 0 ; i < neighborsForbidden.size(); i++){
        NodeInfoBnb* n = new NodeInfoBnb;
        int nodeForbidden = neighborsForbidden[i];
        std::vector <std::vector <double>> currentCosts = fatherCosts;
        currentCosts[node->nodeChosen][nodeForbidden] = currentCosts[nodeForbidden][node->nodeChosen] = 99999999;
        Lagrange* currentLagrange = new Lagrange(&currentCosts);
        double valueChield = currentLagrange->algorithm(upper_bound_lagrange);
        n->lagrangeSolution = currentLagrange;
        //std::cout << "Chield lower_bound: " << valueChield << std::endl;
        tree.push_back(n);

        //std::cout << "arco probido: " << node->nodeChosen << " " << nodeForbidden << "\n";

      }
    }
    this->tree.erase(init);
    initBnb += 1;     
    
  }
  std::cout << "upper_bound: " << upper_bound << std::endl;
}