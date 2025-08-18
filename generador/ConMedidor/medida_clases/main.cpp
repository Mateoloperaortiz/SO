#include <iostream>
#include <vector>
#include <limits>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iomanip>
#include "persona.h"
#include "generador.h"
#include "monitor.h"

/**
 * Muestra el menú principal de la aplicación.
 * 
 * POR QUÉ: Guiar al usuario a través de las funcionalidades disponibles.
 * CÓMO: Imprimiendo las opciones en consola.
 * PARA QUÉ: Interacción amigable con el usuario.
 */
void mostrarMenu() {
    std::cout << "\n\n=== MENÚ PRINCIPAL ===";
    std::cout << "\n0. Crear nuevo conjunto de datos";
    std::cout << "\n1. Mostrar resumen de todas las personas";
    std::cout << "\n2. Mostrar detalle completo por índice";
    std::cout << "\n3. Buscar persona por ID";
    std::cout << "\n4. Análisis: persona más longeva (global y por ciudad)";
    std::cout << "\n5. Análisis: mayor patrimonio (global/ciudad/grupo)";
    std::cout << "\n6. Declarantes por calendario: listar y contar + validar";
    std::cout << "\n7. Mostrar estadísticas de rendimiento";
    std::cout << "\n8. Exportar estadísticas a CSV";
    std::cout << "\n9. Salir";
    std::cout << "\nSeleccione una opción: ";
}

/**
 * Punto de entrada principal del programa.
 * 
 * POR QUÉ: Iniciar la aplicación y manejar el flujo principal.
 * CÓMO: Mediante un bucle que muestra el menú y procesa la opción seleccionada.
 * PARA QUÉ: Ejecutar las funcionalidades del sistema.
 */
