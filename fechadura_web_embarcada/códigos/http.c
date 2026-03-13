#include "http.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "web/login.h"
#include "web/principal.h"
#include "web/principal_admin.h"
#include "autenticacao.h"
#include "autentica_senha_fechadura.h"
#include "web/cadastro-usuario.h"
#include "cadastrar_usuario.h"
#include "logs.h"
#include "web/logs-page.h"
#include "controle_fechadura.h"
#include "web/alterar-senha.h"
#include "modificar_senha_fechadura.h"
#include "web/excluir-usuario.h"
#include "remover_usuario.h"

// Buffer para armazenar resposta HTTP
char http_response[8192];

// Token de sessão fixo
const char *SESSION_TOKEN = "SESSAO123";

// Função para verificar a sessão
bool autenticado(const char *request) {
    char *cookie = strstr(request, "Cookie: ");
    if (!cookie) return false;

    // Avança para o valor dos cookies
    cookie += strlen("Cookie: ");

    // Buffer para extrair o valor do cookie session
    char session_value[64] = {0};

    // Procura por "session=" na string de cookies
    char *session_start = strstr(cookie, "session=");
    if (!session_start) return false;

    session_start += strlen("session=");

    // Extrai o valor do cookie até ; ou espaço ou fim da linha
    int i = 0;
    while (session_start[i] != ';' && session_start[i] != ' ' && session_start[i] != '\r' && session_start[i] != '\n' && i < (int)(sizeof(session_value)-1)) {
        session_value[i] = session_start[i];
        i++;
    }
    session_value[i] = '\0';

    printf("session_value extraído: '%s'\n", session_value);

    // Compara com o token esperado
    return strcmp(session_value, SESSION_TOKEN) == 0;
}

// Função para extrair valor de cookie
bool extrair_valor_cookie(const char *requisicao, const char *nome_cookie, char *saida, size_t tamanho_saida) {
    char *cookie = strstr(requisicao, "Cookie: ");
    if (!cookie) return false;

    cookie += strlen("Cookie: ");
    char *inicio_valor = strstr(cookie, nome_cookie);
    if (!inicio_valor) return false;

    inicio_valor += strlen(nome_cookie);
    int i = 0;
    while (inicio_valor[i] != ';' && inicio_valor[i] != ' ' && 
           inicio_valor[i] != '\r' && inicio_valor[i] != '\n' && 
           i < (int)(tamanho_saida-1)) {
        saida[i] = inicio_valor[i];
        i++;
    }
    saida[i] = '\0';
    return true;
}

// Função para verificar se é admin
bool eh_admin(const char *requisicao) {
    char tipo[16] = {0};
    if (!extrair_valor_cookie(requisicao, "tipo=", tipo, sizeof(tipo))) {
        return false;
    }
    return strcmp(tipo, "admin") == 0;
}

bool verificar_acesso_admin(const char *requisicao) {
    if (!autenticado(requisicao)) {
        printf("Usuário não logado, redirecionando");
        snprintf(http_response, sizeof(http_response),  
            "HTTP/1.1 303 See Other\r\n"
            "Location: /\r\n"
            "Content-Length: 0\r\n"
            "\r\n");
        return false;
    }
    
    if (!eh_admin(requisicao)) {
        printf("Tentativa de acesso não autorizado, redirecionando");
        snprintf(http_response, sizeof(http_response),
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n\r\n"
            "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
            "<script>alert('Acesso negado: apenas administradores podem acessar esta página.');"
            "window.location.href = '/principal';</script>"
            "</head><body></body></html>");
        return false;
    }
    
    return true;
}

