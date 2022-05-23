#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define DMA_BUFF_SZ 16
#define M_PI 3.1415926
#define NUM_CALC_BUFF 3
#define NSUM 4

class vect_calc
{
public:
	vect_calc();
	~vect_calc();
	void init_sin();
	void init_calc(int n_sum, float freq);
	void step();
	void rza_sim();
	void result_calc(int n_sum, float* Re, float* Im);
	void sum_calc(int n_sum, float val);
private:
	float* sin_array;
	int sin_array_sz;
	float Fs;
	float a[NSUM];
	float wx[NSUM];
	float wy[NSUM];
	int num_point[NSUM]; // число точек на период
	float freq; // частота сигнала текущая
	float prev[NSUM]; //z-1
	float prev_prev[NSUM]; //z-2
	float dma_buff[DMA_BUFF_SZ];
	float ADC_data[DMA_BUFF_SZ * NUM_CALC_BUFF];
	int cnt[NSUM]; // Счетчик

};
void vect_calc::init_sin()
{
	int str_cnt = 0;
	char buff[100];
	FILE* rfile = fopen("array2.csv", "r");

	while (fgets(buff, 100, rfile) != NULL)
	{
		str_cnt++;
		//n = atof();
	}
	fclose(rfile);
	rfile = fopen("array2.csv", "r");
	sin_array_sz = str_cnt;
	sin_array = new float[sin_array_sz];
	str_cnt = 0;
	while (fgets(buff, 100, rfile) != NULL)
	{
		sin_array[str_cnt] = atof(buff);
		str_cnt++;
	}
	printf("Massiv: \n");
	for (int i = 0;i < sin_array_sz; i++)
	{
		printf("%.3f \n", sin_array[i]);
	}
	printf("Konec massiva \n");
	printf("Size massiva: %d\n", sin_array_sz);
}
void vect_calc::init_calc(int n_sum, float freq)
{
	//float a, wx, wy, Fs;
	//float n_harm = 1; // номер гармоники
	//freq = 50;
	//Fs = 2000;
	//float num_point_float = Fs / freq;
	//num_point = (int)num_point_float;
	//printf("num_point = %d, num_point_float = %.2f \n", num_point, num_point_float);
	//a = 2 * cos((2 * M_PI * n_harm) / num_point);
	//wx = a / 2;
	//wy = sqrt(1 - wx * wx);
	//printf("a = %.3f, wx = %.3f, wy = %.3f\n", a, wx, wy);
	Fs = 2000;
	float n_harm = 1;
	float num_point_float = Fs / freq;
	num_point[n_sum] = (int)num_point_float;
	a[n_sum] = 2 * cos((2 * M_PI * n_harm) / num_point[n_sum]);
	wx[n_sum] = a[n_sum] / 2;
	wy[n_sum] = sqrt(1 - wx[n_sum] * wx[n_sum]);
	prev[n_sum] = 0;
	prev_prev[n_sum] = 0;
	cnt[n_sum] = 0;
}

void vect_calc::result_calc(int n_sum, float* Re, float* Im)
{
	*Re = (prev[n_sum] * wx[n_sum] - prev_prev[n_sum]) * 2 / num_point[n_sum];
	*Im = prev[n_sum] * wy[n_sum] * 2 / num_point[n_sum];
}

void vect_calc::sum_calc(int n_sum, float val)
{
	float out = a[n_sum] * prev[n_sum] - prev_prev[n_sum] + val;
	prev_prev[n_sum] = prev[n_sum];
	prev[n_sum] = out;
	cnt[n_sum]++;
}

