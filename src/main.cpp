#include <iostream>
#include "kruskal.h"
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <algorithm>

typedef struct{
	int vertice;
	float valueAresta;
}tNode;

std::vector < std::vector < float > > * leitorInstancia(char* instanciaName){
	
	std::string linha;
	std::string valor;
	
	std::fstream* arquivo = new std::fstream(instanciaName, std::ios::in);



	std::vector < std::vector <float>  > * grafo = new std::vector < std::vector < float > >();

	while(std::getline(*arquivo, linha)){
		std::vector < float > arestaValores;
		std::stringstream dadosLinha(linha);
		arestaValores.clear();

		while(std::getline(dadosLinha, valor, ' ')){
			
			std::string::size_type sz;

			arestaValores.push_back(std::stod(valor, &sz));
		}

		grafo->push_back(arestaValores);

	}
	
	arquivo->close();
	return grafo;


}

/* Verifica se os pesos são todos nulos*/
bool not_violation(std::vector<float>* weight_restr){
	int sumWeight = 0;

	for(int i = 0; i < weight_restr->size(); i++){
		
		sumWeight += weight_restr->at(i);
	}
	if(sumWeight == 0){
		return 1;
	}
	return 0;
}

void print_grafo(std::vector < std::vector < float >>* grafo){
	int size = grafo->size();

	for(int i = 0; i < size; i++){

		std::vector < float > vertice = grafo->at(i);
		
		for(int i = 0; i < vertice.size(); i++){
				
			std::cout << vertice[i] << " ";
		}
		std::cout << std::endl;
	
	}
}

void print_matrizAdj(std::vector < std::vector < int >>* matrizAdj){
	int size = matrizAdj->size();

	for(int i = 0; i < size; i++){

		std::vector < int > vertice = matrizAdj->at(i);
		
		for(int i = 0; i < vertice.size(); i++){
				
			std::cout << vertice[i] << " ";
		}
		std::cout << std::endl;
	
	}
}

