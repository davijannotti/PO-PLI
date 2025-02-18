#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

// Estrutura para representar uma aresta no grafo
struct Aresta {
    int origem, destino, capacidade, custo;
};

// Conjuntos do problema
int N, M; // Número de nós e arestas
vector<Aresta> arestas; // Lista de arestas
vector<int> saldo; // Vetor de oferta/demanda dos nós

void cplex() {
    IloEnv env;
    IloModel model(env);
    
    // Variáveis de decisão
    IloArray<IloIntVarArray> x(env, N);
    for (int i = 0; i < N; i++) {
        x[i] = IloIntVarArray(env, N, 0, IloIntMax); // Variáveis inteiras >= 0
    }

    // Função Objetivo: Minimizar custo do fluxo
    IloExpr objetivo(env);
    for (const auto& a : arestas) {
        objetivo += a.custo * x[a.origem][a.destino];
    }
    model.add(IloMinimize(env, objetivo));

    // Restrição de conservação de fluxo
    for (int i = 0; i < N; i++) {
        IloExpr fluxo(env);
        for (const auto& a : arestas) {
            if (a.origem == i) fluxo += x[a.origem][a.destino]; // Fluxo saindo
            if (a.destino == i) fluxo -= x[a.origem][a.destino]; // Fluxo entrando
        }
        model.add(fluxo <= saldo[i]);
    }

    // Restrição de capacidade das arestas
    for (const auto& a : arestas) {
        model.add(x[a.origem][a.destino] <= a.capacidade);
    }

    // Executa o modelo no CPLEX
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, 3600);
    if (cplex.solve()) {
        cout << "Solução ótima encontrada!\n";
        cout << "Custo mínimo: " << cplex.getObjValue() << endl;
        for (const auto& a : arestas) {
            cout << "Fluxo de " << a.origem << " para " << a.destino << ": "
                 << cplex.getValue(x[a.origem][a.destino]) << endl;
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
    saldo.resize(N, 0);
    arestas.resize(M);

    // Leitura dos balanços nos nós (oferta/demanda)
    for (int i = 0; i < N; i++) {
        cin >> saldo[i];
    }

    // Leitura das arestas (origem, destino, capacidade, custo)
    for (int i = 0; i < M; i++) {
        int origem, destino, capacidade, custo;
        if (!(cin >> origem >> destino >> capacidade >> custo) || 
            origem < 0 || origem >= N || destino < 0 || destino >= N) {
            cerr << "Erro na entrada de aresta!" << endl;
            return 1;
        }
        arestas[i] = {origem, destino, capacidade, custo};
    }

    cplex();
    return 0;
}
