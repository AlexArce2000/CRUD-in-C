<a name="readme-top"></a>

# CRUD básico en C + HTMX
Este proyecto implementa un servidor HTTP básico en C que proporciona operaciones CRUD, con interacción dinámica en el frontend mediante HTMX básico, para pasar el rato.
[![C-HTMX.png](https://i.postimg.cc/yxcmDG4b/C-HTMX.png)](https://postimg.cc/BPZLyMvT)

* Biblioteca Cliente de PostgreSQL
````
sudo apt-get install libpq-dev
````
* Biblioteca para Manejo de JSON
````
sudo apt-get install libjansson-dev
````
* Biblioteca para realizar los test
````
sudo apt-get install libcurl4-openssl-dev
````

## Instrucciones de Compilación y Ejecución
```
make run
```
Para eliminar el ejecutable:
```
make clean
```
Para realizar los test:
```
make test
```
## Endopoints
El servidor implementa los siguientes endpoints para operar sobre la base de datos:

**Crear (POST)**

**url:** `/example`

**Método:** POST

**Cuerpo de Solicitud (JSON):**
```
{
    "name": "NombreDelElemento",
    "description": "DescripciónDelElemento"
}
```
**Leer (GET)**

**URL:** `/example`

**Método:** GET

**Descripción:** Obtiene todos los registros de la tabla example en formato JSON.

**Actualizar (PUT)**

**URL:** `/example?id=<id>`

**Método:** PUT

**Descripción:** Actualiza el registro con el id especificado en la URL.

**Eliminar (DELETE)**

**URL:** `/example?id=<id>`

**Método:** DELETE

**Descripción:** Elimina el registro con el id especificado en la URL.

## Integración con HTMX
HTMX simplifica la interacción entre el servidor y el cliente. Al usar HTMX, las acciones como agregar, editar o eliminar un registro se reflejan inmediatamente en la página, sin necesidad de recargarla.

## Configuración de la Base de Datos
PostgreSQL configurada con la siguiente tabla:

```` 
CREATE TABLE example (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT NOT NULL
);
````

<p align="right">(<a href="#readme-top">volver arriba</a>)</p>