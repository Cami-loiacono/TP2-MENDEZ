#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "oponente.h"

#define TAMANIO_TABLERO 10
#define CANTIDAD_BARCOS 5

const int ERROR_LECTURA = 1;
const int ERROR_ESCRITURA = 2;
const int ERROR_ABRIR_ARCHIVO = 3;
const int ERROR_CANTIDAD_ARGUMENTOS = 4;
const int EXITO = 0;
const int INICIALIZACION_INVALIDA = -1;
const int ERROR_GENERAL = -1;

const char AGUA = 'A';
const char TOCADO = 'T';
const char HUNDIDO = 'H';
const char BARCO = 'B';

const int CANTIDAD_BARCOS_LARGO_2 = 1;
const int CANTIDAD_BARCOS_LARGO_3 = 2;
const int CANTIDAD_BARCOS_LARGO_4 = 1;
const int CANTIDAD_BARCOS_LARGO_5 = 1;

const char NORTE = 'N';
const char SUR = 'S';
const char ESTE = 'E';
const char OESTE = 'O';

typedef struct juego
{
    char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO];
    char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO];
    barco_t barcos_jugador[CANTIDAD_BARCOS];
    int cantidad_barcos_leidos;

    int balas_aliadas_acertadas;
    int balas_aliadas_erradas;
    int balas_enemigas_acertadas;
    int balas_enemigas_erradas;
    int barcos_enemigos_hundidos;
    int barcos_aliados_sobrevivientes;
} juego_t;

void inicializar_juego(juego_t *juego)
{
    for (int i = 0; i < TAMANIO_TABLERO; i++)
    {
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            juego->tablero_propio[i][j] = '*';
            juego->tablero_enemigo[i][j] = '*';
        }
    }
    juego->cantidad_barcos_leidos = 0;
    juego->balas_aliadas_acertadas = 0;
    juego->balas_aliadas_erradas = 0;
    juego->balas_enemigas_acertadas = 0;
    juego->balas_enemigas_erradas = 0;
    juego->barcos_enemigos_hundidos = 0;
    juego->barcos_aliados_sobrevivientes = CANTIDAD_BARCOS;
}

