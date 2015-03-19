/**
 * ------------------------------------------------------------------------
 *
 * Dada uma topologia e sua matriz de trafego, o programa deve gerar como
 * saída a capacidade necessária para cada ligação
 * 
 * Alunos: Silvana Trindade e Maurício André Cinelli
 * Ano: 2015
 * ------------------------------------------------------------------------
 */
#include <iostream>
#include <deque>
#include <iterator>
#include <algorithm> 
#include <limits> //limites numéricos
#include <stdexcept> // for std::runtime_error
#include <vector>
#include <random>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

#define LIMIT 1000

using namespace std;

typedef struct _e
{
	int target;					//destino
	int weight; 				//peso atualizado da aresta
	int capacity; 				//capacidade da aresta
	struct _e *next;			//ponteiro para o próximo nó adjacente
	unsigned int dirty; 		//bit correspodente a aresta ativa ou não
}Edge;

typedef struct
{
	Edge  *edges[LIMIT];	//matriz de arestas
	int degree[LIMIT];		//grau do nó
	int numberOfEdges;		//número de arestas
	int numberOfNodes;		//número de nós

}Graph;

int sumDemand; 

void help()
{

}

void InsertEdge(Graph *graph,int source, int target, bool directed)
{
	Edge *e;
	e = ( Edge* ) malloc ( sizeof(Edge) );

	if (e == NULL)
	{
		return;
	}

	e->weight = 0;//peso inicial das arestas é zero
	e->target = target;
	e->dirty = 1;
	e->next = graph->edges[source];

	graph->edges[source] = e; 
	graph->degree[source] += 1;
	
	if (directed == false)
	{
		InsertEdge(graph,target,source,true);
	}
	else
	{
		graph->numberOfEdges++;
	}
}

Graph *readGraph(Graph *graph, string _filename)
{

  ifstream file (_filename);
  string line;
  int nNodes = 0;

  if ( file.good() )
  {
    int lineCount = 0;

    while ( getline (file,line) )
    {

      if (lineCount == 0) 
      {

        stringstream s(line);
        s >> nNodes;
        cout<<"nNodes "<<nNodes<<endl;
        nNodes++;

        graph->numberOfNodes = nNodes;
        graph->numberOfEdges = 0;
        
        for (int i = 0; i < LIMIT; i++) 
        {
          graph->edges[i] = NULL;
          graph->degree[i] = 0;
        }

        lineCount++;

        continue;
      }

      stringstream ss(line); // Insert the string into a stream
      
      int source;
      int target;

      ss >> source >> target;

      cout << source << " " << target << endl;

      InsertEdge(graph,source, target, false);

      lineCount++;
    }

    file.close();
  } 
  else
  {
    printf("Could not open graph file.\n");
    exit(EXIT_FAILURE);
  }

  return graph;
}

vector< vector<int> > readTrafficMatrix(int nodes, string _filename)
{
	sumDemand = 0;

	vector< vector<int> > matrix = vector< vector<int> >(nodes, vector<int>(nodes, -1));

	ifstream file (_filename);
	string line;

	if (file.good())
	{
		while ( getline (file,line) )
		{
		  stringstream ss(line); // Insert the string into a stream
		  unsigned int source;
		  unsigned int target;
		  int traffic;

		  ss >> source >> target >> traffic;


		  matrix[source][target] = traffic;
		  matrix[target][source] = traffic;

		  sumDemand = sumDemand + traffic;
		}

		file.close();
	} 
	else
	{
		printf("Could not open traffic file.\n");
		exit(EXIT_FAILURE);
	}

	return matrix;
}

/**
 * Insere caminho mínimo ao inverso
 */
void insertShortestPath(vector<int> parent, vector<int> &path, int target)
{
	int u = target;

	path.push_back( target );

	while( parent[u] != -1 )
	{
		cout<<" "<<parent[u] <<endl;
		path.push_back( parent[u] );
		u = parent[u];
	}
}

/**
 * Implementação para encontrar um caminho mínimo de u até v
 */
