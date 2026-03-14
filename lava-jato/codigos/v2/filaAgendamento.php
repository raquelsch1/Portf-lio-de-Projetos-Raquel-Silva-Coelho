<?php
session_start();
if (!isset($_SESSION['usuario'])) {
    header('Location: login.php');
    exit();
}

include 'modomonocromatico.php';
?>

<!DOCTYPE html>
<html lang="pt-BR">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fila de Agendamento</title>
    <link rel="stylesheet" href="css/style.css">
    <link href="https://fonts.googleapis.com/css2?family=Sansation&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Smooch+Sans&display=swap" rel="stylesheet">
</head>

<body>
    <div class="header">
        <div class="home-titulo">
            <button class="home-button hover" onclick="window.location.href='<?php echo ($_SESSION['perfil'] == 'administrador') ? 'index_admin.php' : 'index.php'; ?>'">
                <img src="img/casa.png" alt="imagem de casinha">
            </button>
            <div class="vertical-line"></div>
            <p class="title">Monitoramento de Serviços</p>
        </div>
        <div class="botoes">
            <a href="filaAgendamento.php" class="botoes-secao selecionado">Agendamento</a>
            <a href="filaEspera.php" class="botoes-secao hover">Espera</a>
            <a href="mostrarTodos.php" class="botoes-secao hover">Mostrar Todos</a>
        </div>
    </div>

    <div class="container-main">
        <div class="lista">
            <div class="cabecalho-lista">
                <div class="coluna">Cliente</div>
                <div class="coluna">Serviço</div>
                <div class="coluna">Data</div>
                <div class="coluna">Horário</div>
                <div class="coluna">Valor</div>
                <div class="coluna">Operador</div>
                <div class="coluna">Status</div>
            </div>

            <?php
            include 'php_actions/connect_db.php';

            $query = "
                SELECT s.id_servicos, c.nome AS cliente, s.nome_servico, s.data, s.horario, s.valor, s.funcionario, s.stattus
                FROM servicos s
                JOIN cliente c ON s.cliente_id = c.id_cliente
                WHERE s.fila = 'Agendamento' AND s.stattus IN ('Pendente', 'Em Andamento')
                ORDER BY s.data ASC, s.horario ASC;
            ";

            $result = mysqli_query($conn, $query);

            if ($result && mysqli_num_rows($result) > 0) {
                while ($servico = mysqli_fetch_assoc($result)) {
                    // formatar data para dd/mm/yy.
                    $data = new DateTime($servico['data']);
                    $data = $data->format('d/m/y');
                    $valor = number_format($servico['valor'], 2, ',', ''); //formata valor com duas casas decimais
                    if ($servico['stattus'] == 'Pendente') {
                        $classeStatus = 'status-pendente';
                    } else {
                        $classeStatus = 'status-andamento';
                    }

                    echo "
                    <div class='linha-servico'>
                        <div class='coluna'>{$servico['cliente']}</div>
                        <div class='coluna'>{$servico['nome_servico']}</div>
                        <div class='coluna'>{$data}</div>
                        <div class='coluna'>{$servico['horario']}</div>
                        <div class='coluna'>R\${$valor}</div>
                        <div class='coluna'>{$servico['funcionario']}</div>
                        <div class='coluna $classeStatus'>{$servico['stattus']}</div>
                    </div>
                    ";
                }
            } else {
                echo "<div class='linha-servico'><div class='coluna' colspan='7'>Nenhum serviço na fila de agendamento</div></div>";
            }

            mysqli_close($conn);
            ?>
        </div>

        <div class="botoes-laterais">
            <?php
            if ($result && mysqli_num_rows($result) > 0) {
                mysqli_data_seek($result, 0); 
                while ($servico = mysqli_fetch_assoc($result)) {
                    echo "
                    <div class='linha-botoes'>
                        <a href='#' class='proximo-status hover' data-id='{$servico['id_servicos']}'>Próximo Status</a>
                        <a href='#' class='lixeira' data-id='{$servico['id_servicos']}'>
                            <img src='img/lixeira.png' alt='Excluir' class='icone-lixeira'>
                        </a>
                    </div>
                    ";
                }
            }
            ?>
        </div>
    </div>

    <div id="popup-excluir" class="popup-container">
        <div class="popup-content">
            <div class="cabecalho-popup">
                <img src="img/x.png" alt="botao fechar" class="btn-fechar hover" id="btn-fechar">
            </div>
            <p>Tem certeza que deseja excluir <br> o serviço?</p>
            <div class="popup-botoes">
                <button id="btn-nao" class="btn-popup vermelho hover">Não</button>
                <button id="btn-sim" class="btn-popup verde hover">Sim</button>
            </div>
        </div>
    </div>

    <div id="popup-proximo-status" class="popup-container">
        <div class="popup-content">
            <div class="cabecalho-popup">
                <img src="img/x.png" alt="botao fechar" class="btn-fechar hover" id="btn-fechar-proximo-status">
            </div>
            <p>Tem certeza que deseja alterar o status <br> do serviço?</p>
            <div class="popup-botoes">
                <button id="btn-nao-proximo-status" class="btn-popup vermelho hover">Não</button>
                <button id="btn-sim-proximo-status" class="btn-popup verde hover">Sim</button>
            </div>
        </div>
    </div>

    <script src="popup-Monitoramento.js"></script>

</body>
</html>