void liberar_barcos_memoria(juego_t *juego)
{
    for (int i = 0; i < juego->cantidad_barcos_leidos; i++)
    {
        if (juego->barcos_jugador[i].posiciones != NULL)
        {
            free(juego->barcos_jugador[i].posiciones);
            juego->barcos_jugador[i].posiciones = NULL;
        }
    }
}
void mostrar_interfaz(juego_t *juego)
{
    printf("\n==== TU TABLERO (DEFENSA) ====       ==== TABLERO ENEMIGO (ATAQUE) ====\n");
    printf("   1 2 3 4 5 6 7 8 9 10                 1 2 3 4 5 6 7 8 9 10\n");
    for (int i = 0; i < TAMANIO_TABLERO; i++)
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            printf("%c ", juego->tablero_propio[i][j]);
        }
        printf("             %2d ", i + 1);
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            printf("%c ", juego->tablero_enemigo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
bool validar_composicion_barcos(juego_t *juego)
{
    int cont_largo_2 = 0, cont_largo_3 = 0, cont_largo_4 = 0, cont_largo_5 = 0;
    for (int i = 0; i < CANTIDAD_BARCOS; i++)
    {
        if (juego->barcos_jugador[i].largo == 2)
            cont_largo_2++;
        else if (juego->barcos_jugador[i].largo == 3)
            cont_largo_3++;
        else if (juego->barcos_jugador[i].largo == 4)
            cont_largo_4++;
        else if (juego->barcos_jugador[i].largo == 5)
            cont_largo_5++;
    }
    return (cont_largo_2 == 1 && cont_largo_3 == 2 && cont_largo_4 == 1 && cont_largo_5 == 1);
}
int validar_largo_barco(int largo)
{
    return (largo >= 2 && largo <= 5);
}
int validar_largo_cantidad_barcos(int largo, int cantidad)
{
    if (largo == 2)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_2);
    else if (largo == 3)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_3);
    else if (largo == 4)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_4);
    else if (largo == 5)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_5);
    return false;
}
int validar_cantidad_barcos_cargados(int cantidad)
{
    if (cantidad == 0 || cantidad > CANTIDAD_BARCOS)
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}
int validar_direccion(char direccion)
{
    if ((direccion != NORTE) && (direccion != SUR) && (direccion != ESTE) && (direccion != OESTE))
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}
int validacion_largo_barco_cantidad(int largo_barco, FILE *barcos, int cantidad_largo_2, int cantidad_largo_3, int cantidad_largo_4, int cantidad_largo_5)
{
    if (largo_barco == 2)
    {
        cantidad_largo_2++;
        if (!validar_largo_cantidad_barcos(largo_barco, cantidad_largo_2))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 2 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    else if (largo_barco == 3)
    {
        cantidad_largo_3++;
        if (!validar_largo_cantidad_barcos(largo_barco, cantidad_largo_3))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 3 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    else if (largo_barco == 4)
    {
        cantidad_largo_4++;
        if (!validar_largo_cantidad_barcos(largo_barco, cantidad_largo_4))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 4 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    else if (largo_barco == 5)
    {
        cantidad_largo_5++;
        if (!validar_largo_cantidad_barcos(largo_barco, cantidad_largo_5))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 5 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    return EXITO;
}
int validar_datos_barco(int fila, int columna, char direccion, int largo, int cant_barcos, FILE *barcos_jugador, int cant_largo_2, int cant_largo_3, int cant_largo_4, int cant_largo_5)
{
    if (cant_barcos >= CANTIDAD_BARCOS)
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Se han leído más barcos de los permitidos.\n");
        return ERROR_LECTURA;
    }

    if (fila < 1 || fila > TAMANIO_TABLERO || columna < 1 || columna > TAMANIO_TABLERO)
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Posición inicial fuera del mapa: %d;%d\n", fila, columna);
        return ERROR_LECTURA;
    }

    if (validar_direccion(direccion) != EXITO)
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Dirección inválida: %c. \n Solo puede ser: -S-, -N-, -E-, -O-", direccion);
        return ERROR_LECTURA;
    }

    if (!validar_largo_barco(largo))
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Largo de barco inválido: %d\n", largo);
        return ERROR_LECTURA;
    }

    if (validacion_largo_barco_cantidad(largo, barcos_jugador, cant_largo_2, cant_largo_3, cant_largo_4, cant_largo_5) != EXITO)
    {
        fclose(barcos_jugador);
        printf("¡Error! Composición de barcos inválida, el jugador solo debe poder tener:\n %d de largo 2\n %d de largo 3\n %d de largo 4\n %d de largo 5\n", CANTIDAD_BARCOS_LARGO_2, CANTIDAD_BARCOS_LARGO_3, CANTIDAD_BARCOS_LARGO_4, CANTIDAD_BARCOS_LARGO_5);
        return ERROR_LECTURA;
    }

    return EXITO;
}
void cargar_direccion(char direccion, int *posicion_fila, int *posicion_columna)
{
    if (direccion == NORTE)
    {
        *posicion_fila = -1;
    }
    else if (direccion == SUR)
    {
        *posicion_fila = 1;
    }
    else if (direccion == ESTE)
    {
        *posicion_columna = 1;
    }
    else if (direccion == OESTE)
    {
        *posicion_columna = -1;
    }
}
bool validar_y_cargar_posiciones_barco(int fila_matriz, int columna_matriz, int posicion_fila, int posicion_columna, int largo_barco, coordenada_t *posiciones, bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    int fila_actual = INICIALIZACION_INVALIDA;
    int columna_actual = INICIALIZACION_INVALIDA;
    for (int i = 0; i < largo_barco; i++)
    {
        fila_actual = fila_matriz + (posicion_fila * i);
        columna_actual = columna_matriz + (posicion_columna * i);

        if (fila_actual < 0 || fila_actual >= TAMANIO_TABLERO || columna_actual < 0 || columna_actual >= TAMANIO_TABLERO || matriz_ocupacion[fila_actual][columna_actual])
        {
            return false;
        }

        posiciones[i].fila = fila_actual;
        posiciones[i].columna = columna_actual;
    }
    return true;
}
void asignar_barco(juego_t *juego, int largo_barco, coordenada_t *posiciones)
{
    barco_t *nuevo_barco = &(juego->barcos_jugador[juego->cantidad_barcos_leidos]);
    nuevo_barco->largo = largo_barco;
    nuevo_barco->posiciones = posiciones;
}
void dibujar_posiciones_barco(juego_t *juego, coordenada_t *posiciones, int largo_barco, bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = 0; i < largo_barco; i++)
    {
        matriz_ocupacion[posiciones[i].fila][posiciones[i].columna] = true;
        juego->tablero_propio[posiciones[i].fila][posiciones[i].columna] = BARCO;
    }
}
void inicializar_tablero_ocupacion(bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = 0; i < TAMANIO_TABLERO; i++)
    {
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            matriz_ocupacion[i][j] = false;
        }
    }
}

