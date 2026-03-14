package application.service;

import application.port.in.RegistrarProntuarioUseCase;
import application.port.in.dto.RegistrarProntuarioCommand;
import application.port.in.dto.PrescricaoDTO;
import application.port.out.*;
import domain.model.*;

import java.util.List;

public class RegistrarProntuarioService implements RegistrarProntuarioUseCase {

    private final ConsultaRepository consultaRepository;
    private final ProntuarioRepository prontuarioRepository;
    private final MedicamentoRepository medicamentoRepository;
    private final ExameRepository exameRepository;

    public RegistrarProntuarioService(
            ConsultaRepository consultaRepository,
            ProntuarioRepository prontuarioRepository,
            MedicamentoRepository medicamentoRepository,
            ExameRepository exameRepository) {

        this.consultaRepository = consultaRepository;
        this.prontuarioRepository = prontuarioRepository;
        this.medicamentoRepository = medicamentoRepository;
        this.exameRepository = exameRepository;
    }

    @Override
    public void executar(RegistrarProntuarioCommand command) {

        if (command == null) {
            throw new IllegalArgumentException("Comando não pode ser nulo");
        }

        Consulta consulta =
                consultaRepository.buscarPorId(command.getConsultaId());

        if (consulta == null) {
            throw new IllegalArgumentException("Consulta não encontrada");
        }

        Prontuario prontuario = consulta.gerarProntuario(0,
                command.getPeso(),
                command.getAltura(),
                command.getSintomas(),
                command.getObservacaoClinica()
        );

        List<PrescricaoDTO> prescricoes =
                command.getPrescricoes() != null
                        ? command.getPrescricoes()
                        : List.of();

        for (PrescricaoDTO dto : prescricoes) {

            Medicamento medicamento =
                    medicamentoRepository.buscarPorId(dto.getMedicamentoId());

            if (medicamento == null) {
                throw new IllegalArgumentException(
                        "Medicamento não encontrado: " + dto.getMedicamentoId()
                );
            }

            Prescricao prescricao = new Prescricao(
                    dto.getIdPrescricao(),
                    medicamento,
                    dto.getDosagem(),
                    dto.getAdministracao(),
                    dto.getTempoUso()
            );

            prontuario.adicionarPrescricao(prescricao);
        }

        List<Integer> examesIds =
                command.getExamesIds() != null
                        ? command.getExamesIds()
                        : List.of();

        for (Integer exameId : examesIds) {
            Exame exame = exameRepository.buscarPorId(exameId);

            if (exame == null) {
                throw new IllegalArgumentException(
                        "Exame não encontrado: " + exameId
                );
            }

            prontuario.adicionarExame(exame);
        }

        prontuarioRepository.salvar(prontuario);
        consultaRepository.salvar(consulta);
    }
}
