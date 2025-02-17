#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

// Estrutura para representar uma aresta no grafo
struct Aresta {
    int origem, destino, custo;
};

int N, M, s, d; // Número de nós, arestas, nó de origem e destino
vector<Aresta> arestas; // Lista de arestas

void cplex() {
    IloEnv env;
    IloModel model(env);

    // Variáveis de decisão: x[i][j] = 1 se a aresta (i, j) for usada, 0 caso contrário
    IloArray<IloBoolVarArray> x(env, N);
    for (int i = 0; i < N; i++) {
        x[i] = IloBoolVarArray(env, N);
        for (int j = 0; j < N; j++) {
            x[i][j] = IloBoolVar(env);
        }
    }

    // Função Objetivo: Minimizar custo total do caminho
    IloExpr objetivo(env);
    for (const auto& a : arestas) {
        objetivo += a.custo * x[a.origem][a.destino];
    }
    model.add(IloMinimize(env, objetivo));

    // Restrição (2): O nó de origem deve ter exatamente uma aresta saindo
    IloExpr saidaOrigem(env);
    for (const auto& a : arestas) {
        if (a.origem == s) {
            saidaOrigem += x[a.origem][a.destino];
        }
    }
    model.add(saidaOrigem == 1);

    // Restrição (3): O nó de destino deve ter exatamente uma aresta chegando
    IloExpr chegadaDestino(env);
    for (const auto& a : arestas) {
        if (a.destino == d) {
            chegadaDestino += x[a.origem][a.destino];
        }
    }
    model.add(chegadaDestino == 1);

    // Restrição (4): Conservação de fluxo para nós intermediários (exceto s e d)
    for (int k = 0; k < N; k++) {
        if (k == s || k == d) continue;
        IloExpr fluxoEntrada(env), fluxoSaida(env);
        for (const auto& a : arestas) {
            if (a.destino == k) fluxoEntrada += x[a.origem][a.destino];
            if (a.origem == k) fluxoSaida += x[a.origem][a.destino];
        }
        model.add(fluxoEntrada == fluxoSaida);
    }

    // Restrição (5): Variáveis binárias
    for (const auto& a : arestas) {
        model.add(x[a.origem][a.destino] >= 0);
        model.add(x[a.origem][a.destino] <= 1);
    }

    // Executa o modelo no CPLEX
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, 3600);
    
    if (cplex.solve()) {
        cout << "Solução ótima encontrada!\n";
        cout << "Custo mínimo: " << cplex.getObjValue() << endl;
        cout << "Caminho encontrado:\n";
        for (const auto& a : arestas) {
            if (cplex.getValue(x[a.origem][a.destino]) > 0.5) {
                cout << a.origem << " -> " << a.destino << " (custo: " << a.custo << ")\n";
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
    cin >> N >> M >> s >> d;
    arestas.reserve(2 * M); // arestas bidirecionais

    for (int i = 0; i < M; i++) {
        int origem, destino, custo;
        if (!(cin >> origem >> destino >> custo) || 
            origem < 0 || origem >= N || destino < 0 || destino >= N) {
            cerr << "Erro na entrada de aresta!" << endl;
            return 1;
        }
        arestas.push_back({origem, destino, custo});
        arestas.push_back({destino, origem, custo}); // Adicionando a aresta reversa
    }

    cplex();
    return 0;
}