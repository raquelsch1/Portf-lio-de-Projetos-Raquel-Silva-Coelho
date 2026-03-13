# Sistema de Gestão de Lava Jato

Sistema desenvolvido para otimizar o controle de operações, agendamentos e cadastro de clientes de um lava jato.

O projeto destaca-se por sua evolução arquitetural: foi inicialmente concebido como uma aplicação Desktop utilizando conceitos sólidos de Programação Orientada a Objetos e, posteriormente, reestruturado como uma aplicação Web, aplicando metodologias de design e prototipação.

## Evolução do Projeto

### Versão 1: Core em Java e POO (Desktop)
A primeira versão foi desenvolvida em **Java** com interface gráfica em **Swing**. O foco principal desta etapa foi a modelagem do domínio e a aplicação de princípios de Orientação a Objetos (Herança, Encapsulamento e Polimorfismo) e estruturas de dados (Lists, Maps).
* **Arquitetura de Classes:** Separação clara entre `Cliente`, `Veiculo`, `Serviço` e `ServiçoAgendado`.
* **Controle de Acesso:** Implementação de herança para diferenciar usuários comuns (`Usuario`) de administradores/funcionários (`UsuarioPad`).
* **Lógica de Negócio:** Algoritmos para validação de horários, associação de múltiplos veículos a um único cliente e geração de relatórios dinâmicos.

### Versão 2: Aplicação Web em PHP e UX/UI
Na segunda fase, a lógica de negócio foi reaproveitada para a construção de um sistema Web em **PHP**. Esta etapa focou fortemente na experiência do usuário e no ciclo de vida de desenvolvimento de software:
* **Mapeamento de Requisitos:** Aplicação de **Design Thinking** para entender as dores do cliente e do administrador do lava jato.
* **Prototipação:** Criação de interfaces de alta fidelidade visando usabilidade e responsividade.
* 🔗 **[Clique aqui para acessar o protótipo interativo no Figma](https://www.figma.com/proto/vGEb6Fl03NhiwFqcH7pdP1?node-id=91-32)**.
* **Desenvolvimento:** Codificação de todo o projeto proposto no protótipo.

## Funcionalidades Principais

Independentemente da plataforma, o núcleo do sistema garante as seguintes operações operacionais:

* **Gerenciamento de Usuários:** Autenticação com dois níveis de acesso (Administrador e Cliente).
* **Gestão de Frota Pessoal:** Clientes podem cadastrar e gerenciar múltiplos veículos (Placa, Modelo e Tipo) sob o mesmo perfil.
* **Agendamento Inteligente:** Seleção de serviços (Lavagem Simples, Completa, Polimento, etc.), vinculação a um veículo específico e reserva de horários disponíveis.
* **Monitoramento de Fila:** Interface para monitoramento e controle das filas de agendamento e espera em tempo real.
* **Relatórios e Logs:** Geração de relatórios de clientes cadastrados e extratos de serviços agendados para controle gerencial.

## Tecnologias e Metodologias

* **Linguagens:** Java(V1), PHP(V2)
* **Interface:** Java Swing (V1), HTML/CSS/JS (V2)
* **Design e Produto:** Design Thinking, Figma (Prototipação e UI/UX)
* **Conceitos:** Programação Orientada a Objetos (POO), Estruturas de Dados, Engenharia de Requisitos Web.

## Estrutura do Código (Core em Java - V1)

Na implementação inicial em Java, fui responsável por desenvolver toda a arquitetura do back-end, modelando o domínio da aplicação e estruturando as classes e mapeamentos necessários para sustentar as regras de negócio:

**SistemaLavaJato.java:** Atua como o controlador principal do sistema. Utiliza estruturas de dados (`List` e `HashMap`) para gerenciar dinamicamente as instâncias de clientes, veículos, serviços e usuários. Contém a lógica central de autenticação, validação para evitar conflitos de horários de agendamento e algoritmos para a geração de relatórios operacionais.

**Cliente.java ** Implementam a modelagem da entidade Cliente. A classe `Cliente` aplica encapsulamento para proteger dados sensíveis e credenciais, gerenciando sua própria lista de instâncias de `Veiculo`, o que permite buscas de placas e adições dinâmicas de novos carros ao perfil.

## Estrutura do Código (Monitoramento de Filas - V2)

Na implementação Web, atuei diretamente no desenvolvimento da funcionalidade de monitoramento de fluxo operacional, garantindo a visualização e atualização do status dos veículos no lava jato:

**filaAgendamento.php:** Responsável pela interface de controle da fila. O código valida a sessão do usuário e realiza uma junção (`JOIN`) no banco de dados entre clientes e serviços para renderizar dinamicamente os agendamentos que estão com status "Pendente" ou "Em Andamento". A página integra pop-ups de confirmação em JavaScript para evitar exclusões acidentais ou mudanças de status equivocadas.

**atualizarStatus.php:** Atua como o motor lógico (backend) da transição de serviços. Ao receber o ID de um serviço, o script consulta seu estado atual e aplica uma transição automática ("Pendente" avança para "Em Andamento", e este para "Concluído"). Após a atualização no banco de dados, o operador é redirecionado instantaneamente de volta para a visualização da fila, garantindo fluidez na operação.