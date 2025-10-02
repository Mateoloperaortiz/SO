# Sistema de Chat con Colas de Mensajes

Implementación de un chat multi-sala usando colas de mensajes de System V.

## Compilación

```bash
gcc servidor.c -o servidor -lpthread
gcc cliente.c -o cliente -lpthread
```

## Ejecución

1. Inicia el servidor en una terminal:
   ```bash
   ./servidor
   ```
2. En otras terminales, ejecuta clientes indicando el nombre de usuario:
   ```bash
   ./cliente Maria
   ./cliente Juan
   ```

## Comandos del Cliente

- `/join <sala>`: Unirse o crear una sala.
- `/leave`: Salir de la sala actual.
- `/list`: Mostrar las salas disponibles.
- `/users`: Mostrar los usuarios de la sala actual.
- `/quit`: Cerrar sesión y salir.
- `/help`: Ver ayuda.
- Cualquier otro texto se envía como mensaje a la sala.

## Notas Técnicas

- El servidor usa una cola global para control y crea una cola por cada sala.
- Los clientes envían mensajes a la cola de su sala y reciben difusiones filtrando por su PID.
- El servidor gestiona la lista de salas y los usuarios inscritos en cada una.
