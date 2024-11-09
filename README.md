# Optimization of a C Program

<p align="justify">
    The program implements a convolution operation on a two-dimensional matrix of size <code>XDIMxYDIM</code>, where a 3x3 convolution filter defined by the integer array `kern` is applied. The matrix is created using the <code>alloc_matrix()</code> function, which allocates the necessary memory and initializes it with consecutive values. Afterward, the <code>fill()</code> function is used to populate the matrix with random values between 0 and 99, and then the <code>compute()</code> function applies the convolution filter to each pixel in the matrix. Finally, the <code>print()</code> function is used to output the final values of the matrix after the filter is applied.
</p>

### Authors:
- **Bottini, Franco Nicolas**

## Performance Analysis (Original Program)

<p align="justify">
    The performance analysis was conducted using the profiling tool <i>gprof</i>, which provides information on the execution time of each function in the program. The tests were carried out on a 10000x10000 matrix to obtain more significant results. Additionally, the <code>print()</code> function's execution was omitted, and the console output in the <code>compute()</code> function was removed to get a more accurate view of the real performance of the program's most important functions: <code>fill()</code>, <code>compute()</code>, and <code>alloc_matrix()</code>. The program was executed 15 times, and the following results were obtained:
</p>

<p align="center">
    <img src="img/programa_original.png" alt="Image could not be loaded">
</p>

<p align="justify">
    The program's execution time varies slightly between runs, mainly due to how the operating system allocates CPU time among the different processes. The average execution time of this program is 4.13 seconds. Furthermore, analyzing one of the results provided by <i>gprof</i> for a program run:
</p>

```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 95.19      3.96     3.96        1     3.96     3.96  compute
  2.40      4.06     0.10        1     0.10     0.10  fill
  1.92      4.14     0.08        1     0.08     0.08  alloc_matrix
  0.48      4.16     0.02                             _init
```

<p align="justify">
    It is clear that the <code>compute()</code> function consumes the most execution time, followed by <code>fill()</code> and <code>alloc_matrix()</code>. This is because <code>compute()</code> is responsible for applying the convolution filter to each pixel in the matrix, making it the most operation-intensive function. Meanwhile, <code>fill()</code> fills the matrix with random values, being the second most time-consuming function. Finally, <code>alloc_matrix()</code> consumes the least time, as it only allocates memory for the matrix and initializes it. This can be better visualized in the following graph:
</p>

<p align="center">
    <img src="img/gprof_original.png" alt="Image could not be loaded">
</p>

## Code Optimization

<p align="justify">
    One of the first general observations is the declaration of <code>XDIM</code> and <code>YDIM</code> as global variables:
</p>

```C
int XDIM = 10000;
int YDIM = 10000;
```

<p align="justify">
    This declaration is unnecessary since their values remain constant throughout the program's execution, so they can be declared as constants or preprocessor macros. The latter was done in this case:
</p>

```C
#define XDIM 10000
#define YDIM 10000
```

### `compute()` Function

<p align="justify">
    As seen in the previous analysis, the most prominent function in the program's execution is <code>compute()</code>, defined as follows:
</p>

```C
/**
 * Original code of the compute() function with comments and formatting tweaks for better readability
 */

// Receives the two-dimensional array (matrix) and the kernel to perform the convolution
void compute(double** arr, int kern[3][3])
{
    double tmp_sum[9];      // Temporary array to store convolution values
    double dato, accum;     // Temporary variables to store matrix data and convolution sum
    int i, j, k, l;         // Variables to iterate over the matrix and kernel

    for(i = 0; i < XDIM; i++)         // Iterate over matrix rows
    {
        for(j = 0; j < YDIM; j++)     // Iterate over matrix columns
        {    
            if(i >= 1 && j >= 1 && i < XDIM - 1 && j < YDIM - 1) // Check if not at matrix edge
            {
                for(k = 0; k < 3; k++)      // Iterate over kernel rows
                {
                    for(l = 0; l < 3; l++)  // Iterate over kernel columns
                    {
                        int x = i + (l - 1);  // Calculate matrix row of the pixel to convolve
                        int y = j + (k - 1);  // Calculate matrix column of the pixel to convolve

                        dato = arr[x][y];   // Get value of the pixel to convolve

                        tmp_sum[l * 3 + k] = 2 * (2 * kern[l][k] * dato) / 1000 + 1; // Perform convolution
                    }
                }

                accum = 0;  // Initialize accumulation variable to 0

                for(k = 0; k < 3; k++) // Iterate over kernel rows
                    for(l = 0; l < 3; l++) // Iterate over kernel columns
                        accum = accum + tmp_sum[k * 3 + l]; // Accumulate convolution values
            }

            // Error: assignment should be inside the if block
            // Convolution does not affect matrix edges
            // Therefore, edge values should not be modified

            arr[i][j] = accum; // Save convolution value in the matrix
        }    
    }
}
```

