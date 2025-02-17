#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600
	struct aresta
{
	int custo, origem, destino;
};

int N, M;
vector<aresta> arestas;

void cplex()
{
	IloEnv env;

	// Variaveis ---------------------------------------------
	int i, j, k;	   // Auxiliares
	int numberVar = 0; // Total de Variaveis
	int numberRes = 0; // Total de Restricoes

	//---------- MODELAGEM ---------------
	IloNumVarArray x(env);
	for (i = 0; i < N; i++)
	{
		x.add(IloIntVar(env, 0, 1));
		numberVar++;
	}


	// Definicao do ambiente modelo ------------------------------------------
	IloModel model(env);
	IloExpr sum(env);

	// FUNCAO OBJETIVO ---------------------------------------------
	sum.clear();
	for (i = 0; i < N; i++)
	{
		sum += (arestas[i].custo * x[i]);
	}
	model.add(IloMinimize(env, sum)); // Minimizacao

	// RESTRICOES ---------------------------------------------
	// tarefa designada
	for (i = 0; i < M; i++)
	{
	   sum.clear();
	   for (j = 0; j < N; j++) {
			if(arestas[j].origem == i)
	           sum += x[j];
		}
       model.add(sum == 1);
       numberRes++;
	}

	//pessoa designada
	for (i = 0; i < M; i++)
	{
	   sum.clear();
	   for (j = 0; j < N; j++) {
			if(arestas[j].destino == i)
	           sum += x[j];
		}
       model.add(sum == 1);
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
	// cplex.setParam(IloCplex::TreLim, CPLEX_COMPRESSED_TREE_MEM_LIM);
	// cplex.setParam(IloCplex::WorkMem, CPLEX_WORK_MEM_LIM);
	// cplex.setParam(IloCplex::VarSel, CPLEX_VARSEL_MODE);

	time(&timer);
	cplex.solve(); // COMANDO DE EXECUCAO
	time(&timer2);

	// cout << "Solution Status: " << cplex.getStatus() << endl;
	// Results
	bool sol = true;
	/*
	Possible Status:
	- Unknown
	- Feasible
	- Optimal
	- Infeasible
	- Unbounded
	- InfeasibleOrUnbounded
	- Error
	*/
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

		// Results
		// int Nbin, Nint, Ncols, Nrows, Nnodes, Nnodes64;
		objValue = cplex.getObjValue();
		runTime = difftime(timer2, timer);
		// Informacoes Adicionais
		// Nbin = cplex.getNbinVars();
		// Nint = cplex.getNintVars();
		// Ncols = cplex.getNcols();
		// Nrows = cplex.getNrows();
		// Nnodes = cplex.getNnodes();
		// Nnodes64 = cplex.getNnodes64();
		// float gap; gap = cplex.getMIPRelativeGap();

		cout << "Variaveis de decisao: " << endl;
		for (i = 0; i < N; i++)
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

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main()
{
	cin >> N >> M; // Lê N e M
	arestas.resize(N);

	for (int i = 0; i < N; i++)
	{
		int origem, destino, custo;
		cin >> origem >> destino >> custo;

		arestas[i].custo = custo;
		arestas[i].origem = origem;
		arestas[i].destino = destino;
	}

	printf("Verificação da leitura dos dados:\n");
	printf("N: %d  M: %d\n", N, M);


	printf("Arestas (custo):\n");
	for (int i = 0; i < N; i++)
	{
		printf("origem: %d, destino: %d, custo: %d\n", arestas[i].origem, arestas[i].destino, arestas[i].custo);
	}

	// Chama a função CPLEX para resolver o modelo
	cplex();

	return 0;
}