void calculate_graus(std::vector < std::pair <int,int>>* graus, std::vector < std::vector < int>>* matrizAdj){

	int qVertices = matrizAdj->size();
	for(int i = 0; i < qVertices; i++){
		int g = 0;
		for(int j = 0; j < qVertices; j++){
			
			if((*matrizAdj)[i][j])
				g+=1;
		}
		graus->push_back(std::make_pair(i, g));
	}
}
int main(int argc, char** argv){
	
	if(argc < 2){

		std::cout << "Digite ./main -nome instancia-\n";
		exit(1);
	}

	std::vector < std::vector < float > >* grafo = leitorInstancia(argv[1]);
	
	
	
	int qVertices = grafo->size();
	
	float upper_bound = 148; // Iniciando com um valor aleatorio para teste
	std::vector < float > weight_restr(qVertices, 0); // Inicializando o vetor de peso das restrições para cada vértice como nulo
	std::vector < float > subgradiente(qVertices, 0); // Inicializando o vetor de peso das restrições para cada vértice como nulo
	std::vector < float > weight_temp(qVertices, 0);
	float epslon = 1;
	float epslon_min = 0.0005;
	float w_ot = 0;
	int k_max = 30;
	int k = 0;
	float passos = 1.0;
	

	/* Como estamos resolvendo um problema relaxado
	Queremos encontrar o melhor lower bound possivel,
	pois como é um problema relaxado não é possivel ele ser melhor 
	do que uma soluçao do problema original. Desse modo, para o TSP
	é necessário achar uma árvore geradora mínima que não inclua o nó zero
	Se acharmos uma solução em que os vértices não violem a restrição relaxada,
	temos então a solução ótima para o problema relaxado como também para o problema original.
	Os passos do algoritmo são o seguinte, resolvemos o problema relaxado sem o nó zero
	achado sua solução ótima (w), alteramos o valor dos pesos baseado no mi e por fim recalculamos o mi
	se foi achado um LB melhor, altera o LB, aplica na matriz original a redução a partir do subgradiente
	e reseta o valor de k*/
	do{
		/* Primeiro temos que alterar a matriz de peso de acordo com o vetor de pesos */
		/* Inicialmente vamos alterar a matriz toda, depois otimizados para utilizar apenas o triangulo superior
			No primeiro loop esse vetor é nulo então o grafo de peso é o mesmo*/
		for(int i = 0; i < qVertices; i++){
			
			for(int j = 0; j < qVertices; j++){
				if(i == j)
					continue;
				(grafo->at(i))[j] += - weight_restr[i] - weight_restr[j];
			}	
		}
		
		weight_temp = weight_restr;
		std::cout << "Grafo Atualizado" << std::endl;
		/* Printando nova matriz para debug */
		print_grafo(grafo);
		
		/* Inicializando o algoritmo de kruskal */
		Tree* tree = new Tree(qVertices);
		Kruskal kruskal(tree, grafo);
		kruskal.algorithm();
		float w = kruskal.result;
		std::cout << "Valor do kruskal: " << w << std::endl;
		std::vector < std::vector < int > >* matrizAdj = kruskal.getMatrizAdj();
		/*Fim do algoritmo de kruskal */
		std::cout << "Matriz de adjacência antes da inserção do nó zero" << std::endl;
		print_matrizAdj(matrizAdj);
		/* Para definir o lower bound devemos adicionar o nó zero de forma gulosa */
	
		/* Nós ordenados de acordo com sua distancia em relação ao nó zero*/
		std::vector < std::pair<int, float> > nodesSortedByEdge0;
		

		/* Adiciona todos os vértices e suas respectivas distancias ao nó zero	*/
		for(int j = 1; j < qVertices; j++){
			nodesSortedByEdge0.push_back(std::make_pair(j, grafo->at(0)[j]));
		}

		/* A função indica se a deve ser ordenado antes de b ou não,
		se for true, a será ordenado antes de b, caso não será b
		então se a for menor ou igual a b, a será ordenado antes de b
		Logo estamos organizando em ordem descrecente, para podermos pegar os vértices em o(1)*/
		sort(nodesSortedByEdge0.begin(), nodesSortedByEdge0.end(), [](const std::pair<int,float>&a, std::pair<int,float>const &b)
		{
				return a.second >= b.second;
		});

		int vertice_a = nodesSortedByEdge0.back().first;
		nodesSortedByEdge0.pop_back();
		int vertice_b = nodesSortedByEdge0.back().first;
		
		/* Ativando as arestas */
		(*matrizAdj)[0][vertice_a] = (*matrizAdj)[0][vertice_b] = 1;
		(*matrizAdj)[vertice_a][0] = (*matrizAdj)[vertice_b][0] = 1;
		std::cout << "Matriz de adjacência" << std::endl;
		print_matrizAdj(matrizAdj);
		std::cout << "-------------------------------------" << std::endl;
		/* Calculo dos graus */
		std::vector < std::pair < int, int >>graus;
		w += grafo->at(0)[vertice_a] + grafo->at(0)[vertice_b];
		std::cout << "Valor da FO após a inserção do nó zero: " << w << std::endl;
		calculate_graus(&graus, matrizAdj);

		std::cout << "Os melhores vértices com os pesos respectivamente são: " << vertice_a << " " << vertice_b << " " << grafo->at(0)[vertice_a] << " " << grafo->at(0)[vertice_b] << std::endl;
			
		std::cout << "Graus de cada vertice: ";
		for(int i = 0; i < qVertices; i++){
			std::cout << graus.at(i).second << " ";
		}
		std::cout << std::endl;

		/* Calculando o vetor subsubgradiente*/
		std::cout << "Vetor subgradiente: ";
		for(int i = 0; i < qVertices; i++){
			subgradiente[i] = 2 - graus.at(i).second;
			std::cout << subgradiente[i] << " ";
		}
		std::cout << std::endl;
		
		/* Calculando o produto interno bruto do subgradiente*/
		float PI = 0;
		for(int i = 1; i < qVertices; i++){
			PI += (subgradiente[i] * subgradiente[i]);
		}
		std::cout << "Produto interno bruto: " << PI << std::endl;
		std::cout << std::endl;
		passos = ((epslon * (upper_bound - w))) / PI;
		std::cout << "Novo valor de passo: " << passos << std::endl;


		/* Altera o vetor de pesos */
		std::cout << "Novo vetor de pesos: ";
		for(int i = 0; i < qVertices; i++){
			weight_temp[i] += (passos * subgradiente[i]);
			std::cout << weight_temp[i] << " ";
		}
		std::cout << std::endl;
		weight_restr = weight_temp;

		if(w > w_ot){	
			w_ot = w;
			std::cout << "Novo valor do lower bound: " << w_ot << std::endl;
			//getchar();
			k = 0;

		}else{
			k += 1;
			if (k > k_max){
				k = 0;
				epslon /= 2;
				//getchar();
			}
			
		}
		
		getchar();
	}while(epslon > epslon_min or not_violation(&weight_restr));
	std::cout << epslon_min << std::endl;
	std::cout << not_violation(&weight_restr) << std::endl;
	std::cout << w_ot << std::endl;
	return 0;
}
