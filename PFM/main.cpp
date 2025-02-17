/*---------------- File: main.cpp  ---------------------+
|Modelo PLI - Mochila 01                                |
|					      		                        |
|					      		                        |
| Implementado por Guilherme C. Pena em 28/08/2024      |
+-------------------------------------------------------+ */

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN // MACRO - "using namespace" for ILOCPEX

// CPLEX Parameters
#define CPLEX_TIME_LIM 3600 // 3600 segundos
	struct aresta
{
	int capacidade, origem, destino;
};

// Conjuntos do Problema
int numero_aresta, numero_vertice; // Número de nós e arestas
int S, T;
vector<aresta> arestas;				   // Oferta e demanda

void cplex()
{
	// CPLEX

	IloEnv env; // Define o ambiente do CPLEX

	// Variaveis ---------------------------------------------
	int i, j, k;	   // Auxiliares
	int numberVar = 0; // Total de Variaveis
	int numberRes = 0; // Total de Restricoes

	//---------- MODELAGEM ---------------

    IloNumVarArray x(env, numero_aresta, 0, IloInfinity, ILOINT);
    numberVar = numero_aresta;

	// Definicao do ambiente modelo ------------------------------------------
	IloModel model(env);

	IloExpr sum(env);
	IloExpr sum2(env);

	// FUNCAO OBJETIVO ---------------------------------------------
	sum.clear();
	for (i = 0; i < numero_aresta; i++)
	{
	   if(arestas[i].destino == T){

		sum += (x[i]);
		}
	}
	model.add(IloMaximize(env, sum)); //Maximizacao

	// RESTRICOES ---------------------------------------------
	// conservação de fluxo
	for (i = 0; i < numero_vertice; i++)//com exceção s e t
	{
	   if(i == S || i == T){
				continue;
		 }
		sum.clear(); // Somatório 1
		for (j = 0; j < numero_aresta; j++)
		{
			if (arestas[j].origem == i)
			{
				sum += x[j];
			}
		}
		sum2.clear(); // Somatório 2
		for (k = 0; k < numero_aresta; k++)
		{
			if (arestas[k].destino == i)
			{
				sum2 += x[k];
			}
		}
		model.add(sum == sum2);
		numberRes++;
	}
	// capacidade
	for (i = 0; i < numero_aresta; i++)
	{
		model.add(x[i] <= arestas[i].capacidade);
		numberRes++;
	}
	//------ EXECUCAO do MODELO ----------
	time_t timer, timer2;
	IloNum value, objValue;
	double runTime;
	string status;

	// Informacoes ---------------------------------------------
	printf("--------Informacoes da Execucao:----------\n\n");
	printf("#Var: %d\n", numberVar);
	printf("#Restricoes: %d\n", numberRes);
	cout << "Memory usage after variable creation:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

	IloCplex cplex(model);
	cout << "Memory usage after cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

	// Setting CPLEX Parameters
	cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

	time(&timer);
	cplex.solve(); // COMANDO DE EXECUCAO
	time(&timer2);

	// cout << "Solution Status: " << cplex.getStatus() << endl;
	// Results
	bool sol = true;
	switch (cplex.getStatus())
	{
	case IloAlgorithm::Optimal:
		status = "Optimal";
		break;
	case IloAlgorithm::Feasible:
		status = "Feasible";
		break;
	default:
		status = "No Solution";
		sol = false;
	}

	cout << endl
		 << endl;
	cout << "Status da FO: " << status << endl;

	if (sol)
	{

		objValue = cplex.getObjValue();
		runTime = difftime(timer2, timer);

		cout << "Variaveis de decisao: " << endl;
		for (i = 0; i < numero_aresta; i++)
		{
			value = cplex.getValue(x[i]);
			printf("x[%d]: %.0lf\n", i, value);
		}
		printf("\n");

		cout << "Funcao Objetivo Valor = " << objValue << endl;
		printf("..(%.6lf seconds).\n\n", runTime);
	}
	else
	{
		printf("No Solution!\n");
	}

	// Free Memory
	cplex.end();
	sum.end();
	sum2.end();

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main()
{
	// Leitura dos dados:
	cin >> numero_vertice >> numero_aresta >> S >> T; // Lê N e M

	arestas.resize(numero_aresta);

	for (int i = 0; i < numero_aresta; i++)
	{
		int origem, destino, capacidade;
		cin >> origem >> destino >> capacidade;
		arestas[i].capacidade = capacidade;
		arestas[i].origem = origem;
		arestas[i].destino = destino;
	}

	printf("Verificação da leitura dos dados:\n");
	printf("Num. vértices: %d\n", numero_vertice);
	printf("Num. arestas: %d\n", numero_aresta);
	printf("S: %d\n", S);
	printf("T: %d\n", T);

	printf("Matriz de Arestas (custo, capacidade):\n");
	for (int i = 0; i < numero_aresta; i++)
	{
		printf("origem: %d, destino: %d, capacidade: %d\n", arestas[i].origem, arestas[i].destino, arestas[i].capacidade);
	}
	cplex();

	return 0;
}
