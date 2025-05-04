# Nome do arquivo fonte
SOURCE="*.c"

# Nome do executável gerado
OUTPUT="main"

# Remove resíduos
if [ -e /$OUTPUT ]; then
	rm $OUTPUT
fi

# Compilar o arquivo C com opções padrão
gcc -g -o $OUTPUT $SOURCE

# Verificar se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    echo "Compilação bem-sucedida. Executável criado: $OUTPUT"
else
    echo "Erro na compilação."
fi 
