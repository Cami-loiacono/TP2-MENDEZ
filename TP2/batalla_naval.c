#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "oponente.h"

#define TAMANIO_TABLERO 10
#define CANTIDAD_BARCOS 5

const char FORMATO_LECTURA[] = "%[^;];%[^;];%[^;];%[^;]";
const char LECTURA = 'r';
const char ESCRITURA = 'w';

const int ERROR_LECTURA = 1;
const int ERROR_ESCRITURA = 2;
const int ERROR_ABRIR_ARCHIVO = 3;
const int ERROR_CANTIDAD_ARGUMENTOS = 4;
const int EXITO = 0;
const int INICIALIZACION_INVALIDA = -1;
const int ERROR_GENERAL = -1;
const int INICIALIZACION_VALIDA= 0;

const char AGUA = 'A';
const char TOCADO = 'T';
const char HUNDIDO = 'H';
const char BARCO = 'B';
const char MAPA_INCOGNITO = '*';

const int CANTIDAD_BARCOS_LARGO_2 = 1;
const int CANTIDAD_BARCOS_LARGO_3 = 2;
const int CANTIDAD_BARCOS_LARGO_4 = 1;
const int CANTIDAD_BARCOS_LARGO_5 = 1;
const int LARGO_BARCO_2 = 2;
const int LARGO_BARCO_3 = 3;
const int LARGO_BARCO_4 = 4;
const int LARGO_BARCO_5 = 5;
const int LARGO_MINIMO_BARCOS = 2;
const int LARGO_MAXIMO_BARCOS = 5;

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
