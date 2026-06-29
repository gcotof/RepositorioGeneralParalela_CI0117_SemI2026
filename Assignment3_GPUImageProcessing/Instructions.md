# Tarea 4 — Procesamiento de Imágenes en GPU

**Curso:** CI-0117 Programación Paralela y Concurrente
**Tema:** Programación en GPU

---

# Objetivos de aprendizaje

Al finalizar la tarea, el estudiante será capaz de:

* Entender cómo funciona un filtro Gaussiano (Gaussian Blur).
* Implementar un kernel de convolución para aplicar un filtro a una imagen.
* Transferir y ejecutar kernels en la GPU usando OpenACC.
* Usar directivas como:

  * `parallel loop`
  * `data`
  * `copyin`
  * `copyout`
  * `present`
* Comparar tiempos entre CPU y GPU y analizar el *speedup*.
* Generar salidas visuales (imágenes) para validar los resultados.

---

# Descripción general

En esta tarea implementarán un pipeline básico de procesamiento de imágenes acelerado por GPU:

1. Lectura de una imagen (PPM o usando `stb_image`).
2. Conversión a escala de grises.
3. Aplicación de un filtro Gaussiano (*Gaussian Blur*) usando OpenACC.
4. Escribir la imagen resultante a disco.

> **Importante:** La parte que **debe** estar acelerada con OpenACC es el **Blur Gaussiano**.

---

# ¿Qué es un blur Gaussiano?

Un *Gaussian Blur* suaviza una imagen: cada píxel se reemplaza por un promedio ponderado de sus vecinos.

Los vecinos más cercanos tienen mayor peso y los más lejanos tienen menos peso.

La distribución de los pesos sigue una curva Gaussiana (2D), lo que permite eliminar ruido y reducir detalles manteniendo la estructura general de la imagen.

---

# ¿Qué es un kernel (o máscara) Gaussiana?

Un **kernel** es una matriz pequeña (por ejemplo, **3×3** o **5×5**) que contiene los pesos del filtro.

Un kernel Gaussiano típico de **3×3** es:

```text
1 2 1
2 4 2
1 2 1
```

Todo el kernel se divide entre **16**:

```text
1   2   1
2   4   2   × (1/16)
1   2   1
```

Estos valores aproximan una Gaussiana con:

* σ ≈ 1

La suma de todos los valores del kernel debe ser **1** (normalización).

---

# ¿Qué significa "convolución"?

Para obtener el píxel filtrado `B(y,x)`:

1. Se coloca el kernel centrado sobre el píxel `(y,x)`.
2. Se multiplica cada vecino por su peso correspondiente.
3. Se suman todos esos productos.
4. El resultado se escribe en una nueva imagen.

Formalmente:

```text
B(y,x) = Σ Σ K(ky,kx) * I(y+ky, x+kx)

ky, kx ∈ [-R, R]
```

Donde:

* Para un kernel **3×3**:

  * `R = 1`
* Para un kernel **5×5**:

  * `R = 2`

---

# Detalles clave al implementarlo

## A) No modificar la imagen original mientras calculas

Debes utilizar dos arreglos distintos:

* `gray[]` → entrada
* `blur[]` → salida

Si escribes sobre la misma imagen mientras aún lees sus vecinos, el resultado será incorrecto.

---

## B) Manejo de bordes

Cuando el kernel se sale de la imagen existen tres estrategias clásicas:

1. **Clamp**

   * Utilizar el píxel válido más cercano.

2. **Reflect**

   * Reflejar los índices como si fueran un espejo.

3. **Zero-padding**

   * Considerar que los valores fuera de la imagen son cero (produce bordes oscuros).

### Ejemplo usando Clamp

```cpp
yy = min(max(y + ky, 0), H - 1);
xx = min(max(x + kx, 0), W - 1);
```

---

# Pseudocódigo del algoritmo

```text
para cada pixel (y, x):

    suma = 0

    para ky desde -R hasta R:
        para kx desde -R hasta R:

            yy = clamp(y + ky)
            xx = clamp(x + kx)

            suma += K[ky + R][kx + R] * gray[yy, xx]

    blur[y, x] = suma
```

---

# Requerimientos obligatorios

## 1. Implementar el filtro Gaussiano en GPU usando OpenACC

Debe utilizar:

* `#pragma acc data`
* `#pragma acc parallel loop collapse(2)`
* Buen manejo de transferencia de datos entre CPU y GPU.

---

## 2. Pipeline mínimo obligatorio

* Leer imagen.
* Convertir a escala de grises.
* Aplicar Gaussian Blur en GPU.
* Guardar la imagen resultante.

---

## 3. Informe de rendimiento

Debe incluir:

* Tiempo en CPU (solo del Blur).
* Tiempo en GPU.
* Cálculo del speedup:

```text
Speedup = T_CPU / T_GPU
```

---

## 4. Código

El código debe estar:

* Limpio.
* Bien comentado.

---

# Opcionales (25 puntos)

* Implementar la misma tarea utilizando **CUDA**.

---

# Entrega

Debe entregarse:

* Código fuente (`.cpp`, `.hpp`).
* `README` explicando cómo compilar y ejecutar.
* Informe en PDF con:

  * Explicación breve del algoritmo.
  * Tiempos CPU vs GPU en **Kabré (nodo Nukwa)**.
  * Speedup obtenido en **Kabré (nodo Nukwa)**.
  * Capturas de la imagen original y la imagen procesada.