vector<int> shortestPath( Graph *graph, int source, int target) 
{
	Edge *p; 					//vetor temporário
	vector<bool> inTree;		//O nó já esta na árvore?
	vector<int> capacities;		//armazena distância para source
	int v;						//nó atual
	int w;						//candidato a próximo nó
	int capacity;				//peso da aresta
	int cap;					//melhor distância atual para o nó de partida
	vector<int> path;			//armazena caminho mínimo
	vector<int> parent = vector<int> ( graph->numberOfNodes, -1);

	inTree = vector<bool> ( graph->numberOfNodes, false);
	capacities = vector<int> ( graph->numberOfNodes, std::numeric_limits<int>::max() );
	
	v = source;

	capacities[v] = 0;
	inTree[v] = true;
	cout<<"source "<<source<<" target "<<target<<endl;

	while( inTree[target] == false )
	{	
		p = graph->edges[v];//p recebe os nós adjacentes de v
		
		if (p == NULL)
		{
			cout<<"Topologia com nó "<<v<<" desconexo."<<endl;
			exit(EXIT_FAILURE);
		}

		while( p != NULL )
		{
			//aresta indisponível
			if ( p->dirty == 0 )
			{
				continue;
			}

			w = p->target; 
			capacity = p->weight;//recebece o valor do peso da aresta

			/**
			 * Verificação de caminho
			 */
			if ( capacities[w] > ( capacities[v] + capacity ) && inTree[w] == false )
			{
				capacities[w] = capacities[v] + capacity;
				parent[w] = v;

				cout<<" v "<<v<<" w "<<w<<" capacity "<<capacities[w]<<endl;
				
				//insere predecessor na matriz 		
			}

			if (target == w)	
			{
				break;
			}

			p = p->next;
		}

		v = 1;
		cap = std::numeric_limits<int>::max();

		for (int i = 1; i < graph->numberOfNodes; i++) 
		{
			if ( ( inTree[i] == false ) && (cap > capacities[i]) ) 
			{
				cap = capacities[i];
				v = i;
			}
		}
		
		inTree[v] = true;
	}

	cout<<"-----------------------------------\n"<<endl;

	// for (unsigned int i = 1; i < graph->numberOfNodes; i++)
	// {
	// 	for (unsigned int j = 1; j < graph->numberOfNodes; j++)
	// 	{
	// 		cout<<" "<<parent[i][j];
	// 	}
	// 	cout<<endl;
	// }
	insertShortestPath(parent,path,target);
	
	for (unsigned int j = 1; j < parent.size(); j++)
	{
		cout<<" "<<parent[j];
	}
	cout<<endl;
	for (unsigned int j = 0; j < path.size(); j++)
	{
		cout<<" "<<path[j];
	}

	cout<<"\n-----------------------------------\n"<<endl;
	return path;
}

/**
 * Atualiza peso das arestas adjacentes
 * recebendo o valor da demanda de u <-> v
 */
void updateWeight(Graph *graph, vector< vector<int> > trafficMatrix)
{
	/**
	 * Preenche com a demanda para nós adjacentes e
	 * em seguida atualizada a capacidade das arestas
	 */
	for (int i = 1; i < graph->numberOfNodes; i++)
	{
		Edge *p;

		p = graph->edges[i];

		if (graph->edges[i] == NULL)
		{
			exit(EXIT_FAILURE);
		}

		while(p != NULL)
		{
			int target = p->target;

			if ( trafficMatrix[i][target] >= 0 )
			{
				p->weight = trafficMatrix[i][target];
				p->capacity -= p->weight ;//diminiu a capacidade da ligação

				if (p->capacity == 0)
				{
					p->dirty = 0;//aresta passa a ser inativa
				}
			}

			p = p->next;
		}

	}
}

/**
 * Atualiza o peso e a capacidade das arestas pertencentes 
 * ao caminho mínimo passado como parâmetro ( path )
 */
void updateCapacityAndWeightByPath(Graph *graph,vector<vector<int>> traffic,vector<int> path)
{
	int prev = -1;

	Edge *p;

	int weight = traffic[ path[0] ][ path[ path.size() - 1 ] ];

	for (unsigned int j = 0; j < path.size(); j++) 
	{
		cout << "path[ "<<j<<" ] " << path[j] << " weight " << weight << endl;
		if (prev > 0) 
		{
			p = graph->edges[prev];

			if (p == NULL)
			{
				continue;
			}

			while (p != NULL && p->target != path[j]) 
			{
				p = p->next;
			}

			if (p) 
			{
				p->capacity -= weight;
				p->weight += weight;

				if (p->capacity == 0)
				{
					p->dirty = 0;
				}
			}

			p = graph->edges[ path[j] ];

			while (p != NULL && p->target != prev) 
			{
				p = p->next;
			}

			if (p) {
				p->capacity -= weight;
				p->weight += weight;

				if (p->capacity == 0)
				{
					p->dirty = 0;
				}
			}
		}

		prev = path[j];
	}

	for (int i = 1; i < graph->numberOfNodes; i++)
	{
		p = graph->edges[i];

		cout<<"node "<<i<<endl;

		while(p != NULL)
		{
			cout<<" "<<p->target<<"( "<<p->capacity<<" , "<<p->weight<<" )";
			p = p->next;
		} 
		cout<<endl;
	}
}

