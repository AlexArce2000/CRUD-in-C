# CRUD básico en C
Este proyecto implementa un servidor HTTP básico en C que proporciona operaciones CRUD, para pasar el rato.

* Biblioteca Cliente de PostgreSQL
````
sudo apt-get install libpq-dev
````
* Biblioteca para Manejo de JSON
````
sudo apt-get install libjansson-dev

````

## Instrucciones de Compilación y Ejecución
```
make run
```
Para eliminar el ejecutable:
```
make clean
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

## Configuración de la Base de Datos
PostgreSQL configurada con la siguiente tabla:

```` 
CREATE TABLE example (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT NOT NULL
);
````