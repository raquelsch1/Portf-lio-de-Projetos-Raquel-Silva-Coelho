<?php
    include 'php_actions/connect_db.php';

    if (isset($_GET['id'])) {
        $id = $_GET['id'];

        $query = "SELECT stattus FROM servicos WHERE id_servicos = $id";
        $result = mysqli_query($conn, $query);

        if ($result && mysqli_num_rows($result) > 0) {
            $servico = mysqli_fetch_assoc($result);
            $statusAtual = $servico['stattus'];

            if ($statusAtual == 'Pendente') {
                $novoStatus = 'Em Andamento';
            } elseif ($statusAtual == 'Em Andamento') {
                $novoStatus = 'Concluído';
            } else {
                $novoStatus = $statusAtual; 
            }

            $query = "UPDATE servicos SET stattus = '$novoStatus' WHERE id_servicos = $id";
            mysqli_query($conn, $query);
        } 
    }

    mysqli_close($conn);

    if (isset($_SERVER['HTTP_REFERER'])) {
        header("Location: " . $_SERVER['HTTP_REFERER']);
    } else {
        header("Location: mostrarTodos.php");
    }
    exit();
?>