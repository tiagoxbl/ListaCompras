#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define TAM_MAX_NOME 50

// Estrutura para um item da lista de compras
typedef struct {
    char nome[TAM_MAX_NOME];
    int quantidade;
} ItemCompras;

// Protótipos das funções
int conectarBanco(sqlite3 **db);
void criarTabela(sqlite3 *db);
void adicionarItem(sqlite3 *db);
void exibirLista(sqlite3 *db);
void removerItem(sqlite3 *db);

int main() {
    sqlite3 *db;
    int opcao;

    if (conectarBanco(&db) != SQLITE_OK) {
        return 1;
    }

    criarTabela(db);

    while (1) {
        printf("\nMenu da Lista de Compras:\n");
        printf("1. Adicionar item\n");
        printf("2. Exibir lista\n");
        printf("3. Remover item\n");
        printf("4. Sair\n");
        printf("Escolha uma opção (1-4): ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarItem(db);
                break;
            case 2:
                exibirLista(db);
                break;
            case 3:
                removerItem(db);
                break;
            case 4:
                printf("Encerrando o programa...\n");
                sqlite3_close(db);
                return 0;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}

// Conecta ao banco de dados SQLite
int conectarBanco(sqlite3 **db) {
    int rc = sqlite3_open("lista_compras.db", db);
    if (rc != SQLITE_OK) {
        printf("Erro ao conectar ao banco de dados: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    return SQLITE_OK;
}

// Cria a tabela caso ela não exista
void criarTabela(sqlite3 *db) {
    char *erro;
    const char *sql = "CREATE TABLE IF NOT EXISTS Compras ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "nome TEXT NOT NULL, "
                      "quantidade INTEGER NOT NULL);";

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if (rc != SQLITE_OK) {
        printf("Erro ao criar a tabela: %s\n", erro);
        sqlite3_free(erro);
    }
}

// Adiciona um item à lista de compras
void adicionarItem(sqlite3 *db) {
    char nome[TAM_MAX_NOME];
    int quantidade;
    sqlite3_stmt *stmt;

    printf("Digite o nome do item: ");
    scanf(" %[^\n]%*c", nome);
    printf("Digite a quantidade: ");
    scanf("%d", &quantidade);

    const char *sql = "INSERT INTO Compras (nome, quantidade) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, quantidade);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("Item adicionado com sucesso!\n");
        } else {
            printf("Erro ao adicionar o item.\n");
        }

        sqlite3_finalize(stmt);
    } else {
        printf("Erro ao preparar a instrução SQL: %s\n", sqlite3_errmsg(db));
    }
}

// Exibe a lista de compras
void exibirLista(sqlite3 *db) {
    const char *sql = "SELECT id, nome, quantidade FROM Compras;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("Erro ao preparar a consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\nLista de Compras:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *nome = sqlite3_column_text(stmt, 1);
        int quantidade = sqlite3_column_int(stmt, 2);

        printf("%d. %s - Quantidade: %d\n", id, nome, quantidade);
    }

    sqlite3_finalize(stmt);
}

// Remove um item da lista com base no ID
void removerItem(sqlite3 *db) {
    int id;
    sqlite3_stmt *stmt;

    printf("Digite o ID do item que deseja remover: ");
    scanf("%d", &id);

    const char *sql = "DELETE FROM Compras WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("Item removido com sucesso!\n");
        } else {
            printf("Erro ao remover o item.\n");
        }

        sqlite3_finalize(stmt);
    } else {
        printf("Erro ao preparar o comando: %s\n", sqlite3_errmsg(db));
    }
}
