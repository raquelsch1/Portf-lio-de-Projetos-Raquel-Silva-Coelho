package com.mycompany.telalogin;

import com.mycompany.telalogin.UsuarioPad;
import com.mycompany.telalogin.Veiculo;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import com.mycompany.telalogin.Cliente;
import com.mycompany.telalogin.ServiçoAgendado;




public class SistemaLavaJato {

    private static SistemaLavaJato instancia = null;
    
    private List<ServiçoAgendado> serviçosAgendados = new ArrayList<>();
    private List<Cliente> cliente = new ArrayList<>();
    private Map<String, String> credenciais = new HashMap<>();
    private List<UsuarioPad> usuariosPadroes = new ArrayList<>();
    private List<String> horarios = new ArrayList<>();
    private Cliente clienteAutenticado;
    
    public SistemaLavaJato() {
        inicializarUsuariosPadroes();
        preencherHorarios();
    }

    public static SistemaLavaJato getInstance() {
        if (instancia == null) {
            instancia = new SistemaLavaJato();
        }
        return instancia;
    }
    
    private void preencherHorarios() {
        for (int hora = 8; hora <= 12; hora++) {
            horarios.add(String.format("%02d:00", hora));
            horarios.add(String.format("%02d:30", hora));
        }

        for (int hora = 14; hora <= 19; hora++) {
            horarios.add(String.format("%02d:00", hora));
            horarios.add(String.format("%02d:30", hora));
        }
    }

    public List<String> obterHorarios() { // pra gerar a lista de horários 
        return new ArrayList<>(horarios);
    }

    private void inicializarUsuariosPadroes() {
        usuariosPadroes = UsuarioPad.getUsuariosPadroes();
    }

    private void addCredenciaisUsuarioPad() { // pra colocar as credenciais do usuario padrão no mapa de todas as credenciais
        for (UsuarioPad usuarioPadrao : usuariosPadroes) {
            credenciais.put(usuarioPadrao.getLogin(), usuarioPadrao.getSenha());
        }
    }
    
    public boolean cadastrarCliente(Cliente novoCliente) { // cadastra clientes no sistema
        for (Cliente clienteExistente : cliente) {
            if (clienteExistente.getCpf().equals(novoCliente.getCpf())) {
                return false; // pode usar esse return falso pra imprimir mensagem de que esse cpf já ta cadastrado
            }
        }
        cliente.add(novoCliente);
        if (novoCliente.getCredenciais() != null) {
            credenciais.put(novoCliente.getCredenciais().getLogin(), novoCliente.getCredenciais().getSenha());
        }
        return true;
    } 

    public Cliente buscaCliente(String login, String senha) { // função que gera o cliente autenticado pra usar no agendamento de serviços
        for (Cliente clienteExistente : cliente) {
            if (clienteExistente.getCredenciais() != null
                    && clienteExistente.getCredenciais().getLogin().equals(login) && clienteExistente.getCredenciais().getSenha().equals(senha)) {
                return clienteExistente;
            }
        }
        return null;
    }

    public Cliente getClienteAutenticado() {
        return clienteAutenticado;
    }

    public void setClienteAutenticado(Cliente clienteAutenticado) {
        this.clienteAutenticado = clienteAutenticado;
    }

    
    public boolean autenticar(String login, String senha) { // função de autenticar o login
       boolean autenticado = credenciais.containsKey(login) && credenciais.get(login).equals(senha);
       if(autenticado){
           clienteAutenticado = buscaCliente(login, senha);
       }
       return autenticado;    
    }

    public int verificarTipoUsuario(String login, String senha) { 
        for (UsuarioPad usuarioPadrao : usuariosPadroes) {
            if (usuarioPadrao.getLogin().equals(login) && usuarioPadrao.getSenha().equals(senha)) {
                return 1;
            }
        }
        for (Cliente clienteExistente : cliente) {
            if (clienteExistente.getCredenciais() != null
                    && clienteExistente.getCredenciais().getLogin().equals(login)
                    && clienteExistente.getCredenciais().getSenha().equals(senha)) {
                return 2;
            }
        }
        return 0;
    }

