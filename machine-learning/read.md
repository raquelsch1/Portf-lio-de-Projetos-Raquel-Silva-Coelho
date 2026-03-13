# Aprendizagem de Máquina – Previsão de Inadimplência de Crédito

Projeto focado no desenvolvimento de modelos preditivos para identificar clientes com probabilidade de inadimplência no pagamento de cartões de crédito (Credit Card Default Dataset). A solução busca auxiliar instituições financeiras na redução de riscos e na definição de políticas de concessão de crédito mais seguras.

O fluxo de experimentação, incluindo pré-processamento, treinamento de modelos e avaliação de métricas, foi desenvolvido de forma iterativa e detalhada no notebook do projeto.

🔗 **[Acesso ao Notebook no Google Colab](https://colab.research.google.com/drive/1mNSuVXWRFqTDvGdRPFMS5AF4JJdXltm8)**

## Conceitos Aplicados
Tratamento de dados desbalanceados, construção de pipelines (Scikit-Learn), treinamento de modelos Baseline e Ensemble, seleção de atributos (Feature Selection), ajuste de limiar de decisão para otimização de métricas (F1-Macro, ROC-AUC) e deploy de modelo preditivo.

## Estrutura da Aplicação (Demo)
O repositório contém os artefatos necessários para rodar uma aplicação interativa que realiza previsões com os modelos treinados e otimizados na fase de experimentação.

**app.py:** Script principal da interface web desenvolvida em **Streamlit**. Ele permite que o usuário faça o upload de um dataset de teste (CSV) e interaja com o modelo preditivo. Um grande diferencial desta interface é a funcionalidade de **ajuste dinâmico do limiar de decisão (threshold)** via slider, permitindo observar em tempo real como a mudança da sensibilidade do modelo afeta a classificação final de clientes propensos à inadimplência.

**ensemble_baseline.joblib:** Artefato contendo o modelo de *Machine Learning* serializado. Este arquivo encapsula o algoritmo da categoria *Ensemble* que obteve destaque durante a fase de experimentação. Ao exportar o modelo dessa forma, o `app.py` consegue carregá-lo na memória e realizar inferências instantâneas sobre novos dados, garantindo que todo o pipeline de pré-processamento seja aplicado corretamente sem a necessidade de retreinamento.