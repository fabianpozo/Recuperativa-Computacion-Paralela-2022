#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>

    // Se crea un struct con la definicion de equipos
struct equipos {
  std::string nombre;
  std::string comuna;
  std::string estadio;
  double latitud;
  double longitud;
};

    // funcion que calcula la distancia de dos equipos en la superficie de una esfera
    // usando formula de haversine

double distancia_h(double latitud1, double longitud1, double latitud2, double longitud2) {
  const double PI = 3.14159265358979323846;
  latitud1 *= PI / 180.0;
  longitud1 *= PI / 180.0;
  latitud2 *= PI / 180.0;
  longitud2 *= PI / 180.0;

    // calcula la diferencia entre las cordenadas
  double distanciaLat = latitud2 - latitud1;
  double distanciaLon = longitud2 - longitud1;

    // calcular distancia con la formula de haversine
  double x = sin(distanciaLat / 2) * sin(distanciaLat / 2) + cos(latitud1) * cos(latitud2) * sin(distanciaLon / 2) * sin(distanciaLon / 2);
  double resultado = 2 * atan2(sqrt(x), sqrt(1 - x));

    // retorna la distancia
  return resultado * 6371000;
}

    // Funcion para dividir una cadena en un vector de cadenas mediante un carácter separador dado.
std::vector<std::string> split(const std::string& s, char separator) {
    std::vector<std::string> fields;
    std::stringstream ss(s);
    std::string field;

    while (std::getline(ss, field, separator)) {
        fields.push_back(field);
    }

    return fields;
}
int main(int argc, char* argv[])
{
    // Abre el archivo txt
    std::ifstream inputFile("equipos.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo" << std::endl;
        return 1;
    }
    // Lee la primera linea del archivo y la descarta
    std::string header;
    std::getline(inputFile, header);

    // Vector que almacena los equipos del archivo antes leido
    std::vector<equipos> Equipos;

       // lee las lineas del archivo y guarda la informacion de los equipos dentro de un vector
    std::string line;
    while (std::getline(inputFile, line))
    {
     // Separa la informacion a traves de ';'
        std::vector<std::string> fields = split(line, ';');

     // Comprueba que se haya leidos los campos correspondidos
        if (fields.size() != 5)
        {
            std::cerr << "Error: formato de archivo incorrecto" << std::endl;
            return 1;
        }

     // Crear una estructura equipos con la información del archivo
        equipos var;
        var.nombre = fields[0];
        var.comuna = fields[1];
        var.estadio = fields[2];
        var.latitud = std::stod(fields[3]);
        var.longitud = std::stod(fields[4]);

     // Agregar el equipo al vector
        Equipos.push_back(var);
    }
     // Cerrar el archivo de entrada
        inputFile.close();

        // Calcula el numero de equipos
    int num_equipos = static_cast<int>(Equipos.size());

        // crea la matriz de distancias
    std::vector<std::vector<double>> distances(num_equipos, std::vector<double>(num_equipos));


    #pragma omp parallel for
    for (int i = 0; i < num_equipos; i++)
    {
        for (int j = 0; j < num_equipos; j++)
        {

            if (i == j)
            {
                distances[i][j] = 0.0;
                continue;
            }

            // Calcula la distancia entre equipos (i)y(j) con la formula de haversine
            double distance = distancia_h(Equipos[i].latitud, Equipos[i].longitud,
                                                Equipos[j].latitud, Equipos[j].longitud);

            // Almacena las distancias en la matriz
            distances[i][j] = distance;
            distances[j][i] = distance;
        }
    }

    // Crea el archivo CSV
    std::ofstream outputFile("fixture.csv");
    if (!outputFile.is_open())
    {
        std::cerr << "Error: no se pudo crear el archivo CSV" << std::endl;
        return 1;
    }

    // Escribe el encabezado en el archivo CSV
    outputFile << "Date;Local Team;Visiting Team;Distance (m)" << std::endl;

    // Escribe la informacion de los partidos dentro del archivo
    for (int i = 0; i < num_equipos; i++)
    {
        for (int j = 0; j < num_equipos; j++)
        {
            if (i == j)
                continue;

            // Ecribe los correspondientes campos en el archivo como fecha, equipo local, equipo visitante y la distancia entre estadios
            outputFile << i + 1 << ";\"" << Equipos[i].nombre << "\";\"" << Equipos[j].nombre
                       << "\";" << distances[i][j] << std::endl;
        }
    }

    // Cierra el archivo
    outputFile.close();

    return 0;
}
