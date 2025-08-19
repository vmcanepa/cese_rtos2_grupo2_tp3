# Repositorio de TP 3 del grupo 2 - Sistemas Operativos de Tiempo Real (II)

## 1. Objetivo
Desarrollar un programa en RTOS que resuelva el problema planteado en el [enunciado de trabajo práctico 3](https://docs.google.com/document/d/1K6UFSMOq2QiGcxp6YNr2O3WbQZQIADuyz9awLDieVfk/edit?tab=t.0#heading=h.8jl34vsv9f2z), usando como plantillas los desarrollos de trabajo práctico 1 y 2 (objetos reactivos; memoria dinámica) y aplicando lo aprendido en el curso sobre **controladores de dispositivos**.

## 2. Integrantes de Grupo 2

- `e2405` -- Jezabel Victoria Danon.
- `e2406` -- Mariano Ariel Deville.
- `ext161` -- Victor Manuel Canepa.

## 3. Docentes revisores

- Juan Manuel Cruz.
- Sebastian Bedin.

 ## 4. Archivos soporte:
[link al video ](https://drive.google.com/drive/folders/1sKmQOnmRtQkY_eKGqrRGnLYTElz6uiuF?usp=drive_link)


[link al video version segunda entrega](https://drive.google.com/file/d/1-9lmuxtDYT4BDg0LZpagc_7vzvbbg0c5/view?usp=drive_link)

---

## 5. Breve guía para trabajo colaborativo (organización del repositorio git)

- **Uso de ramas (`branches`) para evitar pisar cambios entre integrantes:**
	- **Rama principal [`main`]:** En este _branch_ (rama) solo queda el código **revisado y estable**.
	- **Ramas de desarrollo [`dev_nombre`]:** Cada integrante puede trabajar libremente en su propia rama. Luego, cuando decide hacer pull a la rama `main` lo hace con _pull request_: es un pull que inicia el usuario pero no se completa hasta que el resto del equipo no se sincroniza).

### 5.1 Pasos a seguir:

- **CREA TU RAMA DE TRABAJO (solo se hace una vez)**:

  - Ejecutá los siguientes comandos `git` en el repositorio:
    ```bash
      git checkout main	            # Te aseguras que estás parado en el branch principal (main).
      git checkout -b dev_NOMBRE    # Desde main, el usuario "NOMBRE" crea una rama para desarrollo (dev).
      git push origin dev_NOMBRE    # La nueva rama se sube a github.
    ```

- **SUBIR CAMBIOS DESDE TU RAMA DE TRABAJO**

  Cada uno puede trabajar desde su rama (sin preocuparse de hacer subidas con riesgo de romper la versión revisada y estable):

    - Cada día, al comenzar, haces lo siguiente:
      ```bash
      git branch	 # Te dice en que rama estás trabajando actualmente (siempre debería ser la tuya evitando cambios en main).
      git checkout dev_NOMBRE	 # Si por accidente estás en otra rama, esto te asegura que ahora estás parado en tu branch de desarrollo (dev_NOMBRE).
      # trabajar normalmente y luego subir a github...
      git add .
      git commit -m "tu mensaje que sube a github."
      git push       # aca subis tus cambios y commit a github, pero con el alivio de que no estas modificando la rama main.
      ```
    - Si el grupo actualiza la rama `main` tu rama no sufre cambios. Vos te encargas de traer lo nuevo de `main` a tu rama:
      ```bash
      git checkout dev_NOMBRE
      git merge main  # esto actualiza tu repo local.
      git push
      ```
            
- **PASAR TUS CAMBIOS A LA RAMA COMÚN (`main`)**

  Cuando tengas mejoras significativas que ya se pueden agregar a la rama `main`:
  
  - en github seleccionas la vista de tu rama (`dev_NOMBRE`) y apretás en el botón **Open pull request** Asegurate que el sentido del _pull request_ sea de tu rama a la rama `main`. Listo eso es todo..
    
  - ..luego otro integrante recibe el aviso de que se ha iniciado un _pull request_ y solo debe acceder desde github y apretar en **Merge pull request**. Si durante la revisión del _pull request_ se detecta un conflicto de código, arreglarlo antes de hacer **Merge pull request** (o apretar **Close pull request**).