<p align="justify">
    The first change is to remove the edge verification check, which repeats in every iteration of the nested loops:
</p>

```C
void compute(double** arr, int kern[3][3])
{
    //...

    for(i = 0; i < XDIM; i++)
    {
        for(j = 0; j < YDIM; j++)
        {    
            if(i >= 1 && j >= 1 && i < XDIM - 1 && j < YDIM - 1)
            {
                //...
            }
        }
    }
}
```

<p align="justify">
    This condition is only false at the first and last iterations of the outer loop and the first and last iterations of the inner loop; for all other iterations, the condition is true. We can eliminate <i>(XDIM - 2) * (YDIM - 2)</i> unnecessary conditional evaluations by changing the start and end of the nested <code>for</code> loops:
</p>

```C
void compute(double** arr, int kern[3][3])
{
    //...

    for(i = 1; i < XDIM - 1; i++)       // Start loop at 1 instead of 0 and end at XDIM - 1 instead of XDIM
    {
        for(j = 1; j < YDIM - 1; j++)   // Start loop at 1 instead of 0 and end at YDIM - 1 instead of YDIM
        {    
            // Conditional check is no longer necessary

            //...

            // This also fixes the assignment error mentioned earlier
        }
    }
}
```

<p align="justify">
    Next, the convolution operation can be optimized:
</p>

```C
for(k = 0; k < 3; k++)
{
    for(l = 0; l < 3; l++)
    {
        int x = i + (l - 1);
        int y = j + (k - 1);

        dato = arr[x][y];

        tmp_sum[l * 3 + k] = 2 * (2 * kern[l][k] * dato) / 1000 + 1;
    }
}

accum = 0;
for(k = 0; k < 3; k++)
    for(l = 0; l < 3; l++)
        accum = accum + tmp_sum[k * 3 + l];

arr[i][j] = accum;
```

<p align="justify">
    We can merge these operations to remove the temporary array and the second set of nested loops:
</p>

```C
accum = 0;
for(k = 0; k < 3; k++)
{
    for(l = 0; l < 3; l++)
    {
        int x = i + (l - 1);
        int y = j + (k - 1);

        dato = arr[x][y];

        accum += 2 * (2 * kern[l][k] * dato)

 / 1000 + 1;
    }
}
arr[i][j] = accum;
```

<p align="justify">
    The <code>dato</code> variable is now only used for storing the pixel value once per iteration, and the temporary array is no longer needed, reducing memory accesses and improving performance. Although these changes are minor, they can result in a significant performance boost in a high-repetition operation like this.
</p>

### Final Execution Time

<p align="justify">
    After optimizing the <code>compute()</code> function, the program was executed 15 times, and the average execution time was reduced to 2.75 seconds, representing a 33% improvement. Furthermore, using <i>gprof</i> to profile the program again, the new performance distribution is shown:
</p>

<p align="center">
    <img src="img/gprof_optimizado.png" alt="Image could not be loaded">
</p>

<p align="justify">
    The average execution time was significantly reduced, and the <code>compute()</code> function became slightly more efficient, accounting for 92.50% of the total execution time. The <code>fill()</code> and <code>alloc_matrix()</code> functions remained almost unchanged in terms of execution time. The improvement is evident in the following graph:
</p>

<p align="center">
    <img src="img/comparacion.png" alt="Image could not be loaded">
</p>

### Conclusion

<p align="justify">
    By making minor modifications to the <code>compute()</code> function, such as eliminating unnecessary conditional checks and reducing memory accesses, it was possible to improve the program's performance significantly. Further optimizations, such as parallel processing or additional algorithmic optimizations, could be explored, but the improvements obtained with these changes are already notable.
</p>