int cargar_barcos_propios(const char *archivo_barcos, juego_t *juego)
{
    FILE *barcos_jugador = fopen(archivo_barcos, "r");
    if (!barcos_jugador)
    {
        printf("¡Error de abrir el archivo de barcos!\n");
        return ERROR_ABRIR_ARCHIVO;
    }
    int fila_inicializacion = INICIALIZACION_INVALIDA;
    int columna_inicializacion = INICIALIZACION_INVALIDA;
    int largo_barco = INICIALIZACION_INVALIDA;
    char direccion = '\0';
    int cantidad_largo_2 = 0;
    int cantidad_largo_3 = 0;
    int cantidad_largo_4 = 0;
    int cantidad_largo_5 = 0;
    bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO];
    inicializar_tablero_ocupacion(matriz_ocupacion);

    while ((fscanf(barcos_jugador, "%d;%d;%c;%d", &fila_inicializacion, &columna_inicializacion, &direccion, &largo_barco)) != EOF)
    {
        if (validar_datos_barco(fila_inicializacion, columna_inicializacion, direccion, largo_barco, juego->cantidad_barcos_leidos, barcos_jugador, cantidad_largo_2, cantidad_largo_3, cantidad_largo_4, cantidad_largo_5) != EXITO)
        {
            return ERROR_LECTURA;
        }
        int posicion_fila = 0;
        int posicion_columna = 0;

        cargar_direccion(direccion, &posicion_fila, &posicion_columna);
        coordenada_t *posiciones = malloc(sizeof(coordenada_t) * (size_t)largo_barco);
        if (!posiciones)
        {
            fclose(barcos_jugador);
            return ERROR_LECTURA;
        }

        int fila_matriz = fila_inicializacion - 1;
        int columna_matriz = columna_inicializacion - 1;

        if (!validar_y_cargar_posiciones_barco(fila_matriz, columna_matriz, posicion_fila, posicion_columna, largo_barco, posiciones, matriz_ocupacion))
        {
            free(posiciones);
            fclose(barcos_jugador);
            printf("¡Error de lectura! El barco ocupa posiciones inválidas o superpuestas.\n");
            return ERROR_LECTURA;
        }

        asignar_barco(juego, largo_barco, posiciones);                            
        dibujar_posiciones_barco(juego, posiciones, largo_barco, matriz_ocupacion); 
        juego->cantidad_barcos_leidos++;
    }

    fclose(barcos_jugador);
    int validacion = validar_cantidad_barcos_cargados(juego->cantidad_barcos_leidos);
    if (validacion != EXITO)
    {
        return validacion;
    }
    return EXITO;
}

/*-----------------------------------------------------------------*/
/*revisar de aca hasta abajo*/
void registrar_disparo_enemigo(juego_t *juego, coordenada_t disp)
{
    if (juego->tablero_propio[disp.fila][disp.columna] == 'B')
    {
        juego->tablero_propio[disp.fila][disp.columna] = 'T';
        juego->balas_enemigas_acertadas++;

        for (int i = 0; i < CANTIDAD_BARCOS; i++)
        {
            barco_t *b = &(juego->barcos_jugador[i]);
            bool pertenece = false;
            for (int j = 0; j < b->largo; j++)
            {
                if (b->posiciones[j].fila == disp.fila && b->posiciones[j].columna == disp.columna)
                {
                    pertenece = true;
                    break;
                }
            }
            if (pertenece)
            {
                bool hundido = true;
                for (int j = 0; j < b->largo; j++)
                {
                    if (juego->tablero_propio[b->posiciones[j].fila][b->posiciones[j].columna] != 'T')
                    {
                        hundido = false;
                        break;
                    }
                }
                if (hundido)
                {
                    juego->barcos_aliados_sobrevivientes--;
                    for (int j = 0; j < b->largo; j++)
                    {
                        juego->tablero_propio[b->posiciones[j].fila][b->posiciones[j].columna] = 'H';
                    }
                    printf("¡El enemigo ha hundido uno de tus barcos!\n");
                }
                else
                {
                    printf("¡El enemigo te ha alcanzado!\n");
                }
                break;
            }
        }
    }
    else if (juego->tablero_propio[disp.fila][disp.columna] == '-')
    {
        juego->tablero_propio[disp.fila][disp.columna] = 'A';
        juego->balas_enemigas_erradas++;
        printf("¡El enemigo disparó al agua!\n");
    }
    else
    {
        juego->balas_enemigas_erradas++;
    }
}

