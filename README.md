# Practica-1-Sistemas-Operativos-I
## Miembros: Jose Luque Mut, Krishna Jorda Jimenez, Valentino Coppola Ferrari

## Mejoras realizadas
- Implementado una versión más avanzada del internal_cd, si bien no a la par de la sintaxis de bash.
## Restricciones del programa
- No es posible usar el tabulador para autocompletar comandos
- No es posible usar las teclas UP y DOWN para alternar entre comandos previos
- No es posible usar las teclas LEFT y RIGHT para editar la línea del comando siendo introducido
- No es posible usar la combinación de teclas CTRL+L para limpiar la terminal
- No es posible usar la combinación de teclas CTRL+R para buscar comandos previos
## Sintaxis especifica
- Al indicar una ruta con ```cd``` (internal_cd), es posible indicar el nombre del directorio entre comillas (por si contiene espacios). Sin embargo, no es posible utilizar una combinación de comillas y no comillas (por ejemplo ```cd /Desktop/"Nombre carpeta"``` no sería válido)
- Al indicar una ruta con espacios también es posible utilizar ```\``` para denotar que el espacio que le sigue es parte de la ruta (y no otro argumento del comando). Sin embargo, escribir ```\\``` no se procesa como ```\``` como en bash.
