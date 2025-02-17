#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

// Estrutura para representar uma aresta no grafo
struct Aresta {
    int origem, destino, custo;
};

// Conjuntos do problema
int N, M; // Número de vértices (N) e arestas (M)
vector<int> oferta;  // Vetor de oferta por vértice
vector<int> demanda; // Vetor de demanda por vértice
vector<Aresta> arestas; // Lista de arestas

void cplex() {
    IloEnv env;
    IloModel model(env);
    
    // Variáveis de decisão (x[i][j] ≥ 0, inteiras)
    IloArray<IloIntVarArray> x(env, M);
    for (int i = 0; i < M; i++) {
        x[i] = IloIntVarArray(env, M, 0, IloIntMax);
    }

    // Função Objetivo: Minimizar custo total do fluxo
    IloExpr objetivo(env);
    for (const auto& a : arestas) {
        objetivo += a.custo * x[a.origem][a.destino];
    }
    model.add(IloMinimize(env, objetivo));

    // Restrição de atender demanda nos nós de destino
    for (int j = 0; j < M; j++) { 
        IloExpr fluxo(env);
        for (const auto& a : arestas) {
            if (a.destino == j) fluxo += x[a.origem][a.destino];
        }
        model.add(fluxo == demanda[j]); 
    }

    // Restrição de respeitar a oferta nos nós de origem
    for (int i = 0; i < N; i++) { 
        IloExpr fluxo(env);
        for (const auto& a : arestas) {
            if (a.origem == i) fluxo += x[a.origem][a.destino];
        }
        model.add(fluxo <= oferta[i]); 
    }

    // Executa o modelo no CPLEX
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, 3600);

    if (cplex.solve()) {
        cout << "=============================\n";
        cout << " Status da Solução: ";
        switch (cplex.getStatus()) {
            case IloAlgorithm::Optimal:
                cout << "Ótima\n";
                break;
            case IloAlgorithm::Feasible:
                cout << "Factível\n";
                break;
            default:
                cout << "Sem solucao!\n";
                break;
        }
        cout << "=============================\n";

        // Se solução for encontrada, imprime os detalhes
        if (cplex.getStatus() == IloAlgorithm::Optimal || cplex.getStatus() == IloAlgorithm::Feasible) {
            cout << "Custo mínimo: " << cplex.getObjValue() << endl;
            cout << "Fluxos ótimos por aresta:\n";

            for (const auto& a : arestas) {
                double fluxo = cplex.getValue(x[a.origem][a.destino]);
                cout << "Fluxo de " << a.origem << " → " << a.destino << " = " << fluxo << "\n";
            }
        }
    } else {
        cout << "Nenhuma solução encontrada.\n";
    }

    // Liberação de memória
    cplex.end();
    objetivo.end();
    env.end();
}

int main() {
    cin >> N >> M;
    
    // Redimensionando vetores
    oferta.resize(N);
    demanda.resize(M);
    arestas.clear(); 

    // Leitura da oferta por vértice
    for (int i = 0; i < N; i++) {
        cin >> oferta[i];
    }

    // Leitura da demanda por vértice
    for (int j = 0; j < M; j++) {
        cin >> demanda[j];
    }

    // Leitura da matriz de custos e preenchimento das arestas
    for (int i = 0; i < N; i++) { 
        for (int j = 0; j < M; j++) { 
            int custo;
            cin >> custo;
            if (custo >= 0) { 
                arestas.push_back({i, j, custo});
            }
        }
    }

    cplex();
    return 0;
}
