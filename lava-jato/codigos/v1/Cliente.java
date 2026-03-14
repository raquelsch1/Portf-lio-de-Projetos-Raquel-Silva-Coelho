package com.mycompany.telalogin;
import com.mycompany.telalogin.Usuario;
import com.mycompany.telalogin.Veiculo;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author RAQUEL
 */
public class Cliente {
    private String nome;
    private String cpf;
    private String endereço;
    private String dataNasc;
    private List<Veiculo> veiculo;
    private Usuario credenciais;
    private String senha;

    public Cliente() {
    }

    public Cliente(String nome, String cpf, String endereço, String dataNasc, Usuario credenciais) {
        this.nome = nome;
        this.cpf = cpf;
        this.endereço = endereço;
        this.dataNasc = dataNasc;
        this.veiculo = new ArrayList<>();
        this.credenciais = credenciais;
    }


     public void adicionarVeiculo(Veiculo novoVeiculo) { // pra adicionar veículos durante ou após o cadastro 
        this.veiculo.add(novoVeiculo);
        novoVeiculo.setProprietario(this);
    }
     
    public List<String> obterPlacas() {
        List<String> placas = new ArrayList<>(); // pra usar quando for gerar os carros disponíveis de um cliente
        for (Veiculo veiculo : veiculo) {
            placas.add(veiculo.getPlaca());
        }
        return placas;
    }
    
    public Veiculo buscarVeiculo(String placa) { // com o retorno do veiculo escolhido, usar para selecinar o objeto veículo
        for (Veiculo veiculo : veiculo) {
            if (veiculo.getPlaca().equals(placa)) { 
                return veiculo;
            }
        }
        return null;
    }
     
    public String getEndereço() {
        return endereço;
    }


    public void setEndereço(String endereço) {
        this.endereço = endereço;
    }

    public String getDataNasc() {
        return dataNasc;
    }

    public void setDataNasc(String dataNasc) {
        this.dataNasc = dataNasc;
    }

    public Usuario getCredenciais() {
        return credenciais;
    }

    public void setCredenciais(Usuario credenciais) {
        this.credenciais = credenciais;
    }


    public List<Veiculo> getVeiculo() {
        return veiculo;
    }

    public void setVeiculo(List<Veiculo> veiculo) {
        this.veiculo = veiculo;
    }
    
    public String getNome() {
        return nome;
    }

    public void setNome(String nome) {
        this.nome = nome;
    }

    public String getCpf() {
        return cpf;
    }

    public void setCpf(String cpf) {
        this.cpf = cpf;
    }
    
}
