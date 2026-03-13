CREATE DATABASE gasflow;
USE gasflow;

CREATE TABLE setores (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    nome VARCHAR(150) NOT NULL,
    sigla VARCHAR(20),
    ativo BOOLEAN DEFAULT TRUE
);

CREATE TABLE usuarios (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    nome VARCHAR(150) NOT NULL,
    email VARCHAR(150) UNIQUE NOT NULL,
    setor_id BIGINT,
    
    FOREIGN KEY (setor_id) REFERENCES setores(id)
);

CREATE TABLE processos (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    identificador VARCHAR(8) UNIQUE NOT NULL,
    titulo VARCHAR(200) NOT NULL,
    valor_total DECIMAL(15,2),

    tipo_processo ENUM(
        'CPV',
        'CONTRATO'
    ) NOT NULL,

    tipo_aquisicao ENUM(
        'MATERIAL',
        'SERVICO'
    ),

    setor_demandante_id BIGINT NOT NULL,

    estado_atual ENUM(
		'AGUARDANDO_ANALISE',
		'AGUARDANDO_RECEBIMENTO_EXECUCAO',
		'AGUARDANDO_SOLICITACAO_PAGAMENTO',
		'AGUARDANDO_AUTORIZACAO_PAGAMENTO',
		'AGUARDANDO_VALIDACAO_MATERIAL',
		'ENCERRADO',
		'RECEBIDO_NAO_CONFORME',
		'SOLICITACAO_NAO_CONFORME',
		'NF_NAO_CONFORME'
    ) NOT NULL,

    fiscal_id BIGINT NULL,

    FOREIGN KEY (setor_demandante_id) REFERENCES setores(id),
    FOREIGN KEY (fiscal_id) REFERENCES usuarios(id)
);

CREATE TABLE documentos (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    processo_id BIGINT NOT NULL,
    url_arquivo VARCHAR(500) NOT NULL,
    nome_arquivo VARCHAR(255) NOT NULL,
	tipo_documento ENUM(
        'PABS',
        'MEMORIAL_DESCRITIVO',
        'MAPA_COTACAO',
        'PROPOSTAS',
        'CONTRATO',
        'AUTORIZACAO_COMPRA',
        'NOTA_FISCAL',
        'BOLETO',
        'COMPROVANTE_PAGAMENTO',
        'FOTOS_RECEBIMENTO',
        'OUTROS_DOCUMENTOS'
    ) NOT NULL,

    data_upload TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    usuario_id BIGINT NOT NULL,
    
    FOREIGN KEY (processo_id) REFERENCES processos(id),
    FOREIGN KEY (usuario_id) REFERENCES usuarios(id)
);

CREATE TABLE pagamentos (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    
    processo_id BIGINT NOT NULL,
    NF_documento_id BIGINT,

    solicitado_por BIGINT NOT NULL,
    autorizado_por BIGINT,

    valor_pago DECIMAL(15,2),
    data_pagamento DATE,

    tipo_pagamento ENUM(
        'ADIANTAMENTO',
        'PARCIAL',
        'FINAL'
    ),
    
    status_pagamento ENUM(
        'SOLICITADO',
        'AUTORIZADO',
        'PAGO'
    ) DEFAULT 'SOLICITADO',

    FOREIGN KEY (NF_documento_id) REFERENCES documentos(id),
    FOREIGN KEY (processo_id) REFERENCES processos(id),
    FOREIGN KEY (solicitado_por) REFERENCES usuarios(id),
    FOREIGN KEY (autorizado_por) REFERENCES usuarios(id)
);

CREATE TABLE registro_entregas (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,

    processo_id BIGINT NOT NULL,
    registrado_por_id BIGINT NOT NULL,

    data_registro TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY (processo_id) REFERENCES processos(id),
    FOREIGN KEY (registrado_por_id) REFERENCES usuarios(id)
);

CREATE TABLE validacao_recebimentos (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,

    entrega_id BIGINT NOT NULL,
    validado_por_id BIGINT NOT NULL,

    data_validacao TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    status_validacao ENUM(
        'PENDENTE',
        'APROVADO',
        'REJEITADO'
    ) DEFAULT 'PENDENTE',

    FOREIGN KEY (entrega_id) REFERENCES registro_entregas(id),
    FOREIGN KEY (validado_por_id) REFERENCES usuarios(id)
);

CREATE TABLE historico_processos (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,

    processo_id BIGINT NOT NULL,

    estado_anterior ENUM(
       'AGUARDANDO_ANALISE',
		'AGUARDANDO_RECEBIMENTO_EXECUCAO',
		'AGUARDANDO_SOLICITACAO_PAGAMENTO',
		'AGUARDANDO_AUTORIZACAO_PAGAMENTO',
		'AGUARDANDO_VALIDACAO_MATERIAL',
		'ENCERRADO',
		'RECEBIDO_NAO_CONFORME',
		'SOLICITACAO_NAO_CONFORME',
		'NF_NAO_CONFORME'
    ),

    estado_novo ENUM(
      'AGUARDANDO_ANALISE',
		'AGUARDANDO_RECEBIMENTO_EXECUCAO',
		'AGUARDANDO_SOLICITACAO_PAGAMENTO',
		'AGUARDANDO_AUTORIZACAO_PAGAMENTO',
		'AGUARDANDO_VALIDACAO_MATERIAL',
		'ENCERRADO',
		'RECEBIDO_NAO_CONFORME',
		'SOLICITACAO_NAO_CONFORME',
		'NF_NAO_CONFORME'
    ),

    usuario_id BIGINT NOT NULL,

    data_transicao TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY (processo_id) REFERENCES processos(id),
    FOREIGN KEY (usuario_id) REFERENCES usuarios(id)
);