/**
 * Verifica nós adjacentes
 */
bool isAdjacent(Graph *graph,int source,int target)
{
	Edge *p;

	p = graph->edges[source];

	while(p != NULL	)
	{
		if (p->target == target)
		{
			return true;
		}

		p = p->next;
	}

	return false;
}

void execute(Graph *graph, vector< vector<int> > traffic) 
{

	/**
	 * Inicializa capacidade das ligações
	 */
	Edge *p;

	int maxCapacity = sumDemand*0.8;//capacidade máxima considerada é 70% da soma total das demandas


	for (int i = 1; i < graph->numberOfNodes; i++)
	{
		p = graph->edges[i];

		while(p != NULL)
		{
			p->capacity = maxCapacity;
			p->dirty = 1;
			p = p->next;
		}
	}

	cout<<"maxCapacity "<<maxCapacity<<endl;

	updateWeight(graph,traffic);//atualiza peso das arestas adjacentes no estado inicial

	for (int i = 1; i < graph->numberOfNodes; i++)
	{
		p = graph->edges[i];
		cout<<"node "<<i<<endl;
		while(p != NULL)
		{
			cout<<" "<<p->target<<"( "<<p->capacity<<" , "<<p->weight<<" )";
			p = p->next;
		} 
		cout<<endl;
	}
	/**
	 * Busca caminho mínimo para nós não adjacentes que requerem demanda
	 */
	for (unsigned int u = 1; u < traffic.size(); u++)
  	{
	  	for (unsigned int v = u+1; v < traffic[u].size(); v++)
	  	{
	  		if (traffic[u][v] == -1)
	  		{
	  			continue;
	  		}

	  		bool ehAdjacent = isAdjacent(graph,u,v);//verifica se u e v são adjacentes

	  		if (!ehAdjacent)
	  		{
		  		vector<int> path = shortestPath(graph,u,v);//busca caminho mínimo com maior capacidade disponível
	  			
	  			updateCapacityAndWeightByPath(graph,traffic,path);
	  		}
	  	}
  	}
}

void write(Graph *graph, string fileName) 
{
	vector<vector<int>> temp = vector<vector<int>> (graph->numberOfNodes,vector<int>(graph->numberOfNodes,0) );
	ofstream saida;

    saida.open (fileName);

	for (int i = 1; i < graph->numberOfNodes; i++)
	{
		Edge *p;

		p = graph->edges[i];

		while(p != NULL)
		{
			if (temp[i][p->target] == 0)
			{
				saida<<" "<<i<<" "<<p->target<<" - "<<p->weight<<endl;

				temp[i][p->target] = 1;
				temp[p->target][i] = 1;
			}
			p = p->next;
		}
	}

}

int main(int argc, const char * argv[])
{
  if(argc <= 2)
  {
    help();
    return EXIT_FAILURE;
  }

  string arquivoTrafego = argv[2];

  Graph *graph;

  graph = (Graph*) malloc ( sizeof(Graph) );

  if (graph == NULL)
  {
  	return EXIT_FAILURE;
  }

  readGraph(graph,string(argv[1]));
  vector< vector<int> > trafficMatrix = readTrafficMatrix(graph->numberOfNodes, string(argv[2]));

	
	Edge *p;  

  for (int i = 1; i < graph->numberOfNodes; i++) 
  {
    printf("Node %d: ", i);
   	
   	p = graph->edges[i];

   	while(p != NULL)
   	{
   		cout<<p->target<<"\t";
   		p = p->next;
   	}
    printf("\n---------------\n");
  }

  for (int i = 1; i < graph->numberOfNodes; i++) 
  {
    for (int j = 1; j < graph->numberOfNodes; j++) 
    {
      printf("%d ", trafficMatrix[i][j]);
    }
    printf("\n");
  }

  execute(graph,trafficMatrix);//roteamento

  string temp = "_output_file.txt";
  string fileName = argv[1]+temp;

  write(graph,fileName);

  return EXIT_SUCCESS;
}