## Nota: Acesso ao Sistema
Para testar as funcionalidades de login, utilize o token padrão:
- **TOKEN:** 123456

# Pixeon Medical System - Desafio Técnico

Este projeto é uma estação de trabalho médica (Medical Workstation) de alta performance, desenvolvida em **C++17** e **Qt 6**, focada no fluxo clínico de diagnóstico por imagem (PACS/RIS).

## Requisitos do Desafio
Estação de trabalho para visualização e manipulação de imagens médicas.

- **C++ Moderno (17):** Utilização de padrões robustos da linguagem e STL.
- **Interface Gráfica (Qt 6):** UI responsiva baseada em Qt Widgets.
- **Suporte a Formatos:** Compatibilidade com arquivos **BMP, PNG e JPEG**.
- **Diálogo de Arquivos:** Integração com `QFileDialog` para seleção e importação de exames.
- **Controles de Imagem:** Implementação de `QSlider` para ajuste de Brilho e Contraste.
- **Estados da UI:** Controles habilitados dinamicamente apenas após a carga da imagem.
- **Funcionalidades:** 
	- **Multi-imagem:** Listagem lateral para seleção e alternância entre múltiplos exames.
	- **Interação via Mouse:**
		- Arrastar horizontal: Ajuste de Brilho.
		- Arrastar vertical: Ajuste de Contraste.
		- Sincronização em tempo real com os Sliders.
	- **Ferramentas de Manipulação:**
		- **Zoom:** Escala fluida para análise de detalhes.
		- **Pan:** Deslocamento da imagem para navegação em zoom.

- **Arquitetura Multithread:** *Worker Pattern* para processamento de imagem e I/O assíncrono, garantindo fluidez na UI.
- **Qualidade de Software:** Suíte de **9 testes unitários e de integração** automatizados via NMake.
- **Persistência:** Gestão de dados em JSON com estrutura de storage organizada por ID de paciente.

## Arquitetura Técnica
O sistema utiliza o padrão **MVC (Model-View-Controller)**:
- **Model:** Gerencia o estado dos pacientes e exames via `GlobalState`.
- **Worker Thread:** Realiza cálculos de pixels e escrita em disco sem bloquear a interface principal.
- **Logging:** Utilização da biblioteca proprietária [AppLogger](https://github.com) (desenvolvida pelo autor) para registro de eventos críticos e auditoria.

## Building

cmd
mkdir build && cd build

"D:\Apps\Qt\Tools\CMake_64\bin\cmake.exe" -G "NMake Makefiles" -DCMAKE_CXX_COMPILER="C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.50.35717/bin/Hostx64/x64/cl.exe" -DCMAKE_PREFIX_PATH="D:/Apps/Qt/6.5.3/msvc2019_64" ..

nmake

Deploy e Execução

"D:\Apps\Qt\6.5.3\msvc2019_64\bin\windeployqt.exe" PixeonChallenge.exe

# Para rodar os testes:
tests\PixeonTests.exe

# Para rodar a aplicação:
PixeonChallenge.exe
