# 3D Terminal

Um visualizador interativo de modelos 3D (.obj) em tempo real no terminal usando a biblioteca ncurses. O programa renderiza objetos 3D diretamente no terminal e pode capturar frames para gerar vídeos.

## Características

- **Renderização 3D em Terminal**: Visualize modelos 3D complexos diretamente no seu terminal
- **Suporte a arquivos OBJ**: Carregue qualquer arquivo de modelo 3D no formato .obj
- **Captura de Frames**: Gera imagens PPM dos frames renderizados
- **Geração de Vídeo**: Converta os frames capturados em vídeo MP4 usando FFmpeg
- **Reprodução de Vídeo**: Reproduza os vídeos gerados diretamente com mpv

## Requisitos

- **Linux** (ou WSL no Windows)
- **GCC/G++** (compilador C++)
- **ncurses-dev** (biblioteca de interface em terminal)
- **FFmpeg** (para conversão de imagens em vídeo)
- **mpv** (player de vídeo)

## Instalação de Dependências

Para instalar todas as dependências necessárias automaticamente:

```bash
make dependencies
```

Este comando instalará:
- build-essential
- libncurses-dev
- ffmpeg
- mpv

## Compilação

Para compilar o projeto:

```bash
make build
```

Ou simplesmente:

```bash
make
```

Isso gerará o executável `3D` na raiz do projeto.

## Uso

### Opção 1: Pipeline Completo (Recomendado)

Execute o pipeline completo que compila, executa, converte e reproduz:

```bash
make pipeline OBJ_FILE=caminho/para/seu/modelo.obj
```

### Opção 2: Passo a Passo

1. **Compilar o projeto:**
   ```bash
   make build
   ```

2. **Executar o programa (gera frames PPM):**
   ```bash
   make run OBJ_FILE=caminho/para/seu/modelo.obj
   ```

3. **Converter frames em vídeo MP4:**
   ```bash
   make convert
   ```

4. **Reproduzir o vídeo:**
   ```bash
   make watch
   ```

### Opção 3: Execução Manual

```bash
# Compilar
g++ -O3 ./main.cpp -o 3D -lncurses

# Executar (gera frames)
./3D caminho/para/seu/modelo.obj

# Converter para vídeo (dentro do diretório output)
cd output
ffmpeg -i %d.ppm -r 60 ../output.mp4
cd ..

# Reproduzir
mpv ./output.mp4
```

## IMPORTANTE

### Diminua o ZOOM do Terminal ao MÁXIMO!

A qualidade e performance da visualização dependem muito do tamanho dos caracteres no terminal.

**Ações recomendadas:**
1. **Minimize o zoom do terminal** ao máximo (pressione `Ctrl + -` várias vezes)
2. Ou aumente significativamente o tamanho da janela do terminal
3. A resolução ideal é um terminal com **muitos caracteres** visíveis

> Quanto menor os caracteres (maior a resolução terminal), melhor a qualidade da renderização 3D.

## Estrutura do Projeto

```
3D-terminal/
├── main.cpp          # Código-fonte principal
├── Makefile          # Automação de compilação e pipeline
├── README.md         # Este arquivo
├── LICENSE           # Licença do projeto
├── 3D                # Executável compilado (gerado)
├── output/           # Diretório de saída de frames PPM
│   └── *.ppm        # Frames capturados durante execução
├── output_video/     # Diretório para vídeos gerados
└── output.mp4        # Vídeo final (gerado)
```

## Controles Durante Execução

O programa renderiza o modelo em tempo real. Os frames são capturados automaticamente durante a execução.

## Parâmetros Técnicos

- **Taxa de frames**: 60 FPS (configurável no código)
- **Formato de saída**: PPM (Portable Pixmap)
- **Codec de vídeo**: H.264 (padrão do FFmpeg)
- **Resolução**: Depende do tamanho do terminal

## Targets do Makefile

| Target | Descrição |
|--------|-----------|
| `make help` | Exibe todos os targets disponíveis |
| `make dependencies` | Instala as dependências do sistema |
| `make build` | Compila o programa |
| `make run OBJ_FILE=...` | Executa o programa e gera frames |
| `make convert` | Converte PPM frames em MP4 |
| `make watch` | Reproduz o vídeo gerado |
| `make pipeline OBJ_FILE=...` | Executa pipeline completo |
| `make clean` | Remove executável e vídeo |
| `make clean-all` | Remove tudo incluindo frames PPM |

## Exemplo de Uso Completo

```bash
# 1. Instalar dependências (primeira vez)
make dependencies

# 2. Compilar
make build

# 3. Executar a pipeline completa com um modelo
make pipeline OBJ_FILE=seu_modelo_3d.obj

# O programa irá:
# - Renderizar o modelo no terminal
# - Capturar frames em output/
# - Converter para output.mp4
# - Reproduzir automaticamente com mpv
```

## Troubleshooting

### "Error: OBJ_FILE parameter is required"
- Certifique-se de passar o arquivo .obj: `make run OBJ_FILE=seu_arquivo.obj`

### "No PPM files found in output/"
- Execute primeiro `make run OBJ_FILE=seu_arquivo.obj` para gerar os frames

### "output.mp4 not found"
- Execute primeiro `make convert` após gerar os frames

### Renderização muito pixelada
- **DIMINUA O ZOOM DO TERMINAL** - Este é o fator mais importante!
- Maximize a resolução do terminal em caracteres

### FFmpeg error durante conversão
- Certifique-se de que FFmpeg está instalado: `make dependencies`
- Verifique se há espaço em disco disponível

## Licença

Veja o arquivo [LICENSE](LICENSE) para detalhes.
