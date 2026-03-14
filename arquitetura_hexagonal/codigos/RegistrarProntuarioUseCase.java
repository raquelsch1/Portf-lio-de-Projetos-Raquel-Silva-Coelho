package application.port.in;

import application.port.in.dto.RegistrarProntuarioCommand;

public interface RegistrarProntuarioUseCase {
    void executar(RegistrarProntuarioCommand command);
}
