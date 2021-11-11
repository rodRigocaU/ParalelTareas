# Tareas 4 - CUDA IMAGE TO GREY AND BLUR
SE REALIZA LA CONVERSION DE UNA IMAGEN UNA IMAGEN A UNA ESCALA DE GRISES O A DIFUMINADO USANDO CUDA GPU/ I REALISE THE CONVERSION OF AN IMAGE TO HIS GREY VERSION OR TO HIS BLUR VERSION USING CUDA GPU

ESTE TRABAJO FUE DESARROLLADO EN VISUAL STUDIO 2019 / THIS WORK WAS DEVELOP IN VISUAL STUDIO 2019
## REQUISITOS / REQUIREMENTS:
OPENCV-CUDA

PARA EJECUTARLO PODEMOS ABRIRLO DESDE EL VISUAL STUDIO Y EJECUTAR EL ARCHIVO "imag2G.cpp" que sera el main, nos devolvera 3 imagenes que son los 3 metodos, dos para obtener la imagen gris y uno para la imagen difuminada / TO EXECUTE IT WE CAN OPEN IT FROM THE VISUAL STUDIO AND EXECUTE THE FILE "imag2G.cpp" which will be the main, it will return 3 images of the 3 methods, two to obtain the gray image (result, result2) and one for the blurred image (result3). 

En el archivo de "kernel.cu" se encuentran los metodos para la obtencion de las imagenes, en "imag2G.cpp" se encuentra la lectura y escritura de las imagenes / In the file "kernel.cu" are the methods for obtaining the images, in "imag2G.cpp" is the reading and writing of the images.

