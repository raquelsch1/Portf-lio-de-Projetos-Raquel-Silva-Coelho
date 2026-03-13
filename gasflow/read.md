# GasFlow – Gestão de Processos de Compras

O GasFlow é um portal unificado para o acompanhamento e controle de processos de aquisição, projetado para digitalizar o fluxo de compras da Gasmar Gás Natural. O sistema substitui controles manuais e descentralizados, garantindo transparência e rastreabilidade entre a Área Demandante, o Setor de Compras (GERAF) e o Setor Contábil (GECONT).

🔗 **[Clique aqui para acessar o protótipo interativo no Figma](https://www.figma.com/proto/g7IaPwoak01Uw2y73TnPi2/GasFlow?node-id=10-110&p=f&t=mWMPF9K8J8CuV1k1-1&scaling=contain&content-scaling=fixed&page-id=0%3A1&starting-point-node-id=10%3A110&show-proto-sidebar=1)**.

## Minha Atuação (Visão Sistêmica e Arquitetura)

Neste projeto, atuei na fase de concepção, análise de requisitos e definição da arquitetura de software, preparando o terreno técnico para a equipe de desenvolvimento. Minhas principais contribuições incluíram:

* **Mapeamento de Processos:** Desenho do fluxograma "As-Is" (atual) e "To-Be" (ideal) para o MVP, definindo o ciclo de vida da compra (abertura, diligenciamento, validação de nota fiscal e pagamento).
* **Prototipação UI/UX:** Contribuição na criação das interfaces no Figma, garantindo que o design refletisse as necessidades dos diferentes setores envolvidos no processo de aquisição.
* **Definição de Stack Tecnológica:** Escolha da arquitetura baseada em **Spring Boot** para o back-end, **Thymeleaf** para o front-end e **HTMX** para interatividade, visando simplicidade de manutenção e atualização dinâmica de componentes sem a complexidade de uma SPA.
* **Padronização Técnica:** Elaboração de documentos de diretrizes para a equipe, definindo padrões de identificação de processos, regras de armazenamento de arquivos (documentos no File System, metadados no BD) e nomenclatura de status.

## Modelagem do Banco de Dados

Fui responsável pela engenharia de dados inicial do sistema, construindo o Diagrama Entidade-Relacionamento (ER) e o script SQL estrutural. A modelagem garante a integridade e rastreabilidade do processo através de:

* **Centralização do Processo:** Tabela central `processos` que consolida os valores, status e tipo de aquisição (CPV ou Contrato).
* **Histórico de Auditoria:** Criação da tabela `historico_processos` para registrar cada transição de estado da compra, indicando quem fez a alteração e quando, garantindo compliance.
* **Gestão de Documentos (1:N):** Tabela `documentos` que relaciona múltiplos arquivos (PABS, Nota Fiscal, Boletos) a um único processo, armazenando os metadados e os caminhos (URLs) do servidor de arquivos.

## Próximos Passos: Desenvolvimento do MVP

Com as fases de análise, engenharia de requisitos, prototipação e modelagem de banco de dados concluídas, o projeto avançará agora para a etapa de codificação.

O sistema será desenvolvido utilizando o ecossistema **Java com Spring Boot, e eu estarei atuando ativamente nesta fase. Participarei do desenvolvimento do back-end, integração do banco de dados e implementação das regras de negócio mapeadas, garantindo que a arquitetura idealizada seja executada com sucesso.

## Arquivos do Repositório

Nesta pasta, encontram-se os artefatos gerados durante a fase de análise e planejamento do sistema:

**script_gasflow.sql e diagramaER_GasFlow.png:** Contêm a modelagem estrutural do banco de dados, estabelecendo as entidades, relacionamentos e restrições para suportar o fluxo de compras, pagamentos e registro de histórico.

**Fluxograma - GásFlow MVP (7).pdf:** Apresenta o mapeamento visual da regra de negócio, detalhando o "caminho feliz" do processo no MVP, desde o login e abertura do processo até a validação do recebimento e pagamento final.

**Planejamento Inicial - GásFlow (1) (2).pdf:** Documenta as decisões arquiteturais fundamentais, como a escolha do Spring Boot com Thymeleaf e HTMX, a estratégia de armazenamento local de arquivos associados a metadados no banco de dados e a centralização do controle de usuários.

**Padronizações necessárias para integração futura.pdf:** Define as diretrizes técnicas para a equipe de desenvolvimento, incluindo a estrutura de pastas para upload de arquivos (`/uploads/processos/...`), a padronização dos estados do sistema para auditoria e a nomenclatura dos identificadores únicos de processos (ex: 001-2026).