void url_decode(char *str) {
    char *src = str, *dst = str;
    char hex[3] = {0};
    while (*src) {
        if ((*src == '%') && src[1] && src[2]) {
            hex[0] = src[1];
            hex[1] = src[2];
            *dst++ = (char) strtol(hex, NULL, 16);
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

void url_encode(const char *src, char *dest, size_t max_len) {
    const char *hex = "0123456789ABCDEF";
    size_t i = 0;

    while (*src && i < max_len - 1) {
        char c = *src++;
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            dest[i++] = c;
        } else {
            if (i + 3 >= max_len) break; 
            dest[i++] = '%';
            dest[i++] = hex[(c >> 4) & 0xF];
            dest[i++] = hex[c & 0xF];
        }
    }
    dest[i] = '\0';
}

static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;
    printf("Requisição recebida:\n%s\n", request);

    // Rota POST /login
    if (strstr(request, "POST /login")) {
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4;

            // Verifica o Content-Length
            char *cl_header = strstr(request, "Content-Length: ");
            if (cl_header) {
                int content_length = atoi(cl_header + 16);

                if (p->tot_len < (body - request) + content_length) {
                    tcp_recved(tpcb, p->tot_len);
                    pbuf_free(p);
                    return ERR_OK;
                }

                char body_copy[512] = {0};
                memcpy(body_copy, body, content_length);
                body_copy[content_length] = '\0';

                printf("Corpo bruto seguro: %s\n", body_copy);

                char username[64] = {0};
                char password[64] = {0};
               
                char *username_ptr = strstr(body_copy, "username=");
                char *password_ptr = strstr(body_copy, "password=");

                if (username_ptr && password_ptr) {
                    username_ptr += strlen("username=");
                    char *amp = strchr(username_ptr, '&');
                    if (amp) *amp = '\0';  
                    strncpy(username, username_ptr, sizeof(username) - 1);
                    strncpy(password, password_ptr + strlen("password="), sizeof(password) - 1);

                    url_decode(username);
                    url_decode(password);
                    
                    printf("DEBUG: username='%s', password='%s'\n", username, password);

                    char tipo[16] = {0};
                    char nome_completo[64] = {0}; 

                    if (autenticar(username, password, tipo, nome_completo)) {
                        printf("DEBUG: tipo='%s' (tamanho: %zu)\n", tipo, strlen(tipo)); 
                        printf("DEBUG: Comparação com 'admin': %d\n", strcmp(tipo, "admin"));
                        const char *pagina = strcmp(tipo, "admin") == 0 ? "/principal_admin" : "/principal";

                        printf("DEBUG: Redirecionando para: %s\n", pagina);

                        char nome_codificado[128] = {0};
                        url_encode(nome_completo, nome_codificado, sizeof(nome_codificado));

                        snprintf(http_response, sizeof(http_response),
                            "HTTP/1.1 303 See Other\r\n"
                            "Set-Cookie: session=%s; Path=/;\r\n"
                            "Set-Cookie: nome=%s; Path=/;\r\n"
                            "Set-Cookie: tipo=%s; Path=/;\r\n"
                            "Location: %s\r\n"
                            "Content-Length: 0\r\n"
                            "\r\n", SESSION_TOKEN, nome_codificado, tipo, pagina);

                        printf("DEBUG: resposta HTTP:\n%s\n", http_response);
                    } else {
                        create_login_page("Usuário ou senha inválidos.");
                    }
                } else {
                    create_login_page("Erro ao extrair dados do formulário.");
                }
            } else {
                create_login_page("Erro no cabeçalho Content-Length");
            }
        } else {
            create_login_page("Erro no envio do formulário");
        }
    }

    // Rota POST /senha-fechadura (autentica senha da fechadura)
    else if (strstr(request, "POST /senha-fechadura")) {
        char *body = strstr(request, "\r\n\r\n");
        if (body && autenticado(request)) {
            body += 4;

            char tipo_extraido[16] = {0};

            // Verifica Content-Length
            char *cl_header = strstr(request, "Content-Length: ");
            if (cl_header) {
                int content_length = atoi(cl_header + 16);

                if (p->tot_len < (body - request) + content_length) {
                    tcp_recved(tpcb, p->tot_len);
                    pbuf_free(p);
                    return ERR_OK;
                }

                char body_copy[128] = {0};
                memcpy(body_copy, body, content_length);
                body_copy[content_length] = '\0';

                char *nome_usuario = NULL;
                char nome_extraido[64] = {0};

                char *cookie = strstr(request, "Cookie: ");
                if (cookie) {
                    cookie += strlen("Cookie: ");
                    char *nome_start = strstr(cookie, "nome=");
                    if (nome_start) {
                        nome_start += strlen("nome=");
                        int i = 0;
                        while (nome_start[i] != ';' && nome_start[i] != ' ' && nome_start[i] != '\r' && nome_start[i] != '\n' && i < 63) {
                            nome_extraido[i] = nome_start[i];
                            i++;
                        }
                        nome_extraido[i] = '\0';
                        url_decode(nome_extraido); 
                        nome_usuario = nome_extraido;
                        printf("Usuário autenticado: %s\n", nome_usuario);
                    }
                }

                char *tipo_start = strstr(cookie, "tipo=");
                if (tipo_start) {
                    tipo_start += strlen("tipo=");
                    int i = 0;
                    while (tipo_start[i] != ';' && tipo_start[i] != ' ' && tipo_start[i] != '\r' && tipo_start[i] != '\n' && i < 15) {
                        tipo_extraido[i] = tipo_start[i];
                        i++;
                    }
                    tipo_extraido[i] = '\0';

                    printf("Tipo de usuário: %s\n", tipo_extraido);
                }

                char senha[64] = {0};
                if (sscanf(body_copy, "senha=%63s", senha) == 1) {
                    printf("Senha recebida: %s\n", senha);

                   if (autentica_senha_fechadura(senha)) {
                    destranca_fechadura(); 
                    atualiza_logs(nome_usuario, 2, tipo_extraido);

                    const char *pagina_destino = strcmp(tipo_extraido, "admin") == 0 ? "/principal_admin" : "/principal";

                    snprintf(http_response, sizeof(http_response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                        "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
                        "<script>alert('Fechadura aberta!!');"
                        "window.location.href = '%s';</script>"
                        "</head><body></body></html>", pagina_destino); 
                } else {
                    if (strcmp(tipo_extraido, "admin") == 0) {
                        create_principal_admin_page("Senha incorreta. Tente novamente.");
                    } else {
                        create_principal_page("Senha incorreta. Tente novamente.");
                    }
                }
                } else {
                    if (strcmp(tipo_extraido, "admin") == 0) {
                        create_principal_admin_page("Erro ao processar a senha.");
                    } else {
                        create_principal_page("Erro ao processar a senha.");
                    }
                }
            } else {
                if (strcmp(tipo_extraido, "admin") == 0) {
                    create_principal_admin_page("Erro no cabeçalho Content-Length.");
                } else {
                    create_principal_page("Erro no cabeçalho Content-Length.");
                }
            }
        } else {
            snprintf(http_response, sizeof(http_response),
                "HTTP/1.1 401 Unauthorized\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
        }
    }

    // Rota POST /trancar-fechadura
    else if (strstr(request, "POST /trancar-fechadura")) {
        char *body = strstr(request, "\r\n\r\n");
        if (body && autenticado(request)) {
            body += 4;

            char tipo_extraido[16] = {0};
            char nome_extraido[64] = {0};
            const char *pagina_destino = "/principal";  // padrão

            // Verifica Content-Length
            char *cl_header = strstr(request, "Content-Length: ");
            if (cl_header) {
                int content_length = atoi(cl_header + 16);

                if (p->tot_len < (body - request) + content_length) {
                    tcp_recved(tpcb, p->tot_len);
                    pbuf_free(p);
                    return ERR_OK;
                }

                // Extrai nome e tipo do cookie
                char *cookie = strstr(request, "Cookie: ");
                if (cookie) {
                    cookie += strlen("Cookie: ");
                    char *nome_start = strstr(cookie, "nome=");
                    if (nome_start) {
                        nome_start += strlen("nome=");
                        int i = 0;
                        while (nome_start[i] != ';' && nome_start[i] != ' ' && nome_start[i] != '\r' && nome_start[i] != '\n' && i < 63) {
                            nome_extraido[i] = nome_start[i];
                            i++;
                        }
                        nome_extraido[i] = '\0';
                        url_decode(nome_extraido);
                    }

                    char *tipo_start = strstr(cookie, "tipo=");
                    if (tipo_start) {
                        tipo_start += strlen("tipo=");
                        int i = 0;
                        while (tipo_start[i] != ';' && tipo_start[i] != ' ' && tipo_start[i] != '\r' && tipo_start[i] != '\n' && i < 15) {
                            tipo_extraido[i] = tipo_start[i];
                            i++;
                        }
                        tipo_extraido[i] = '\0';
                    }
                }

                // Atualiza página destino com base no tipo
                if (strcmp(tipo_extraido, "admin") == 0) {
                    pagina_destino = "/principal_admin";
                }

                tranca_fechadura();
                atualiza_logs(nome_extraido, 3, tipo_extraido);

                snprintf(http_response, sizeof(http_response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                    "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
                    "<script>alert('Fechadura trancada com sucesso!');"
                    "window.location.href = '%s';</script>"
                    "</head><body></body></html>", pagina_destino);
            } else {
                if (strcmp(tipo_extraido, "admin") == 0) {
                    create_principal_admin_page("Erro no cabeçalho Content-Length.");
                } else {
                    create_principal_page("Erro no cabeçalho Content-Length.");
                }
            }
        } else {
            snprintf(http_response, sizeof(http_response),
                "HTTP/1.1 401 Unauthorized\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
        }
    }

    // Rota para alterar senha da fechadura
    else if (strstr(request, "POST /modificar-senha")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4;

            char tipo_extraido[16] = {0};
            char nome_extraido[64] = {0};

            char *cl_header = strstr(request, "Content-Length: ");
            if (cl_header) {
                int content_length = atoi(cl_header + 16);

                if (p->tot_len < (body - request) + content_length) {
                    tcp_recved(tpcb, p->tot_len);
                    pbuf_free(p);
                    return ERR_OK;
                }

                char body_copy[128] = {0};
                memcpy(body_copy, body, content_length);
                body_copy[content_length] = '\0';

                char nova_senha[64] = {0};
                sscanf(body_copy, "nova_senha=%63s", nova_senha);
                url_decode(nova_senha);

                char *cookie = strstr(request, "Cookie: ");
                if (cookie) {
                    cookie += strlen("Cookie: ");

                    char *nome_start = strstr(cookie, "nome=");
                    if (nome_start) {
                        nome_start += strlen("nome=");
                        int i = 0;
                        while (nome_start[i] != ';' && nome_start[i] != ' ' && nome_start[i] != '\r' && nome_start[i] != '\n' && i < 63) {
                            nome_extraido[i] = nome_start[i];
                            i++;
                        }
                        nome_extraido[i] = '\0';
                        url_decode(nome_extraido);
                    }

                    char *tipo_start = strstr(cookie, "tipo=");
                    if (tipo_start) {
                        tipo_start += strlen("tipo=");
                        int i = 0;
                        while (tipo_start[i] != ';' && tipo_start[i] != ' ' && tipo_start[i] != '\r' && tipo_start[i] != '\n' && i < 15) {
                            tipo_extraido[i] = tipo_start[i];
                            i++;
                        }
                        tipo_extraido[i] = '\0';
                    }
                }

                printf("Nova senha recebida: %s\n", nova_senha);
                printf("Usuário que solicitou: %s (tipo: %s)\n", nome_extraido, tipo_extraido);

                if (alterar_senha_fechadura(nova_senha)) {
                    atualiza_logs(nome_extraido, 4, tipo_extraido);

                    snprintf(http_response, sizeof(http_response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                        "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
                        "<script>alert('Senha alterada com sucesso!');"
                        "window.location.href = '/principal_admin';</script>"
                        "</head><body></body></html>");
                } else {
                    create_alterar_senha_page("Erro ao alterar a senha. Tente novamente.");
                }
            } else {
                create_alterar_senha_page("Erro no cabeçalho Content-Length.");
            }
        } else {
            snprintf(http_response, sizeof(http_response),
                "HTTP/1.1 401 Unauthorized\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
        }
    }

    // Rota Post Cadastrar Usuário
    else if (strstr(request, "POST /cadastrar-usuario")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        
        char *body = strstr(request, "\r\n\r\n");
        if (body) {
            body += 4;

            char *cl_header = strstr(request, "Content-Length: ");
            if (cl_header) {
                int content_length = atoi(cl_header + 16);

                if (p->tot_len < (body - request) + content_length) {
                    tcp_recved(tpcb, p->tot_len);
                    pbuf_free(p);
                    return ERR_OK;
                }

                char body_copy[512] = {0};
                memcpy(body_copy, body, content_length);
                body_copy[content_length] = '\0';

                printf("Formulário recebido: %s\n", body_copy);

                char nome[64] = {0};
                char tipo[16] = {0};
                char usuario[32] = {0};
                char senha[64] = {0};

                sscanf(body_copy,
                       "nome=%63[^&]&tipo=%15[^&]&usuario=%31[^&]&senha=%63s",
                       nome, tipo, usuario, senha);

                url_decode(nome);
                url_decode(tipo);
                url_decode(usuario);
                url_decode(senha);

                printf("DEBUG: nome='%s', tipo='%s', usuario='%s', senha='%s'\n", nome, tipo, usuario, senha);

                if (cadastrar_usuario(nome, tipo, usuario, senha)) {
                    snprintf(http_response, sizeof(http_response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                        "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
                        "<script>alert('Usuário criado com sucesso!');"
                        "window.location.href = '/principal_admin';</script>"
                        "</head><body></body></html>");
                    } else {
                        create_cadastro_usuario_page("Erro ao cadastrar usuário. Tente novamente");
                    }
                } else {
                    create_cadastro_usuario_page("Erro no cabeçalho Content-Length.");
                }
            } else {
                create_cadastro_usuario_page("Erro no corpo da requisição.");
            }
        }

    // Rota ver logs
    else if (strstr(request, "GET /ver-logs-page")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        create_logs_page();  
    }

    // Rota para excluir-usuario
    else if (strstr(request, "POST /excluir-usuario")) {
    if (!verificar_acesso_admin(request)) {
        tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
        pbuf_free(p);
        return ERR_OK;
    }

    char *body = strstr(request, "\r\n\r\n");
    if (body) {
        body += 4;

        char *cl_header = strstr(request, "Content-Length: ");
        if (cl_header) {
            int content_length = atoi(cl_header + 16);

            if (p->tot_len < (body - request) + content_length) {
                tcp_recved(tpcb, p->tot_len);
                pbuf_free(p);
                return ERR_OK;
            }

            char body_copy[256] = {0};
            memcpy(body_copy, body, content_length);
            body_copy[content_length] = '\0';

            char nome[64] = {0};
            char tipo[16] = {0};
            char usuario[64] = {0};

            sscanf(body_copy, "nome=%63[^&]&tipo=%15[^&]&usuario=%63s", nome, tipo, usuario);
            url_decode(nome);
            url_decode(tipo);
            url_decode(usuario);

            printf("Excluir usuário: nome='%s', tipo='%s', usuario='%s'\n", nome, tipo, usuario);

            if (remover_usuario(nome, tipo, usuario)) {
                snprintf(http_response, sizeof(http_response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                    "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
                    "<script>alert('Usuário removido com sucesso!');"
                    "window.location.href = '/excluir-usuario';</script>"
                    "</head><body></body></html>");
                } else {
                    create_excluir_usuario_page("Erro ao excluir usuário.");
                }
            } else {
                create_excluir_usuario_page("Erro no cabeçalho Content-Length");  
            }
        } else {
            create_excluir_usuario_page("Erro no corpo da requisição.");  
        }
    }


    // Rota excluir-usuario
     else if (strstr(request, "GET /excluir-usuario")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        create_excluir_usuario_page(NULL);  
    }

    // Rota alterar senha (exibir página)
     else if (strstr(request, "GET /alterar-senha")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        create_alterar_senha_page(NULL);  
    }

    // Rota limpar logs antigos
    else if (strncmp(request, "POST /limpar-logs", 17) == 0) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        
        limpar_logs();
        snprintf(http_response, sizeof(http_response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n\r\n"
            "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
            "<script>alert('Logs antigos removidos com sucesso!');"
            "window.location.href = '/ver-logs-page';</script>"
            "</head><body></body></html>");
    }

    // Rota logout
    else if (strncmp(request, "POST /logout", 13) == 0) {
        snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 302 Found\r\n"
        "Set-Cookie: session=; Max-Age=0; Path=/;\r\n"
        "Set-Cookie: nome=; Max-Age=0; Path=/;\r\n"
        "Set-Cookie: tipo=; Max-Age=0; Path=/;\r\n"
        "Location: /\r\n"
        "Content-Length: 0\r\n"
        "\r\n");
    }

    // Rota GET / Login
    else if (strstr(request, "GET / ") || strstr(request, "GET /HTTP")) {
        create_login_page(NULL);
    }
    
    // Rota GET /principal
    else if (strstr(request, "GET /principal ") || strstr(request, "GET /principal\r") || strstr(request, "GET /principal\n")) {
        if (autenticado(request)) {
            create_principal_page(NULL);
        } else {
             // Resposta mais robusta com headers para evitar cache
        snprintf(http_response, sizeof(http_response),
            "HTTP/1.1 303 See Other\r\n"
            "Location: /\r\n"
            "Content-Length: 0\r\n"
            "\r\n");
        
        // Log para debug (remover em produção)
        printf("Redirecionando usuário não autenticado para login\n");
        }
    }

    // Rota GET /principal_admin
    else if (strstr(request, "GET /principal_admin")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        create_principal_admin_page(NULL);
    }
    
    // Rota GET /cadastrar-usuario-page
    else if (strstr(request, "GET /cadastrar-usuario-page")) {
        if (!verificar_acesso_admin(request)) {
            tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
            pbuf_free(p);
            return ERR_OK;
        }
        create_cadastro_usuario_page(NULL);
    }
    
    // Qualquer outra rota
    else {
        snprintf(http_response, sizeof(http_response),
            "HTTP/1.1 303 See Other\r\n"
            "Location: /\r\n"
            "Content-Length: 0\r\n"
            "\r\n");
    }

    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

// Callback de conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Inicia o servidor HTTP
void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) return;
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) return;
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
}