int main() {
    srand(time(nullptr)); // Semilla para generación aleatoria
    
    // Puntero inteligente para gestionar la colección de personas
    // POR QUÉ: Evitar fugas de memoria y garantizar liberación automática.
    std::unique_ptr<std::vector<Persona>> personas = nullptr;
    
    Monitor monitor; // Monitor para medir rendimiento
    
    int opcion;
    do {
        mostrarMenu();
        std::cin >> opcion;
        
        // Variables locales para uso en los casos
        size_t tam = 0;
        int indice;
        std::string idBusqueda;
        
        // Iniciar medición de tiempo y memoria para la operación actual
        monitor.iniciar_tiempo();
        long memoria_inicio = monitor.obtener_memoria();
        
        switch(opcion) {
            case 0: { // Crear nuevo conjunto de datos
                int n;
                std::cout << "\nIngrese el número de personas a generar: ";
                std::cin >> n;
                
                if (n <= 0) {
                    std::cout << "Error: Debe generar al menos 1 persona\n";
                    break;
                }
                
                // Generar el nuevo conjunto de personas
                auto nuevasPersonas = generarColeccion(n);
                tam = nuevasPersonas.size();
                
                // Mover el conjunto al puntero inteligente (propiedad única)
                personas = std::make_unique<std::vector<Persona>>(std::move(nuevasPersonas));
                
                // Medir tiempo y memoria usada
                double tiempo_gen = monitor.detener_tiempo();
                long memoria_gen = monitor.obtener_memoria() - memoria_inicio;
                
                std::cout << "Generadas " << tam << " personas en " 
                          << tiempo_gen << " ms, Memoria: " << memoria_gen << " KB\n";
                
                // Registrar la operación
                monitor.registrar("Crear datos", tiempo_gen, memoria_gen);
                break;
            }
                
            case 1: { // Mostrar resumen de todas las personas
                if (!personas || personas->empty()) {
                    std::cout << "\nNo hay datos disponibles. Use opción 0 primero.\n";
                    break;
                }
                
                tam = personas->size();
                std::cout << "\n=== RESUMEN DE PERSONAS (" << tam << ") ===\n";
                for(size_t i = 0; i < tam; ++i) {
                    std::cout << i << ". ";
                    (*personas)[i].mostrarResumen();
                    std::cout << "\n";
                }
                
                double tiempo_mostrar = monitor.detener_tiempo();
                long memoria_mostrar = monitor.obtener_memoria() - memoria_inicio;
                monitor.registrar("Mostrar resumen", tiempo_mostrar, memoria_mostrar);
                break;
            }
                
            case 2: { // Mostrar detalle por índice
                if (!personas || personas->empty()) {
                    std::cout << "\nNo hay datos disponibles. Use opción 0 primero.\n";
                    break;
                }
                
                tam = personas->size();
                std::cout << "\nIngrese el índice (0-" << tam-1 << "): ";
                if(std::cin >> indice) {
                    if(indice >= 0 && static_cast<size_t>(indice) < tam) {
                        (*personas)[indice].mostrar();
                    } else {
                        std::cout << "Índice fuera de rango!\n";
                    }
                } else {
                    std::cout << "Entrada inválida!\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                
                double tiempo_detalle = monitor.detener_tiempo();
                long memoria_detalle = monitor.obtener_memoria() - memoria_inicio;
                monitor.registrar("Mostrar detalle", tiempo_detalle, memoria_detalle);
                break;
            }
                
            case 3: { // Buscar por ID
                if (!personas || personas->empty()) {
                    std::cout << "\nNo hay datos disponibles. Use opción 0 primero.\n";
                    break;
                }
                
                std::cout << "\nIngrese el ID a buscar: ";
                std::cin >> idBusqueda;
                
                if(const Persona* encontrada = buscarPorID(*personas, idBusqueda)) {
                    encontrada->mostrar();
                } else {
                    std::cout << "No se encontró persona con ID " << idBusqueda << "\n";
                }
                
                double tiempo_busqueda = monitor.detener_tiempo();
                long memoria_busqueda = monitor.obtener_memoria() - memoria_inicio;
                monitor.registrar("Buscar por ID", tiempo_busqueda, memoria_busqueda);
                break;
            }
                
            case 4: { // Análisis: más longeva (global/ciudad)
                if (!personas || personas->empty()) {
                    std::cout << "\nNo hay datos disponibles. Use opción 0 primero.\n";
                    break;
                }
                std::cout << "\n=== Persona más longeva (Global) ===\n";
                if (const Persona* p = personaMasLongeva(*personas)) {
                    int edad = calcularEdad(p->getFechaNacimiento());
                    std::cout << "ID " << p->getId() << ": " << p->getNombre() << " " << p->getApellido()
                              << " | Edad: " << edad << " | Ciudad: " << p->getCiudadNacimiento() << "\n";
                } else {
                    std::cout << "Sin datos válidos de edad.\n";
                }

                std::cout << "\n=== Persona más longeva por ciudad ===\n";
                {
                    auto mapa = personaMasLongevaPorCiudad(*personas);
                    for (const auto& kv : mapa) {
                        const std::string& ciudad = kv.first;
                        const Persona* p = kv.second;
                        int edad = calcularEdad(p->getFechaNacimiento());
                        std::cout << ciudad << ": ID " << p->getId() << " - " << p->getNombre() << " " << p->getApellido()
                                  << " | Edad: " << edad << "\n";
                    }
                }

                {
                    double t = monitor.detener_tiempo();
                    long m = monitor.obtener_memoria() - memoria_inicio;
                    monitor.registrar("Análisis longevidad", t, m);
                }
                break;
            }

            case 5: { // Análisis: mayor patrimonio (global/ciudad/grupo)
                if (!personas || personas->empty()) {
                    std::cout << "\nNo hay datos disponibles. Use opción 0 primero.\n";
                    break;
                }
                std::cout << "\n=== Mayor patrimonio (Global) ===\n";
                if (const Persona* p = mayorPatrimonioGlobal(*personas)) {
                    std::cout << "ID " << p->getId() << ": " << p->getNombre() << " " << p->getApellido()
                              << " | Patrimonio: $" << std::fixed << std::setprecision(2) << p->getPatrimonio()
                              << " | Ciudad: " << p->getCiudadNacimiento()
                              << " | Grupo: " << grupoCalendarioPorID(p->getId()) << "\n";
                }

                std::cout << "\n=== Mayor patrimonio por ciudad ===\n";
                {
                    auto mapaC = mayorPatrimonioPorCiudad(*personas);
                    for (const auto& kv : mapaC) {
                        const std::string& ciudad = kv.first;
                        const Persona* p = kv.second;
                        std::cout << ciudad << ": ID " << p->getId() << " - " << p->getNombre() << " " << p->getApellido()
                                  << " | Patrimonio: $" << std::fixed << std::setprecision(2) << p->getPatrimonio()
                                  << " | Grupo: " << grupoCalendarioPorID(p->getId()) << "\n";
                    }
                }

                std::cout << "\n=== Mayor patrimonio por grupo (A/B/C) ===\n";
                {
                    auto mapaG = mayorPatrimonioPorGrupo(*personas);
                    for (char g : std::string("ABC")) {
                        auto it = mapaG.find(g);
                        if (it != mapaG.end() && it->second) {
                            const Persona* p = it->second;
                            std::cout << "Grupo " << g << ": ID " << p->getId() << " - " << p->getNombre() << " " << p->getApellido()
                                      << " | Patrimonio: $" << std::fixed << std::setprecision(2) << p->getPatrimonio() << "\n";
                        } else {
                            std::cout << "Grupo " << g << ": (sin datos)\n";
                        }
                    }
                }

                {
                    double t = monitor.detener_tiempo();
                    long m = monitor.obtener_memoria() - memoria_inicio;
                    monitor.registrar("Análisis patrimonio", t, m);
                }
                break;
            }

            case 6: { // Declarantes por calendario: listar/contar + validar
                if (!personas || personas->empty()) {
                    std::cout << "\nNo hay datos disponibles. Use opción 0 primero.\n";
                    break;
                }
                std::cout << "\n=== Conteo por grupo (todos) ===\n";
                auto cnt = conteoPorGrupo(*personas);
                std::cout << "A: " << cnt['A'] << ", B: " << cnt['B'] << ", C: " << cnt['C'] << "\n";

                std::cout << "\n=== Conteo de declarantes por grupo ===\n";
                auto cntD = conteoDeclarantesPorGrupo(*personas);
                std::cout << "A: " << cntD['A'] << ", B: " << cntD['B'] << ", C: " << cntD['C'] << "\n";

                std::cout << "\n=== Listado de declarantes por grupo ===\n";
                auto listas = listarDeclarantesPorGrupo(*personas);
                for (char g : std::string("ABC")) {
                    std::cout << "Grupo " << g << " (" << listas[g].size() << "):\n";
                    for (const auto* p : listas[g]) {
                        std::cout << "  - [" << p->getId() << "] " << p->getNombre() << " " << p->getApellido() << "\n";
                    }
                }

                size_t invalidos = conteoInvalidosGrupo(*personas);
                std::cout << "\nIDs inválidos por calendario: " << invalidos << "\n";

                {
                    double t = monitor.detener_tiempo();
                    long m = monitor.obtener_memoria() - memoria_inicio;
                    monitor.registrar("Declarantes por grupo", t, m);
                }
                break;
            }

            case 7: // Mostrar estadísticas de rendimiento
                monitor.mostrar_resumen();
                break;
                
            case 8: // Exportar estadísticas a CSV
                monitor.exportar_csv();
                break;
                
            case 9: // Salir
                std::cout << "Saliendo...\n";
                break;
                
            default:
                std::cout << "Opción inválida!\n";
        }
        
        // Mostrar estadísticas de la operación (opciones principales)
        if (opcion >= 0 && opcion <= 6) {
            double tiempo = monitor.detener_tiempo();
            long memoria = monitor.obtener_memoria() - memoria_inicio;
            monitor.mostrar_estadistica("Opción " + std::to_string(opcion), tiempo, memoria);
        }
        
    } while(opcion != 9);
    
    return 0;
}