    public boolean agendarServiço(ServiçoAgendado serviço) { // função de gerar os serviços
        for (ServiçoAgendado agendado : serviçosAgendados) {
            if (agendado.getVeiculo().equals(serviço.getVeiculo()) && agendado.getHorario().equals(serviço.getHorario())) {
                return false; // pode usar esse return false pra gerar uma mensagem de "já cadastrou este serviço"
            }
        }
        serviçosAgendados.add(serviço);
        return true;
    }

    public List<String> gerarRelatorioClientes() {
        List<String> relatorio = new ArrayList<>();

        if (cliente.isEmpty()) {
            relatorio.add("Nenhum cliente encontrado.");
        } else {
            for (Cliente cadastro : cliente) {
                relatorio.add("\nNome: " + cadastro.getNome());
                relatorio.add("CPF: " + cadastro.getCpf());
                relatorio.add("Endereço: " + cadastro.getEndereço());
                relatorio.add("Data de Nascimento: " + cadastro.getDataNasc());
                relatorio.add("Veículos:");

                List<Veiculo> veiculos = cadastro.getVeiculo();
                for (Veiculo veiculo : veiculos) {
                    relatorio.add("  - Placa: " + veiculo.getPlaca());
                    relatorio.add("    Modelo: " + veiculo.getModelo());
                    relatorio.add("    Tipo: " + veiculo.getTipo());
                    relatorio.add("------------------------");
                    relatorio.add("");
                }
                relatorio.add("");
            }
        }
        return relatorio;
    }

    public List<String> gerarRelatorioAgendamentos() {
        List<String> listaAgendamentos = new ArrayList<>();
        if (serviçosAgendados.isEmpty()) {
            listaAgendamentos.add("Nenhum agendamento encontrado.");
        } else {
            for (ServiçoAgendado agendamento : serviçosAgendados) {
                listaAgendamentos.add("Cliente: " + agendamento.getVeiculo().getProprietario().getNome());
                listaAgendamentos.add("Veículo: " + agendamento.getVeiculo().getPlaca());
                listaAgendamentos.add("Serviço: " + agendamento.getServiçoAgend().getNome() + " - R$" + agendamento.getServiçoAgend().getValor());
                listaAgendamentos.add("Data: " + agendamento.getData());
                listaAgendamentos.add("Horário: " + agendamento.getHorario());
                listaAgendamentos.add("------------------------");
            }
            listaAgendamentos.add("");
        }
        return listaAgendamentos;
    }

    public List<String> gerarRelatorioUsuarioPad() {
        List<String> listaUsuariosPadroes = new ArrayList<>();
        for (UsuarioPad usuarioPadrao : usuariosPadroes) {
            listaUsuariosPadroes.add("CPF: " + usuarioPadrao.getCpf());
            listaUsuariosPadroes.add("Login: " + usuarioPadrao.getLogin());
            listaUsuariosPadroes.add("Senha: " + usuarioPadrao.getSenha());
            listaUsuariosPadroes.add("------------------------");
        }

        return listaUsuariosPadroes;
    }

    public List<String> gerarRelatorioCliAgend(Cliente clienteAutenticado) {
        List<String> relatorio = new ArrayList<>();

        if (serviçosAgendados.isEmpty()) {
            relatorio.add("Nenhum agendamento encontrado para o cliente.");
        } else {
            for (ServiçoAgendado agendamento : serviçosAgendados) {
                if (agendamento.getVeiculo().getProprietario().equals(clienteAutenticado)) {

                    String placaVeiculo = agendamento.getVeiculo().getPlaca();
                    String modeloVeiculo = agendamento.getVeiculo().getModelo();
                    String tipoVeiculo = agendamento.getVeiculo().getTipo();

                    String nomeServiço = agendamento.getServiçoAgend().getNome();
                    double valorServiço = agendamento.getServiçoAgend().getValor();
                    String horarioAgendamento = agendamento.getHorario();
                    String dataAgendamento = agendamento.getData();

                    relatorio.add("Cliente: " + clienteAutenticado.getNome());
                    relatorio.add("Veículo: " + placaVeiculo + " (" + modeloVeiculo + " - " + tipoVeiculo + ")");
                    relatorio.add("Serviço: " + nomeServiço + " - R$" + valorServiço);
                    relatorio.add("Data: " + dataAgendamento);
                    relatorio.add("Horário: " + horarioAgendamento);
                    relatorio.add("------------------------");
                }
            }
            relatorio.add("");
        }
        return relatorio;
    }

}
