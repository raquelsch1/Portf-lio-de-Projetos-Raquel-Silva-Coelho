# Projeto em Java – Sistema de Consultas Médicas (Arquitetura Hexagonal)

Sistema desenvolvido para o gerenciamento de consultas e prontuários médicos de uma clínica pediátrica. O projeto foi construído aplicando rigorosamente a Arquitetura Hexagonal (Ports and Adapters), com o foco em isolar o domínio da aplicação, garantir baixo acoplamento, alta coesão e facilitar a testabilidade do código, além de aplicar princípios SOLID e Padrões de Projeto.

## Minha Atuação (Camada de Aplicação)

Fiquei responsável pelo desenvolvimento da camada de Aplicação, que atua como o núcleo orquestrador da Arquitetura Hexagonal. Minha função foi implementar os casos de uso do sistema definindo os contratos de entrada (In Ports) e saída (Out Ports), garantindo que as regras de negócio fossem executadas sem qualquer acoplamento a frameworks externos, bancos de dados ou interfaces de usuário.

## Estrutura do Código (Arquivos Selecionados)

Os arquivos disponibilizados neste repositório exemplificam a estruturação das portas e serviços que desenvolvi:

**RegistrarProntuarioUseCase.java:** Atua como uma Porta de Entrada (In Port). É a interface que define o contrato do caso de uso para o registro de um prontuário, isolando a intenção da ação da sua implementação real.

**RegistrarProntuarioService.java:** É o serviço que implementa o caso de uso. Ele orquestra o fluxo de negócio validando informações, buscando consultas, associando prescrições de medicamentos e exames ao prontuário do paciente, e delegando a persistência aos repositórios correspondentes.

**ProntuarioRepository.java:** Representa uma Porta de Saída (Out Port). Consiste na interface que define o contrato para a persistência dos dados do prontuário, aplicando a inversão de dependência para que o serviço não precise conhecer os detalhes tecnológicos do banco de dados.

## Tecnologias e Conceitos Aplicados

O projeto foi inteiramente codificado em Java, utilizando Programação Orientada a Objetos (POO). A base do desenvolvimento estruturou-se na Arquitetura Hexagonal, na forte separação de responsabilidades orientada pelos princípios SOLID e no isolamento e modularização do código para suportar evoluções futuras.