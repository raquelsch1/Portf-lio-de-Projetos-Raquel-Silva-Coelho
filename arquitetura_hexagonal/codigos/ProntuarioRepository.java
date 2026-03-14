package application.port.out;

import domain.model.Prontuario;
import java.util.List;

public interface ProntuarioRepository {

    void salvar(Prontuario prontuario);

    List<Prontuario> buscarPorPaciente(int pacienteId);
}