void vect_calc::step()
{
	/*prev = 0;
	prev_prev = 0;

	for (int i = 0; i < sin_array_sz && i < num_point; i++)
	{

		float out = a * prev - prev_prev + sin_array[i];
		prev_prev = prev;
		prev = out;

	}
	float out_real = (prev * wx - prev_prev) * 2 / num_point;
	float out_imag = prev * wy * 2 / num_point;
	float out_abs = sqrt(out_real * out_real + out_imag * out_imag);
	float out_arg = atan2(out_imag, out_real);
	printf("out = %.2f |_ %.2f \n", out_abs, out_arg * 180 / M_PI);*/
}
void vect_calc::rza_sim()
{
	for (int n_sum = 0; n_sum < NSUM; n_sum++)
	{
		init_calc(n_sum, 50);
	}
	for (int i = 0; i < sin_array_sz; i++)
	{
		float ADC_val = sin_array[i];
		for (int n_sum = 0; n_sum < NSUM; n_sum++)
		{
			sum_calc(n_sum, ADC_val);
			if (cnt[n_sum] >= num_point[n_sum])
			{
				float out_real;
				float out_image;
				result_calc(n_sum, &out_real, &out_image);
				float out_abs = sqrt(out_real * out_real + out_image * out_image);
				float out_arg = atan2(out_image, out_real);
				printf("out = %.2f |_ %.2f \n", out_abs, out_arg * 180 / M_PI);
				init_calc(n_sum, 50);
			}
		}

	}



	//int buff_idx = 0;
	//int num_step = sin_array_sz / DMA_BUFF_SZ;
	//// Инициализация нулями расчётного буфера
	//for (int i = 0; i < NUM_CALC_BUFF * DMA_BUFF_SZ; i++) ADC_data[i] = 0;
	//// Основной цикл устройства
	//for (int step = 0; step < num_step; step++)
	//{
	//	int start_index = step * DMA_BUFF_SZ;
	//	printf("\nStep %d \n", step);
	//	for (int i = 0; i < DMA_BUFF_SZ; i++)
	//	{
	//		dma_buff[i] = sin_array[start_index + i];
	//		printf("[%.2f]", dma_buff[i]);
	//		ADC_data[buff_idx * DMA_BUFF_SZ + i] = dma_buff[i];
	//	}
	//	printf("\n");

	//	int start_pt = DMA_BUFF_SZ - (num_point - 2 * DMA_BUFF_SZ); //старт в  1 буфере
	//	printf("start_pt =  %d \n", start_pt);
	//	if (start_pt < 0) start_pt = 0;
	//	if (start_pt >= DMA_BUFF_SZ) start_pt = DMA_BUFF_SZ - 1;

	//	prev = 0; prev_prev = 0;
	//	char FName[128];
	//	sprintf(FName, "oscillo%d.csv", step);
	//	FILE* wfile = fopen(FName, "w");
	//	int start_buff_idx = buff_idx + 1; if (start_buff_idx >= NUM_CALC_BUFF) start_buff_idx = 0;
	//	for (int j = 0; j < NUM_CALC_BUFF; j++) // цикл по буферам
	//	{
	//		printf("start_buff_idx = %d \n", start_buff_idx);
	//		if (j != 0) start_pt = 0;
	//		for (int i = start_pt; i < DMA_BUFF_SZ; i++) // цикл по точкам внутри буфера
	//		{

	//			fprintf(wfile, "%.2f;", ADC_data[i + start_buff_idx * DMA_BUFF_SZ]);
	//			float out = a * prev - prev_prev + ADC_data[i + start_buff_idx * DMA_BUFF_SZ];
	//			prev_prev = prev;
	//			prev = out;
	//		}
	//		start_buff_idx++; if (start_buff_idx >= NUM_CALC_BUFF) start_buff_idx = 0;
	//	}
	//	fclose(wfile);
	//	float out_real = (prev * wx - prev_prev) * 2 / num_point;
	//	float out_imag = prev * wy * 2 / num_point;
	//	float out_abs = sqrt(out_real * out_real + out_imag * out_imag);
	//	float out_arg = atan2(out_imag, out_real);
	//	printf("out = %.2f |_ %.2f \n", out_abs, out_arg * 180 / M_PI);
	//	buff_idx++; if (buff_idx >= NUM_CALC_BUFF) buff_idx = 0;
	//}
}
vect_calc::vect_calc() {}
vect_calc::~vect_calc() {}
int main()
{
	vect_calc* calc_ptr = new vect_calc();
	calc_ptr->init_sin();
	calc_ptr->rza_sim();
	/*printf("\nFunction step\n");
	calc_ptr->step();*/
}

