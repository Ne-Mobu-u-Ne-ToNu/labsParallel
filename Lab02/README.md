Был использован #pragma omp parallel for для распараллеливания итераций в циклах и #pragma omp critical для защиты 
критических секций кода, когда несколько потоков могут обращаться к общим ресурсам, таким как вектор matrices и стандартный вывод.
Для измереня времени была использована функция omp_get_wtime()
<br/>
<br/>
<br/>
![image](https://github.com/Ne-Mobu-u-Ne-ToNu/labsParallel/assets/99840120/fed5147f-3c64-4d17-9c94-1a85d3b48de1)