int guardar_reporte(const char *path, juego_t *juego)
{
    FILE *f = fopen(path, "w");
    if (!f)
        return ERROR_ESCRITURA;
    bool error = false;
    if (fprintf(f, "Balas aliadas acertadas: %d\n", juego->balas_aliadas_acertadas) < 0 ||
        fprintf(f, "Balas aliadas erradas: %d\n", juego->balas_aliadas_erradas) < 0 ||
        fprintf(f, "Balas enemigas acertadas: %d\n", juego->balas_enemigas_acertadas) < 0 ||
        fprintf(f, "Balas enemigas erradas: %d\n", juego->balas_enemigas_erradas) < 0 ||
        fprintf(f, "Barcos enemigos hundidos: %d\n", juego->barcos_enemigos_hundidos) < 0 ||
        fprintf(f, "Barcos aliados sobrevivientes: %d\n", juego->barcos_aliados_sobrevivientes) < 0)
    {
        error = true;
    }
    if (fclose(f) != 0)
    {
        error = true;
    }
    if (error)
    {
        return ERROR_ESCRITURA;
    }
    return EXITO;
}
int verificar_cant_argumentos(int argc)
{
    if (argc != 3)
    {
        printf("Error de cantidad de argumentos. Uso: <programa_ejecutable> <archivo_barcos> <archivo_reporte>\n");
        return ERROR_CANTIDAD_ARGUMENTOS;
    }
    return EXITO;
}

void empezar_batalla(oponente_t *oponente, juego_t *juego, char *argv[])
{
    printf("Empieza la batalla!\n");
    while (juego->barcos_aliados_sobrevivientes > 0 && juego->barcos_enemigos_hundidos < CANT_BARCOS)
    {
        mostrar_interfaz(juego);
        int fila_ingresada = 0, columna_ingresada = 0;
        printf("Ingrese su disparo (FILA;COLUMNA): ");
        scanf("%d;%d", &fila_ingresada, &columna_ingresada);

        if (fila_ingresada < 1 || fila_ingresada > TAMANIO_TABLERO || columna_ingresada < 1 || columna_ingresada > TAMANIO_TABLERO)
        {
            printf("Coordenadas fuera del mapa (1 a 10). Intente de nuevo.\n");
            continue;
        }

        coordenada_t disp_jugador;
        disp_jugador.fila = fila_ingresada - 1;
        disp_jugador.columna = columna_ingresada - 1;

        char resultado_jugador = oponente_recibe_disparo(oponente, disp_jugador);
        printf("Resultado de tu disparo: ");
        if (resultado_jugador == AGUA)
        {
            printf("Agua\n");
            juego->tablero_enemigo[disp_jugador.fila][disp_jugador.columna] = 'A';
            juego->balas_aliadas_erradas++;
        }
        else if (resultado_jugador == TOCADO)
        {
            printf("Tocado\n");
            juego->tablero_enemigo[disp_jugador.fila][disp_jugador.columna] = 'T';
            juego->balas_aliadas_acertadas++;
        }
        else if (resultado_jugador == 'H')
        {
            printf("Hundido\n");
            juego->tablero_enemigo[disp_jugador.fila][disp_jugador.columna] = 'H';
            juego->balas_aliadas_acertadas++;
            juego->barcos_enemigos_hundidos++;
        }

        if (juego->barcos_enemigos_hundidos == CANT_BARCOS)
        {
            break;
        }

        // --- TURNO OPONENTE ---
        coordenada_t disp_enemigo = oponente_realiza_disparo(oponente);
        printf("El oponente dispara en la posición: %d;%d\n", disp_enemigo.fila + 1, disp_enemigo.columna + 1);

        registrar_disparo_enemigo(juego, disp_enemigo);
    }
}
void mostrar_mensaje_final(juego_t juego)
{
    if (juego.barcos_enemigos_hundidos == CANT_BARCOS)
    {
        printf("¡Felicidades Homero! ¡Lograste vencer a la computadora de manera justa!\n");
    }
    else
    {
        printf("Has sido derrotado. La MAQUINA hundió toda tu flota.\n");
    }
}
void mostrar_mensaje_error(int estado_carga)
{
    if (estado_carga == ERROR_ABRIR_ARCHIVO)
    {
        printf("Error de abrir el archivo.\n");
    }
    else
    {
        printf("Error de lectura.\n");
    }
}

int main(int argc, char *argv[])
{

    int estado_carga = verificar_cant_argumentos(argc);
    if (estado_carga != EXITO)
    {
        return estado_carga;
    }

    juego_t juego;
    inicializar_juego(&juego);
    estado_carga = cargar_barcos_propios(argv[1], &juego);

    if (estado_carga != EXITO)
    {
        mostrar_mensaje_error(estado_carga);
        liberar_barcos_memoria(&juego);
        return estado_carga;
    }

    oponente_t *oponente = oponente_crear(juego.barcos_jugador);
    if (!oponente)
    {
        liberar_barcos_memoria(&juego);
        return ERROR_LECTURA;
    }
    empezar_batalla(oponente, &juego, argv);

    mostrar_interfaz(&juego);
    mostrar_mensaje_final(juego);

    int estado_reporte = guardar_reporte(argv[2], &juego);
    oponente_destruir(oponente);
    liberar_barcos_memoria(&juego);

    if (estado_reporte != EXITO)
    {
        printf("Error de escritura.\n");
        return estado_reporte;
    }

    return EXITO;
}