# Fechadura Eletrônica IoT

Sistema de **controle de acesso embarcado** desenvolvido para **Raspberry Pi Pico W**, com interface web integrada para autenticação de usuários e acionamento da fechadura.

O sistema funciona de forma **autônoma**, criando seu próprio **Access Point** e hospedando uma **interface web embarcada**, permitindo que usuários autorizados realizem login e controlem a abertura da fechadura.

---

## Funcionamento do Sistema

Fluxo principal do sistema:

1. O dispositivo cria um **Access Point (Wi-Fi)**
2. O usuário conecta à rede da fechadura
3. O usuário acessa a **interface web** pelo navegador
4. O sistema recebe **usuário e senha**
5. A senha é convertida para **hash**
6. O hash é comparado com os registros armazenados
7. O acesso é **liberado ou negado**
8. A tentativa é registrada em **log**
9. O usuário pode realizar destrancamento da fechadura e, caso seja administrador, também pode gerenciar usuários ou alterar a senha

---

## Tecnologias Utilizadas

- C
- Raspberry Pi Pico W
- Servidor HTTP embarcado
- HTML / CSS / JavaScript
- Biblioteca LittleFS para armazenamento de arquivos persistentes em memória flash
- Hash de senha para autenticação

---

## Conceitos Aplicados

- Sistemas embarcados
- Comunicação em rede
- Interface web embarcada
- Autenticação de usuários
- Persistência de dados em memória flash
- Registro de logs de acesso

---
