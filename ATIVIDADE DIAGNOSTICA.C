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

void criarConta();
void consultarExtrato();
void realizarDeposito();
void realizarSaque();
void realizarTransferencia();
void listarContas();
int buscarConta(int numero, struct Conta *c, long *posicao);
void menu();

int main() {
    FILE *fp = fopen(ARQUIVO_DADOS, "ab");
    if (fp) fclose(fp);

    menu();
    return 0;
}

void menu() {
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
            case 1: criarConta(); 
            break;
            case 2: consultarExtrato(); 
            break;
            case 3: realizarDeposito();
            break;
            case 4: realizarSaque(); 
            break;
            case 5: realizarTransferencia(); 
            break;
            case 6: listarContas(); 
            break;
            case 7: printf("Encerrando o sistema...\n"); 
            break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 7);
}

void criarConta() {
    struct Conta novaConta;
    FILE *fp = fopen(ARQUIVO_DADOS, "rb+");
    
    if (fp == NULL) 
    {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long tam = ftell(fp);
    int totalContas = tam / sizeof(struct Conta);
    novaConta.numero = totalContas + 1;

    printf("Digite o nome do titular: ");
    fgets(novaConta.titular, 50, stdin);
    novaConta.titular[strcspn(novaConta.titular, "\n")] = 0;

    novaConta.saldo = 0.0;

    fwrite(&novaConta, sizeof(struct Conta), 1, fp);
    printf("Conta criada com sucesso! Numero da conta: %d\n", novaConta.numero);
    
    fclose(fp);
}

int buscarConta(int numero, struct Conta *c, long *posicao) {
    FILE *fp = fopen(ARQUIVO_DADOS, "rb");
    if (fp == NULL) return 0;

    struct Conta temp;
    while(fread(&temp, sizeof(struct Conta), 1, fp)) {
        if(temp.numero == numero) {
            *c = temp;
            *posicao = ftell(fp) - sizeof(struct Conta);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void consultarExtrato() {
    int num;
    struct Conta c;
    long pos;

    printf("Digite o numero da conta: ");
    scanf("%d", &num);

    if (buscarConta(num, &c, &pos)) {
        printf("\n--- EXTRATO ---\n");
        printf("Conta: %d\n", c.numero);
        printf("Titular: %s\n", c.titular);
        printf("Saldo: R$ %.2f\n", c.saldo);
    } else {
        printf("Conta nao encontrada.\n");
    }
}

void realizarDeposito() {
    int num;
    float valor;
    struct Conta c;
    long pos;

    printf("Digite o numero da conta: ");
    scanf("%d", &num);

    if (buscarConta(num, &c, &pos)) 
    {
        printf("Valor a depositar: ");
        scanf("%f", &valor);

        c.saldo += valor;

        FILE *fp = fopen(ARQUIVO_DADOS, "rb+");
        fseek(fp, pos, SEEK_SET);
        fwrite(&c, sizeof(struct Conta), 1, fp);
        fclose(fp);
        
        printf("Deposito realizado! Novo saldo: R$ %.2f\n", c.saldo);
    } else 
    {
        printf("Conta nao encontrada.\n");
    }
}

void realizarSaque() {
    int num;
    float valor;
    struct Conta c;
    long pos;

    printf("Digite o numero da conta: ");
    scanf("%d", &num);

    if (buscarConta(num, &c, &pos)) 
    {
        printf("Valor a sacar: ");
        scanf("%f", &valor);

        if (c.saldo >= valor) {
            c.saldo -= valor;
            
            FILE *fp = fopen(ARQUIVO_DADOS, "rb+");
            fseek(fp, pos, SEEK_SET);
            fwrite(&c, sizeof(struct Conta), 1, fp);
            fclose(fp);

            printf("Saque realizado! Novo saldo: R$ %.2f\n", c.saldo);
        } else {
            printf("Saldo insuficiente.\n");
        }
    } 
    else 
    {
        printf("Conta nao encontrada.\n");
    }
}

void realizarTransferencia() {
    int numOrigem, numDestino;
    float valor;
    struct Conta cOrigem, cDestino;
    long posOrigem, posDestino;

    printf("Conta de Origem: ");
    scanf("%d", &numOrigem);
    printf("Conta de Destino: ");
    scanf("%d", &numDestino);
    printf("Valor da transferencia: ");
    scanf("%f", &valor);

    if (buscarConta(numOrigem, &cOrigem, &posOrigem) && buscarConta(numDestino, &cDestino, &posDestino)) {
        
        if (cOrigem.saldo >= valor) 
        {
            cOrigem.saldo -= valor;
            cDestino.saldo += valor;

            FILE *fp = fopen(ARQUIVO_DADOS, "rb+");
            
            fseek(fp, posOrigem, SEEK_SET);
            fwrite(&cOrigem, sizeof(struct Conta), 1, fp);
            
            fseek(fp, posDestino, SEEK_SET);
            fwrite(&cDestino, sizeof(struct Conta), 1, fp);
            
            fclose(fp);
            
            printf("Transferencia realizada com sucesso!\n");
        } else 
        {
            printf("Saldo insuficiente na conta de origem.\n");
        }
    } else 
    {
        printf("Uma ou ambas as contas nao foram encontradas.\n");
    }
}

void listarContas() {
    char senha[20];
    printf("Digite a senha de administrador: ");
    scanf("%s", senha);

    if (strcmp(senha, SENHA_ADMIN) == 0) 
    {
        FILE *fp = fopen(ARQUIVO_DADOS, "rb");
        struct Conta c;
        
        if (fp == NULL) {
            printf("Nenhuma conta cadastrada.\n");
            return;
        }

        printf("\n--- RELATORIO GERAL ---\n");
        while(fread(&c, sizeof(struct Conta), 1, fp)) {
            printf("Conta: %d | Titular: %-20s | Saldo: R$ %.2f\n", c.numero, c.titular, c.saldo);
        }
        fclose(fp);
    } else 
    {
        printf("Senha incorreta!\n");
    }
}