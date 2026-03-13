#include <stdio.h>
#include <string.h>
#include "autenticacao.h"
#include "hash/sha-256.h"
#include "pico_hal.h"
#include "logs.h"

bool autenticar(const char *username, const char *senha, char *tipo_out, char *nome_out) {
    int file = pico_open("usuarios.txt", LFS_O_RDONLY);
    if (file < 0) return false;

    // Gera hash SHA-256 da senha passada
    uint8_t hash_bin[32];
    calc_sha_256(hash_bin, senha, strlen(senha));

    // Converte hash para string hexadecimal
    char hash_hex[65];
    hash_to_hex(hash_bin, hash_hex);

    char buffer[128];
    char linha[128];
    int linha_pos = 0;

    int read_bytes;
    while ((read_bytes = pico_read(file, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < read_bytes; i++) {
            char c = buffer[i];
            if (c == '\n' || linha_pos >= (int)(sizeof(linha) - 1)) {
                linha[linha_pos] = '\0'; // finaliza a linha

                // Formato: nome,usuario,tipo,hash
                char nome[64], tipo[16], user[32], hash_str[65];
                if (sscanf(linha, "%63[^,],%15[^,],%31[^,],%64s", nome, tipo, user, hash_str) == 4) {
                    if (strcmp(user, username) == 0) {
                        if (strcmp(hash_str, hash_hex) == 0) {
                            strcpy(tipo_out, tipo);
                            strcpy(nome_out, nome);
                            atualiza_logs(nome, 1, tipo);
                            pico_close(file);
                            return true;
                        }
                    }
                }
                linha_pos = 0;  
            } else {
                linha[linha_pos++] = c;
            }
        }
    }

    pico_close(file);
    return false;
}
