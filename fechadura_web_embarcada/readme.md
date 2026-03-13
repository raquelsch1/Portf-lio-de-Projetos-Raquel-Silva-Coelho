# Fechadura Eletrônica IoT

Sistema de **controle de acesso embarcado** desenvolvido para **Raspberry Pi Pico W**, com interface web integrada para autenticação de usuários e acionamento da fechadura.

O sistema funciona de forma **autônoma**, criando seu próprio **Access Point** e hospedando uma **interface web embarcada**, permitindo que usuários autorizados realizem login e controlem a abertura da fechadura.

## Destaques e Reconhecimentos
Este trabalho obteve nota máxima na avaliação final do curso e alcançou projeção acadêmica e profissional. O projeto foi apresentado no **Fórum Energias Renováveis Maranhão 2025** (participação a convite) e no **Universo IFMA 2025** (trabalho aprovado por submissão). Os registros fotográficos das exibições nestes eventos podem ser visualizados na pasta `/fotos` deste repositório (ex: [Apresentação Fórum](fotos/forum.jpg) e [Apresentação IFMA](fotos/ifma.jpg)).

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

## Estrutura do Código
Os arquivos centrais deste repositório ilustram a arquitetura e a lógica do firmware:

**main.c:** Arquivo principal que orquestra a inicialização do sistema. Ele configura o hardware, incluindo o relógio RTC DS1307, levanta a rede Wi-Fi em modo Access Point e inicia os servidores DHCP, DNS e HTTP. Também é responsável por montar o sistema de arquivos LittleFS na flash e gerar os arquivos e credenciais de administrador caso o sistema seja ligado pela primeira vez.

**http.c:** Concentra a lógica do servidor web embarcado e o roteamento de requisições HTTP. Processa os métodos GET e POST, gerencia as sessões dos usuários através da leitura e validação de cookies de sessão e aplica as restrições de acesso, bloqueando rotas não autorizadas e direcionando funcionalidades exclusivas para administradores.

**autenticacao.c:** Gerencia a regra de negócio da verificação de credenciais de login. Ele recebe os dados do usuário, calcula o hash SHA-256 da senha inserida e compara com os registros seguros lidos da memória flash. Ao confirmar a identidade, autoriza a sessão e aciona a função para registro do evento nos logs do sistema. Esse arquivo demonstra a lógica de acesso de dados no sistema que ficaram armazenados na flash do microcontrolador em arquivos .txt através de uma biblioteca LittleFS que cria um diretório de arquivos na própria placa.

**principal_admin.h:** Exemplifica a estruturação do front-end embarcado. Demonstra a abordagem de construir a interface web dinamicamente, declarando a função que injeta o HTML, CSS e JavaScript diretamente como strings no código C para renderizar a página do administrador.
