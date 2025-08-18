#ifndef GENERADOR_H
#define GENERADOR_H

#include "persona.h"
#include <vector>
#include <unordered_map>

// --- Funciones para generación de datos aleatorios ---

// Genera fecha aleatoria entre 1960-2010
std::string generarFechaNacimiento();

// Genera ID único secuencial
std::string generarID();

// Genera número decimal en rango [min, max]
double randomDouble(double min, double max);

// Crea una persona con datos aleatorios
Persona generarPersona();

// Genera colección de n personas
std::vector<Persona> generarColeccion(int n);

// Busca persona por ID en un vector
// Retorna puntero a persona si la encuentra, nullptr si no
const Persona* buscarPorID(const std::vector<Persona>& personas, const std::string& id);

// --- Utilidades y análisis ---
// Calcula edad a partir de una fecha en formato DD/MM/AAAA (retorna -1 si no puede parsear)
int calcularEdad(const std::string& fechaNacimiento);

// Determina grupo tributario por los dos últimos dígitos del documento ('A','B','C', o 'X' si inválido)
char grupoCalendarioPorID(const std::string& id);

// Persona más longeva (global)
const Persona* personaMasLongeva(const std::vector<Persona>& personas);

// Persona más longeva por ciudad
std::unordered_map<std::string, const Persona*> personaMasLongevaPorCiudad(const std::vector<Persona>& personas);

// Mayor patrimonio (global)
const Persona* mayorPatrimonioGlobal(const std::vector<Persona>& personas);

// Mayor patrimonio por ciudad
std::unordered_map<std::string, const Persona*> mayorPatrimonioPorCiudad(const std::vector<Persona>& personas);

// Mayor patrimonio por grupo tributario (A/B/C)
std::unordered_map<char, const Persona*> mayorPatrimonioPorGrupo(const std::vector<Persona>& personas);

// Conteo de personas por grupo tributario (A/B/C)
std::unordered_map<char, size_t> conteoPorGrupo(const std::vector<Persona>& personas);

// Conteo de declarantes por grupo tributario (A/B/C)
std::unordered_map<char, size_t> conteoDeclarantesPorGrupo(const std::vector<Persona>& personas);

// Listado de declarantes por grupo tributario (A/B/C)
std::unordered_map<char, std::vector<const Persona*>> listarDeclarantesPorGrupo(const std::vector<Persona>& personas);

// Conteo de IDs inválidos según la asignación de grupo (devuelve cantidad)
size_t conteoInvalidosGrupo(const std::vector<Persona>& personas);

#endif // GENERADOR_H
