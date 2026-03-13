#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "dhcpserver/dhcpserver.h"
#include "dnsserver/dnsserver.h"
#include "lwip/ip4_addr.h"
#include "http.h"
#include "hash/sha-256.h"
#include "controle_fechadura.h"
#include "controle_DS1307.h"

#include "pico_hal.h"
#include "stdinit.h"

// pinos do módulo DS1307
#define PORTA_I2C       i2c0
#define PINO_SDA        8
#define PINO_SCL        9
#define ENDERECO_DS1307 0x68

// pino do relé
#define RELE 18

dns_server_t dns_server;
 
// arquivos de cabeçalho para a lógica do alarme e do servidor http
#include "http.h"

// struct para configurações de rede
typedef struct {
    ip_addr_t gw;
    ip_addr_t mask;
} NET_CONFIG_T;

// Variáveis globais de rede
NET_CONFIG_T net_config;
dhcp_server_t dhcp_server;

// identificação SSID e senha do ponto de acesso
#define WIFI_SSID "fechadura_wifi"
#define WIFI_PASS "fechadura123"

void criar_arquivo_usuarios_inicial() {
    printf("Montando o sistema de arquivos...\n");
    int err = pico_mount(false);
    printf("pico_mount(false) retornou: %d\n", err);

    if (err != LFS_ERR_OK) {
        printf("Sistema de arquivos não montado, formatando...\n");
        err = pico_mount(true); 
        printf("pico_mount(true) retornou: %d\n", err);
        if (err != LFS_ERR_OK) {
            printf("Erro fatal ao montar sistema de arquivos após formatação.\n");
            return;
        }
    }

    printf("Tentando abrir usuarios.txt para leitura...\n");
    int file = pico_open("usuarios.txt", LFS_O_RDONLY);
    printf("pico_open('usuarios.txt', LFS_O_RDONLY) retornou: %d\n", file);

    if (file < 0) {
        printf("Arquivo usuarios.txt não existe, criando...\n");
        file = pico_open("usuarios.txt", LFS_O_WRONLY | LFS_O_CREAT);
        printf("pico_open('usuarios.txt', LFS_O_WRONLY | LFS_O_CREAT) retornou: %d\n", file);

        if (file >= 0) {
            // 1. Gera o hash da senha "123456" com SHA-256
            const char *senha = "123456";
            uint8_t hash_bin[32];
            calc_sha_256(hash_bin, senha, strlen(senha));

            // 2. Converte para string hexadecimal
            char hash_hex[65];
            hash_to_hex(hash_bin, hash_hex);

            // 3. Monta a linha com o usuário admin
            char linha_admin[200];
            snprintf(linha_admin, sizeof(linha_admin),
                     "Raquel Coelho,admin,admin,%s\n", hash_hex);

            // 4. Escreve no arquivo
            int written = pico_write(file, linha_admin, strlen(linha_admin));
            printf("pico_write retornou: %d\n", written);
            pico_close(file);
            printf("Arquivo usuarios.txt criado com senha criptografada com SHA-256.\n");

        } else {
            printf("Erro ao criar usuarios.txt: %d\n", file);
        }

    } else {
        printf("Arquivo usuarios.txt já existe.\n");
        pico_close(file);
    }
}

void criar_senha_fechadura() {
    printf("Verificando se senha_fechadura.txt existe...\n");

    int file = pico_open("senha_fechadura.txt", LFS_O_RDONLY);
    if (file < 0) {
        printf("Arquivo senha_fechadura.txt não existe, criando...\n");

        file = pico_open("senha_fechadura.txt", LFS_O_WRONLY | LFS_O_CREAT);
        if (file >= 0) {
            const char *senha = "123456";
            uint8_t hash_bin[32];
            calc_sha_256(hash_bin, senha, strlen(senha));

            char hash_hex[65];
            hash_to_hex(hash_bin, hash_hex);

            char linha[66];
            snprintf(linha, sizeof(linha), "%s\n", hash_hex);

            int written = pico_write(file, linha, strlen(linha));
            printf("pico_write retornou: %d\n", written);
            pico_close(file);
            printf("Arquivo senha_fechadura.txt criado.\n");
        } else {
            printf("Erro ao criar senha_fechadura.txt: %d\n", file);
        }
    } else {
        printf("Arquivo senha_fechadura.txt já existe.\n");
        pico_close(file);
    }
}

void inserir_logs_simulados() {
    int file = pico_open("logs.txt", LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (file < 0) {
        printf("Erro ao criar ou abrir logs.txt para escrita\n");
        return;
    }

    const char *logs[] = {
        "Raquel Coelho,Administrador,Autenticou no sistema,10/01/2025 18:14:20\n",
        "Raquel Coelho,Administrador,Autenticou no sistema,10/01/2025 19:45:03\n",
        "Raquel Coelho,Administrador,Autenticou no sistema,10/01/2025 20:10:55\n"
    };

    for (int i = 0; i < 3; i++) {
        pico_write(file, logs[i], strlen(logs[i]));
    }

    pico_close(file);
    printf("Logs simulados inseridos com sucesso.\n");
}

int main() {
    stdio_init_all();
    sleep_ms(3000);

    printf("Criando usuário inicial");

    criar_arquivo_usuarios_inicial();

    printf("Criando senha fechadura");

    criar_senha_fechadura();

    inserir_logs_simulados();

    printf("Inicializando relógio DS1307...\n"); 
    inicializar_relogio_gpio();

    // Define a hora atual (seg, min, hora, dia da semana, dia, mês, ano)
    uint8_t data_hora_inicial[7] = {0, 54, 18, 3, 14, 7, 25}; // 15/07/2025 13:42:00
    configurar_relogio(data_hora_inicial);

    char data_hora[20];
    obter_data_hora(data_hora, sizeof(data_hora));
    printf("Data e hora atual: %s\n", data_hora);

    printf("Inicializando fechadura");
    init_fechadura();

    printf("Iniciando sistema...\n");

    // Wi-Fi
    printf("Inicializando Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
        return 1;
    }

    // define IP e máscara do ponto de acesso
    IP4_ADDR(&net_config.gw, 192, 168, 4, 1);
    IP4_ADDR(&net_config.mask, 255, 255, 255, 0);
   
    // inicializa o servidor DHCP
    dhcp_server_init(&dhcp_server, &net_config.gw, &net_config.mask);
    dns_server_init(&dns_server, &net_config.gw);

    //Ativa o modo Access Point com SSID e senha definidos
    printf("Ativando Access Point...\n");
    cyw43_arch_enable_ap_mode(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK);  
    printf("IP configurado: %s\n", ip4addr_ntoa((const ip4_addr_t *)&net_config.gw)); // exibe o ip configurado para o access point
    sleep_ms(1000);

    start_http_server(); // starta o servidor http

    while (true) {
        cyw43_arch_poll(); // mantém o Wi-Fi funcionando
    }

    // nunca alcançado, mas boa prática
    dns_server_deinit(&dns_server);
    dhcp_server_deinit(&dhcp_server);
    cyw43_arch_deinit();
    return 0;
}
