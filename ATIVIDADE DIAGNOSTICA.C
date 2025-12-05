#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SENHA_ADMIN "admin123"
#define ARQUIVO_DADOS "contas.dat"

struct Conta {
    int numero;
    char titular[50];
    float saldo;
};

int criarConta();
int consultarExtrato();
int realizarDeposito();
int realizarSaque();
int realizarTransferencia();
int listarContas();
int menu();

int main() {
    FILE *fp = fopen(ARQUIVO_DADOS, "ab");
    if (fp) fclose(fp);

    menu();
    return 0;
}

int menu() {
    int opcao;
    do {
        printf("\n=== SISTEMA BANCARIO ===\n");
        printf("1. Criar Conta\n");
        printf("2. Extrato\n");
        printf("3. Deposito\n");
        printf("4. Saque\n");
        printf("5. Transferencia\n");
        printf("6. Listar todas as contas (Admin)\n");
        printf("7. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        switch(opcao) {
            case 1: criarConta(); break;
            case 2: consultarExtrato(); break;
            case 3: realizarDeposito(); break;
            case 4: realizarSaque(); break;
            case 5: realizarTransferencia(); break;
            case 6: listarContas(); break;
            case 7: printf("Encerrando o sistema...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 7);
    return 0;
}

int criarConta() {
    struct Conta novaConta;
    FILE *fp = fopen(ARQUIVO_DADOS, "rb+");
    int tam;          
    int totalContas;
    
    if (fp == NULL) {
        printf("Erro ao abrir arquivo.\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    tam = (int)ftell(fp); 
    totalContas = tam / sizeof(struct Conta);
    novaConta.numero = totalContas + 1;

    printf("Digite o nome do titular: ");
    fgets(novaConta.titular, 50, stdin);
    novaConta.titular[strcspn(novaConta.titular, "\n")] = 0;

    novaConta.saldo = 0.0;

    fwrite(&novaConta, sizeof(struct Conta), 1, fp);
    printf("Conta criada com sucesso! Numero da conta: %d\n", novaConta.numero);
    
    fclose(fp);
    return 1;
}

int consultarExtrato() {
    int num, encontrado = 0;
    struct Conta c;
    FILE *fp = fopen(ARQUIVO_DADOS, "rb");

    if (fp == NULL) return 0;

    printf("Digite o numero da conta: ");
    scanf("%d", &num);

    while(fread(&c, sizeof(struct Conta), 1, fp)) {
        if(c.numero == num) {
            printf("\n--- EXTRATO ---\n");
            printf("Conta: %d\n", c.numero);
            printf("Titular: %s\n", c.titular);
            printf("Saldo: R$ %.2f\n", c.saldo);
            encontrado = 1;
            break;
        }
    }
    
    if(!encontrado) printf("Conta nao encontrada.\n");
    
    fclose(fp);
    return encontrado;
}

int realizarDeposito() {
    int num, encontrado = 0;
    float valor;
    struct Conta c;
    FILE *fp = fopen(ARQUIVO_DADOS, "rb+");

    if (fp == NULL) return 0;

    printf("Digite o numero da conta: ");
    scanf("%d", &num);

    while(fread(&c, sizeof(struct Conta), 1, fp)) {
        if(c.numero == num) {
            printf("Valor a depositar: ");
            scanf("%f", &valor);

            c.saldo += valor;

            
            fseek(fp, -((int)sizeof(struct Conta)), SEEK_CUR);
            fwrite(&c, sizeof(struct Conta), 1, fp);
            
            printf("Deposito realizado! Novo saldo: R$ %.2f\n", c.saldo);
            encontrado = 1;
            break;
        }
    }

    if(!encontrado) printf("Conta nao encontrada.\n");

    fclose(fp);
    return encontrado;
}

int realizarSaque() {
    int num, encontrado = 0;
    float valor;
    struct Conta c;
    FILE *fp = fopen(ARQUIVO_DADOS, "rb+");

    if (fp == NULL) return 0;

    printf("Digite o numero da conta: ");
    scanf("%d", &num);

    while(fread(&c, sizeof(struct Conta), 1, fp)) {
        if(c.numero == num) {
            printf("Valor a sacar: ");
            scanf("%f", &valor);

            if (c.saldo >= valor) {
                c.saldo -= valor;
                
                fseek(fp, -((int)sizeof(struct Conta)), SEEK_CUR);
                fwrite(&c, sizeof(struct Conta), 1, fp);

                printf("Saque realizado! Novo saldo: R$ %.2f\n", c.saldo);
            } else {
                printf("Saldo insuficiente.\n");
            }
            encontrado = 1;
            break;
        }
    }

    if(!encontrado) printf("Conta nao encontrada.\n");

    fclose(fp);
    return encontrado;
}

int realizarTransferencia() {
    int numOrigem, numDestino;
    float valor;
    struct Conta cOrigem, cDestino, temp;
    int posOrigem = -1, posDestino = -1; 
    FILE *fp = fopen(ARQUIVO_DADOS, "rb+");

    if (fp == NULL) return 0;

    printf("Conta de Origem: ");
    scanf("%d", &numOrigem);
    printf("Conta de Destino: ");
    scanf("%d", &numDestino);
    printf("Valor da transferencia: ");
    scanf("%f", &valor);

    rewind(fp);
    while(fread(&temp, sizeof(struct Conta), 1, fp)) {
        if(temp.numero == numOrigem) {
            cOrigem = temp;
            posOrigem = (int)ftell(fp) - sizeof(struct Conta); 
        }
        if(temp.numero == numDestino) {
            cDestino = temp;
            posDestino = (int)ftell(fp) - sizeof(struct Conta); 
        }
    }

    if (posOrigem != -1 && posDestino != -1) {
        if (cOrigem.saldo >= valor) {
            cOrigem.saldo -= valor;
            cDestino.saldo += valor;

            fseek(fp, posOrigem, SEEK_SET);
            fwrite(&cOrigem, sizeof(struct Conta), 1, fp);
            
            fseek(fp, posDestino, SEEK_SET);
            fwrite(&cDestino, sizeof(struct Conta), 1, fp);
            
            printf("Transferencia realizada com sucesso!\n");
        } else {
            printf("Saldo insuficiente na conta de origem.\n");
        }
    } else {
        printf("Uma ou ambas as contas nao foram encontradas.\n");
    }

    fclose(fp);
    return 1;
}

int listarContas() {
    char senha[20];
    struct Conta c;
    FILE *fp = fopen(ARQUIVO_DADOS, "rb");

    printf("Digite a senha de administrador: ");
    scanf("%s", senha);

    if (strcmp(senha, SENHA_ADMIN) == 0) {
        if (fp == NULL) {
            printf("Nenhuma conta cadastrada.\n");
            return 0;
        }

        printf("\n--- RELATORIO GERAL ---\n");
        while(fread(&c, sizeof(struct Conta), 1, fp)) {
            printf("Conta: %d | Titular: %-20s | Saldo: R$ %.2f\n", c.numero, c.titular, c.saldo);
        }
        fclose(fp);
    } else {
        printf("Senha incorreta!\n");
    }
    return 1;
}
